#include "main.h"
#include "util/ANSI_CONSOLE.h"
#include <math.h>
#include "util/GPIO.hpp"
#include "util/Blinker.hpp"
PID mainPID(1.0f,0.2f,0.5f,0,3800,false,"MAIN",false);

typedef enum main_state_e {
    STATE_NOBATTERY = 0,
    STATE_DETECTING_BATTERY = 1,
    STATE_BULK_ABSORPTION = 2,
    STATE_FLOAT = 3,
    STATE_EQUALIZATION = 4,
    STATE_BATTERY_USAGE = 5,
    STATE_BATTERY_LOW = 6,
} main_state_t;
main_state_t mainState;

typedef enum charging_state_e {
    CHARGING_STATE_NOT_CHARGING = 0,
    CHARGING_STATE_BATTERY_DETECTION = 1,
    CHARGING_STATE_BULK_ABSORPTION = 2,
    CHARGING_STATE_FLOAT = 3,
    CHARGING_STATE_EQUALIZATION = 4,
} charging_state_t;
charging_state_t chargingState;





float V_out = 10.5f;
float I_out = 3.0f;
float P_out = 100.0f;
bool outputEnable = false;
float batteryAmper = 7;
uint8_t batteryCellCount = 0;

void updatePWMs() {
    float vMargin = V_out - outputVoltage;
    float iMargin = I_out - outputCurrent;
    float wMargin = P_out - outputWattage;
    float minMargin = vMargin;
    if (iMargin < minMargin) minMargin = iMargin;
    if (wMargin < minMargin) minMargin = wMargin;
    static uint32_t conv = 0;
    if (minMargin == vMargin) {
        conv = mainPID.update(V_out,outputVoltage);
    } else if (minMargin == iMargin) {
        conv = mainPID.update(I_out,outputCurrent);
    } else if (minMargin == wMargin) {
        conv = mainPID.update(P_out,outputWattage);
    }

    if (outputEnable) {
        if (conv <= 1900) {
            TIM1->CCR1 = conv;
            TIM3->CCR1 = 0;
        } else {
            TIM1->CCR1 = 1900;
            TIM3->CCR1 = conv - 1900;
        }
    } else {
        TIM1->CCR1 = 0;
        TIM3->CCR1 = 0;
        mainPID.reset();
        mainPID.value = 0;
    }
}

void detectBatteryCellCount() {
    outputEnable = false;
    updatePWMs();
    for(int i=0; i<10000;i++) calculateFeedbacks();
    //if (averageOutputVoltage <= 0)
}

void indicateWaitingForPower() {
    static bool state = false;
    static uint32_t lastChange = msTicks;
    if (state == false && msTicks - lastChange > 1000) {
        setChargeLED(true);
        setBuzzer(true);    
        lastChange = msTicks;    
        state = !state;
    } else if (state == true && msTicks - lastChange > 100) {
        setChargeLED(false);
        setBuzzer(false);
        lastChange = msTicks;    
        state = !state;
    }
}
void indicateWaitingForBattery() {
    static bool state = false;
    static uint32_t lastChange = msTicks;
    if (state == false && msTicks - lastChange > 5000) {
        setBatteryLowLED(true);
        setBuzzer(true);
        lastChange = msTicks;  
        state = !state;  
    } else if (state == true && msTicks - lastChange > 20) {
        setBatteryLowLED(false);
        setBuzzer(false);
        lastChange = msTicks;  
        state = !state;  
    }
}

void delay2(uint32_t ms) {
    for(int i=0;i<ms;i++){
        calculateFeedbacks();
        delay(1);
    }
}

bool isBatteryConnected() {
    return averageOutputVoltage > 1;
}
bool isPowerConnected() {
    return (ACInTicks > 0); 
}
bool isInputVoltageValid() {
    return (averageInputVoltage > 10 && averageInputVoltage < 65);
}
bool isBatteryVoltageValid() {
    return (averageOutputVoltage > 5 && averageOutputVoltage < 65);
}
uint8_t getBatteryCellCount() {
    const float vb = averageOutputVoltage;
    if (vb >= 1 * 6 && vb <= 2.5 * 6) return 6;
    if (vb >= 1 * 12 && vb <= 2.5 * 12) return 12;
    if (vb >= 1 * 18 && vb <= 2.5 * 18) return 18;
    if (vb >= 1 * 24 && vb <= 2.5 * 24) return 24;
    return 255;
}
float lastPowerVoltage = 0.0f;

