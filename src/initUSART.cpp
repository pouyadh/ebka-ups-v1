void initUSART() {
    #ifdef ENV_BLUEPILL
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;
    // PA9 = USART1_TX alternate function push-pull, 50MHz output
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |=  (GPIO_CRH_MODE9_1 | GPIO_CRH_MODE9_0);
    GPIOA->CRH |=  GPIO_CRH_CNF9_1;
    // Baudrate = 250000 @ 48MHz: 48,000,000 / 250000 = 192 = 0x00C0
    USART1->BRR = 0x00C0;
    USART1->CR1 = USART_CR1_TE | USART_CR1_UE;
    #endif
    #ifdef ENV_STM32F030K6T6
    // Enable GPIOA and USART1 clocks
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;      // GPIOA clock
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  // USART1 clock

    // PA9 = USART1_TX: Alternate function push-pull output, 50 MHz
    // STM32F0 uses GPIOx->MODER and GPIOx->AFR registers (not CRH/CRL)
    GPIOA->MODER &= ~(GPIO_MODER_MODER9);        // Clear mode bits for PA9
    GPIOA->MODER |= (GPIO_MODER_MODER9_1);       // Alternate function mode (10b)

    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_9);        // Output push-pull
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9;    // High speed (11b)

    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR9);        // No pull-up/pull-down

    // Set alternate function 1 (AF1) for USART1 on PA9
    GPIOA->AFR[1] &= ~(0xF << ((9 - 8) * 4));    // Clear AF bits for pin 9
    GPIOA->AFR[1] |= (1 << ((9 - 8) * 4));       // AF1 = USART1

    // Baudrate calculation for 48 MHz clock and 250000 baud:
    // USARTDIV = 48,000,000 / 250,000 = 192 (0x00C0)
    USART1->BRR = 0x00C0;

    // Enable USART1 transmitter and USART
    USART1->CR1 = USART_CR1_TE | USART_CR1_UE;
    #endif 
}