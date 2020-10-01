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
#include "Session.h"
#include <qb/actor.h>
#include <qb/io/async.h>

class Session;
class SessionPoolActor
    : public qb::Actor
    , public qb::io::use<SessionPoolActor>::tcp::io_handler<Session> {

    Session::Protocol::Router _router;

public:
    SessionPoolActor() noexcept;

    Session::Protocol::Router const &
    router() {
        return _router;
    }

    // qb core events
    void on(NewSessionEvent &e);
};

#endif // QB_SAMPLE_PROJECT_SESSIONPOOLACTOR_H
