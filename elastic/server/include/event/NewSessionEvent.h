//
// Created by isnDev on 5/10/2020.
//

#ifndef QB_SAMPLE_PROJECT_EVENT_NEWSESSION_H
#define QB_SAMPLE_PROJECT_EVENT_NEWSESSION_H
#include <qb/event.h>
#include <qb/io/tcp/socket.h>

struct NewSessionEvent : public qb::Event {
    qb::io::tcp::socket socket;
};

#endif //QB_SAMPLE_PROJECT_EVENT_NEWSESSION_H
