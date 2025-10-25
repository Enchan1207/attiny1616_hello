#include <avr/io.h>
#include <util/delay.h>

PORT_t* const port = &PORTA;
const uint8_t pin = 7;

int main() {
    CCP = CCP_IOREG_gc;
    CLKCTRL_MCLKCTRLB = CLKCTRL_PDIV_0_bm | CLKCTRL_PEN_bm;

    port->DIR |= (1 << pin);
    port->OUT &= ~(1 << pin);

    while (1) {
        port->OUT ^= (1 << pin);
        _delay_ms(1000);
    }
}
