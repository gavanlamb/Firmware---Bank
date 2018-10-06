#include  <cstdint>

#ifndef RELAY_H
#define RELAY_H

struct Relay{
    uint16_t alias;
    uint16_t pin;
    int timer;
};

#endif
