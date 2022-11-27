#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#define TRIG 6 //HC-SR04 Trigger 신호 (출력 PE6)
#define ECHO 7 //HC-SR04 Echo 신호 (입력 PE7)
#define SOUND_VELOCITY 340UL //소리속도 (m/sec)
#define NULL 0x00
#define BIT4_LINE2_DOT58 0x28 //Function Set : 4비트 모드, 2줄 , 5x8 Dot
#define DISPON_CUROFF_BLKOFF 0x0c //Display ON/OFF control : 디스플레이 ON,커서 OFF,깜빡임 OFF
#define DISPOFF_CUROFF_BLKOFF 0x08 // Display OFF
#define INC_NOSHIFT 0x06 //Entry mode set : 커서 증가모드 , display no shift
#define DISPCLEAR 0x01 // Clear display
#define CUR1LINE 0x80 //Cursor Position Line 1 First
#define CUR2LINE 0xc0 //Cursor Position Line 2 First
#define STATE0 0
#define STATE1 1

unsigned int read_distance();
void init_CLCD();
void CLCD_cmd(char);
void CLCD_data(char);
void CLCD_puts(char*);
void init_SWITCH();
void CLCD_display_num(int);
char msg1[] = "If on bottom";
char msg2[] = "If on head";
char msg3[] = "Press switch!";
char msg4[] = "Your height";
char error[] = "ERROR!";
char CLCD_HEIGHT[];
int state = STATE1;
unsigned int bottom,head; // 키를 측정하기 위한 전역변수
int height;

int main(void)
{
   init_CLCD(); //CLCD 초기화
   init_SWITCH(); //스위치 초기화
   DDRE = ((DDRE|(1<<TRIG))&~(1<<ECHO)); //TRIG = 출력 ECHO = 입력으로 세팅
   CLCD_cmd(DISPCLEAR);
   _delay_ms(2); //디스플레이 클리어 실행 시간 동안 대기
   CLCD_cmd(CUR1LINE);
   CLCD_puts(msg1);
   CLCD_cmd(CUR2LINE);
   CLCD_puts(msg3);
   while(1)
   {
      ;
   }
}

void init_SWITCH()
{
   DDRE = 0x00; //PE4를 스위치 입력신호로 둔다.
   sei(); //interrupt.h 에 있는 함수로 SREG 7번비트(I) set
   //"SREG |= 0x80"과 동일한 기능을 수행한다.
   EICRB = 0x02; //0000 0010 INT4, INT5 트리거는 falling edge
   EIMSK = 0x10; //0001 0000 INT4, INT5 인터럽트 enable
}

void init_CLCD()
{
   DDRC = 0xff; //PORT C : CLCD 데이터 신호 할당
   DDRD = 0xff; //PORT D : CLCD 제어 신호 할당
   _delay_ms(50); //전원 인가 후 CLCD 셋업 시간
   CLCD_cmd(BIT4_LINE2_DOT58);
   CLCD_cmd(DISPON_CUROFF_BLKOFF);
   CLCD_cmd(INC_NOSHIFT);
   CLCD_cmd(DISPCLEAR);
   _delay_ms(2); //디스플레이 클리어 실행 시간 동안 대기
}

void CLCD_data(char data)
{
   PORTD = 0x04; //E(bit4) = 0, R/W(bit3) = 0
   //RS(bit2) = 1 Write사이클, 데이터모드
   _delay_us(1); //셋업타임
   PORTD = 0x14; //E(bit4) = 1, R/W(bit3) = 0
   //RS(bit2) = 1 Enable
   PORTC = data & 0xf0; //8비트 데이터 중 상위 4비트 준비
   PORTD = 0x04; //E(bit4) = 0, R/W(bit3) = 0
   //RS(bit2) = 1 Write사이클, 데이터모드
   _delay_us(2); //홀드타임 & 셋업타임
   PORTD = 0x14; //E(bit4) = 1, R/W(bit3) = 0
   //RS(bit2) = 1 Enable
   PORTC = (data<<4) & 0xf0; //8비트 데이터 중 하위 4비트 준비
   PORTD = 0x04; //E(bit4) = 0, R/W(bit3) = 0
   //RS(bit2) = 1 사이클 종료
   _delay_ms(1); //홀드타임 & 실행시간
}

