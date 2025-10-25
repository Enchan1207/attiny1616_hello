#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define UART_TX_PIN 2  // PB2
#define BAUD_RATE 115200
#define USART_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

void uart_init(void) {
    cli();

    PORTB.DIRSET = (1 << UART_TX_PIN);
    USART0.BAUD = (uint16_t)USART_BAUD_RATE(BAUD_RATE);
    USART0.CTRLB = USART_TXEN_bm;
}

void uart_send(uint8_t data) {
    while (!(USART0.STATUS & USART_DREIF_bm));
    USART0.TXDATAL = data;
}

int main() {
    CCP = CCP_IOREG_gc;
    CLKCTRL_MCLKCTRLB &= ~CLKCTRL_PEN_bm;

    uart_init();

    while (1) {
        uart_send('A');
        _delay_ms(1000);
    }
}
