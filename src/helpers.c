#include "helpers.h"

uint8_t matrix[8][8] = {0};
volatile uint32_t turn_start_time = 0;
volatile uint8_t button_pressed_flag = 0;
uint32_t score_player1 = 0;
uint32_t score_player2 = 0;

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

void update_player_leds(uint8_t current_player, uint32_t time_elapsed) {
    uint8_t should_blink_on = 1;

    if (time_elapsed >= 6000) {
        if ((uptime_ms() / 500) % 2 == 0) {
            should_blink_on = 1;
        } else {
            should_blink_on = 0;
        }
    }

    if (current_player == 1) {
        if (should_blink_on) {
            PORTD |= (1 << PD4);
        } else {
            PORTD &= ~(1 << PD4);
        }
        PORTD &= ~(1 << PD5);
    } 
    else if (current_player == 2) {
        if (should_blink_on) {
            PORTD |= (1 << PD5);
        } else {
            PORTD &= ~(1 << PD5);
        }
        PORTD &= ~(1 << PD4);
    }
}

void display_score() {
    USART0_print("===================================\r\n");
    
    // Afișăm scorul pentru Jucătorul 1
    USART0_print("  Jucator 1 (ROSU)  : ");
    USART0_print_u32(score_player1);
    USART0_print(" puncte\r\n");
    
    // Afișăm scorul pentru Jucătorul 2
    USART0_print("  Jucator 2 (VERDE) : ");
    USART0_print_u32(score_player2);
    USART0_print(" puncte\r\n");
}

void game_final(uint8_t winner) {
    USART0_print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
    if (score_player1 == 5) {
        USART0_print("  FELICITARI! JUCATORUL 1 A CASTIGAT MECIUL!\r\n");
    } else {
        USART0_print("  FELICITARI! JUCATORUL 2 A CASTIGAT MECIUL!\r\n");
    }
    USART0_print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
    USART0_print(" Resetati placuta pentru un joc nou.\r\n\r\n");

    sound_play_win();
    _delay_ms(200);
    sound_play_win();

    while (1) {
        victory_animation(winner);
        _delay_ms(15); 
    }
}