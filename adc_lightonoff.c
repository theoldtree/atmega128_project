#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define CDS_10 200 // 실험실 밝기에 따라 설정한 적당한 가상 값
#define CDS_20 380 // 실험실 밝기에 따라 설정한 적당한 가상 값
#define CDS_50 560 // 실험실 밝기에 따라 설정한 적당한 가상 값
#define CDS_80 720 // 실험실 밝기에 따라 설정한 적당한 가상 값
#define CDS_100 900 // 실험실 밝기에 따라 설정한 적당한 가상 값
void init_adc(); // 초기 adc설정 함수
unsigned short read_adc(); // ads로부터 값을 읽는 함수
void show_adc_led(unsigned short data); // adc의 data값에 따라 led를 제어하는 함수
volatile int midnight = 0; // 저녁인지 아닌지를 나타내는 변수

int main()
{
   unsigned short value; // digital 값을 저장하기 위한 변수
   DDRA=  0xff; // LED 포트 출력모드
   DDRF= 0x00; // ADC 포트 입력모드
   init_adc(); // ADC 초기화
   while(1)
   {
      value=read_adc(); // AD변환 시작 및 결과 읽어옴
      show_adc_led(value); // value값에 따라 led 를 제어함
   }
}
void init_adc()
{
   ADMUX=0x40; // 5v 기준 전압 사용, 오른쪽 정렬, ADC0사용
   ADCSRA=0x87; // ADC enable, 한번만 변환, 프리스케일러 128분주
}
unsigned short read_adc() // adc 값을 읽는 함수
{
   unsigned char adc_low, adc_high; // ADCL, ADCH 값을 저장할 변수
   unsigned short value; // char이 1byte기 때문에 2byte값을 저장하기 위한 변수 선언
   ADCSRA |=0x40; // ADC start conversion
   while((ADCSRA&0x10)!=0x10) // ADC변환 완료 검사
   ;
   adc_low=ADCL; // adc 값 저장
   adc_high=ADCH; // adc값 저장
   value=(adc_high<<8)|adc_low; // high 값과 low 값을 합침
   return value; // value값 변환
}
void show_adc_led(unsigned short value) // 읽은 값에 따라 led를 점등하는 함수
{
   if(value<=CDS_20) midnight=1; // 읽정 밝기 이상으로 내려 갔을때 led를 점등함
   if(midnight==1){ // 어두울때 led 모두 작동하고 밝아짐에 따라 led가 서서히 꺼짐
      if(value<=CDS_10) PORTA = 0xff; 
      else if (value <= CDS_20) PORTA =0x3f;
      else if (value <= CDS_50) PORTA =0x0f;
      else if (value<= CDS_80) PORTA=0x03;
      else if (value<= CDS_100) PORTA=0x01;
      else if (value > CDS_100) midnight=0; // 밝기가 밝을때는 낮을 의미
   }
   else if(midnight==0) PORTA=0x00; // 낮일때는 led 모두 off
}