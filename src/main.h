#pragma once
#include <unistd.h>
#include <stdio.h>
#include "PID.h"

#define FW_VER "1.0.0"
extern volatile uint16_t adc_readings[5];
extern float inputVoltage,outputCurrent,outputVoltage,vdd,outputWattage,outputCurrentAdjust;
extern float averageInputVoltage,averageOutputVoltage,averageOutputCurrent,averageOutputWattage,averageOutputCurrentAdjust;
extern bool isOuputVoltageUpdated;
extern volatile uint32_t msTicks;
extern volatile uint32_t sTicks;
extern volatile uint32_t ACInTicks;

void initSystemClock();
void initUSART();
void initPWMs();
void initADC();
void initSysTick(void);
void initFan();
void initBypassRelay();
void initBatteryLowLED();
void initBuzzer();
void initChargeLED();
void initACIn();
void initDSEqualization();
void initPBStopStart();

void calculateFeedbacks(bool calibrate = false);
void delay(uint32_t ms);

bool getDSEqualization();
bool getPBStopStart();

void setFan(bool en);
void setBypassRelay(bool en);
void setBatteryLowLED(bool en);
void setBuzzer(bool en);
void setChargeLED(bool en);