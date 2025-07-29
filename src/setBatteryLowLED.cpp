void setBatteryLowLED(bool en) {
    #ifdef ENV_BLUEPILL
    if (en) {
        GPIOB->BSRR = GPIO_BSRR_BS15; // Set PB15 to high
    } else {
        GPIOB->BSRR = GPIO_BSRR_BR15; // Set PB15 to low
    }
    #endif
    #ifdef ENV_STM32F030K6T6
    if (en) {
        GPIOB->BSRR = GPIO_BSRR_BS_3; // Set PB3 to high
    } else {
        GPIOB->BSRR = GPIO_BSRR_BR_3; // Set PB3 to low
    }
    #endif
}