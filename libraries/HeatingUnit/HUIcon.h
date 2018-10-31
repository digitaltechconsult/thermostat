#ifndef HUIcon_h
#define HUIcon_h

#include <Arduino.h>

struct HUIcon {
    int             x;
    int             y;
    int             width;
    int             height;
    const uint8_t*  bits;
    bool            visible;
    unsigned long   timer;
};

#endif