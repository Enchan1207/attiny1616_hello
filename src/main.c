#include <avr/io.h>
#include <util/delay.h>

PORT_t* const port = &PORTA;
const uint8_t pin = 7;

int main() {
    CCP = 0xD8;
    CLKCTRL_MCLKCTRLB &= ~(1);

    port->DIR |= (1 << pin);
    port->OUT &= ~(1 << pin);

    while (1) {
        port->OUT ^= (1 << pin);
        _delay_ms(1000);
    }
}
