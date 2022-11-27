#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define NULL 0x00// CLCD Command & Data
#define BIT4_LINE2_DOT58 0x28 // 4 Bit Mode, 2Lines, 5x8 Dot
#define DISPON_CUROFF_BLKOFF 0x0C // Display On,Cursor Off, Blink Off
#define DISPOFF_CUROFF_BLKOFF 0x08 // Display Off, Cursor Off, Blink Off
#define INC_NOSHIFT 0x06 // Entry Mode, Cursor Increment,
// Display No Shift
#define DISPCLEAR 0x01 // Display Clear, Address 0
// Position, Cursor 0
#define CUR1LINE 0x80 // Cursor Position Line 1 First
#define CUR2LINE 0xC0 // Cursor Position Line 2 First
#define CURHOME 0x02 // Cursor Home
#define E_BIT 0x04 // Enable Bit #
#define RW_BIT 0x03 // Read Write Bit #
#define RS_BIT 0x02 // Register Select Bit #
void CLCD_cmd(char); // 명령어 전송 함수
void CLCD_data(char); // 데이터 Write 함수
void CLCD_puts(char *); // 문자열 처리 함수
char num[4]; // 네자리 숫자를 저장할 변수

void init_adc()
{
   ADMUX = 0x40;
   // REFS(1:0) = 01 : AVCC(+5V) 기준 전압 사용
   // ADLAR = 0 : 디폴트 오른쪽 정렬
   // MUX(4:0) = 00000 : ADC0 사용, 단극 입력
   ADCSRA = 0x87;
   // ADEN = 1 : ADC Enable
   // ADFR = 0 : single conversion(한번만 변환) 모드
   // ADPS(2:0) = 111 : 프리스케일러 128분주, 0.125Mhz 주기
}

unsigned short read_adc()
{
   unsigned char adc_low, adc_high;
   unsigned short value;
   ADCSRA |= 0x40; // ADC start conversion, ADSC(비트5) = 1
   while ((ADCSRA & 0x10) != 0x10)
   ; // ADC 변환 완료 검사(ADIF) (비트4)
   adc_low = ADCL; // 변환된 Low 값 읽어오기
   adc_high = ADCH; // 변환된 High 값 읽어오기
   value = (adc_high << 8) | adc_low; // value는 High 및 Low 연결 16 비트 값
   return value;
}

int main(void)
{
   init_adc(); // 조도센서 초기화
   unsigned short value; // 광량을 저장할 변수 선언
   _delay_ms(50); // 전원 인가후 CLCD 셋업 시간
   DDRC = 0xff; // PORTC : command/data port
   DDRD = 0xff; // PORTD : control port
   DDRF = 0x00; // PORTF : 광량 입력 port 
   CLCD_cmd(BIT4_LINE2_DOT58); // 4 Bit Mode, 2 Lines, 5x8 Dot
   CLCD_cmd(DISPON_CUROFF_BLKOFF); // Display On, Cursor Off, Blink Off
   CLCD_cmd(INC_NOSHIFT); // Entry Mode, Cursor Increment,
   // Display No Shift
   CLCD_cmd(DISPCLEAR); // Display Clear, Address 0
   // Position, Cursor 0
   _delay_ms(2); // 디스플레이 클리어 실행 시간 동안 대기
   while(1) 
   {
      _delay_ms(100);
      value = read_adc(); // 광량을 읽어서 value에 저장
      num[0] = (value/1000) + 0x30; // 천의 자리 숫자를 char형으로 변환
      num[1] = (value/100)%10 + 0x30; // 백의 자리 숫자를 char형으로 변환
      num[2] = (value/10)%10 + 0x30; // 십의 자리 숫자를 char형으로 변환
      num[3] = value%10 + 0x30; // 일의 자리 숫자를 char형으로 변환
      CLCD_cmd(CUR1LINE); // LCD중의 첫번째 줄을 사용함
      CLCD_puts(num); // 문자열을 display함
   }
}

void CLCD_puts(char *ptr) // 문자열을 write하는 함수
{
   while(*ptr != NULL) // 문자열이 마지막(NULL)인지 검사
   CLCD_data(*ptr++); // 마지막이 아니면 1 문자 디스플레이
}

void CLCD_data(char data) // 데이터를 wite 하는 함수
{
   PORTD = 0x04; // 0b0000100, E(bit4)=0, R/W(bit3)=0,
   // RS(bit2)=1, Write 사이클, 데이터 모드
   _delay_us(1); // Setup Time
   PORTD = 0x14; // 0b00010100, E(bit4)=1, R/W(bit3)=0,
   // RS(bit2)=1 Enable
   PORTC = data & 0xf0; // upper 4bit
   PORTD = 0x04; // 0b0000100, E(bit4)=0, R/W(bit3)=0,
   // RS(bit2)=1, Write 사이클, 데이터 모드
   _delay_us(2); // Hold & Set Time
   PORTD = 0x14; // 0b00010100, E(bit4)=1, R/W(bit3)=0,
   // RS(bit2)=1 Enable
   PORTC = (data << 4) & 0xf0; // lower 4bit
   PORTD = 0x04; // 0b00000100, E(bit4)=0, R/W(bit3)=0,RS(bit2)=1 사이클 종료
   _delay_ms(1); // Hold Time & Execution Time
}

void CLCD_cmd(char cmd) // 명령어 전송 함수
{
   PORTD = 0x00; // 0b0000000, E(bit4)=0, R/W(bit3)=0,
   // RS(bit2)=0, Write 사이클, 명령어 모드
   _delay_us(1); // Setup Time
   PORTD = 0x10; // 0b00010000, E(bit4)=1, R/W(bit3)=0,RS(bit2)=0 Enable
   PORTC = cmd & 0xf0; // upper 4bit 전송
   PORTD = 0x00; // 0b0000000, E(bit4)=0, R/W(bit3)=0,RS(bit2)=0, Write 사이클, 명령어 모드
   _delay_us(2); // Hold & Setup Time
   PORTD = 0x10; // 0b00010000, E(bit4)=1, R/W(bit3)=0,RS(bit2)=0 Enable
   PORTC = (cmd << 4) & 0xf0; // lower 4bit 전송
   PORTD = 0x00; // 0b00000100, E(bit4)=0, R/W(bit3)=0,RS(bit2)=0 사이클 종료
   _delay_ms(1); // Hold Time & Execution Time
}