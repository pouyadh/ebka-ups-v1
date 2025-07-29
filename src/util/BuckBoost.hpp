#pragma once
#include "PID.h"
class BuckBoost{
    PID pid();
    float voltageLimit;
    float currentLimit;
    float wattageLimit;
    uint32_t hsLimit,lsLimit;
    public:
    BuckBoost(uint32_t hsLimit,uint32_t lsLimit): hsLimit(hsLimit),lsLimit(lsLimit) {};
    void setVoltageLimit(float v){voltageLimit = v;};
    void setCurrentLimit(float c){currentLimit =c;};
    void setWattageLimit(float w){wattageLimit =w;};
    void update(float outputVoltage,float outputCurrent) {

    };
}