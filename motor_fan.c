#include <avr/io.h>
#define F_CPU 16000000UL
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>
#include <avr/interrupt.h>
#define LOW_SPEED 77 // OCR값 - 약풍
#define MID_SPEED 153 // OCR값 - 중풍
#define HIGH_SPEED 230 // OCR값 - 강풍
#define MOTOR_CW 0xb0 // 모터 시계방향
#define MOTOR_CCW 0x70 // 모터 반시계방향
#define MOTOR_BREAK 0xd0 // 모터 정지
#define MOTOR_STANDBY 0x00 // 모터 대기
#define ON 1 // 켠 상태
#define BREAK 0 // 잠시 정지 상태
#define STOP -1 // 예약 정지 상태
#define WEAK 0 // 바람세기 인덱스 - 약풍
#define MID 1 // 바람세기 인덱스 - 중풍
#define STR 2 // 바람세기 인덱스 - 강풍
volatile int count = 0; // 시간을 세기 위한 변수
volatile int strength = WEAK; // 모터 속도의 인덱스값을 설정하기 위한 변수
volatile int state = BREAK; // 모터의 상태를 정하기 위한 변수
int motor_speed[3] = {LOW_SPEED,MID_SPEED,HIGH_SPEED}; // 약 중 강풍의 OCR값 

ISR(INT4_vect){ // 1번 스위치 : 정지 -> 가동, 가동 -> 정지상태로 전환
   _delay_ms(100);
   EIFR |= 0x10;
   if((PINE & 0x10) == 0x10) return; 
   // 스위치가 눌린것이 맞다면
   if(state == ON) state = BREAK; // 가동상태면 정지로 전환
   else if (state == BREAK | state==STOP) state = ON; // 정지면 가동상태로 전환
}
ISR(INT5_vect){ // 2번스위치 : 바람세기의 인덱스를 조절함
   _delay_ms(100);
   EIFR |= 0x20;
   if((PINE & 0x20) == 0x20) return;
   // 스위치가 눌린것이 맞다면
   if(strength == WEAK) strength = MID; // 약 ->  중
   else if(strength == MID) strength = STR; // 중 ->  강
   else if (strength == STR) strength = WEAK; // 강 ->  약
}
ISR(INT6_vect){ // 3번 스위치
   _delay_ms(100);
   EIFR |= 0x40;
   if((PINE & 0x40) == 0x40) return;
   // 스위치가 눌린것이 맞다면
   state=STOP; // 예약정지 상태로 전환
}
ISR(TIMER0_OVF_vect){ // 시간을 조절하기위한 인터럽트 서비스루틴
   if(count < 10000) {
      TCNT0 = 0x06; // TCNT0 초기화
      count ++; // 카운트 값은 매번 증가    
   }
   /* TCNT0 가 0x06으로 초기화되면 1/16000000*32*(256-6) = 0.5ms
   시간이 걸린다 따라서 이를 10000번 반복하면 5초가 되는 원리를 이용하여
   TCNT0값을 초기화 한다.
   */
   else PORTB = MOTOR_BREAK; // 5초가 지난후 모터 정지
}

int main(void){
   DDRB = 0xf0; // 포트 B를 출력으로 설정 : 모터 
   PORTB = MOTOR_STANDBY; // 모터 대기
   sei(); // 외부전역인터럽트 활성화
   EIMSK = 0x70; // 외부 인터럽트 456 활성화
   EICRB = 0x2a; // 외부 인터럽트 456번을 하강에지에서 발생하도록 설정
   TCCR2 = 0x6b; // fast PWM 모드, 비교출력모드는 클리어 OC2모드 선택 64분주
   PORTB = MOTOR_CW; // 모터는 시계방향
   TCCR0 = 0x03; // 일반보드 32분주 
   TIMSK |= 0x01; // TCNT0에서 오버플로우가 발생하면 인터럽트 발생
   while(1){
      if(state == ON){ // 가동 상태일때
         PORTB = MOTOR_CW; // 모터는 시계방향
         OCR2 = motor_speed[strength]; // pwm을 해당바람세기에 해당하는 모터속도로전환
      }
      else if(state == BREAK){ // 정지상태이면
         PORTB = MOTOR_BREAK; // 모터 정지
      }
      else if(state == STOP){ // 예약정지상태이면
         TCNT0 = 0x06; // TCNT0를 설정하여 시간세팅
      }
   }
}