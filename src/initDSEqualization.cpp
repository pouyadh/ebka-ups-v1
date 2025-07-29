void initDSEqualization() {
    #ifdef ENV_BLUEPILL
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_MODE7);
    GPIOB->CRL |= (2 << GPIO_CRL_CNF7_Pos); // CNF = 10 (PU/PD input), MODE = 00
    GPIOB->ODR &= ~GPIO_ODR_ODR7; // pull-down
    #endif
    #ifdef ENV_STM32F030K6T6
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; //Enable GPIOB clock
    GPIOB->MODER &= ~(3 << GPIO_MODER_MODER5_Pos);   // input mode
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR5);
    GPIOB->PUPDR |=  (2 << GPIO_PUPDR_PUPDR5_Pos); // pull-down
    #endif
}