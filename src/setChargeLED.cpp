void setChargeLED(bool en) {
    #ifdef ENV_BLUEPILL
    if (en) {
        GPIOB->BSRR = GPIO_BSRR_BS14; // Set PB14 to high
    } else {
        GPIOB->BSRR = GPIO_BSRR_BR14; // Set PB14 to low
    }
    #endif
    #ifdef ENV_STM32F030K6T6
    if (en) {
        GPIOA->BSRR = GPIO_BSRR_BS_15; // Set PA15 to high
    } else {
        GPIOA->BSRR = GPIO_BSRR_BR_15; // Set PA15 to low
    }
    #endif
}