/*
 * qb - C++ Actor Framework
 * Copyright (C) 2011-2020 isndev (www.qbaf.io). All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *         limitations under the License.
 */

#include <iostream>
#include <fstream>
#include <regex>

#include <qb/main.h>
#include <qb/actor.h>
#include <qb/io/async.h>
#include <qb/io/protocol/text.h>
#include <qb/io/transport/file.h>
#include <json/json.h>

struct LoggerTag {};

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct LogEvent : public qb::Event {
    qb::json::object log;
};

class LogPusherActor : public qb::ServiceActor<LoggerTag>
                  , public qb::io::use<LogPusherActor>::tcp::client<> {
    const std::string _host;
    const uint16_t _port;

public:

    using Protocol = qb::json::protocol<LogPusherActor>;

    LogPusherActor(std::string const& host, uint16_t const port)
        : _host(host), _port(port) {
        registerEvent<LogEvent>(*this);
    }

    bool
    onInit() final {
        // try connect
        if (qb::io::SocketStatus::Done == transport().connect(_host, _port)) {
            start(); // register io to listener
            return true;
        }
        return false;
    }

    // called when client has been disconnected
    void
    on(qb::io::async::event::disconnected const&) {
        std::cout << "Disconnected by server" << std::endl;
        // kill all actor
        broadcast<qb::KillEvent>();
    }

    void on(Protocol::message const&) const noexcept {}

    void on(LogEvent const& event) {
        *this << event.log << Protocol::end;
    }

};

class LogReaderActor
    : public qb::Actor
    , public qb::io::use<LogReaderActor>::file {
    // const event pipe to client actor
    const qb::Pipe _logger_pipe;
    const std::string _log_format;
    const std::string _fpath;

    const class LogParser {
    public:
        using ParsedLogs = qb::unordered_map<std::string, std::string>;
        using ParameterNames = std::vector<std::string>;

    private:
        ParameterNames _param_names;
        //ParsedLogs _parameters;
        const std::regex _regex;

        std::string
            init(std::string const& format) {
            std::string build_regex = format, search = format;
            const std::regex pieces_regex("\\((\\w+)\\)");
            std::smatch what;
            while (std::regex_search(search, what, pieces_regex)) {
                _param_names.push_back(what[1]);
                //_parameters.emplace(*_param_names.rbegin(), "");
                build_regex = build_regex.replace(build_regex.find(what[0]),
                    what[0].length(), "(.+)");
                search = what.suffix();
            }

            return std::move(build_regex);
        }

    public:
        explicit LogParser(std::string const& log_format)
            : _regex(init(log_format)) {}

        ~LogParser() = default;

        template <typename _Path, typename _Func>
		bool
		parse(_Path const& path, _Func func) const {
			std::match_results<typename _Path::const_iterator> what;
			auto ret = std::regex_match(path.cbegin(), path.cend(), what, _regex);
			if (ret) {
				for (size_t i = 1; i < what.size(); ++i) {
                                    func(_param_names[i - 1], what[i].str());
				}
			}
			return ret;
		}
    } _parser;
    
public:
    LogReaderActor() = delete;

    using Protocol = qb::protocol::text::command<LogReaderActor>;
    struct InitReaderEvent : public qb::Event {};

    // constructor
    explicit LogReaderActor(qb::CoreId const logger_core_id,
        std::string const &log_format,
        std::string const &fpath) noexcept
        : _logger_pipe(getPipe(this->getServiceId<LoggerTag>(logger_core_id)))
        , _log_format(log_format)
        , _fpath(fpath)
        , _parser(log_format)
    {
        registerEvent<InitReaderEvent>(*this);
    }

    bool onInit() final {
        transport().open(_fpath);
        if (transport().is_open()) {
            push<InitReaderEvent>(id());
            return true;
        }
        return false;
    }

    uint64_t _parsed_line = 0;

    void on(InitReaderEvent const &) {
        if (read_all() < 0)
            broadcast<qb::KillEvent>();
        else {
            // start io handling
            start(_fpath, 0.01); // 10ms
        }
    }

    void on(Protocol::message const &data) {
        if (!data.text.empty()) {
            auto& log = _logger_pipe.push<LogEvent>().log = {
                {"file", _fpath},
                {"ts", time() / 1000000000},
                {"full_log", data.text },
            };
            _parser.parse(data.text, [&log](auto key, auto value) {
                //std::cout << "key[" << key << "]value[" << value << "]" << std::endl;
                log[std::move(key)] = std::move(value);
             });
        }
        // futur usage this counter to start from this next time
        ++_parsed_line;
    }
};

int
main(int argc, char *argv[]) {
    // (optional) initialize the logger
    qb::io::log::init(argv[0]); // filepath
    qb::io::log::setLevel(
        qb::io::log::Level::INFO); // log only warning, error an critical

    if (argc < 2) {
        std::cout << "usage: ./qb-sample-elastic-client [Format:(key):(key2)...)] [FILE] [IP?] [PORT?]" << std::endl;
        return 1;
    }

    // parser cfg
    std::string log_format(argv[1]);
    std::string fpath(argv[2]);

    // default connection
    std::string ip = "127.0.0.1";
    uint16_t port = 60123;
    // usage: ./qb-sample-elastic-client [Format:(key):(key2)...)] [FILE] [IP?] [PORT?]
    if (argc > 3)
        ip = argv[1];
    if (argc > 4)
        port = std::atoi(argv[2]);

    // configure the Core
    qb::Main main;

    main.core(0)
        .setLatency(100)
        .addActor<LogReaderActor>(1, log_format, fpath);
    main.core(1)
        .setLatency(100)
        .addActor<LogPusherActor>(ip, port);

    main.start(); // start the engine asynchronously
    if (!main.hasError()) {
        std::cout << "Started watching " << fpath << ", listening on " << ip << ":" << port << std::endl;
        main.join();  // Wait for the running engine
        std::cout << "Stopped normally" << std::endl;
    } else
        main.join();  // Wait for the running engine
    // if all my actors are destroyed then it will release the wait !
    return 0;
}