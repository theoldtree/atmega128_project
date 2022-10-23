#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define STOP 1
#define GO 2
volatile int cur_time = 9999;
volatile int state = GO;

unsigned char digit[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
0x6d, 0x7c, 0x07, 0x7f, 0x67}; // fnd에 표시되는 디지털 숫자배열
unsigned char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08}; // fnd칸을 선택하는 배열

ISR(INT4_vect) // 스위치1이 눌렸을때 작동하는 인터럽트 서비스 루틴
{
   _delay_ms(100); // 스위치 바운스 기간동안 기다림
   EIFR = 1 << 4; // 그 사이에 바운스에 의하여 생긴 인터럽트는 무효화
   if ((PINE & 0x10) == 0x10) return; // 스위치가 눌러진 상태가 아니면 리턴
   if (state == STOP) state = GO; // stop상태면 go로 상태전환
   else
   {
      state = STOP; // go 상태면 stop으로 상태전환
   }
}

ISR(INT5_vect) // 스위치2가 눌렸을때 작동하는 인터럽트 서비스 루틴
{
   _delay_ms(100); // 스위치 바운스 기간동안 기다림
   EIFR = 1 << 5; // 그 사이에 바운스에 의하여 생긴 인터럽트는 무효화
   if ((PINE & 0x20) == 0x20) return; // 스위치가 눌러진 상태가 아니면 리턴
   if(cur_time != 0) cur_time += 1000; // 시간이 0이 아니라면 시간에 1000추가 
   if (cur_time >= 9999) cur_time = 9999; // 시간의 최대값은 9999임
}

void init_stopwatch(void);
void display_fnd(int);

int main()
{
   init_stopwatch(); // 포트 방향 설정 및 인터럽트 활성화
   while(1)
   {
      display_fnd(cur_time); // 시간을 표시함
      cur_time--; // 시간이 감소함
      if (cur_time == 0) state = STOP; // 시간이 0이되면 stop상태가 됨
   }
}

void init_stopwatch(void) // 포트방향과 인터럽트를 세팅하기 위한 코드
{
   DDRC = 0xff; // 포트 C의 0~7번핀을 출력으로 설정
   DDRG = 0x0f; // 포트 G의 0~4번핀을 출력으로 설정
   DDRE = 0x00; // 포트 E의 0~7번핀을 입력으로 설정
   sei(); // 글로벌 인터럽트 활성화
   EICRB = 0x0a; // 하강에지에서 인터럽트가 발생하도록 설정
   EIMSK = 0x30; // INT4, INT5 활성화
}

void display_fnd(int count) // fnd에 표시하기 위함 함수
{
   int i,j, fnd[4]; 
   fnd[3] = (count/1000)%10; // 천 자리
   fnd[2] = (count/100)%10; // 백 자리
   fnd[1] = (count/10)%10; // 십 자리
   fnd[0] = count%10; // 일 자리
   for (i=0; i<10; i++) 
   // 이 함수의 1회 수행시간은 약 10ms(1/100초)로 _delay_ms()를 10번 반복하여 1/10초를 만듦 
   {
      for(j=0;j<4;j++)
      {
         PORTC = digit[fnd[j]];
         PORTG = fnd_sel[j]; // fnd에 차례대로 번갈아 가며 숫자를 표시함
         if (j%2)
         _delay_ms(2); // j 가 홀수일 때는 2ms,
         else
         _delay_ms(3); // 짝수일 때는 3ms, 딜레이 
         // 한 번 루프를 돌때 총 10ms(1/100초) 경과}
      }
   }
}