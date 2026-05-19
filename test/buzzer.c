#include <avr/io.h>
#include <util/delay.h>

// Pe Arduino Uno, pinul D9 este PB1 (OC1A)
#define BUZZER_PIN PB1
#define BUZZER_DDR DDRB

void buzzer_init() {
    // Setăm pinul ca ieșire
    BUZZER_DDR |= (1 << BUZZER_PIN);

    // Configurare Timer 1 în mod CTC (modul 4)
    // WGM12 = 1 (restul biților WGM sunt 0)
    TCCR1A = 0; 
    TCCR1B = (1 << WGM12);

    // Setăm prescaler la 64
    // 16MHz / 64 = 250.000 unități pe secundă
    TCCR1B |= (1 << CS11) | (1 << CS10);
}

void play_tone(uint16_t frequency) {
    if (frequency == 0) {
        // Oprim sunetul: deconectăm pinul de la timer
        TCCR1A &= ~(1 << COM1A0);
        return;
    }

    // Calculăm valoarea pentru OCR1A conform formulei:
    // OCR1A = (F_CPU / (2 * Prescaler * Frecvență)) - 1
    // Pentru 16MHz și prescaler 64: OCR1A = (125.000 / frecvență) - 1
    uint16_t ocr_val = (125000 / frequency) - 1;
    OCR1A = ocr_val;

    // Conectăm pinul la timer în modul "Toggle on Compare Match"
    // Asta va genera o undă pătrată pe pinul PB1/D9
    TCCR1A |= (1 << COM1A0);
}

int main(void) {
    buzzer_init();

    while (1) {
        play_tone(440); // Nota La (440 Hz)
        _delay_ms(200);
        
        play_tone(0);   // Pauză
        _delay_ms(800);
    }
    return 0;
}