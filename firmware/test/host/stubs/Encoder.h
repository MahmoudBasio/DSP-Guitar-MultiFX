#pragma once
#include "Arduino.h"
struct Encoder {
    Encoder(int, int) {}
    long read() const { return TestHardware::encoder; }
};
