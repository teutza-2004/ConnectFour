#include <avr/io.h>

#include "adc.h"

void adc_init(void)
{
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX |= (1 << REFS0);
    ADCSRA |= (1 << ADEN);
}

uint16_t analog_read(uint8_t channel)
{
    channel &= 0b00000111;
    ADMUX &= 0b11100000;
    ADMUX |= channel;
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));

    return (ADC);
}
