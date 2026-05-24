#include "helpers.h"
#include "matrix_functions.h"

extern uint32_t score_player1;
extern uint32_t score_player2;

// LOGICA MATRICE LED
void __attribute__((noinline)) ws2812_send_byte(uint8_t dat) {
    uint8_t ctr;
    asm volatile (
        "    ldi %1, 8  \n"
        "loop%=: \n"
        "    sbi %0, %2 \n"
        "    sbrs %3, 7 \n"
        "    rjmp bit0%= \n"    
        "    nop \n" "nop \n" "nop \n" "nop \n" "nop \n" "nop \n" "nop \n" "nop \n" 
        "    cbi %0, %2 \n"
        "    rjmp end%= \n"     
        "bit0%=: \n"
        "    nop \n" 
        "    cbi %0, %2 \n"
        "    nop \n" "nop \n" "nop \n" "nop \n"
        "end%=: \n"
        "    lsl %3 \n"
        "    dec %1 \n"
        "    brne loop%= \n"     
        : : "I" (_SFR_IO_ADDR(DIGITAL_PORT)), "r" (ctr), "I" (MATRIX_BIT), "r" (dat)
    );
}

void send_pixel(uint8_t green, uint8_t red, uint8_t blue) {
    ws2812_send_byte(green); // G
    ws2812_send_byte(red); // R
    ws2812_send_byte(blue); // B
}

void show_board() {
    uint8_t led_buffer[64][3] = {0};

    for (uint8_t r = 0; r < 8; r++) {
        for (uint8_t c = 0; c < 8; c++) {
            uint8_t idx = get_led_index(r, c);
            
            if (matrix[r][c] == 1) { // jucator 1
                led_buffer[idx][0] = 0;
                led_buffer[idx][1] = 20;
                led_buffer[idx][2] = 0;
            } else if (matrix[r][c] == 2) { // jucator 2
                led_buffer[idx][0] = 20;
                led_buffer[idx][1] = 0;
                led_buffer[idx][2] = 0;
            }
        }
    }

    // oprire intreruperi pt a trimite datele la matrice
    cli();
    for (uint8_t i = 0; i < 64; i++) {
        send_pixel(led_buffer[i][0], led_buffer[i][1], led_buffer[i][2]);
    }
    sei();

    // latch
    _delay_us(300);
}

// LOGICA JOC

static uint32_t last_joystick_move = 0;

uint8_t get_led_index(uint8_t row, uint8_t col) {
    return (row * 8) + col;
}

int8_t drop_piece(uint8_t col, uint8_t player) {
    int8_t final_row = -1;

    // unde se opreste piesa
    for (int8_t row = 7; row > 0; row--) { // nu iau si prima linie (aia e pt cursor)
        if (matrix[row][col] == 0) {
            final_row = row;
            break;
        }
    }

    if (final_row == -1) { // coloana plina
        return -1; 
    }

    // drop-ul efectiv
    for (int8_t row = 0; row <= final_row; row++) {
        matrix[row][col] = player;
        show_board();
        sound_play_step();
        _delay_ms(60); 
        if (row < final_row) {
            matrix[row][col] = 0;
        }
    }

    return final_row;
}

void update_cursor(uint8_t *cursor) {
    uint32_t current_time = uptime_ms();
    
    if (current_time - last_joystick_move < 200) {
        return; 
    }

    uint16_t joy_x = analog_read(JOY_X_BIT);

    if (joy_x < 200) { // st
        if (*cursor > 0) {
            (*cursor)--;
            last_joystick_move = current_time;
        }
    } 
    else if (joy_x > 800) { // dr
        if (*cursor < 7) {
            (*cursor)++;
            last_joystick_move = current_time;
        }
    }
}

void draw_cursor(uint8_t cursor, uint8_t player) {
    // curatare rand sus
    for (uint8_t c = 0; c < 8; c++) {
        if (matrix[0][c] == player) {
            matrix[0][c] = 0; 
        }
    }
    
    if ((uptime_ms() / 400) % 2 == 0) {
        if (matrix[0][cursor] == 0) {
            matrix[0][cursor] = player;
        }
    }

    show_board();
}

