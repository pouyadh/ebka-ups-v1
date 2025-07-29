void setBuzzer(bool en) {
    #ifdef ENV_BLUEPILL
    if (en) {
        GPIOB->BSRR = GPIO_BSRR_BS13; // Set PB13 to high
    } else {
        GPIOB->BSRR = GPIO_BSRR_BR13; // Set PB13 to low
    }
    #endif
    #ifdef ENV_STM32F030K6T6
    if (en) {
        GPIOA->BSRR = GPIO_BSRR_BS_12; // Set PA12 to high
    } else {
        GPIOA->BSRR = GPIO_BSRR_BR_12; // Set PA12 to low
    }
    #endif
}