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

#include "SessionPoolActor.h"
#include "Session.h"
#include <iostream>
#include <json/json.h>
#include <utility>

SessionPoolActor::SessionPoolActor() noexcept
{
    qb::http::Response res;
    res.status_code = HTTP_STATUS_NOT_FOUND;
    res.headers["Server"] = {"qb/2.0.0"};
    res.headers["Content-Type"] = {"text/html"};
    res.headers["Connection"] = {"close"};

    _router.setDefaultResponse(res)
        .GET("/",
             [](auto &ctx) {
                 ctx.response.status_code = HTTP_STATUS_OK;
                 ctx.response.body = "<!DOCTYPE html>"
                                     "<html>"
                                     "<head><title>Home</title></head>"
                                     "<body>Home</body>"
                                     "</html>";
                ctx.session << ctx.response;
             })
        .GET("/file/:path",
             [](auto &ctx) {
                 std::string fname = "./" + ctx.param("path", "index.html");
                 if (ctx.session._file.open(fname)) {
                     ctx.response.status_code = HTTP_STATUS_OK;
                     ctx.response.content_length = ctx.session._file.expected_size();
                 }
                 ctx.session << ctx.response;
             })
        .GET("/message/:msg", [](auto &ctx) {
            ctx.response.status_code = HTTP_STATUS_OK;
            ctx.response.headers["Content-Type"] = {"application/json"};
            ctx.response.body = qb::json::object{
                {"message", ctx.param("msg", "empty")},
                {"valid", ctx.query("valid", 0, "false")}
            }.dump();
            ctx.session << ctx.response;
        });
    registerEvent<NewSessionEvent>(*this);
}

// qb core events
void
SessionPoolActor::on(NewSessionEvent &e) {
    registerSession(std::move(e.socket));
}