void victory_animation(uint8_t winner) {
    static uint8_t victory_buffer[64][3];
    uint8_t center_line = (uptime_ms() / 80) % 8;

    for (uint8_t r = 0; r < 8; r++) {
        uint8_t intensity = 0;

        // calculez distanta simetrica
        int8_t diff = abs(r - center_line);
        
        if (diff > 4) {
            diff = 8 - diff;
        }
        
        uint8_t dist = (uint8_t)diff;
        // calcul intensitate lumina
        if (dist == 0) {
            intensity = 90;
        } 
        else if (dist == 1) {
            intensity = 45;
        } 
        else if (dist == 2) {
            intensity = 15;
        } 
        else if (dist == 3) {
            intensity = 7;
        } 
        else {
            intensity = 2;
        }

        // Îpun intensitatea pe toata linia
        for (uint8_t c = 0; c < 8; c++) {
            uint8_t idx = get_led_index(r, c);

            if (winner == 1) { // val rosu
                victory_buffer[idx][0] = 0;
                victory_buffer[idx][1] = intensity;
                victory_buffer[idx][2] = 0;
            } else if (winner == 2) { // val verde
                victory_buffer[idx][0] = intensity;
                victory_buffer[idx][1] = 0;
                victory_buffer[idx][2] = 0;
            }
        }
    }

    cli();
    for (uint16_t i = 0; i < 64; i++) {
        send_pixel(victory_buffer[i][0], victory_buffer[i][1], victory_buffer[i][2]);
    }
    sei();

    _delay_us(300); // latch
}

uint8_t drop_pressed(uint8_t *cursor, uint8_t *player) {
    if (button_pressed_flag == 1) { // verif daca SW e apasat
        matrix[0][*cursor] = 0;
        
        int8_t landed_row = drop_piece(*cursor, *player);

        if (landed_row == -1) {
            sound_play_error();
            button_pressed_flag = 0;
            return 1;
        }

        // verif daca exista winner
        uint8_t winner = check_winner();
        if (winner != 0) {
            button_pressed_flag = 0; 
            _delay_ms(200);

            // actualizare scor
            if (winner == 1) {
                score_player1++;
            } else if (winner == 2) {
                score_player2++;
            }

            display_score();

            if (score_player1 == 5 || score_player2 == 5) {
                game_final(winner);
            }

            // bucla animatie castig
            while (button_pressed_flag == 0) {
                sound_play_win();
                victory_animation(winner);
                _delay_ms(15); 
            }

            // incepe runda noua
            return 1; 
        }

        if (*player == 1) {
            *player = 2;
        }
        else {
            *player = 1;
        }

        // resetare timep miscare
        turn_start_time = uptime_ms();

        // asteptam eliberarea SW
        while (!(PIND & (1 << SW_BIT))) {
            show_board();
        }

        // reset flag pt noua apasare
        button_pressed_flag = 0;
    }

    return 0;
}

uint8_t check_winner() {
    for (uint8_t r = 0; r < 8; r++) {
        for (uint8_t c = 0; c < 8; c++) {
            uint8_t player = matrix[r][c];
            
            if (player == 0) {
                continue;
            }

            // verif spre dr
            if (c <= 4) {
                if (matrix[r][c+1] == player && matrix[r][c+2] == player && matrix[r][c+3] == player) {
                    return player;
                }
            }

            // verif in jos
            if (r <= 4) {
                if (matrix[r+1][c] == player && matrix[r+2][c] == player && matrix[r+3][c] == player) {
                    return player;
                }
            }

            // verif diag principala jos
            if (r <= 4 && c <= 4) {
                if (matrix[r+1][c+1] == player && matrix[r+2][c+2] == player && matrix[r+3][c+3] == player) {
                    return player;
                }
            }

            // verif diag secundara jos
            if (r <= 4 && c >= 3) {
                if (matrix[r+1][c-1] == player && matrix[r+2][c-2] == player && matrix[r+3][c-3] == player) {
                    return player;
                }
            }
        }
    }
    
    return 0;
}