#include <cstdio>  // for printf
#include "PID.h"
//#include "util/ANSI_CONSOLE.h"
#include <cmath>

PID::PID(float kp, float ki, float kd,
               volatile float initialValue,
               float maxValue,
               bool inverted,const char *name,bool debug,float acceptableError)
    : Kp(kp), Ki(ki), Kd(kd),
      integral(0.0f), prevError(0.0f),
      value(initialValue), maxValue(maxValue),
      invert(inverted), enabled(true),name(name),debug(debug),acceptableError(acceptableError) {
        integral = 0.0f;
      }

void PID::reset() {
    integral = 0.0f;
    prevError = 0.0f;
}

void PID::enable() {
    enabled = true;
}

void PID::disable() {
    enabled = false;
    value = 0;
}

void PID::setDebug(bool enable) {
    debug = enable;
}

void PID::setValue(float val) {
    value = val;
    reset();
}

float PID::update(float setpoint, float measured) {
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

    if (debug) {
        // printf("%s[%s] SP: %05.2f, PV: %05.2f, Err: %05.2f, Out: %05.2f\r\n",
        //        isOK?ansi::color::GREEN:ansi::color::WHITE,name,setpoint, measured, error, output);
    }
    return value;
}