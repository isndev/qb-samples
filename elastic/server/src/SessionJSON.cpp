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

#include "SessionJSON.h"
#include "SessionJSONActor.h"
#include <iostream>
#include <string_view>

SessionJSON::SessionJSON(SessionJSONActor &server)
    : client(server) {
}

// client is receiving a new message
void
SessionJSON::on(Protocol::message &&msg) {
    // handle your message here
    std::cout << "Received from SessionJSON(" << transport().ident() << ") ip("
              << transport().getRemoteAddress() << ")" << std::endl
              << "-> Message (" << msg.size
              << "): " << std::string_view(msg.data, msg.size) << std::endl;
    server().sendToDatabase(std::move(msg.json));
}

// client is being disconnected
void
SessionJSON::on(qb::io::async::event::disconnected const &event) {
    std::cout << "SessionJSON(" << transport().ident() << ") ip(" << transport().getRemoteAddress()
              << ") disconnected -> ";
    switch (event.reason) {
    case DisconnectedReason::ByUser:
        std::cout << "By User" << std::endl;
        break;
    case ServerError:
        std::cout << "Server error" << std::endl;
        break;
    default:
        break;
    }
}