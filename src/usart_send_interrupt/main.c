#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <util/delay.h>

#define UART_TX_PIN 2  // PB2
#define BAUD_RATE 115200
#define USART_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

static const char* tx_data = NULL;
static volatile bool tx_empty = true;

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
    USART0.CTRLB = USART_TXEN_bm;
}

void uart_send(const char* const data) {
    while (!tx_empty);

    tx_data = data;
    tx_empty = false;
    USART0.CTRLA |= USART_DREIE_bm;
}

int main() {
    CCP = CCP_IOREG_gc;
    CLKCTRL_MCLKCTRLB &= ~CLKCTRL_PEN_bm;

    uart_init();

    sei();

    while (1) {
        uart_send("Hello, World!\r\n");
        uart_send("interrupt!!\r\n");

        _delay_ms(1000);
    }
}
