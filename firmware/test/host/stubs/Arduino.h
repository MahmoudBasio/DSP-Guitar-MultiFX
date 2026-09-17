#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

#define HIGH 1
#define LOW 0
#define INPUT_PULLUP 2
#define OUTPUT 3
#define PI 3.14159265358979323846
#define F_CPU_ACTUAL 600000000UL

namespace TestHardware {
extern uint32_t now;
extern int pins[64];
extern long encoder;
extern std::string serialOutput;
extern std::string serialInput;
}
inline unsigned long millis() { return TestHardware::now; }
inline void delay(unsigned long ms) { TestHardware::now += ms; }
inline void pinMode(int, int) {}
inline int digitalRead(int pin) { return TestHardware::pins[pin]; }
inline void digitalWrite(int pin, int value) { TestHardware::pins[pin] = value; }
template<class T> inline T constrain(T x, T lo, T hi) { return std::max(lo, std::min(x, hi)); }

struct MockSerial {
    void begin(int) {}
    explicit operator bool() const { return true; }
    int available() const { return static_cast<int>(TestHardware::serialInput.size()); }
    int read() {
        const int c = TestHardware::serialInput.front();
        TestHardware::serialInput.erase(0, 1);
        return c;
    }
    int availableForWrite() const { return 4096; }
    template<class... Args> void printf(const char *fmt, Args... args) {
        char text[1024];
        std::snprintf(text, sizeof(text), fmt, args...);
        TestHardware::serialOutput += text;
    }
    void println(const char *s) { TestHardware::serialOutput += std::string(s) + "\n"; }
    void write(const uint8_t *data, size_t len) {
        TestHardware::serialOutput.append(reinterpret_cast<const char *>(data), len);
    }
};
extern MockSerial Serial;
