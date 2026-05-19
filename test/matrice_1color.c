#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define PIXEL_PORT  PORTD
#define PIXEL_DDR   DDRD
#define PIXEL_BIT   6

// Culori foarte slabe pentru a testa stabilitatea
#define R_VAL 0
#define G_VAL 10
#define B_VAL 20

void __attribute__((noinline)) ws2812_send_byte(uint8_t dat) {
    uint8_t ctr;
    // Aceasta sectiune trebuie sa fie neintrerupta
    asm volatile (
        "    ldi %1, 8  \n"
        "loop%=: \n"
        "    sbi %0, %2 \n"      // HIGH
        "    sbrs %3, 7 \n"      
        "    rjmp bit0%= \n"    
        "    nop \n" "nop \n" "nop \n" "nop \n" "nop \n" 
        "    nop \n" "nop \n" "nop \n" 
        "    cbi %0, %2 \n"      // LOW (Bit 1)
        "    rjmp end%= \n"     
        "bit0%=: \n"
        "    nop \n" 
        "    cbi %0, %2 \n"      // LOW (Bit 0)
        "    nop \n" "nop \n" "nop \n" "nop \n"
        "end%=: \n"
        "    lsl %3 \n"
        "    dec %1 \n"
        "    brne loop%= \n"     
        :
        : "I" (_SFR_IO_ADDR(PIXEL_PORT)), "r" (ctr), "I" (PIXEL_BIT), "r" (dat)
    );
}

int main(void) {
    PIXEL_DDR |= (1 << PIXEL_BIT);
    PIXEL_PORT &= ~(1 << PIXEL_BIT);

    while (1) {
        // 1. BLOCARE TOTALA intreruperi
        uint8_t sreg = SREG;
        cli(); 

        for (uint16_t i = 0; i < 64; i++) {
            ws2812_send_byte(G_VAL);
            ws2812_send_byte(R_VAL);
            ws2812_send_byte(B_VAL);
        }

        // 2. RESTAURARE stare intreruperi
        SREG = sreg; 

        // 3. RESET mai lung (foarte important pentru stabilitate)
        // Multe clone de WS2812B (SK6812) au nevoie de 300us
        _delay_us(300); 
        
        // 4. Asteptam 1 secunda intre refresh-uri
        _delay_ms(1000); 
    }
}