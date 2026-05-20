#include "helpers.h"
#include "matrix_functions.h"

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
uint8_t get_led_index(uint8_t row, uint8_t col) {
    return (row * 8) + col;
}

int8_t drop_piece(uint8_t col, uint8_t player) {
    int8_t final_row = -1;

    // unde se opreste piesa
    for (int8_t row = 7; row >= 0; row--) {
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
        _delay_ms(60); 
        if (row < final_row) {
            matrix[row][col] = 0;
        }
    }

    return final_row;
}

void update_cursor(uint8_t *cursor) {
    uint16_t joy_x = analog_read(JOY_X_BIT); 

    if (joy_x < 200) {  // st
        if (*cursor > 0) {
            (*cursor)--;
        }
        _delay_ms(200);
    } 
    else if (joy_x > 800) { // dr
        if (*cursor < 7) {
            (*cursor)++;
        }
        _delay_ms(200);
    }
}

void draw_cursor(uint8_t cursor, uint8_t player) {
    // curatare rand sus
    for (uint8_t c = 0; c < 8; c++) {
        if (matrix[0][c] == player) {
            matrix[0][c] = 0; 
        }
    }
    
    if (matrix[0][cursor] == 0) {
        matrix[0][cursor] = player;
    }

    show_board();
}

void drop_pressed(uint8_t *cursor, uint8_t *player) {
    // verif daca SW e apasat
    if (!(PIND & (1 << SW_BIT))) {
        _delay_ms(50); // debounce
        if (!(PIND & (1 << SW_BIT))) {
            matrix[0][*cursor] = 0;

            int8_t landed_row = drop_piece(*cursor, *player);

            //  schimba jucatorul
            if (landed_row != -1) {
                if (*player == 1) {
                    *player = 2;
                } else {
                    *player = 1;
                }
            }

            // blocheaza cat timp jucatorul tine apasat SW
            while (!(PIND & (1 << SW_BIT))) {
                show_board();
            }
        }
    }
}