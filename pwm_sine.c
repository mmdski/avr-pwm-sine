#include <avr/io.h>
#include <avr/interrupt.h>
#include "sine.h"

#define PITCH   PA0

#define ATTACK  PA1
#define DECAY   PA2

volatile int8_t g_sample;

volatile uint8_t g_pitch;

volatile uint8_t g_amp;

int main()
{

    void init(void);
    
    init();

    for (;;)
    {
        g_pitch = ADCH;
        PORTB = ~g_pitch;
    }


}

void init(void)
{
    
    cli();

    void init_timer0(void);
    void init_timer1(void);
    void init_timer2(void);
    
    extern void init_adc(void);



    /* set PORTB to output to see g_pitch value on leds */
    DDRB = 0xFF;

    init_timer2();
    init_timer1();
    init_timer0();

    init_adc();

    sei();

}


/* use timer 0 to update pwm value */
void init_timer0(void)
{

    /* set timer 0 for mode 2: CTC */
    TCCR0A = (1<<WGM01);

    /* enable timer 0 output compare match b interrupt */
    TIMSK0 |= (1<<OCIE0A);

    /* set output compare register A to 177 */  // attempting to play at 440Hz
    OCR0A = 43;                                 // 20MHz/(8*(43+1)*128)= 443.89Hz

    /* start timer 0 with prescale = 8 */
    TCCR0B = (1<<CS01);

}


/* use timer 1 to update amplification value for envelope effect */
void init_timer1(void)
{

    /* set timer 1 for mode 4: CTC*/
    TCCR1B = (1<<WGM12);

    /* enable timer 1 output compare match a interrupt */
    TIMSK1 |= (1<<OCIE1A);

    /* set OCR1B to desired value */        // attempting to play 2 seconds
    OCR1A = 9765;                           // (256*(9765+1)*16)/20MHz = 2.00

    /* start timer 1 with prescale = 256 */
    TCCR1B |= (1<<CS12);

}


/* use timer 2 for pwm */
void init_timer2(void)
{

    /* set OC2B (PD6) as output */
    DDRD |= (1<<PD6);

    /* set timer 2 for mode 1: PWM, Phase Correct, non-inverting */
    TCCR2A = (1<<COM2B1)|(1<<WGM20);

    /* enable timer 2 overflow interrupt */
    TIMSK2 = (1<<TOIE2);

    /* start timer 2 with prescale = 1 */
    TCCR2B = (1<<CS20);

}




/* timer 0 interrupt service routine 
   set g_sample to next sample value in waveform array
   update output compare register to new pitch value
*/
ISR(TIMER0_COMPA_vect)
{

    static uint8_t i = 0;

    /* update to new pitch value */
    //g_pitch = ADCH;
    OCR0A = g_pitch;

    /* get sample value and index to next */
    g_sample = sine128[i++];

    /* stay within sample array */
    if (i > 127)
        i = 0;
}


/* timer 1 interrupt service routine
   set amplification value
*/
ISR (TIMER1_COMPA_vect)
{

    static uint8_t attack = 1;

    if (attack)
    {
        if (g_amp < 8)
            g_amp++;
        else
            attack = 0;
    }
    
    else
    {
        if (g_amp == 0)
            attack = 1;
        else
            g_amp--;
    }


}


/* timer 2 interrupt service routine 
   set output compare register 2B to shifted sample value
*/
ISR(TIMER2_OVF_vect)
{

    /* shift current sample to uint8_t and set OCR */
    OCR2B = (g_amp*g_sample)+127;
    //OCR2B = g_sample+127;

}
