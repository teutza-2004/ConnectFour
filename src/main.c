#include "matrix_functions.h"

uint8_t matrix[8][8] = {0};

int main(void) {
    DDRD |= (1 << MATRIX_BIT);
    
    while (1) {
        show_board();
        _delay_ms(500);
    }
}