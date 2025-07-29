volatile uint32_t msTicks = 0;  // 1 ms tick counter
volatile uint32_t sTicks = 0; // 1 sec tick counter
volatile uint32_t ms4sTicks = 0;
volatile uint32_t ACInTicks = 0;

extern "C" void SysTick_Handler(void) {
    msTicks++;  // Called every 1ms
    ms4sTicks++;
    if (ms4sTicks >= 1000) {
        ms4sTicks = 0;
        sTicks++;
    }
    if (ACInTicks > 0) ACInTicks--;
}

void initSysTick(void) {
    SysTick->LOAD = (SystemCoreClock / 1000) - 1;  // 1 ms interval
    SysTick->VAL = 0;                              // Clear current value
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |   // Processor clock
                    SysTick_CTRL_TICKINT_Msk   |   // Enable SysTick interrupt
                    SysTick_CTRL_ENABLE_Msk;       // Enable SysTick
}
void delay(uint32_t ms) {
    uint32_t start = msTicks;
    while ((msTicks - start) < ms);
}
