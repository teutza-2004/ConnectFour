#include "helpers.h"
#include "matrix_functions.h"
#include "buzzer.h"

uint8_t cursor = 3;
uint8_t current_player = 1;

int main(void) {
    config_init();
    uptime_init();
    adc_init();
    buzzer_init();
    USART0_init(9600);

    // ecran initial inainte de inceperea jocului
    show_start_screen();
    start_game();

    display_score();

    update_player_leds(current_player, 0);

    while (1) {
        uint32_t move_time = uptime_ms() - turn_start_time;

        update_cursor(&cursor);
        draw_cursor(cursor, current_player);
        show_board();
        
        uint8_t move = drop_pressed(&cursor, &current_player);
        if (move == -1) { // coloana plina
            
        } else if (move == 1) { // logica restart game
            sound_play_start();

            while (!(PIND & (1 << SW_BIT))) {
                _delay_ms(10);
            }
            _delay_ms(100);

            // reset flag-uri
            button_pressed_flag = 0;
            EIFR |= (1 << INTF0);

            // golesc matricea logica
            for (uint8_t r = 0; r < 8; r++) {
                for (uint8_t c = 0; c < 8; c++) {
                    matrix[r][c] = 0;
                }
            }

            // reset variablie stare
            cursor = 3;
            update_player_leds(current_player, 0);
            turn_start_time = uptime_ms();
            
            continue; 
        }

        if (move_time >= 10000) {
            sound_play_error(); // s-a scurs timpul
            
            matrix[0][cursor] = 0;

            // se schimba jucatorul fortat
            if (current_player == 1) {
                current_player = 2;
            } else {
                current_player = 1;
            }

            cursor = 3;
            turn_start_time = uptime_ms();
            move_time = 0;
        }

        update_player_leds(current_player, move_time);
    }
}