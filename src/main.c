#include "helpers.h"
#include "matrix_functions.h"
#include "buzzer.h"

uint8_t cursor = 0;
uint8_t current_player = 1;

int main(void) {
    config_init();
    uptime_init();
    adc_init();
    buzzer_init();

    // ecran initial inainte de inceperea jocului
    show_start_screen();
    start_game();

    update_player_leds(current_player);
    while (1) {
        update_cursor(&cursor);
        draw_cursor(cursor, current_player);
        
        // logica restart game
        if (drop_pressed(&cursor, &current_player)) {
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
            cursor = 0;
            update_player_leds(current_player);
            
            continue; 
        }

        update_player_leds(current_player);
    }
}