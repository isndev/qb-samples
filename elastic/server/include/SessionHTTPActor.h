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

#ifndef QB_SAMPLE_PROJECT_SESSIONPOOLACTOR_H
#define QB_SAMPLE_PROJECT_SESSIONPOOLACTOR_H
#include "event/NewSessionEvent.h"
#include "event/GetDataEvent.h"
#include "SessionHTTP.h"
#include <qb/actor.h>
#include <qb/io/async.h>

class SessionHTTP;
class SessionHTTPActor
    : public qb::Actor
    , public qb::io::use<SessionHTTPActor>::tcp::io_handler<SessionHTTP> {

    qb::ActorIdList const& _db_ids;
    SessionHTTP::Protocol::Router _router;

public:
    SessionHTTPActor(qb::ActorIdList const &db_ids) noexcept;

    SessionHTTP::Protocol::Router const &
    router() {
        return _router;
    }

    // send event to db
    bool askToDatabase(SessionHTTP & session, qb::http::Request<std::string_view> const &request);

    // qb core events
    void on(NewSessionEvent &e);
    void on(GetDataEvent const& e);
};

#endif // QB_SAMPLE_PROJECT_SESSIONPOOLACTOR_H
