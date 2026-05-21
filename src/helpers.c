#include "helpers.h"

uint8_t matrix[8][8] = {0};

volatile uint8_t button_pressed_flag = 0;

ISR(INT0_vect) {
    if (!(PIND & (1 << SW_BIT))) {
        button_pressed_flag = 1;
    }
}

void config_init() {
    // iesire matrice
    DDRD |= (1 << MATRIX_BIT);
    
    // intrare SW + pullup
    DDRD &= ~(1 << SW_BIT);
    PORTD |= (1 << SW_BIT);

    // iesire led-uri
    DDRD |= (1 << LED1_BIT) | (1 << LED2_BIT);
    PORTD &= ~((1 << LED1_BIT) | (1 << LED2_BIT));

    // intrerupere
    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);
    EIMSK |= (1 << INT0);
}

void show_start_screen() {
    for (uint8_t r = 0; r < 8; r++) {
        for (uint8_t c = 0; c < 8; c++) {
            matrix[r][c] = 0;
        }
    }
    
    matrix[0][0] = 1; matrix[7][0] = 1;
    matrix[0][7] = 2; matrix[7][7] = 2;
    
    show_board();
}

void start_game() {
    while (button_pressed_flag == 0) {}

    sound_play_start();
    button_pressed_flag = 0;

    // curatare ecran 
    for (uint8_t r = 0; r < 8; r++) {
        for (uint8_t c = 0; c < 8; c++) {
            matrix[r][c] = 0;
        }
    }
}

void update_player_leds(uint8_t player) {
    if (player == 1) {
        PORTD |= (1 << LED1_BIT);
        PORTD &= ~(1 << LED2_BIT);
    } else if (player == 2) {
        PORTD |= (1 << LED2_BIT);
        PORTD &= ~(1 << LED1_BIT);
    }
}