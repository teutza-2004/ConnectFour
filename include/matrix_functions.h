#ifndef MATRIX_FUNCTIONS_H
#define MATRIX_FUNCTIONS_H

extern volatile uint8_t button_pressed_flag;

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
// logica miscare cursor (partea sus a matricei)
void update_cursor(uint8_t *cursor);
// desenare cursor pe matrice
void draw_cursor(uint8_t cursor, uint8_t player);
// este actionat buton-ul de drop a unei piese
void drop_pressed(uint8_t *cursor, uint8_t *player);

#endif