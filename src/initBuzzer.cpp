void initBuzzer() {
    #ifdef ENV_BLUEPILL
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    GPIOB->CRH |=  (GPIO_CRH_MODE13_1); // Output - PP - 2MHz
    GPIOB->BSRR = GPIO_BSRR_BR13; // Set PB13 to low
    #endif
    #ifdef ENV_STM32F030K6T6
    // 1. Enable GPIOA clock
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // 2. Configure PA12 as General Purpose Output (MODER = 01)
    GPIOA->MODER &= ~(3 << GPIO_MODER_MODER12_Pos);   // Clear mode bits
    GPIOA->MODER |=  (1 << GPIO_MODER_MODER12_Pos);   // Set to output mode

    // 3. Set output type to Push-Pull
    GPIOA->OTYPER &= ~(1 << GPIO_OTYPER_OT_12);    // Push-pull (0)

    // 4. Set output speed to medium (optional, roughly matches 2 MHz)
    GPIOA->OSPEEDR &= ~(3 << GPIO_OSPEEDR_OSPEEDR12_Pos);
    GPIOA->OSPEEDR |=  (1 << GPIO_OSPEEDR_OSPEEDR12_Pos); // Medium speed

    // 5. Disable pull-up/pull-down
    GPIOA->PUPDR &= ~(3 << GPIO_PUPDR_PUPDR12_Pos);    // No pull

    // 6. Set PA12 low
    GPIOA->BRR = GPIO_BRR_BR_12;
    #endif
}