#ifdef ENV_BLUEPILL
auto bypassRelay = GPIO(GPIOB,12).output().pushpull().mediumSpeed();
auto buzzer = GPIO(GPIOB,13).output().pushpull().mediumSpeed();
auto ledCharge = GPIO(GPIOB,14).output().pushpull().mediumSpeed();
auto ledBatteryLow = GPIO(GPIOB,15).output().pushpull().mediumSpeed();
auto pbStartStop = GPIO(GPIOB,4).input().pulldown();
auto dswEqualization = GPIO(GPIOB,7).input().pullup();
auto ac = GPIO(GPIOB,8).output().pushpull().mediumSpeed();
auto fan = GPIO(GPIOB,9).output().pushpull().mediumSpeed();
#endif
#ifdef ENV_STM32F030K6T6
auto bypassRelay = GPIO(GPIOA,11).output().pushpull().mediumSpeed();
auto buzzer = GPIO(GPIOA,12).output().pushpull().mediumSpeed();
auto ledCharge = GPIO(GPIOA,15).output().pushpull().mediumSpeed();
auto ledBatteryLow = GPIO(GPIOB,3).output().pushpull().mediumSpeed();
auto pbStartStop = GPIO(GPIOB,4).input().pulldown();
auto dswEqualization = GPIO(GPIOB,5).input().pullup();
auto ac = GPIO(GPIOB,6).output().pushpull().mediumSpeed();
auto fan = GPIO(GPIOB,7).output().pushpull().mediumSpeed();
#endif

