#pragma once
#include <cstdint>
#include <cmath>

class PID {
    float Kp, Ki, Kd;
    float integral;
    float prevError;
    float maxValue;
    bool invert;
    bool enabled;
    const char *name;
    float acceptableError;
public:
    bool isOK = false;
    volatile float value;
    PID(float kp, float ki, float kd,
    volatile float initialValue,
    float maxValue,
    bool inverted = false,
    const char *name = "",
    float acceptableError = 0.0f)
    : Kp(kp), Ki(ki), Kd(kd),
      integral(0.0f),
      prevError(0.0f),
      maxValue(maxValue),
      invert(inverted),
      enabled(true),
      name(name),
      acceptableError(acceptableError),
      value(initialValue)
    {
        integral = 0.0f;
    }

    void reset(){integral = 0.0f; prevError = 0.0f;}
    void enable() {enabled = true;}
    void disable() {enabled = false;}
    void setValue(float val){ value = val; reset(); }
    float update(float setpoint, float measured) {
        if (!enabled) {
            value=0;
            return value;
        }
        if (std::isnan(integral)) {
            integral = 0.0f;
        }

        float error = setpoint - measured;
        if (invert) error = -error;

        integral += error;
        
        float derivative = error - prevError;

        float output = Kp * error + Ki * integral + Kd * derivative;
        prevError = error;
        if (error > 0) {
            isOK = error <= acceptableError;
        } else {
            isOK = error >= -acceptableError;
        }

        if (output < 0) output = 0;
        if (output > maxValue) output = maxValue;

        if (!isOK) value = output;
        return value;
    }
};