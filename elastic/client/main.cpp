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
#include <qb/main.h>
#include <qb/actor.h>
#include <qb/io/async.h>
#include <json/json.h>

struct LoggerTag {};

struct LogEvent : public qb::Event {
    qb::json::object log;
};

class LoggerActor : public qb::ServiceActor<LoggerTag>
                  , public qb::io::use<LoggerActor>::tcp::client<> {
    const std::string _host;
    const uint16_t _port;

public:

    using Protocol = qb::json::protocol<LoggerActor>;

    LoggerActor(std::string const& host, uint16_t const port)
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
        // kill all actor
        broadcast<qb::KillEvent>();
    }

    void on(Protocol::message const&) const noexcept {}

    void on(LogEvent const& event) {
        *this << event.log << Protocol::end;
    }

};

class CmdActor
    : public qb::Actor
    , public qb::ICallback {
    // const event pipe to client actor
    const qb::Pipe _logger_pipe;

public:
    CmdActor() = delete;
    // constructor
    explicit CmdActor(qb::CoreId const logger_core_id) noexcept
        : _logger_pipe(getPipe(this->getServiceId<LoggerTag>(logger_core_id)))
    {
        registerCallback(*this);
    }

    // called each core loop
    void onCallback() final {
        std::string cmd;
        // /!\ blocking core, but it's ok for the example
        // CmdActor is alone in its core
        if (std::getline(std::cin, cmd))
            _logger_pipe.push<LogEvent>().log = {
                {"client", "client_cpp"},
                {"timestamp", time() / 1000000000},
                {"log", cmd}
            }; // push pseuo log to logger actor
        else {
            _logger_pipe.push<qb::KillEvent>(); // push event to kill client actor
            kill();                             // kill cmd Actor
        }
    }
};

int
main(int argc, char *argv[]) {
    // (optional) initialize the logger
    qb::io::log::init(argv[0]); // filepath
    qb::io::log::setLevel(
        qb::io::log::Level::WARN); // log only warning, error an critical

    // configure the Core
    qb::Main main;

    // default connection
    std::string ip = "127.0.0.1";
    uint16_t port = 60123;
    // usage: ./qb-sample-elastic-client [IP] [PORT]
    if (argc > 1)
        ip = argv[1];
    if (argc > 2)
        port = std::atoi(argv[2]);

    main.core(2).addActor<CmdActor>(3);
    main.core(3).addActor<LoggerActor>(ip, port);

    main.start(); // start the engine asynchronously
    main.join();  // Wait for the running engine
    // if all my actors had been destroyed then it will release the wait !
    return 0;
}