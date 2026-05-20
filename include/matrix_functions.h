#ifndef MATRIX_FUNCTIONS_H
#define MATRIX_FUNCTIONS_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define ARDUINO_PORT PORTD
#define SW_BIT PD2
#define BUZZER_BIT PD3
#define LED1_BIT PD4
#define LED2_BIT PD5
#define MATRIX_BIT PD6

extern uint8_t matrix[8][8];

// LOGICA MATRICE LED
// transmite byte spre matrice (ASM pt ca operatiile dureaza mai putine cicluri => nu se strica logica)
void __attribute__((noinline)) ws2812b_send_byte(uint8_t dat);
// wrapper pt a trimite un pixel intreg
void send_pixel(uint8_t green, uint8_t red, uint8_t blue);
// afisare matrice
void show_board();

// LOGICA JOC
// returneaza index-ul din matLED stiind indecsii logici
uint8_t get_led_index(uint8_t row, uint8_t col);
// returneaza linia din matrice unde ajunge piesa, -1 daca e plina coloana
int8_t drop_piece(uint8_t col, uint8_t player);

#endif