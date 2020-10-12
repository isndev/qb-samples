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

#ifndef QB_SAMPLE_PROJECT_SESSIONJSONACTOR_H
#define QB_SAMPLE_PROJECT_SESSIONJSONACTOR_H
#include "event/NewSessionEvent.h"
#include "SessionJSON.h"
#include <qb/actor.h>
#include <qb/io/async.h>

class SessionJSON;
class SessionJSONActor
    : public qb::Actor
    , public qb::io::use<SessionJSONActor>::tcp::io_handler<SessionJSON> {

    qb::ActorIdList const& _db_ids;

public:
    SessionJSONActor() = delete;
    SessionJSONActor(qb::ActorIdList const &db_ids) noexcept;

    // qb core events
    void on(NewSessionEvent &e);

    void sendToDatabase(qb::json::object&& obj) const noexcept;
};

#endif // QB_SAMPLE_PROJECT_SESSIONJSONACTOR_H
