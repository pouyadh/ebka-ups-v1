#define NOP() __NOP()
#define NOP10() NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();

void initPWMs() {
    #ifdef ENV_BLUEPILL
    // Enable PA, PB, AF, TIM1 and TIM3 Clocks
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_TIM1EN ; 
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // PA8 -> 2MHz AF PP (H)
    GPIOA->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_MODE8);
    GPIOA->CRH |=  (GPIO_CRH_CNF8_1 | GPIO_CRH_MODE8_1);
    // PA6 -> 2MHz AF PP (L)
    GPIOA->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_MODE6);
    GPIOA->CRL |=  (GPIO_CRL_CNF6_1 | GPIO_CRL_MODE6_1);

    TIM1->PSC = 0;
    TIM1->ARR = 1919;     
    TIM1->CCR1 = 0;   //H  
    TIM1->CNT = 0;
    TIM1->RCR = 0;
    TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M);
    TIM1->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->EGR = TIM_EGR_UG;
    TIM1->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
    TIM1->CR1 |= TIM_CR1_ARPE;     // Auto-reload preload enable
    TIM1->CCER |=  TIM_CCER_CC1E;

    TIM3->PSC = 0;
    TIM3->ARR = 1919;
    TIM3->CCR1 = 0;
    TIM3->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM3->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos); // PWM mode 1
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;   // Preload enable
    TIM3->CR1 |= TIM_CR1_ARPE;     // Auto-reload preload enable
    TIM3->EGR |= TIM_EGR_UG;       // Update event
    TIM3->CCER |=  TIM_CCER_CC1E;

    TIM1->CR1 = TIM_CR1_CEN;
    NOP10();NOP10();
    TIM3->CR1 |= TIM_CR1_CEN;      // Start timer
    #endif
    #ifdef ENV_STM32F030K6T6
    // 1. Enable clocks: GPIOA, TIM1, TIM3
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // 2. Configure PA8 as AF output push-pull, 2 MHz (TIM1_CH1)
    GPIOA->MODER &= ~(3 << (8 * 2));
    GPIOA->MODER |=  (2 << (8 * 2));       // Alternate function mode (10)
    GPIOA->OTYPER &= ~(1 << 8);            // Push-pull
    GPIOA->OSPEEDR &= ~(3 << (8 * 2));
    GPIOA->OSPEEDR |=  (1 << (8 * 2));     // Medium speed (~2 MHz)
    GPIOA->AFR[1] &= ~(0xF << ((8 - 8)*4));
    GPIOA->AFR[1] |=  (2 << ((8 - 8)*4));  // AF2 = TIM1 on PA8

    // 3. Configure PA6 as AF output push-pull, 2 MHz (TIM3_CH1)
    GPIOA->MODER &= ~(3 << (6 * 2));
    GPIOA->MODER |=  (2 << (6 * 2));       // Alternate function mode
    GPIOA->OTYPER &= ~(1 << 6);
    GPIOA->OSPEEDR &= ~(3 << (6 * 2));
    GPIOA->OSPEEDR |=  (1 << (6 * 2));     // Medium speed
    GPIOA->AFR[0] &= ~(0xF << (6 * 4));
    GPIOA->AFR[0] |=  (1 << (6 * 4));      // AF1 = TIM3 on PA6

    // 4. TIM1 setup
    TIM1->PSC = 0;
    TIM1->ARR = 1919;
    TIM1->CCR1 = 0;
    TIM1->CNT = 0;
    TIM1->RCR = 0;
    TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M);
    TIM1->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE; // PWM mode 1 + preload enable
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->EGR = TIM_EGR_UG;
    TIM1->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
    TIM1->CR1 |= TIM_CR1_ARPE;  // Auto-reload preload enable
    TIM1->CCER |= TIM_CCER_CC1E;

    // 5. TIM3 setup
    TIM3->PSC = 0;
    TIM3->ARR = 1919;
    TIM3->CCR1 = 0;
    TIM3->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM3->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE; // PWM mode 1 + preload enable
    TIM3->CR1 |= TIM_CR1_ARPE;
    TIM3->EGR |= TIM_EGR_UG;
    TIM3->CCER |= TIM_CCER_CC1E;

    // 6. Enable timers
    TIM1->CR1 |= TIM_CR1_CEN;
    NOP10();NOP10();
    TIM3->CR1 |= TIM_CR1_CEN;
    #endif
}