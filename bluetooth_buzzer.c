#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#define ON 1 //버저 ON
#define OFF 0 //버저 OFF
#define NULL 0
#define DO 0
#define RE 1
#define MI 2
#define FA 3
#define SOL 4
#define RA 5
#define SI 6
#define DDO 7

char f_table[] = {17,43,66,77,97,114,129,137};

volatile int tone ;
volatile int sound_pulse = OFF;
int state = OFF;
int index = 0;
char tone_num[8];
char set_message[] = "\nOk let's play the piano!\n";

void init_uart1(); // uart1 초기화 함수
void putchar1(char c); //1문자를 송신하는 함수
void puts(char *ptr); //문자열을 송신하는 함수
char getchar1(); //1문자를 수신하는 함수


ISR(TIMER0_OVF_vect) //Timer/counter0 오버플로우 인터럽트 서비스 루틴
{
   TCNT0 = f_table[tone]; //TCNT0 초기화(주어진 음을 만들기 위한)
   if(state==ON)
   {
      if (sound_pulse == OFF)
      {
         PORTE |= 1<<4; //버저 포트 ON (다른 기능은 그대로 두고 PB4만)
         sound_pulse = ON;
      }
      else
      {
         PORTE &= ~(1<<4); //다른 기능은 그대로 두고 PB4 포트만 OFF
         sound_pulse = OFF;
      }
   }
   
}

ISR(USART1_RX_vect) //UART1 receive interrupt routine
{
   char c;
   if (UDR1 >'z' || UDR1 < '0') return; //
   if(index == 8) //해당되는 문자열일때, 
   {
      c = UDR1; //어떤 음을 연주할지에 대해 사용자 지정 음을 받음
      putchar1(c); //echo back
      
      if (c == tone_num[0]) {tone = 0;}
      else if(c == tone_num[1]) {tone = 1;}
      else if(c == tone_num[2]) {tone = 2;}
      else if(c == tone_num[3]) {tone = 3;}
      else if(c == tone_num[4]) {tone = 4;}
      else if(c == tone_num[5]) {tone = 5;}
      else if(c == tone_num[6]) {tone = 6;}
      else if(c == tone_num[7]) {tone = 7;}
    st_buzz = ON;

   }
   else
   {
      tone_num[index] = UDR1;
      putchar1(tone_num[index]); //echo back
      if(index == 7)
      {
         puts(set_message); //사용자 지정 값 세팅이 완료되었다는 메세지 출력
      }
      index++;
   }
}

int main()
{
   char setting_msg[] = "set the melody form\n";
   DDRE = 0x10; //경고음(버저)포트 = PE4
   TCCR0 = 0x03; //프리스케일러 64분주로 설정 
   TIMSK = 0x01; //오버플로우 인터럽트 활성화 
   init_uart1();//uart1 초기화
   sei(); //인터럽트 활성화
   puts(setting_msg); //가장 먼저 세팅 메세지 출력
   while(1)
   {
      if(state == ON)
      {
         _delay_ms(1000);
         state = OFF;
      }
   }
}


void init_uart1()
{
   UCSR1B = 0x98; //송신 TX,RX Enable ,Receive interrupt enable
   UCSR1C = 0x07; //UART mode, 8bit, not parity, 1 stop bit
   UBRR1H = 0; //baudrate 세팅
   UBRR1L = 8; //16Mhz, 115200 baud
}

void putchar1(char c)
{
   while(!(UCSR1A & (1<<UDRE1))); //UDRE1 : UCSR1A 5번 비트 0x20과 &
   //데이터가 비었는지 확인한다. 비어있다면 1
   UDR1 = c; //비어있으면 UDR1에 c를 넣어준다.
}

void puts(char *ptr)
{
   while(1)
   {
      if(*ptr != NULL) //string의 마지막에는 NULL이 있음
      {
         putchar1(*ptr++); //포인터를 하나씩 옮겨가며 송신
      }
      else //NULL을 만나면
      return;
   }
   
}