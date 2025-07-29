#pragma once
#include "PID.hpp"
class BuckBoost{
    PID *pid;
    float voltageLimit = 0;
    float currentLimit = 0;
    float wattageLimit = 0;
    uint32_t hsLimit,lsLimit;
    uint32_t hsv,lsv;
    bool enabled = false;
    public:
    BuckBoost(uint32_t hsLimit,uint32_t lsLimit): hsLimit(hsLimit),lsLimit(lsLimit) {
        pid = new PID(1.0f,0.2f,0.5f,0,hsLimit + lsLimit);
    };
    void setVoltageLimit(float v){voltageLimit = v;};
    void setCurrentLimit(float c){currentLimit =c;};
    void setWattageLimit(float w){wattageLimit =w;};
    void update(float outputVoltage,float outputCurrent) {
        float outputWattage = outputVoltage * outputCurrent;
        float vMargin = voltageLimit - outputVoltage;
        float iMargin = currentLimit - outputCurrent;
        float wMargin = wattageLimit - outputWattage;
        float minMargin = vMargin;
        if (iMargin < minMargin) minMargin = iMargin;
        if (wMargin < minMargin) minMargin = wMargin;
        static uint32_t conv = 0;
        if (minMargin == vMargin) {
            conv = pid->update(voltageLimit,outputVoltage);
        } else if (minMargin == iMargin) {
            conv = pid->update(currentLimit,outputCurrent);
        } else if (minMargin == wMargin) {
            conv = pid->update(wattageLimit,outputWattage);
        }

        if (enabled) {
            if (conv <= hsLimit) {
                hsv = conv;
                lsv = 0;
            } else {
                hsv = hsLimit;
                lsv = conv - hsLimit;
            }
        } else {
            hsv = 0;
            lsv = 0;
            pid->reset();
            pid->value = 0;
        }
    };
    uint32_t getHsValue() { return enabled ? hsv : 0; };
    uint32_t getLsValue() { return enabled ? lsv : 0; };
    void enable() { enabled = true; };
    void disable() { enabled = false; };
};