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

#include "AcceptActor.h"
#include <iostream>
#include <utility>

AcceptActor::AcceptActor(std::string iface, uint16_t port, qb::ActorIdList const &pool_ids) noexcept
    : _iface(std::move(iface))
    , _port(port)
    , _pool_ids(pool_ids){
}

// Actor initialization
bool
AcceptActor::onInit() {
    if (qb::io::SocketStatus::Done == transport().listen(_port, _iface)) {
        std::cout << "Server started listening on " << _iface << ":" << _port
                  << std::endl;
        // register io to qb::io::listener
        start();
        return true;
    }

    return false;
}

// Called from qb::io on new session connected
void
AcceptActor::on(accepted_socket_type &&new_io) {
    // simple round robin id
    const auto pool_id = _pool_ids[_session_counter++ % _pool_ids.size()];
    std::cout << "Session(" << new_io.fd() << ") ip("
              << new_io.getRemoteAddress() << ") connected and transferred to core id(" << pool_id.index() << ")" << std::endl;
    push<NewSessionEvent>(pool_id)
            .socket = new_io;
}

// Called from qb::io on server disconnected
void
AcceptActor::on(qb::io::async::event::disconnected const &) {
    // kill all actors
    broadcast<qb::KillEvent>();
}