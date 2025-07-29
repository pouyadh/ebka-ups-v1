#pragma once

class GPIO {
    GPIO_TypeDef *_GPIO;
    uint8_t pin, pinT2;
    bool _active = true;
    bool isOutput = false;

public:
    GPIO(GPIO_TypeDef *_GPIO, uint8_t pin, bool _active = true) : _GPIO(_GPIO), pin(pin), _active(_active) {
        pinT2 = pin * 2;
        #ifdef ENV_STM32F030K6T6
        if (_GPIO == GPIOA) RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
        if (_GPIO == GPIOB) RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
        if (_GPIO == GPIOC) RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
        #endif
        #ifdef ENV_STM32F103C8T6
        if (_GPIO == GPIOA) RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
        if (_GPIO == GPIOB) RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
        if (_GPIO == GPIOC) RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
        RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
        #endif
    }

    #ifdef ENV_STM32F030K6T6
    GPIO& input() { _GPIO->MODER &= ~(3 << pinT2); isOutput = false; return *this; }
    GPIO& output() { _GPIO->MODER &= ~(3 << pinT2); _GPIO->MODER |= (1 << pinT2); isOutput = true; return *this; }
    GPIO& af() { _GPIO->MODER &= ~(3 << pinT2); _GPIO->MODER |= (2 << pinT2); return *this; }
    GPIO& analog() { _GPIO->MODER |= (3 << pinT2); return *this; }
    GPIO& pushpull() { _GPIO->OTYPER &= ~(1 << pin); return *this; }
    GPIO& opendrain() { _GPIO->OTYPER |= (1 << pin); return *this; }
    GPIO& lowSpeed() { _GPIO->OSPEEDR &= ~(3 << pinT2); return *this; }
    GPIO& mediumSpeed() { _GPIO->OSPEEDR &= ~(3 << pinT2); _GPIO->OSPEEDR |= (1 << pinT2); return *this; }
    GPIO& highSpeed() { _GPIO->OSPEEDR &= ~(3 << pinT2); _GPIO->OSPEEDR |= (3 << pinT2); return *this; }
    GPIO& nopullupdown() { _GPIO->PUPDR &= ~(3 << pinT2); return *this; }
    GPIO& pullup() { _GPIO->PUPDR &= ~(3 << pinT2); _GPIO->PUPDR |= (1 << pinT2); return *this; }
    GPIO& pulldown() { _GPIO->PUPDR &= ~(3 << pinT2); _GPIO->PUPDR |= (2 << pinT2); return *this; }
    #endif

    #ifdef ENV_STM32F103C8T6
    GPIO& input() { setCR(4); isOutput = false; return *this; }
    GPIO& output() { setCR(3); isOutput = true; return *this; }
    GPIO& af() { setCR(11); return *this; }
    GPIO& analog() { setCR(0); return *this; }
    GPIO& pushpull() { uint32_t cr = getCR(); setCR((cr & 12)); return *this; }
    GPIO& opendrain() { uint32_t cr = getCR(); setCR((cr & 12) | 4); return *this; }
    GPIO& lowSpeed() { setSpeed(2); return *this; }      // 2 MHz
    GPIO& mediumSpeed() { setSpeed(1); return *this; }   // 10 MHz
    GPIO& highSpeed() { setSpeed(3); return *this; }     // 50 MHz
    GPIO& nopullupdown() { return input(); }
    GPIO& pullup() { _GPIO->ODR |= (1 << pin); setCR(8); return *this; }
    GPIO& pulldown() { _GPIO->ODR &= ~(1 << pin); setCR(8); return *this; }

    void setCR(uint8_t config) { volatile uint32_t *CR = (pin < 8) ? &_GPIO->CRL : &_GPIO->CRH; uint8_t shift = (pin % 8) * 4; *CR &= ~(0xF << shift); *CR |= (config << shift); }
    uint8_t getCR() { volatile uint32_t *CR = (pin < 8) ? &_GPIO->CRL : &_GPIO->CRH; uint8_t shift = (pin % 8) * 4; return (*CR >> shift) & 0xF; }
    void setSpeed(uint8_t modeBits) { volatile uint32_t *CR = (pin < 8) ? &_GPIO->CRL : &_GPIO->CRH; uint8_t shift = (pin % 8) * 4; *CR &= ~(3 << shift); *CR |= (modeBits << shift); }
    #endif

    void high() { _GPIO->BSRR = (1 << pin); }
    void low() {
        #ifdef ENV_STM32F030K6T6
        _GPIO->BSRR = (1 << (pin + 16));
        #else
        _GPIO->BRR = (1 << pin);
        #endif
    }
    void toggle() { _GPIO->ODR ^= (1 << pin); }
    bool getValue() { return isOutput ? (_GPIO->ODR & (1 << pin)) > 0 : (_GPIO->IDR & (1 << pin)) > 0; }
    void setValue(bool _high) { _high ? high() : low(); }
    void set(bool active) { active ? setValue(_active) : setValue(!_active); }
    bool get() { return _active ? getValue() : !getValue(); }
};
