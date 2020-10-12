//
// Created by isnDev on 5/10/2020.
//

#ifndef QB_SAMPLE_PROJECT_EVENT_NEWDATA_H
#define QB_SAMPLE_PROJECT_EVENT_NEWDATA_H
#include <qb/event.h>
#include <json/json.h>

struct NewDataEvent : public qb::Event {
    qb::json::object data;
};

#endif //QB_SAMPLE_PROJECT_EVENT_NEWDATA_H
