#include "timer1.h"

void timer1_init(void){
    #ifdef IDLEDEBUG
    DDRB |= (1 << PINB0);
    #endif

    // Start the timer with prescaler 1024 (16MHz CLK)
    TCCR1B |= (1<<CS10) | (1<<CS12);
    /*
    // 10 Hz = 0.1s -> so we need 1562
    // OCR1A = (16MHz / (1024 * 10Hz)) = 1562 for 0.1s duration
    */
    OCR1A = 15620;
    TCNT1 = 0;
};

void timer1_stop(void){
    TCNT1 = 0; // Reset timer register
    TIFR1 |= (1<<OCF1A); // Clear overflow flag
    return;
};
