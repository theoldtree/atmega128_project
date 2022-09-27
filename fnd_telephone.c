#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
int main( )
{
   char tel_num[20] = {0x00,0x00,0x00,0x00,0x3f,0x06,0x3f,0x40,0x4f,0x66,0x4f,
   0x4f,0x40,0x5b,0x07,0x6f,0x66,0x00,0x00,0x00}; //010-3433-2794
   int i,j;
   DDRC = 0xff; // FND를 연결한 포트 C를 출력으로 설정
   DDRG = 0x0f; // FND 선택 신호인 포트 G를 출력으로 설정
   while(1) 
   {
      for(i=0;i<17;i++){
         for(j=0;j<25;j++){
            PORTC = tel_num[i]; PORTG = 0x08; _delay_ms(5);
            PORTC = tel_num[i+1]; PORTG = 0x04; _delay_ms(5);
            PORTC = tel_num[i+2]; PORTG = 0x02; _delay_ms(5);
            PORTC = tel_num[i+3]; PORTG = 0x01; _delay_ms(5);
         }
         PORTG = 0x00;
         _delay_ms(500);
      }
   }
}