#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// port-uri digitale
#define DIGITAL_PORT PORTD
#define SW_BIT PD2
#define BUZZER_BIT PD3
#define LED1_BIT PD4
#define LED2_BIT PD5
#define MATRIX_BIT PD6

// porturi analog
#define ANALOG_PORT PORTC
#define JOY_X_BIT PC0

extern uint8_t matrix[8][8];

// configuratii port-uri iesire/intrare
void config_init();
// ecran de start
void show_start_screen();
// apasare buton SW si incepere joc
void start_game();
// aprindere/stingere red-uri playeri
void update_player_leds(uint8_t player);