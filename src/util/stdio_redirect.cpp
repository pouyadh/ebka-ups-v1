#include <unistd.h>
#include <stdio.h>
// Override _write syscall for printf support
#ifdef ENV_BLUEPILL
extern "C" {
    int _write(int file, char *ptr, int len) {
        for (int i = 0; i < len; i++) {
            // Wait until transmit data register is empty
            while (!(USART1->SR & USART_SR_TXE));
            USART1->DR = ptr[i];
        }
        return len;
    }
}
#endif
#ifdef ENV_STM32F030K6T6
extern "C" {
    int _write(int file, char *ptr, int len) {
        for (int i = 0; i < len; i++) {
            // Wait until transmit data register is empty
            while (!(USART1->ISR & USART_ISR_TXE));
            USART1->TDR = ptr[i];
        }
        return len;
    }
}
#endif