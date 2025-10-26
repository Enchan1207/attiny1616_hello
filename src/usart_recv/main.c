#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/delay.h>

#define BAUD_RATE 115200
#define USART_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

PORT_t* const port = &PORTA;
const uint8_t pin = 7;

void uart_init(void) {
    cli();

    USART0.BAUD = (uint16_t)USART_BAUD_RATE(BAUD_RATE);
    USART0.CTRLB = USART_RXEN_bm;
}

uint8_t uart_recv() {
    while (!(USART0.STATUS & USART_RXCIF_bm));
    return USART0.RXDATAL;
}

int main() {
    CCP = CCP_IOREG_gc;
    CLKCTRL_MCLKCTRLB &= ~CLKCTRL_PEN_bm;

    port->DIR |= (1 << pin);
    port->OUT &= ~(1 << pin);

    uart_init();

    while (1) {
        uint8_t data = uart_recv();

        switch (data) {
            case '0':
                port->OUTCLR = 1 << pin;
                break;

            case '1':
                port->OUTSET = 1 << pin;
                break;

            default:
                break;
        }
    }
}
