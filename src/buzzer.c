#include "buzzer.h"
#include "helpers.h"
#include "uptime.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t sound_end_time = 0;
volatile uint8_t is_sound_playing = 0;

ISR(TIMER1_COMPA_vect) {
    if (is_sound_playing) {
        if (uptime_ms() >= sound_end_time) {
            is_sound_playing = 0;
            PORTD &= ~(1 << BUZZER_BIT); // LOW
            TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10)); // stop clock timer
        } else {
            PORTD ^= (1 << BUZZER_BIT); // genereaza sunetul
        }
    }
}

void buzzer_init(void) {
    // iesire buzzer
    DDRD |= (1 << BUZZER_BIT);
    
    // timer1
    TCCR1A = 0;
    TCCR1B = (1 << WGM12);
    TIMSK1 |= (1 << OCIE1A);
}

void play_tone(uint16_t freq, uint16_t duration) {
    if (freq == 0) {
        return;
    }

    // OCR1A = (F_CPU / (2 * Prescaler * Frecventa)) - 1
    OCR1A = (uint16_t)((F_CPU / (2UL * 8UL * freq)) - 1);

    // momentul de timp cand sunetul se opreste
    sound_end_time = uptime_ms() + duration;
    is_sound_playing = 1;

    // start timer1 cu prescaler 8
    TCCR1B |= (1 << CS11);
    TCCR1B &= ~((1 << CS12) | (1 << CS10));
}

void sound_play_start(void) {
    play_tone(1000, 150);
    _delay_ms(160);
    play_tone(1250, 200);
}

void sound_play_step(void) {
    play_tone(600, 30);
}

void sound_play_error(void) {
    play_tone(90, 400);
}

void sound_play_win(void) {
    play_tone(523, 100); _delay_ms(110); // Do
    play_tone(659, 100); _delay_ms(110); // Mi
    play_tone(784, 100); _delay_ms(110); // Sol
    play_tone(1046, 300);                // Do sus
}