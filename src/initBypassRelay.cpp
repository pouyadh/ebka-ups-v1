void initBypassRelay() {
    #ifdef ENV_BLUEPILL
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH &= ~(GPIO_CRH_CNF12 | GPIO_CRH_MODE12);
    GPIOB->CRH |=  (GPIO_CRH_MODE12_1); // Output - PP - 2MHz
    GPIOB->BSRR = GPIO_BSRR_BR12; // Set PB12 to low
    #endif
    #ifdef ENV_STM32F030K6T6
    // 1. Enable GPIOA clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // 2. Configure PA11 as General Purpose Output
    GPIOA->MODER &= ~(3 << GPIO_MODER_MODER11_Pos);   // Clear mode bits
    GPIOA->MODER |=  (1 << GPIO_MODER_MODER11_Pos);   // Set to output mode (01)

    // 3. Set output type to Push-Pull
    GPIOA->OTYPER &= ~(1 << GPIO_OTYPER_OT_11);    // Push-pull (0)

    // 4. Set output speed to medium (optional)
    GPIOA->OSPEEDR &= ~(3 << GPIO_OSPEEDR_OSPEEDR11_Pos);
    GPIOA->OSPEEDR |=  (1 << GPIO_OSPEEDR_OSPEEDR11_Pos); // Medium speed

    // 5. Disable pull-up/pull-down
    GPIOA->PUPDR &= ~(3 << GPIO_PUPDR_PUPDR11_Pos);    // No pull

    // 6. Set PA11 low
    GPIOA->BRR = GPIO_BRR_BR_11;
    #endif
}