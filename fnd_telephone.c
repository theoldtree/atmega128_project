#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
int main( )
{
   char tel_num[20] = {0x00, 0x00, 0x00, 0x00, 0x3f, 0x06, 0x3f, 0x40, 0x4f, 0x66, 0x4f, 0x4f, 0x40, 0x5b, 0x07, 0x6f, 0x66, 0x00, 0x00, 0x00}; // 공백을 포함하여 010-3433-2794 전화번호를 표시하기 위한 배열 선언
   int i,j; // 반복문에 사용되는 변수 i,j
   DDRC = 0xff; // FND를 연결한 포트 C를 출력으로 설정
   DDRG = 0x0f; // FND 선택 신호인 포트 G를 출력으로 설정
   while(1) // 반복적으로 디스플레이
   // 1 루프 소요시간 = 5ms x 4 = 20ms
   {
      for(i=0;i<17;i++){
         for(j=0;j<25;j++){
            PORTC = tel_num[i]; PORTG = 0x08; _delay_ms(5);
            PORTC = tel_num[i+1]; PORTG = 0x04; _delay_ms(5);
            PORTC = tel_num[i+2]; PORTG = 0x02; _delay_ms(5);
            PORTC = tel_num[i+3]; PORTG = 0x01; _delay_ms(5);
         }  // 5ms * 4 * 25= 500ms(0.5초) 동안 배열의 숫자가 차례대로 FND에 표시된다.
         PORTG = 0x00; 
         _delay_ms(500); // 0.5초 동안은 아무것도 표시하지 않는다.
      } // 1초 동안 다음과 같은 반복문이 실행되며 한번 사이클이 돌면 배열의 다음 번호가 FND에 차례대로 표시된다.
   }
}