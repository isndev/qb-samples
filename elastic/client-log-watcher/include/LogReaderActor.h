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

#ifndef QB_SAMPLE_PROJECT_LOGREADERACTOR_H
#define QB_SAMPLE_PROJECT_LOGREADERACTOR_H
#include <regex>
#include <qb/actor.h>
#include <qb/io/async.h>
#include <qb/io/protocol/text.h>
#include <qb/io/transport/file.h>
#include <json/json.h>
#include "tag/LogPusherTag.h"
#include "event/LogEvent.h"

// Actor reading from file, parsing and converting log lines
// then push it to LogPusherActor through exclusive pipe
class LogReaderActor
    : public qb::Actor
    , public qb::io::use<LogReaderActor>::file {
    // const event pipe to client actor
    const qb::Pipe _logger_pipe;
    const std::string _log_format;
    const std::string _fpath;

    const class LogParser {
        using ParameterNames = std::vector<std::string>;

        ParameterNames _param_names;
        const std::regex _regex;

        // build regex from log format
        // ex: convert (date):(my_log)
        //     to      (.+):(.+)
        //     and save parameters name
        std::string init(std::string const& format) {
            std::string build_regex = format, search = format;
            const std::regex pieces_regex("\\((\\w+)\\)");
            std::smatch what;
            while (std::regex_search(search, what, pieces_regex)) {
                _param_names.push_back(what[1]);
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

        // parse a line and call lambda for each key/value found
        // ex: log_format => "(date): (log)"
        //     line => "01/01/1904: my log"
        //     parse will
        //           call func("date", "01/01/1904")
        //           call func("log", "my log")
        template <typename _Line, typename _Func>
        bool parse(_Line const& path, _Func func) const {
            std::match_results<typename _Line::const_iterator> what;
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
        : _logger_pipe(getPipe(this->getServiceId<LogPusherTag>(logger_core_id)))
        , _log_format(log_format)
        , _fpath(fpath)
        , _parser(log_format)
    {
        registerEvent<InitReaderEvent>(*this);
    }

    // called during initialization by the engine
    bool onInit() final {
        transport().open(_fpath);
        if (transport().is_open()) {
            push<InitReaderEvent>(id());
            return true;
        }
        return false;
    }

    uint64_t _parsed_line = 1;

    // qb-core events
    // called to consume all file from the beginning (step 1)
    // then start handling async
    void on(InitReaderEvent const &) {
        if (read_all() < 0)
            broadcast<qb::KillEvent>();
        else {
            // start io handling
            start(_fpath, 0.01); // 10ms
        }
    }

    // called when receiving new line from file
    void on(Protocol::message const &data) {
        if (!data.text.empty()) {
            // fill global log data
            qb::json::object log = {
                {"file", _fpath},
                {"ts", time() / 1000000000},
                {"full_log", data.text },
            };

            // parse log line and fill <key, value> in json from format
            const auto success = _parser.parse(data.text, [&log](auto key, auto value) {
              //std::cout << "key[" << key << "]value[" << value << "]" << std::endl;
              log[std::move(key)] = std::move(value);
            });

            if (success)
                _logger_pipe.push<LogEvent>().log = std::move(log); // push new log to LogPusher
            else
                std::cout << *this << " bad log format at line " << _parsed_line << std::endl;
        }
        // futur usage this counter to start from this next time
        ++_parsed_line;
    }
};

#endif // QB_SAMPLE_PROJECT_LOGREADERACTOR_H
