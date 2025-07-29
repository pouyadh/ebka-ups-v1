void initChargeLED() {
    #ifdef ENV_BLUEPILL
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH &= ~(GPIO_CRH_CNF14 | GPIO_CRH_MODE14);
    GPIOB->CRH |=  (GPIO_CRH_MODE14_1); // Output - PP - 2MHz
    GPIOB->BSRR = GPIO_BSRR_BR14; // Set PB14 to low
    #endif
    #ifdef ENV_STM32F030K6T6
    // 1. Enable GPIOA clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // 2. Set PA15 as General Purpose Output (MODER = 01)
    GPIOA->MODER &= ~(3 << GPIO_MODER_MODER15_Pos);   // Clear mode bits
    GPIOA->MODER |=  (1 << GPIO_MODER_MODER15_Pos);   // Set to output mode

    // 3. Set output type to Push-Pull
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_15);    // Push-pull (0)

    // 4. Set speed to medium (optional — 2 MHz is roughly low/medium)
    GPIOA->OSPEEDR &= ~(3 << GPIO_OSPEEDR_OSPEEDR15_Pos);
    GPIOA->OSPEEDR |=  (1 << GPIO_OSPEEDR_OSPEEDR15_Pos); // Medium speed

    // 5. No pull-up/pull-down
    GPIOA->PUPDR &= ~(3 << GPIO_PUPDR_PUPDR15_Pos);    // No pull

    // 6. Set PA15 low
    GPIOA->BRR = GPIO_BRR_BR_15;
    #endif
}