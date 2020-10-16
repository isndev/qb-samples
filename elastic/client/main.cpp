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

#include <iostream>
#include <qb/main.h>

#include "LogReaderActor.h"
#include "LogPusherActor.h"

int
main(int argc, char *argv[]) {
    // (optional) initialize the qb logger
    qb::io::log::init(argv[0]); // filepath
    qb::io::log::setLevel(
        qb::io::log::Level::INFO); // log Info, Warning and Critical

    if (argc < 2) {
        std::cout << "usage: ./qb-sample-elastic-client [Format:(key):(key2)...)] [FILE] [IP?] [PORT?]" << std::endl;
        return 1;
    }

    // parser cfg
    std::string log_format(argv[1]);
    std::string fpath(argv[2]);

    // default connection
    std::string ip = "127.0.0.1";
    uint16_t port = 60123;

    if (argc > 3)
        ip = argv[1];
    if (argc > 4)
        port = std::atoi(argv[2]);

    // configure the Core
    qb::Main main;

    main.core(0)
        .setLatency(100)
        .addActor<LogReaderActor>(1, log_format, fpath);
    main.core(1)
        .setLatency(100)
        .setAffinity({0}) // use same physical core as reader
        .addActor<LogPusherActor>(ip, port);

    main.start(); // start the engine asynchronously
    if (!main.hasError()) {
        std::cout << "Started watching " << fpath << ", listening on " << ip << ":" << port << std::endl;
        main.join();  // Wait for the running engine
        std::cout << "Stopped normally" << std::endl;
    } else
        main.join();  // Wait for the running engine
    // if all my actors are destroyed then it will release the wait !
    return 0;
}