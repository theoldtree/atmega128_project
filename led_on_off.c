#include <avr/io.h>
#include <stdlib.h> 
#define __DELAY_BACKWARD_COMPATIBLE__
#define F_CPU 16000000UL
#include <util/delay.h> 
int main( )
{
   DDRA = 0xff; // 포트 A의 핀 전체를 출력 포트로 사용
   while(1) // 무한 반복문
   {
      volatile int i; // 출력포트를 조절하기 위한 변수 선언

      // 첫번째 LED부터 마지막 LED까지 순차적으로 켜지는 루프
      i = 0x01; // 0b'1대입
      while(i<=0xff) // i의 값이 0b'1111 1111이하일 때 까지
      {
         PORTA = i; // i 값에 따라 PORTA의 핀이 결정됨
         _delay_ms(100);// 0.1ms 딜레이 
         i = (i<<1)+1; // i*2 + 1 연산을 하여 출력핀이 한개씩 늘어나도록 설정
      }

      // LED가 전부 점등되어 있을때 마지막부터 첫번째 까지 순차적으로 줄어드는 로프
      i=0xff; // 0b'1111 1111 대입
      while(i>=0x01) // i의 값이 0'b1이상일 때 까지
      {
         PORTA = i; // i 값에 따라 PORTA의 핀이 결정됨
      _delay_ms(100); // 0.1ms 딜레이
      i = (i>>1);// i/2를 통해 출력핀이 한개씩 줄어들도록 설정
   }
}
