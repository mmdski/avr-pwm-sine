#include <avr/io.h>

/* use adc to get inputs from potentiometers */
void adc_init(void)
{
	
    /* set adc multiplexer mode:
       AREF, internal Vref turned off
       adc left adjust result
    */
    ADMUX = (1<<ADLAR);

    /* start adc with fcpu/64 prescale */
    ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS1)|(0<<ADPS0);

    /* wait for read to complete */
	while ((ADCSRA & (1<<ADSC)));

}


/* get analog value on desired adc input pin */
uint8_t adc_get_val(uint8_t input)
{
    
    /* select ADC pin to read */
    ADMUX |= input;

    /* start conversion */
    ADCSRA |= (1<<ADSC);

    /* wait for conversion to complete */
    while ((ADCSRA & (1<<ADSC)));

    /* clear mux */
    ADMUX &= ~0x0F;

    return ADCH;
}
