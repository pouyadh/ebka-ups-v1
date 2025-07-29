void initBatteryLowLED() {
    #ifdef ENV_BLUEPILL
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH &= ~(GPIO_CRH_CNF15 | GPIO_CRH_MODE15);
    GPIOB->CRH |=  (GPIO_CRH_MODE15_1); // Output - PP - 2MHz
    GPIOB->BSRR = GPIO_BSRR_BR15; // Set PB15 to low
    #endif
    #ifdef ENV_STM32F030K6T6
    // 1. Enable GPIOB clock
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // 2. Configure PB3 as General Purpose Output (MODER = 01)
    GPIOB->MODER &= ~(3 << GPIO_MODER_MODER3_Pos);   // Clear mode bits
    GPIOB->MODER |=  (1 << GPIO_MODER_MODER3_Pos);   // Output mode

    // 3. Set output type to Push-Pull
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_3);    // Push-pull

    // 4. Set output speed to medium (optional)
    GPIOB->OSPEEDR &= ~(3 << GPIO_OSPEEDR_OSPEEDR3_Pos);
    GPIOB->OSPEEDR |=  (1 << GPIO_OSPEEDR_OSPEEDR3_Pos); // Medium speed

    // 5. Disable pull-up/pull-down
    GPIOB->PUPDR &= ~(3 << GPIO_PUPDR_PUPDR3_Pos);    // No pull-up/down

    // 6. Set PB3 low
    GPIOB->BRR = GPIO_BRR_BR_3;

    #endif
}