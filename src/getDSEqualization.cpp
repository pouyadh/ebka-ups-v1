bool getDSEqualization() {
    #ifdef ENV_BLUEPILL
    return (GPIOB->IDR & GPIO_IDR_IDR7) > 0;
    #endif
    #ifdef ENV_STM32F030K6T6
    return (GPIOB->IDR & GPIO_IDR_5) > 0;
    #endif
}