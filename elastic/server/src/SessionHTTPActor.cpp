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

#include "SessionHTTPActor.h"
#include "SessionHTTP.h"
#include <iostream>
#include <json/json.h>
#include <utility>

SessionHTTPActor::SessionHTTPActor(qb::ActorIdList const &db_ids) noexcept
    : _db_ids(db_ids)
{
    qb::http::Response res;
    res.status_code = HTTP_STATUS_NOT_FOUND;
    res.headers["Server"] = {"elastic_qb/2.0.0"};
    res.headers["Content-Type"] = {"text/html"};
    res.headers["Connection"] = {"close"};

    _router.setDefaultResponse(res)
        .GET("/",
             [](auto &ctx) {
                 ctx.response.status_code = HTTP_STATUS_OK;
                 ctx.response.body = "<!DOCTYPE html>"
                                     "<html>"
                                     "<head><title>Elastic qb</title></head>"
                                     "<body>Server is up</body>"
                                     "</html>";
                ctx.session << ctx.response;
             })
        .GET("/get", [this](auto &ctx) {
             if (!askToDatabase(ctx.session, ctx.request)) {
                ctx.response.status_code = HTTP_STATUS_BAD_REQUEST;
                ctx.session << ctx.response;
             }
        });
    registerEvent<NewSessionEvent>(*this);
    registerEvent<GetDataEvent>(*this);
}

bool
SessionHTTPActor::askToDatabase(SessionHTTP& session, qb::http::Request<std::string_view> const& request) {
    // (should parse request body as json)
    auto &e = push<GetDataEvent>(*_db_ids.begin());
    e.session_id = session.getSessionId(); // uniq session id (naive timestamp)
    e.ident = session.transport().fd(); // socket handler
    // e.data = std::move(parsed_json);
    return true;
}

// qb core events
void
SessionHTTPActor::on(NewSessionEvent &e) {
    registerSession(std::move(e.socket));
}

void
SessionHTTPActor::on(GetDataEvent const& e) {
    auto it = sessions().find(e.ident);
    if (it != sessions().end() && it->second.getSessionId() == e.session_id) {
        qb::http::Response res;

        res.status_code = HTTP_STATUS_OK;
        res.headers["Content-Type"] = { "application/json" };
        //res.body = e.data.dump();
        it->second << res << e.data;
    }
    else {
        // session closed before retrieving requested data
        // should log something
    }
}