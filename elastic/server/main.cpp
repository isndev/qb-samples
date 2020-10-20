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

#include <fstream>
#include <qb/main.h>
#include "AcceptActor.h"
#include "SessionJSONActor.h"
#include "SessionHTTPActor.h"
#include "DatabaseActor.h"

int
main(int argc, char *argv[]) {
    // (optional) initialize the logger
    qb::io::log::init(argv[0]); // filepath
    qb::io::log::setLevel(
        qb::io::log::Level::INFO); // log only info, warning, error an critical

    // configure the Core
    qb::Main main;
    qb::ActorIdList db_ids;
    qb::ActorIdList json_ids;
    qb::ActorIdList http_ids;
    // parse config
    try {
        std::ifstream config_file(argc > 1 ? argv[1] : "config_multi.json");
        qb::json::object config;

        config_file >> config;
        // databases json
        for (auto it : config["Database"])
            db_ids.push_back(main.addActor<DatabaseActor>(it["core"].get<qb::CoreId>()));
        // session handler json
        for (auto it : config["JSONSessionHandler"])
            json_ids.push_back(main.addActor<SessionJSONActor>(it["core"].get<qb::CoreId>(), db_ids));
        // session handler http
        for (auto it : config["HTTPSessionHandler"])
            http_ids.push_back(main.addActor<SessionHTTPActor>(it["core"].get<qb::CoreId>(), db_ids));
        // acceptor json
        for (auto it : config["JSONAcceptor"]) {
            main.core(it["core"].get<qb::CoreId>())
                .addActor<AcceptActor>(it["listen"].get<std::string>(),
                                            it["port"].get<uint16_t>(),
                                            json_ids);
        }
        // acceptor http
        for (auto it : config["HTTPAcceptor"]) {
            main.core(it["core"].get<qb::CoreId>())
                .addActor<AcceptActor>(it["listen"].get<std::string>(),
                                            it["port"].get<uint16_t>(),
                                            http_ids);
        }
    } catch (...) {
        std::cerr << "failed to parse config file" << std::endl;
        return EXIT_FAILURE;
    }

    for (auto core_id : main.usedCoreSet()) {
        main.core(core_id).setLatency(100);
    }

    main.start(); // start the engine asynchronously
    main.join();  // Wait for the running engine
    // if all my actors had been destroyed then it will release the wait !
    return main.hasError() ? EXIT_FAILURE : EXIT_SUCCESS;
}