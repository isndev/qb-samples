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

#ifndef QB_SAMPLE_PROJECT_LOGPUSHERACTOR_H
#define QB_SAMPLE_PROJECT_LOGPUSHERACTOR_H
#include <qb/actor.h>
#include <qb/io/async.h>
#include "tag/LogPusherTag.h"
#include "event/LogEvent.h"

// Actor receiving Logs and publish it to server
class LogPusherActor : public qb::ServiceActor<LogPusherTag>
    , public qb::io::use<LogPusherActor>::tcp::client<> {
    const std::string _host;
    const uint16_t _port;

public:

    // tcp client will use json protocol message is [json\0]
    using Protocol = qb::json::protocol<LogPusherActor>;

    // no default constructor
    LogPusherActor() = delete;
    // constructor
    LogPusherActor(std::string const& host, uint16_t const port)
        : _host(host), _port(port) {
        // listening qb core event
        registerEvent<LogEvent>(*this);
    }

    // called during initialization by the engine
    bool onInit() final {
        // try connect
        if (qb::io::SocketStatus::Done == transport().connect(_host, _port)) {
            start(); // start listening async qb io
            return true;
        }
        return false;
    }

    // qb-io events
    // called when client has been disconnected
    void on(qb::io::async::event::disconnected const&) {
        std::cout << "Disconnected by server" << std::endl;
        // kill all actor
        broadcast<qb::KillEvent>();
    }

    // called when receiving message from server
    // actually unused
    void on(Protocol::message const&) const noexcept {}

    // qb-core events
    // called when received qb core LogEvent from actors
    void on(LogEvent const& event) {
        // qb async io publish new message [json\0] to server
        *this << event.log << Protocol::end; // same as publish(...)
    }

};

#endif // QB_SAMPLE_PROJECT_LOGPUSHERACTOR_H
