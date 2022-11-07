#include <avr/io.h>
void init_uart0() // UART0 초기화 함수
{
   UCSR0B = 0x18; // 수신 Receive(RX) 및 송신 Transmit(TX) Enable
   UCSR0C = 0x06; // UART Mode, 8 Bit Data, No Parity, 1 Stop Bit
   UBRR0H = 0; // Baudrate 세팅
   UBRR0L = 103; // 16Mhz, 9600 baud
}
void putchar0(char c) // 1 문자를 송신(Transmit)하는 함수
{
   while(!(UCSR0A & (1<<UDRE0))) ; // UCSR0A 5번 비트 = UDRE(UDR Empty)
   // UDRE0는 define된 값이 5이므로 5번 비트 검사
   UDR0 = c; // 1 문자 전송
}
char getNum(char ascii){ // 문자열을 숫자값으로 반환하는 함ㅜ
   return ascii-0x30;
} 
char getchar0() // 1 문자를 수신(receive)하는 함수
{
   while (!(UCSR0A & (1<<RXC0)))
   ; // UCSR0A 7번 비트, RXC의 define 값 = 7
   // 즉, 1을 7번 왼쪽으로 shift한
   // 값이므로 0x80과 & 하는 효과가 있음
   return(UDR0); // 1 문자 수신, UDR0에서 수신 데이터를 가져옴
}
void getMuls(char num, char value){ // 숫자를 받아 구구단을 출력하는 함수
   if((value >= 0x30)&&(value <= 0x39)){ // 0~9의 숫자일때
      int i=1; char a,b;
      for(i=1; i<10; i++){ // 구구단을 출력함
        //구구단 형식
         putchar0(value); 
         putchar0(' ');
         putchar0('x');
         putchar0(' ');
         putchar0(i+0x30);
         putchar0('=');
         a = ((num*i)/10); b =(num*i)%10; // 십의 자리와 일의 자리 저장
         if(a==0){
            putchar0(b+0x30); // 십의 자리수가 0이면 일의자리만 출력
         }
         else{ // 십의 자리가 있을 때
            putchar0(a+0x30); // 십의자리 숫자 출력
            putchar0(b+0x30); // 일의자리 숫자 출력
         }
         putchar0('\n'); // 개행
         putchar0('\r'); // 커서 이동
      }
   }
}

int main()
{
   char value;
   char num = 0;
   init_uart0(); // UART0 초기화
   while(1) // 문자 수신 무한 루프 수행
   {
      value = getchar0(); // 문자 입력 기다림
      num = getNum(value); // 문자를 숫자로 전환
      getMuls(num,value); // 구구단을 출력함
   }
}