#include "main.h"
#define NUM_CHANNELS 5
volatile uint16_t adc_readings[NUM_CHANNELS];

void initADC(void) {
    #ifdef ENV_BLUEPILL
    // Enable clocks
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN | RCC_APB2ENR_IOPAEN; // ADC1 + GPIOA
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // Configure PA0..PA3 as analog input (CNF=00, MODE=00)
    GPIOA->CRL &= ~(
        GPIO_CRL_MODE0 | GPIO_CRL_CNF0 |
        GPIO_CRL_MODE1 | GPIO_CRL_CNF1 |
        GPIO_CRL_MODE2 | GPIO_CRL_CNF2 |
        GPIO_CRL_MODE3 | GPIO_CRL_CNF3
    );

    // Configure DMA1 Channel1
    DMA1_Channel1->CCR = 0;                // Disable channel
    DMA1->IFCR = 0x0F;                    // Clear flags

    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;   // Peripheral address
    DMA1_Channel1->CMAR = (uint32_t)adc_readings;  // Memory buffer
    DMA1_Channel1->CNDTR = NUM_CHANNELS;

    DMA1_Channel1->CCR = DMA_CCR_MINC
                      | DMA_CCR_CIRC
                      | DMA_CCR_PSIZE_0
                      | DMA_CCR_MSIZE_0
                      | DMA_CCR_PL_0 | DMA_CCR_PL_1
                      | DMA_CCR_EN;

    // ADC prescaler PCLK2/6
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;

    ADC1->CR1 = ADC_CR1_SCAN;               // Scan mode
    ADC1->CR2 = ADC_CR2_DMA | ADC_CR2_CONT; // DMA + continuous conversion
    ADC1->CR2 |= ADC_CR2_TSVREFE;

    // Number of conversions = 6
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR1 |= (NUM_CHANNELS - 1) << 20;

    // Set sequence channels 0-5 and 7
    ADC1->SQR3 = (0 << 0)  | (1 << 5)  | (2 << 10) | (3 << 15);
    ADC1->SQR2 = (17 << 5);

    // Sample time 55.5 cycles for channels 0..5 and 7
    ADC1->SMPR2 = 0;
    ADC1->SMPR2 |= (ADC_SMPR2_SMP0_2 | ADC_SMPR2_SMP0_1 | ADC_SMPR2_SMP0_0);
    ADC1->SMPR2 |= (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP1_1 | ADC_SMPR2_SMP1_0);
    ADC1->SMPR2 |= (ADC_SMPR2_SMP2_2 | ADC_SMPR2_SMP2_1 | ADC_SMPR2_SMP2_0);
    ADC1->SMPR2 |= (ADC_SMPR2_SMP3_2 | ADC_SMPR2_SMP3_1 | ADC_SMPR2_SMP3_0);
    ADC1->SMPR1 |= (ADC_SMPR1_SMP17_2 | ADC_SMPR1_SMP17_1 | ADC_SMPR1_SMP17_0); // 239.5 cycles

    // Enable ADC
    ADC1->CR2 |= ADC_CR2_ADON;

    // Calibration
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    while (ADC1->CR2 & ADC_CR2_RSTCAL);
    ADC1->CR2 |= ADC_CR2_CAL;
    while (ADC1->CR2 & ADC_CR2_CAL);

    // Start conversion
    ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG | ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2;
    #endif 
    #ifdef ENV_STM32F030K6T6
    // 1. Enable Clocks
    RCC->AHBENR  |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_DMA1EN;
    RCC->APB2ENR |= RCC_APB2ENR_ADCEN;

    // 2. Configure PA0–PA3 as analog
    GPIOA->MODER |= (3 << GPIO_MODER_MODER0_Pos) |
                    (3 << GPIO_MODER_MODER1_Pos) |
                    (3 << GPIO_MODER_MODER2_Pos) |
                    (3 << GPIO_MODER_MODER3_Pos);  // Analog mode (MODER = 11)

    // 3. Enable VRefInt channel (must also be enabled in ADC_CCR)
    ADC->CCR |= ADC_CCR_VREFEN;

    // 4. Configure DMA for ADC1
    DMA1_Channel1->CCR = 0; // Disable before config
    DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;
    DMA1_Channel1->CMAR = (uint32_t)adc_readings;
    DMA1_Channel1->CNDTR = NUM_CHANNELS;

    DMA1_Channel1->CCR = DMA_CCR_MINC      // Memory increment
                    | DMA_CCR_CIRC      // Circular mode
                    | DMA_CCR_MSIZE_0   // Memory size = 16 bits
                    | DMA_CCR_PSIZE_0   // Peripheral size = 16 bits
                    | DMA_CCR_PL_1      // High priority
                    | DMA_CCR_EN;       // Enable

    // 5. Calibrate ADC
    ADC1->CR &= ~ADC_CR_ADEN;              // Ensure ADC is disabled
    if (ADC1->CR & ADC_CR_ADEN) {
        ADC1->CR |= ADC_CR_ADDIS;
        while (ADC1->CR & ADC_CR_ADEN);
    }
    ADC1->CR |= ADC_CR_ADCAL;              // Start calibration
    while (ADC1->CR & ADC_CR_ADCAL);       // Wait

    // 6. Configure ADC
    ADC1->CFGR1 = ADC_CFGR1_DMAEN         // Enable DMA
                | ADC_CFGR1_DMACFG        // Circular DMA
                | ADC_CFGR1_CONT          // Continuous mode
                | ADC_CFGR1_SCANDIR       // Scan sequence from CH0 up
                ; // No external trigger

    ADC1->CHSELR = ADC_CHSELR_CHSEL0   // PA0 = ADC_IN0
                | ADC_CHSELR_CHSEL1   // PA1 = ADC_IN1
                | ADC_CHSELR_CHSEL2   // PA2 = ADC_IN2
                | ADC_CHSELR_CHSEL3   // PA3 = ADC_IN3
                | ADC_CHSELR_CHSEL17; // VRefInt = ADC_IN17

    // 7. Set sampling time (all channels same)
    ADC1->SMPR |= ADC_SMPR_SMP_2 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_0; // 239.5 cycles

    // 8. Enable ADC
    ADC1->CR |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait until ready

    // 9. Start ADC
    ADC1->CR |= ADC_CR_ADSTART;
    #endif
}