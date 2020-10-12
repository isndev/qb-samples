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

#include "SessionJSONActor.h"
#include "SessionJSON.h"
#include "event/NewDataEvent.h"
#include <iostream>
#include <json/json.h>
#include <utility>

SessionJSONActor::SessionJSONActor(qb::ActorIdList const &db_ids) noexcept
    : _db_ids(db_ids)
{
    registerEvent<NewSessionEvent>(*this);
}

// qb core events
void
SessionJSONActor::on(NewSessionEvent &e) {
    registerSession(std::move(e.socket));
}

void
SessionJSONActor::sendToDatabase(qb::json::object&& obj) const noexcept {
    // Todo : storage strategy
    auto& e = push<NewDataEvent>(*_db_ids.begin());
    e.data = std::move(obj);
}