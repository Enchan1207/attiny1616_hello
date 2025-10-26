#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/delay.h>

#define BAUD_RATE 115200
#define USART_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

static volatile uint8_t data = 0x00;
static volatile bool rx_available = false;

PORT_t* const port = &PORTA;
const uint8_t pin = 7;

// USART0受信完了割込み
ISR(USART0_RXC_vect) {
    data = USART0.RXDATAL;
    rx_available = true;
}

void uart_init(void) {
    cli();

    USART0.BAUD = (uint16_t)USART_BAUD_RATE(BAUD_RATE);
    USART0.CTRLB = USART_RXEN_bm;
    USART0.CTRLA |= USART_RXCIE_bm;
}

uint8_t uart_recv() {
    while (!rx_available);
    rx_available = false;
    return data;
}

int main() {
    CCP = CCP_IOREG_gc;
    CLKCTRL_MCLKCTRLB &= ~CLKCTRL_PEN_bm;

    port->DIR |= (1 << pin);
    port->OUT &= ~(1 << pin);

    uart_init();

    sei();

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
