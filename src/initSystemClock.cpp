void initSystemClock() {
    #ifdef ENV_BLUEPILL
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_1;
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
    RCC->CFGR |= RCC_CFGR_PLLMULL12;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
    SystemCoreClockUpdate();
    #endif
    #ifdef ENV_STM32F030K6T6
    uint32_t timeout;

    // 1. Try HSE
    RCC->CR |= RCC_CR_HSEON;
    timeout = 100000;
    while (!(RCC->CR & RCC_CR_HSERDY) && --timeout);  // wait for HSE

    if (RCC->CR & RCC_CR_HSERDY) {
        // HSE is ready
        RCC->CFGR = RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLMUL6; // 8 MHz / 1 * 6 = 48 MHz
    } else {
        // HSE failed, fallback to HSI
        RCC->CR |= RCC_CR_HSION;
        while (!(RCC->CR & RCC_CR_HSIRDY)); // wait for HSI ready
        RCC->CFGR = RCC_CFGR_PLLSRC_HSI_DIV2 | RCC_CFGR_PLLMUL12; // 8 MHz / 2 * 12 = 48 MHz
    }

    // 2. Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));  // Wait for PLL ready

    // 3. Switch SYSCLK to PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Wait for switch

    // 6. Update SystemCoreClock variable
    SystemCoreClockUpdate();
    #endif
}