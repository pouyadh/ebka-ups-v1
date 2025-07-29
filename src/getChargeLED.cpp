bool getChargeLED() {
    return (GPIOA->ODR & GPIO_ODR_15) > 0;
}