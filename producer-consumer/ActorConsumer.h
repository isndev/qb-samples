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

#include "MyEvent.h"
#include <qb/actor.h>
#include <qb/system/timestamp.h>
#include <vector>

#ifndef ACTORCONSUMER_H_
#    define ACTORCONSUMER_H_

class ActorConsumer
    : public qb::Actor     // /!\ should inherit from qb actor
    , public qb::ICallback // (optional) required to register actor callback
{
    uint64_t timer = 0;
    uint64_t counter = 0;

    void
    reset_timer() {
        timer = time() + qb::Timestamp::seconds(1).nanoseconds();
    }

public:
    ActorConsumer() = default; // default constructor
    ~ActorConsumer() final = default;

    // will call this function before adding MyActor
    bool
    onInit() final {
        registerEvent<MyEvent>(*this); // will listen MyEvent
        registerCallback(*this);       // each core loop will call onCallback
        reset_timer();
        return true; // init ok, MyActor will be added
    }

    // will call this function each core loop
    void
    onCallback() final {
        if (time() > timer) {
            qb::io::cout() << "Consumer(" << id() << ") received " << counter << "/s"
                           << std::endl;
            //            LOG_INFO("Consumer(" << id() << ") received " << counter <<
            //            "/s");
            reset_timer();
            counter = 0;
        }
    }

    // will call this function when MyActor received MyEvent
    void
    on(MyEvent const &) {
        ++counter;
    }
};

#endif