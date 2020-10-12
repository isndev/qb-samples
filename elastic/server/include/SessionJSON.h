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

#ifndef QB_SAMPLE_PROJECT_SESSION_JSON_H
#define QB_SAMPLE_PROJECT_SESSION_JSON_H

#include <qb/io/async.h>
#include <json/json.h>

class SessionJSONActor;
class SessionJSON
    : public qb::io::use<SessionJSON>::tcp::client<SessionJSONActor> {
    // clean way to identify why client is disconnected
    enum DisconnectedReason : int {
        ByUser = 0,
        ServerError,
        Undefined // should never happen
    };

public:
    using Protocol = qb::json::protocol<SessionJSON>;

    SessionJSON() = delete;
    explicit SessionJSON(SessionJSONActor &server);

    // client is receiving a new message
    void on(Protocol::message &&msg);
    // client is being disconnected
    void on(qb::io::async::event::disconnected const &event);
};

#endif // QB_SAMPLE_PROJECT_SESSION_H
