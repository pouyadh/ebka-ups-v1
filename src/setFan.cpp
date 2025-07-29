void setFan(bool en) {
    #ifdef ENV_BLUEPILL
    if (en) {
        GPIOB->BSRR = GPIO_BSRR_BS9; // Set PB9 to high
    } else {
        GPIOB->BSRR = GPIO_BSRR_BR9; // Set PB9 to low
    }
    #endif
    #ifdef ENV_STM32F030K6T6
    if (en) {
        GPIOB->BSRR = GPIO_BSRR_BS_7; // Set PB7 to high
    } else {
        GPIOB->BSRR = GPIO_BSRR_BR_7; // Set PB7 to low
    }
    #endif
}