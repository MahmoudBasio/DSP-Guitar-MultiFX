#ifndef CORE_DEBOUNCE_H
#define CORE_DEBOUNCE_H

#include <stdint.h>

// Independent stable-state debounce. Time subtraction tolerates millis() rollover.
class DebouncedInput {
public:
    void begin(bool pressed, uint32_t now) {
        raw = stable = pressed;
        changedAt = now;
    }

    // Returns true on either accepted edge, once per stable transition.
    bool update(bool pressed, uint32_t now, uint32_t intervalMs) {
        if (pressed != raw) { raw = pressed; changedAt = now; }
        if (raw != stable && static_cast<uint32_t>(now - changedAt) >= intervalMs) {
            stable = raw;
            return true;
        }
        return false;
    }

    bool isPressed() const { return stable; }

private:
    bool raw = false;
    bool stable = false;
    uint32_t changedAt = 0;
};

#endif