int main(void) {
    initSystemClock();
    initSysTick();
    initUSART();
    initADC();
    initPWMs();
    initFan();
    initBypassRelay();
    initACIn();
    initBatteryLowLED();
    initBuzzer();
    initChargeLED();
    initDSEqualization();
    initPBStopStart();


    delay2(1000);
    setBuzzer(true);delay(100);setBuzzer(false); delay(1000);
    
    Blinker chargingBlinkDelays(&ledCharge);
    chargingBlinkDelays.setDelays(new BlinkDelays((const uint32_t []){10,1000},2));
    setFan(true);

    while (1)
    {

        static uint32_t lastmsTicks = msTicks;
        if (lastmsTicks  != msTicks) {
            lastmsTicks = msTicks;
            chargingBlinkDelays.msLoop();
        }
    }
    

    // while (1)
    // {

    //     bool isCharging = isBatteryConnected() && isPowerConnected();
    //     if (!isCharging) chargingStage = STAGE_BATTERY_DETECTION;
    
    //     if (isPowerConnected() && !isBatteryConnected()) {
    //         setBypassRelay(false);
    //         setBatteryLowLED(true);delay(10);setBatteryLowLED(false); delay(1000);
    //     } else if (isBatteryConnected() && !isPowerConnected()) {
    //         // if (powerRefrenceVoltage) {
    //         //     if (isBatteryMatchesThePower(powerRefrenceVoltage)) {
    //         //         setBatteryLowLED(false);
    //         //         if (!isBatteryLow()) {
    //         //             setBypassRelay(true);
    //         //         } else {
    //         //             setBypassRelay(false);
    //         //             setBatteryLowLED(true);
    //         //         }
    //         //     } else {
    //         //         setBypassRelay(false);
    //         //     }
    //         // } else {
    //         //     setBypassRelay(false);
    //         //     setBuzzer(true); delay2(10); setBuzzer(false); delay2(1000);
    //         // }
    //     } else if (isPowerConnected() && isBatteryConnected()) {
    //         setBypassRelay(false);
    //         setChargeLED(true);
    //     } else {
    //         setBypassRelay(false);
    //         setBuzzer(true); setChargeLED(true); delay2(3000); 
    //         setBuzzer(false); setChargeLED(false); delay2(1000);
    //     }



    //     static uint32_t chargingLEDChangeTick;
    //     switch (chargingState)
    //     {
    //     case CHARGING_STATE_NOT_CHARGING:
    //         setChargeLED(false);
    //         break;
    //     case CHARGING_STATE_BATTERY_DETECTION:
    //         if (msTicks - chargingLEDChangeTick > 10) 
    //         break;
    //     case CHARGING_STATE_BULK_ABSORPTION:
    //         break;
    //     case CHARGING_STATE_FLOAT:
    //         break;
    //     case CHARGING_STATE_EQUALIZATION:
    //         break;
    //     default:
    //         break;
    //     }

    // }

    

    

    // while (1)
    // {
    //     if (ACInTicks == 0 && averageOutputVoltage > 10) {
    //         //  Battery
    //         while(ACInTicks == 0 || averageInputVoltage < 10) {
    //             setChargeLED(true); delay2(100); setChargeLED(false); delay2(100);
    //             setChargeLED(true); delay2(100); setChargeLED(false); delay2(2000);
    //         }
    //     } else if (ACInTicks > 0 && averageOutputVoltage < 10) {
    //         //  AC
    //         indicateWaitingForBattery();
    //     } else if (ACInTicks > 0 && averageOutputVoltage > 10) {
    //         //  AC and Battery
    //         setBuzzer(true);delay(100);setBuzzer(false); delay(100);
    //         setBuzzer(true);delay(2000);setBuzzer(false); delay(100);
    //     }
    //     printf("%05.2f,%05.2f,%05.2f,%05.2f,",inputVoltage,outputVoltage,outputCurrent,outputWattage);
    //     printf("%05.2f,%05.2f,%05.2f,%05.2f,%u,%u,%05.2f,%05.2f,%05.2f\r\n",averageInputVoltage,averageOutputVoltage,averageOutputCurrent,averageOutputWattage,mainState,ACInTicks > 0 ? 1 : 0,outputCurrentAdjust,averageOutputCurrentAdjust,vdd);
    // }
    
    

    // //setBypassRelay(true);delay(1000);setBypassRelay(false);delay(1000);
    // //setBatteryLowLED(true);delay(1000);setBatteryLowLED(false);delay(1000);
    // //setBuzzer(true);delay(1000);setBuzzer(false);delay(1000);
    // //setChargeLED(true);delay(1000);setChargeLED(false);delay(1000);
    // //setFan(true);delay(1000);setFan(false);delay(1000);

    // while (1)
    // {
    //     calculateFeedbacks();
    //     outputEnable = false;
    //     updatePWMs();
    //     printf("%05.2f,%05.2f,%05.2f,%05.2f,",inputVoltage,outputVoltage,outputCurrent,outputWattage);
    //     printf("%05.2f,%05.2f,%05.2f,%05.2f,%u,%u,%05.2f,%05.2f,%05.2f\r\n",averageInputVoltage,averageOutputVoltage,averageOutputCurrent,averageOutputWattage,mainState,ACInTicks > 0 ? 1 : 0,outputCurrentAdjust,averageOutputCurrentAdjust,vdd);

    //     setBatteryLowLED(getDSEqualization());
    //     setChargeLED(getPBStopStart());
    // }
    

    // outputEnable = false;
    // updatePWMs();

    // delay(5000);
    // mainState = STATE_NOBATTERY;
    // if (ACInTicks == 0) mainState = STATE_BATTERY_USAGE;
    // //ansi::clear_screen();
    // //ansi::cursor_hide();
    // // printf("Startup ...\r\nVersion:" FW_VER "\r\n");
    // delay(5000);
    // while (1) {
    //     calculateFeedbacks();
    //     // ansi::cursor_move_to();
    //     // printf("%sIV:%05.2f OV:%05.2f OC:%05.2f OW:%05.2f VDD:%05.2f%s\r\n",ansi::color::YELLOW, 
    //     //    inputVoltage,outputVoltage,outputCurrent,outputWattage,vdd,ansi::color::RESET);
    //     static uint32_t tStart = sTicks; 
    //     switch (mainState)
    //     {
    //     case STATE_NOBATTERY: 
    //         outputEnable = false;
            
    //         if (averageOutputVoltage < 2) tStart = sTicks;
    //         if (sTicks-tStart > 10) {
    //             tStart = sTicks; 
    //             mainState = STATE_DETECTING_BATTERY;
    //         }
    //         break;
    //     case STATE_DETECTING_BATTERY: 
    //         outputEnable = false;
    //         if (averageOutputVoltage < 2) mainState = STATE_NOBATTERY;
    //         if (sTicks-tStart > 10) {
    //             if (averageOutputVoltage < 15.0f) {
    //                 batteryCellCount = 6;
    //             } else if (averageOutputVoltage < 30.0f) {
    //                 batteryCellCount = 12;
    //             } else if (averageOutputVoltage < 45.0f) {
    //                 batteryCellCount = 18;
    //             } else if (averageOutputVoltage < 60.0f) {
    //                 batteryCellCount = 24;
    //             } else {
    //                 batteryCellCount = 0;
    //             }
    //             mainState = STATE_BULK_ABSORPTION;
    //         }
    //         break;
    //     case STATE_BULK_ABSORPTION: 
    //         V_out = batteryCellCount * 2.6;
    //         I_out = batteryAmper / 10;
    //         outputEnable = true;
    //         if (averageOutputVoltage >= batteryCellCount * 2.6 && averageOutputCurrent <= batteryAmper / 50) {
    //             mainState = STATE_FLOAT;
    //         }
    //         break;
    //     case STATE_FLOAT: 
    //         V_out = batteryCellCount * 2.25;
    //         I_out = batteryAmper / 10;
    //         outputEnable = true;
    //         break;
    //     case STATE_EQUALIZATION: 
    //         break;
    //     case STATE_BATTERY_USAGE: 
    //         break;
    //     case STATE_BATTERY_LOW:
    //         break;
        
    //     default:
    //         break;
    //     }
        
        
    //     updatePWMs();

    //     printf("%05.2f,%05.2f,%05.2f,%05.2f,",inputVoltage,outputVoltage,outputCurrent,outputWattage);
    //     printf("%05.2f,%05.2f,%05.2f,%05.2f,%u,%u\r\n",averageInputVoltage,averageOutputVoltage,averageOutputCurrent,averageOutputWattage,mainState,ACInTicks > 0 ? 1 : 0);
    //     //ansi::clear_line_from_cursor();
    // }
}

void HardFault_Handler(void) {
    // Optional: Blink LED, log error, reset MCU, etc.
    while (1);  // Stay here to catch the fault
}