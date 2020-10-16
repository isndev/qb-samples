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

#ifndef QB_SAMPLE_PROJECT_DATABASEACTOR_H
#define QB_SAMPLE_PROJECT_DATABASEACTOR_H
#include <qb/actor.h>
#include "event/NewDataEvent.h"
#include "event/GetDataEvent.h"

class DatabaseActor : public qb::Actor {
    qb::json::object _storage;

public:

    DatabaseActor() {
        registerEvent<NewDataEvent>(*this);
        registerEvent<GetDataEvent>(*this); 
    }

    void on(NewDataEvent& event) {
        // naive store step1
        _storage.push_back(std::move(event.data));
    }

    void on(GetDataEvent& event) {
        auto& e = push<GetDataEvent>(event.getSource());
        e.session_id = event.session_id;
        e.ident = event.ident;
        // naive get step1
        // Todo : retrieve data from json in event
        for (const auto& j : _storage) {
            e.data.push_back(j);
        }
    }
};

#endif // QB_SAMPLE_PROJECT_DATABASEACTOR_H
