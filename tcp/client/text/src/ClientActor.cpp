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

#include "ClientActor.h"
#include <string_view>
#include <utility>

// constructor
ClientActor::ClientActor(std::string ip, uint16_t port) noexcept
    : _ip(std::move(ip))
    , _port(port) {
    // core will sleep if no activity
    // setCoreLowLatency(false);
    // register events
    registerEvent<CommandEvent>(*this);
    registerEvent<RetryConnectEvent>(*this);
}

// function to connect to remote server
bool
ClientActor::connect() {
    // try connect
    if (qb::io::SocketStatus::Done == transport().connect(_ip, _port)) {
        start(); // register io to listener
        return true;
    }
    // if connection fails then
    // push event to retry connection
    push<RetryConnectEvent>(id());
    return false;
}

// Actor initialization override
bool
ClientActor::onInit() {
    // engine will not start if connection fails first time
    return connect();
}

// received new message from remote
void
ClientActor::on(Protocol::message &&message) {
    // print received message
    std::cout << "Received (" << message.size << "): " << message.text << std::endl;
}

// on disconnect received command event
void
ClientActor::on(CommandEvent &event) {
    // publish message to remote server
    *this << event.message << Protocol::end;
}

// retry connection event
void
ClientActor::on(RetryConnectEvent const &event) {
    connect();
}

// called when client has been disconnected
void
ClientActor::on(qb::io::async::event::disconnected const &) {
    // push event to retry connection
    push<RetryConnectEvent>(id());
}