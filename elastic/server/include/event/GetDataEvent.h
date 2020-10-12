//
// Created by isnDev on 5/10/2020.
//

#ifndef QB_SAMPLE_PROJECT_EVENT_GETDATA_H
#define QB_SAMPLE_PROJECT_EVENT_GETDATA_H
#include <qb/event.h>
#include <json/json.h>

struct GetDataEvent : public qb::Event {
    uint64_t session_id;
    uint32_t ident;
    qb::json::object data;
};

#endif //QB_SAMPLE_PROJECT_EVENT_GETDATA_H
