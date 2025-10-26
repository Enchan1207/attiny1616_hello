#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/delay.h>

#define UART_TX_PIN 2  // PB2
#define UART_RX_PIN 3  // PB3
#define BAUD_RATE 115200
#define USART_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

static const char* tx_data = NULL;
static volatile bool tx_empty = true;

PORT_t* const port = &PORTA;
const uint8_t pin = 7;

// USART0データエンプティ割込み
ISR(USART0_DRE_vect) {
    if (*tx_data != '\0') {
        USART0.TXDATAL = *tx_data++;
    } else {
        tx_empty = true;
        USART0.CTRLA &= ~USART_DREIE_bm;
    }
}

void uart_init(void) {
    cli();

    PORTB.DIRSET = (1 << UART_TX_PIN);
    USART0.BAUD = (uint16_t)USART_BAUD_RATE(BAUD_RATE);
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
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
