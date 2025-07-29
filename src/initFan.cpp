void initFan() {
    #ifdef ENV_BLUEPILL
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9);
    GPIOB->CRH |=  (GPIO_CRH_MODE9_1); // Output - PP - 2MHz
    GPIOB->BSRR = GPIO_BSRR_BR9; // Set PB9 to low
    #endif
    #ifdef ENV_STM32F030K6T6
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~(3 << GPIO_MODER_MODER7_Pos);   // Clear mode bits
    GPIOB->MODER |=  (1 << GPIO_MODER_MODER7_Pos);   // Output mode
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_7);    // Push-pull
    GPIOB->OSPEEDR &= ~(3 << GPIO_OSPEEDR_OSPEEDR7_Pos);
    GPIOB->OSPEEDR |=  (1 << GPIO_OSPEEDR_OSPEEDR7_Pos); // Medium speed
    GPIOB->PUPDR &= ~(3 << GPIO_PUPDR_PUPDR7_Pos); // No pull-up, pull-down
    GPIOB->BRR = GPIO_BRR_BR_7;
    #endif
}