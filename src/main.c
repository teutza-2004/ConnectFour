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
        drop_pressed(&cursor, &current_player);
        update_player_leds(current_player);
    }
}