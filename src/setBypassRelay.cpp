void setBypassRelay(bool en) {
    #ifdef ENV_BLUEPILL
    if (en) {
        GPIOB->BSRR = GPIO_BSRR_BS12; // Set PB12 to high
    } else {
        GPIOB->BSRR = GPIO_BSRR_BR12; // Set PB12 to low
    }
    #endif
    #ifdef ENV_STM32F030K6T6
    if (en) {
        GPIOA->BSRR = GPIO_BSRR_BS_11; // Set PA11 to high
    } else {
        GPIOA->BSRR = GPIO_BSRR_BR_11; // Set PA11 to low
    }
    #endif
}