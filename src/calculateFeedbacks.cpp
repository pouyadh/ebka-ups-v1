#include "main.h"
#include "util/adc2voltage.h"
#include "util/resistorDividerVin.h"
#include "util/shantCurrent.h"
#include "PID.h"
#include <math.h>

float inputVoltage,outputCurrent,outputVoltage,vdd,outputWattage,outputCurrentAdjust;
float averageInputVoltage,averageOutputVoltage,averageOutputCurrent,averageOutputWattage,averageOutputCurrentAdjust;
uint32_t averageCounter = 0;
const uint32_t averageSampleCount = 200;
//PID stableInputVoltagePID(0.001f,0.01f,0.001f,0,100,false,"SIV",false,0.1f);
//PID stableOutputVoltagePID(0.001f,0.01f,0.001f,0,100,false,"SOV",false,0.1f);
//PID stableOutputCurrentPID(0.00001f,0.01f,0.00001f,0,100,false,"SOC",false,0.1f);
//PID stableOutputWattagePID(0.001f,0.01f,0.001f,0,500,false,"SOW",false,0.1f);
void calculateFeedbacks(bool calibrate) {
    
    
    #ifdef ENV_BLUEPILL
    //vdd = 1.2f * 4096.0f / adc_readings[4];
    vdd = 3.263f;
    inputVoltage = resistorDividerVin(adc2voltage(adc_readings[0],vdd), 10000, 470);
    outputCurrent = (adc2voltage(adc_readings[1],vdd)/0.030f)/10;
    outputVoltage = resistorDividerVin(adc2voltage(adc_readings[2],vdd), 10000, 470);
    outputCurrentAdjust = adc2voltage(adc_readings[3],vdd) * 10 / 3.3;
    #endif
    #ifdef ENV_STM32F030K6T6
    // SCANDIR has no effect !!
    //vdd = 1.2f * 4096.0f / adc_readings[0];
    vdd = 3.263f;
    inputVoltage = resistorDividerVin(adc2voltage(adc_readings[4],vdd), 10000, 470);
    outputCurrent = (adc2voltage(adc_readings[3],vdd)/0.030f)/10;
    outputVoltage = resistorDividerVin(adc2voltage(adc_readings[2],vdd), 10000, 470);
    outputCurrentAdjust = adc2voltage(adc_readings[1],vdd) * 10 / 3.3;
    #endif
    outputWattage = outputVoltage * outputCurrent;

    #define ivsmpc 10
    static float sampleInputVoltageBuffer[ivsmpc];
    static uint32_t sampleInputVoltageBufferIndex = 0;
    if (sampleInputVoltageBufferIndex == 0) {
        sampleInputVoltageBuffer[0] = inputVoltage;
        sampleInputVoltageBufferIndex++;
    } else {
        if (abs(sampleInputVoltageBuffer[sampleInputVoltageBufferIndex-1]-inputVoltage) <= 0.1f) {
            sampleInputVoltageBuffer[sampleInputVoltageBufferIndex] = inputVoltage;
            sampleInputVoltageBufferIndex++;
            if (sampleInputVoltageBufferIndex == ivsmpc) {
                averageInputVoltage = 0;
                for (int i =0; i<ivsmpc;i++) averageInputVoltage+= sampleInputVoltageBuffer[i];
                averageInputVoltage = round(averageInputVoltage / ivsmpc * 10) / 10.0f;
                sampleInputVoltageBufferIndex = 0;
            }
        } else {
            sampleInputVoltageBufferIndex = 0;
        }
    }


    #define ovsmpc 10
    static float sampleOutputVoltageBuffer[ovsmpc];
    static uint32_t sampleOutputVoltageBufferIndex = 0;
    if (sampleOutputVoltageBufferIndex == 0) {
        sampleOutputVoltageBuffer[0] = outputVoltage;
        sampleOutputVoltageBufferIndex++;
    } else {
        if (abs(sampleOutputVoltageBuffer[sampleOutputVoltageBufferIndex-1]-outputVoltage) <= 0.5f) {
            sampleOutputVoltageBuffer[sampleOutputVoltageBufferIndex] = outputVoltage;
            sampleOutputVoltageBufferIndex++;
            if (sampleOutputVoltageBufferIndex == ovsmpc) {
                averageOutputVoltage = 0;
                for (int i =0; i<ovsmpc;i++) averageOutputVoltage+= sampleOutputVoltageBuffer[i];
                averageOutputVoltage = round(averageOutputVoltage / ovsmpc * 10) / 10.0f;
                sampleOutputVoltageBufferIndex = 0;
            }
        } else {
            sampleOutputVoltageBufferIndex = 0;
        }
    }

    #define ocsmpc 100
    static float sampleOutputCurrentBuffer[ocsmpc];
    static uint32_t sampleOutputCurrentBufferIndex = 0;
    if (sampleOutputCurrentBufferIndex == 0) {
        sampleOutputCurrentBuffer[0] = outputCurrent;
        sampleOutputCurrentBufferIndex++;
    } else {
        if (abs(sampleOutputCurrentBuffer[sampleOutputCurrentBufferIndex-1]-outputCurrent) <= 0.1f) {
            sampleOutputCurrentBuffer[sampleOutputCurrentBufferIndex] = outputCurrent;
            sampleOutputCurrentBufferIndex++;
            if (sampleOutputCurrentBufferIndex == ocsmpc) {
                averageOutputCurrent = 0;
                for (int i =0; i<ocsmpc;i++) averageOutputCurrent+= sampleOutputCurrentBuffer[i];
                averageOutputCurrent = round(averageOutputCurrent / ocsmpc * 10) / 10.0f;
                sampleOutputCurrentBufferIndex = 0;
            }
        } else {
            sampleOutputCurrentBufferIndex = 0;
        }
    }
    averageOutputWattage = averageOutputVoltage * averageOutputCurrent;

    #define ocasmpc 10
    static float sampleOutputCurrentAdjustBuffer[ocasmpc];
    static uint32_t sampleOutputCurrentAdjustBufferIndex = 0;
    if (sampleOutputCurrentAdjustBufferIndex == 0) {
        sampleOutputCurrentAdjustBuffer[0] = outputCurrentAdjust;
        sampleOutputCurrentAdjustBufferIndex++;
    } else {
        if (abs(sampleOutputCurrentAdjustBuffer[sampleOutputCurrentAdjustBufferIndex-1]-outputCurrentAdjust) <= 0.1f) {
            sampleOutputCurrentAdjustBuffer[sampleOutputCurrentAdjustBufferIndex] = outputCurrentAdjust;
            sampleOutputCurrentAdjustBufferIndex++;
            if (sampleOutputCurrentAdjustBufferIndex == ocasmpc) {
                averageOutputCurrentAdjust = 0;
                for (int i =0; i<ocasmpc;i++) averageOutputCurrentAdjust+= sampleOutputCurrentAdjustBuffer[i];
                averageOutputCurrentAdjust = round(averageOutputCurrentAdjust / ocasmpc * 10) / 10.0f;
                sampleOutputCurrentAdjustBufferIndex = 0;
            }
        } else {
            sampleOutputCurrentBufferIndex = 0;
        }
    }
}