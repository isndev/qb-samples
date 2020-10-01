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

#ifndef QB_SAMPLE_PROJECT_ACCEPTACTOR_H
#define QB_SAMPLE_PROJECT_ACCEPTACTOR_H
#include <qb/actor.h>
#include <qb/io/async.h>
#include "event/NewSessionEvent.h"

class AcceptActor
    : public qb::Actor
    , public qb::io::use<AcceptActor>::tcp::acceptor {

    const std::string _iface;
    const uint16_t _port;
    qb::ActorIdList const &_pool_ids;

    std::size_t _session_counter = 0;

public:
    AcceptActor() = delete;
    AcceptActor(std::string iface, uint16_t port, qb::ActorIdList const &pool_ids) noexcept;

    // override Actor Initialization
    bool onInit() final;
    // events
    // called from qb::io on new session connected
    void on(accepted_socket_type &&new_io);
    // called from qb::io on server disconnected
    void on(qb::io::async::event::disconnected const &);
};

#endif // QB_SAMPLE_PROJECT_ACCEPTACTOR_H
