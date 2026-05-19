#include <avr/io.h>
#include <util/delay.h>
#include "adc.h" // Includem fisierul tau modularizat

#define LED_RED PD4

int main(void) {
    adc_init(); // Apelam functia ta din adc.c
    
    // Setam pinul PD4 (D4) ca iesire
    DDRD |= (1 << LED_RED);

    while (1) {
        // Citim canalul 0 (A0) folosind functia ta
        uint16_t val = analog_read(0);

        // Testam daca joystick-ul este miscat
        // Valoarea de repaus este ~512. 
        // Daca scade sub 300 sau creste peste 700, aprindem LED-ul.
        if (val < 300 || val > 700) {
            PORTD |= (1 << LED_RED);
        } else {
            PORTD &= ~(1 << LED_RED);
        }

        _delay_ms(50);
    }
    return 0;
}