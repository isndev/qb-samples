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

#include <qb/main.h>
#include "AcceptActor.h"
#include "SessionJSONActor.h"
#include "SessionHTTPActor.h"
#include "DatabaseActor.h"

int
main(int, char *argv[]) {
    // (optional) initialize the logger
    qb::io::log::init(argv[0]); // filepath
    qb::io::log::setLevel(
        qb::io::log::Level::WARN); // log only warning, error an critical

    // configure the Core
    qb::Main main;

    // core db
    auto db_ids = main.core(1).setLatency(100)
                      .builder()
        .addActor<DatabaseActor>()
        .idList();

    // core io
    main.core(0).setLatency(100);
    // json feed part
    auto json_ids = main.core(0).builder()
        .addActor<SessionJSONActor>(db_ids)
        .idList();
    // http api part
    auto http_ids = main.core(0).builder()
        .addActor<SessionHTTPActor>(db_ids)
        .idList();

    // acceptors
    main.core(0).builder()
        .addActor<AcceptActor>(std::string("127.0.0.1"), 60123, json_ids)
        .addActor<AcceptActor>(std::string("127.0.0.1"), 8080, http_ids);

    main.start(); // start the engine asynchronously
    main.join();  // Wait for the running engine
    // if all my actors had been destroyed then it will release the wait !
    return 0;
}