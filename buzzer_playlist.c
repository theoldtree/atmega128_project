#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>
#define DO      0
#define RE      1
#define MI      2
#define FA      3
#define SOL     4
#define RA      5
#define SI      6
#define DDO     7
#define REST    8
#define EOS     -1
#define ON      0
#define OFF     1
#define N2      1250
#define N4      625
#define N8N16   469
#define N8      313
#define N16     156
#define R       1
#define SONG_1   0
#define SONG_2   1
#define SONG_3   2

volatile int state, tone, song_index=0; // 상태, 음계, 음계차례를 저장하기 위한 전역변수
volatile int now_song = SONG_1; // 현재 진행되는 노래를 저장하기 위한 변수

// 도레미파솔라시도에 해당하는 TCNT0 값
char f_table[] = {17, 43, 66, 77, 97, 114, 117, 137, 255};

// 비행기 음계와 박자(1번 노래)
int song_1[] = {MI, RE, DO, RE, MI, REST, MI, REST, MI, RE, REST, RE, REST, RE, 
MI, REST, SOL, REST, SOL, REST, MI, RE, DO, RE, MI, REST, MI, REST, MI, RE, REST, 
RE, MI, RE, DO, EOS};
int time_1[] = {N4,N4,N4,N4,N4,R,N4,R,N2,N4,R,N4,R,N2,N4,N4,R,N2,N4,N4,N4,N4,N4,
R,N4,R,N2,N4,R,N4,N4,N4,N2};

// 산토끼 음계와 박자(2번 노래)
int song_2[] = {SOL, MI, REST, MI, SOL, MI, DO, RE, MI, RE, DO, MI, SOL, 
DDO,SOL, DDO, SOL, DDO, SOL, MI, SOL, RE, FA, MI, RE, DO, EOS}; 
int time_2[] = {N4, N8, R, N8, N8, N8, N4, N4, N8, N8, N8, N8, N4,
N8N16, N16, N8, N8, N8, N8, N4, N4, N8, N8, N8, N8, N4}; 

//학교종이 땡땡땡 음계와 박자(3번 노래)
int song_3[] = {SOL, REST, SOL, RA, REST, RA, SOL, REST, SOL, MI, SOL, REST, 
SOL, MI, REST, MI, RE, RE, SOL, REST, SOL, RA, REST, RA, SOL, REST, SOL, MI, 
SOL, MI, RE, MI, DO, DO, EOS};
int time_3[] = {N4, R, N4, N4, R, N4, N4, R, N4, N2, N4, R, N4, N4, R, N4, N2, 
N4, N4, R, N4, N4, R, N4, N4, R, N4, N2, N4, N4, N4, N4, N2, N4};

// 각 음계에 따라 켜지는 LED
char LED[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00};

// 음계를 내기 위한 인터럽트 서비스 루틴
ISR(TIMER0_OVF_vect) // TCNT0값에 오버플로우가 발생하면 실행됨 
{
   TCNT0 = f_table[tone]; // 오버플로우가 발생한 후 다시 TCNT0값 해당음계로 초기화
   if (state == ON) // on 상태이면
   {
      PORTB |= 1 << 4; // 4번핀에 1을 입력
      state = OFF; // 상태교환
   }
   else // off 상태이면
   {
      PORTB &= ~(1 << 4); // 4번핀에 0대입
      state = ON; // 상태교환
   }
}

ISR(INT4_vect) // 스위치를 눌러 falling edge가 감지되면 인터럽트서비스루틴이 실행됨
{
   _delay_ms(100); // 디바운싱
   EIFR = 1 << 4; // 비트클리어
   if ((PINE & 0x10) == 0x10) return; // 스위치가 눌린게 아니면
   if (now_song == SONG_1) // 현재 1번 노래인 경우
   {
      // 2번 노래로 교환 후 인덱스 초기화
      now_song = SONG_2;
      song_index = 0;
   }
   else if (now_song == SONG_2) // 현재 2번 노래인 경우
   {
      // 3번 노래로 교환 후 인덱스 초기화
      now_song = SONG_3; 
      song_index = 0;
   }
   else // 현재 3번 노래인 경우
   {
      // 1번 노래로 교환 후 인덱스 초기화
      now_song = SONG_1; 
      song_index = 0;
   }
}

void playSong(int song[], int time[]){ // 노래를 플레이하기 위한 함수
   TCNT0 = f_table[song[song_index]];  // 인덱스 값에 해당하는 TCNT0값 대입
   tone = song[song_index]; // 인덱스에 해당하는 음 대입
   PORTA = LED[tone]; // 인덱스에 해당하는 LED 출력
   _delay_ms(time[song_index]); // 인덱스 값에 해당하는 시간만큼 딜레이
   song_index += 1; // 인덱스 값 증가
}

int main(void)
{
   DDRA = 0xff; // A포트를 출력으로 설정
   DDRB |= 0x10; // B포트의 4번핀을 출력으로 설정
   DDRE = 0x00; // E포트를 입력으로 설정
   sei(); // 전역 인터럽트 활성화
   EIMSK = 0x10; // INT4 활성화
   EICRB = 0x02; // INT4 falling edge일 시 발생
   TIMSK = 0x01; // TIMER0 활성화
   TCCR0 = 0x03; // 프리스케일러를 32분주로 설정
   TCNT0 = f_table[song_1[song_index]]; // 1번 노래의 첫음의 TCNT0값 대입
   
    while(1){
        do{
        if(now_song==SONG_1){ // 1번 노래일 경우
            playSong(song_1,time_1); // 1번 노래 재생
        }
        else if(now_song==SONG_2){ // 2번 노래일 경우
            playSong(song_2,time_2); // 2번 노래 재생
        }
        else{ // 3번 노래일 경우
            playSong(song_3,time_3); // 3번 노래 재생
        }
        }while (tone!=EOS); // 노래가 종료될때 까지 진행
    }
}