void CLCD_cmd(char cmd)
{
   PORTD = 0x00; //E(bit4) = 0, R/W(bit3) = 0,
   //RS(bit2) = 0, Write 사이클, 명령어 모드
   _delay_us(1); //Setup time
   PORTD = 0x10; //E(bit4) = 1, R/W(bit3) = 0,
   //RS(bit2) = 0 Enable
   PORTC = cmd & 0xf0; //8비트 명령어중 상위 4비트 준비
   PORTD = 0x00; //E(bit4) = 0, R/W(bit3) = 0,
   //RS(bit2) = 0, Write 사이클, 명령어 모드
   _delay_us(2); //홀드타임 & 셋업타임
   PORTD = 0x10; //E(bit4) = 1, R/W(bit3) = 0,
   //RS(bit2) = 0 Enable
   PORTC = (cmd<<4) & 0xf0;
   PORTD = 0x00; //사이클 종료
   _delay_ms(1); //Hold time & 실행시간
}

void CLCD_puts(char *ptr)
{
   while(*ptr != NULL) //문자열이 마지막 NULL인지 검사
   CLCD_data(*ptr++); //마지막이 아니면 한 문자 디스플레이
}

void CLCD_display_num (int num)
{
   CLCD_HEIGHT[0] = (num/1000)%10 + 0x30; // 천의자리
   CLCD_HEIGHT[1] = (num/100)%10 + 0x30; // 백의자리
   CLCD_HEIGHT[2] = (num/10)%10 + 0x30; // 십의자리
   CLCD_HEIGHT[3] = '.'; // 소수점표현
   CLCD_HEIGHT[4] = (num/1)%10 + 0x30; // 일의자리
   CLCD_HEIGHT[5] = 'c';
   CLCD_HEIGHT[6] = 'm'; // cm 표현
   CLCD_puts(CLCD_HEIGHT);
}

unsigned int read_distance()
{
   unsigned int distance;
   TCCR1B = 0x03; //프리스케일러 64분주 : 즉 주기 4us
   PORTE &= ~(1<<TRIG); // TRIG LOW상태 유지
   _delay_us(10); //최소 10us동안
   PORTE |= (1<<TRIG); //Trig = HIGH -> 거리측정명령시작
   _delay_us(10); //10us를 유지
   PORTE &= ~(1<<TRIG); //Trig = LOW -> 거리측정명령 끝
   while(!(PINE&(1<<ECHO))); //Echo가 High가 될 떄 까지 대기
   TCNT1 = 0x0000; //16비트 타이머, Counter/timer 1 값 초기화
   while((PINE&(1<<ECHO))); //Echo가 Low가 될 때 까지 대기
   TCCR1B = 0x00; //Counter/timer 1 클록 정지
   distance = (unsigned int)(SOUND_VELOCITY*(TCNT1*4/2)/1000); //거리단위는 mm
   return distance;
}

ISR(INT4_vect) // 상태에 따라 원하는 기능 구현
{
   _delay_ms(100); 
   EIFR = 0x10; 
   if((PINE & 0x10)==0x10) 
   return;
    else if(state == STATE1)
    {
        CLCD_cmd(DISPCLEAR);
        _delay_ms(2); // 디스플레이 클리어 실행 시간 동안 대기
        CLCD_cmd(CUR1LINE);
        CLCD_puts(msg2);
        CLCD_cmd(CUR2LINE);
        CLCD_puts(msg3);
        head = read_distance(); // 키를 읽음
        state = STATE0;
    }
    else if(state == STATE0)
    {
        CLCD_cmd(DISPCLEAR);
        _delay_ms(2); //디스플레이 클리어 실행 시간 동안 대기
        CLCD_cmd(CUR1LINE);
        CLCD_puts(msg4);
        height = bottom - read_distance(); // 키 측정
        if (height < 0) // 음수일 경우 에러 메시지 출력
        {
            CLCD_cmd(CUR2LINE);
            CLCD_puts(error);
        }
        else
        {
            CLCD_cmd(CUR2LINE);
            CLCD_display_num(height);
        }
    }
}
