#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

// initializare timer1
void buzzer_init(void);

// functie pt a canta un tune
void play_tone(uint16_t freq, uint16_t duration);
// ding dublu la start joc
void sound_play_start(void);
// sunet la caderea pieselor
void sound_play_step(void);
// sunet de eroare (coloana plina)
void sound_play_error(void);
// sunet de victorie
void sound_play_win(void);

#endif // BUZZER_H