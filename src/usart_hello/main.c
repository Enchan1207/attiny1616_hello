#include <avr/io.h>

int main() {
    CCP = CCP_IOREG_gc;
    CLKCTRL_MCLKCTRLB = CLKCTRL_PDIV_0_bm | CLKCTRL_PEN_bm;

    while (1) {
    }
}
