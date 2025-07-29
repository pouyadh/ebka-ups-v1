#ifndef PID_H
#define PID_H

#include <cstdint>

class PID {
public:
    PID(float kp, float ki, float kd,
           volatile float initialValue,
           float maxValue,
           bool inverted = false,
            const char *name="",bool debug=false,float acceptableError=0.0f);

    void reset();
    void enable();
    void disable();
    float update(float setpoint, float measured);
    void setDebug(bool enable);
    void setValue(float val);
    bool isOK = false;
    volatile float value;

private:
    float Kp, Ki, Kd;
    float integral;
    float prevError;
    float maxValue;
    bool invert;
    bool enabled;
    const char *name;
    bool debug = false;
    float acceptableError;
};

#endif