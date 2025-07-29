void initPBStopStart() {
    #ifdef ENV_BLUEPILL
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRL &= ~(GPIO_CRL_CNF4 | GPIO_CRL_MODE4);
    GPIOB->CRL |= (2 << GPIO_CRL_CNF4_Pos); // CNF = 10 (PU/PD input), MODE = 00
    GPIOB->ODR |= GPIO_ODR_ODR4; // pull-up
    #endif
    #ifdef ENV_STM32F030K6T6
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN; //Enable GPIOB clock
    GPIOB->MODER &= ~(3 << GPIO_MODER_MODER4_Pos); //Input mode
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR4);
    GPIOB->PUPDR |=  (1 << GPIO_PUPDR_PUPDR4_Pos); // 10 = pull-down
    #endif
}