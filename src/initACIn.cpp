extern volatile uint32_t ACInTicks;
void initACIn() {
    #ifdef ENV_BLUEPILL
    // 1. Enable GPIOB and AFIO clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

    // 2. Configure PB8 as input with pull-up
    GPIOB->CRH &= ~GPIO_CRH_MODE8;    // MODE8 = 00 (input)
    GPIOB->CRH &= ~GPIO_CRH_CNF8;
    GPIOB->CRH |=  GPIO_CRH_CNF8_0;   // CNF8 = 10 (input pull-up/down)
    //GPIOB->ODR &=  ~GPIO_ODR_ODR8;     // Pull-down

    // 3. Map EXTI8 to PB8
    AFIO->EXTICR[2] &= ~AFIO_EXTICR3_EXTI8;       // Clear
    AFIO->EXTICR[2] |=  AFIO_EXTICR3_EXTI8_PB;    // Map PB8 to EXTI8

    // 4. Configure EXTI line 8 for rising edge
    EXTI->IMR  |= EXTI_IMR_MR8;      // Unmask EXTI8
    EXTI->RTSR |= EXTI_RTSR_TR8;     // ✅ Rising edge
    EXTI->FTSR &= ~EXTI_FTSR_TR8;    // ❌ Falling edge (disable)

    // 5. Enable interrupt in NVIC
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    #endif
    #ifdef ENV_STM32F030K6T6
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; // Enable GPIOB
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN; // Enable SYSCFG

    GPIOB->MODER &= ~(GPIO_MODER_MODER6); // Input mode
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR6);

    SYSCFG->EXTICR[1] &= ~(SYSCFG_EXTICR2_EXTI6);
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI6_PB;

    EXTI->IMR |= EXTI_IMR_IM6;
    EXTI->RTSR |= EXTI_RTSR_TR6;
    EXTI->FTSR &= ~EXTI_FTSR_TR6;

    NVIC_EnableIRQ(EXTI4_15_IRQn);
    #endif
}

extern "C" void EXTI4_15_IRQHandler(void) {
    #ifdef ENV_BLUEPILL
    if (EXTI->PR & EXTI_PR_PR8) {
        EXTI->PR |= EXTI_PR_PR8;  // Clear interrupt pending bit
        ACInTicks = 200;
    }
    #endif
    #ifdef ENV_STM32F030K6T6
    if (EXTI->PR & EXTI_PR_PR6) {
        EXTI->PR = EXTI_PR_PR6;
        ACInTicks = 200;
    }
    #endif
}