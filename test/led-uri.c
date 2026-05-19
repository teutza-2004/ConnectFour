#include <avr/io.h>
#include <util/delay.h>

// Pe Arduino Uno (ATmega328P):
// Pinul D4 este PD4 (Port D, Bit 4)
// Pinul D5 este PD5 (Port D, Bit 5)
#define LED_PORT PORTD
#define LED_DDR  DDRD
#define LED_RED    PD4
#define LED_GREEN  PD5

int main(void) {
    // Configuram pinii 4 si 5 din PORTD ca iesire
    LED_DDR |= (1 << LED_RED);
    LED_DDR |= (1 << LED_GREEN);

    while (1) {
        // Aprinde Rosu, stinge Verde
        LED_PORT |= (1 << LED_RED);
        LED_PORT &= ~(1 << LED_GREEN);
        _delay_ms(500);

        // Stinge Rosu, aprinde Verde
        LED_PORT &= ~(1 << LED_RED);
        LED_PORT |= (1 << LED_GREEN);
        _delay_ms(500);
    }

    return 0;
}