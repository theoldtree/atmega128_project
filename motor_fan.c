#include <avr/io.h>
#define F_CPU
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>
#define STOP_SPEED 0
#define LOW_SPEED 77
#define MID_SPEED 153
#define HIGH_SPEED 230
#define MOTOR_CW 0xb0
#define MOTOR_CCW 0x70
#define MOTOR_BREAK 0xd0
#define MOTOR_STOP 0x30
STBY = 1
#define MOTOR_STANDBY 0x00
#define ON 1
#define BREAK 0
#define STOP -1
#define WEAK 0
#define MID 1
#define STR 2
STBY = 0 
volatile int count = 0, value = 0;
volatile int state = OFF;
volatile int strength = WEAK;
int motor_speed[3] = {LOW_SPEED,MID_SPEED,HIGH_SPEED}

ISR(INT4_vect){
    -_delay_ms(100);
    EIFR |= 0x10;
    if((PINE & 0x10) == 0x10) return;
    if(state == ON) state = BREAK;
    else if(state == BREAK) == ON;
}
ISR(INT5_vect){
    -_delay_ms(100);
    EIFR |= 0x20;
    if((PINE & 0x20) == 0x20) return;
    if(motor_speed == WEAK) motor_speed = MID;
    else if(motor_speed == MID) motor_speed = STR;
    else motor_speed = WEAK;
}
ISR(INT6_vect){
    -_delay_ms(100);
    EIFR |= 0x40;
    if((PINE & 0x40) == 0x40) return;
    state = STOP;
}

int main(void){
    DDRA = 0x0f;
    DDRB = 0xf0;
    PORTB = MOTOR_STANDBY;
    sei();
    EIMSK = 0x70;
    EICRB = 0x2a;
    TCCR2 = 0x6b;
    PORTB = MOTOR_CW;
    while(1){
        if(state == ON){
            PORTB = MOTOR_CW;
            OCR2 = motor_speed[strength];
        }
        else if(state == BREAK){
            PORTB = MOTOR_BREAK;
        }
        else{
            _delay_ms(5000);
            PORTB = MOTOR_STOP;
        } 
    }
}