#include <avr/io.h>
#include <avr/interrupt.h>
#include "uptime.h"

#define LED_PIN PD5
#define SW_PIN  PD2

volatile uint32_t last_press_time = 0;

ISR(INT0_vect) {
    uint32_t current_time = uptime_ms();
    
    // Dacă au trecut mai mult de 200ms de la ultima apăsare corectă
    if (current_time - last_press_time > 200) {
        PORTD ^= (1 << LED_PIN); // Schimbă starea LED-ului
        last_press_time = current_time;
    }
}

int main(void) {
    uptime_init();     // Inițializăm ceasul sistemului (Timer 2)
    
    // Configurare LED
    DDRD |= (1 << LED_PIN);
    
    // Configurare Buton (INT0 pe PD2)
    DDRD &= ~(1 << SW_PIN);
    PORTD |= (1 << SW_PIN); // Pull-up activat
    
    // Configurare Intrerupere Externă
    EICRA |= (1 << ISC01); // Falling edge
    EIMSK |= (1 << INT0);  // Activare INT0
    
    sei(); // Activare întreruperi globale

    while (1) {
        // Bucla e goală, totul e gestionat prin întreruperi și timere!
    }
    return 0;
}