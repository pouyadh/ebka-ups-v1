#pragma once
#include "GPIO.hpp"
struct BlinkDelays {
    public:
    const uint32_t *delayArrey;
    uint32_t len;
    BlinkDelays(const uint32_t *delayArrey,uint32_t len) : delayArrey(delayArrey) , len(len) {};
};


class Blinker {
    private:
    GPIO *gpio;
    BlinkDelays *blinkDelays = nullptr;
    uint32_t index;
    uint32_t tick;
    public:
    Blinker(GPIO *gpio) : gpio(gpio) {};
    void msLoop() {
        if (blinkDelays) {
            if (++tick >= blinkDelays->delayArrey[index]) {
                index++;
                tick = 0;
                if (index > blinkDelays->len-1) index = 0;
                gpio->set(index%2 == 0);
            }
        } else {
            gpio->set(false);
        }
    };
    void reset() {
        index = 0;
        tick = 0;
        gpio->set(blinkDelays?true:false);
    }
    void setDelays(BlinkDelays *delays) {
        if (blinkDelays != delays) {
            blinkDelays = delays;
            reset();
        }
    }
    void clear() {
        blinkDelays = nullptr;
        reset();
    }
};