#define  F_CPU 16000000UL

#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

unsigned char oc=100;				//비교값
unsigned char pwm = 20;				//듀티 조절
unsigned char cnt=1;		//가속,감속	카운팅

unsigned int a=70;					//초기속도
unsigned int b=0;					//비교값
unsigned int c=0;					//0:정지1:가속 2:감속 
unsigned int dir=0;					//방향 1전진	2후진	3좌	4우

unsigned char rx_data;

//////////수신/////////
ISR(USART0_RX_vect)			//rx 인터럽트
{
	rx_data = UDR0;
}
//////////수신//////////

////////pwm///////////////
ISR(TIMER0_COMP_vect)		//pwm 파형 생성 및 방향
{
	if(c>=1)						//동작
	{
		if(pwm>oc)
		{
			PORTA=0x00;
		}
		else
		{
			if(dir==1)				//전진
			{
				PORTA=0x11;
			}
			if(dir==2)				//후진
			{
				PORTA=0x33;
			}
			if(dir==3)				//좌
			{
				PORTA=0x31;
			}
			if(dir==4)				//우
			{
				PORTA=0x13;
			}
		}
		if(++pwm>200)				//듀티 조절 부분
		{
			pwm=20;
		}
	}
}

void pwm_init()
{
/////////////타이머//////////	
	TCCR0 |= 0x0A; 		//CTC모드 프리스케일 = CK/8
	TCNT0 = 0x0;		//카운터 레지스터 클리어
	OCR0 = 20;			//1usec
	TIMSK |= 0x02;		// output compare match 인터럽트 활성화
	TIFR = 0x00;		//모든 인터럽트 플래그 클리어
////////////타이머/////////////
}

///////////uart////////////
void uart_init()
{
	UBRR0H = 103>>8;		//baud rate = 9600
	UBRR0L = 103;			//baud rate = 9600
	UCSR0A = 0x00;			// 전송속도 1배
	UCSR0B |=  0x90;		//rx complete 인터럽트 활성화,rx허용
	UCSR0C |= 0x06;			// parity x, 1 stop bit, 8 data bits

	rx_data = UDR0;			//UDR0 비워줌
}

///////////uart/////////////
void speed_up()					//가속
{
	if((c==1)&&(1<=cnt<5))		//cnt는 속도 카운트
	{
		for(oc=b;oc<a;oc++)
		{
			_delay_ms(1);
			if(oc>=a-1)
			{
				b=a+1;			//듀티 초기화방지
			}
		}
	}
}
void speed_down()				//감속
{
	if((c==2)&&(1<=cnt<5))		//cnt는 속도 카운트
	{
		for(oc=a;oc>b;oc--)
		{
			_delay_ms(1);
			if(oc>=b-1)
			{
				a=b+2;			//듀티 초기화 방지
			}
		}
	}
}

unsigned char control(unsigned char value)
{

	unsigned static int unstart=0;				//가속도로 시작 방지

	if(value=='q')				//출발
	{
		c=1;						//가속
		a=70;						//start
		unstart=1;

		if(cnt<2)					//저속모드
		{
			dir=1;						//방향
		}
	}

	if(value=='w')				//가속
	{
		if(unstart==1)				//가속도 시작 금지
		{
			if(cnt<5)				//4단변속
			{
				c=1;					//가속
				a=a+20;					//가속 비율 조정
				cnt++;					//횟수 카운팅
			}
		}
		rx_data=0;					//rx_data 초기화
	}

	if(value=='e')				//감속
	{
		if(cnt>1)
		{
			c=2;						//감속
			b=b-20;						//감속 비율 조정
			cnt--;						//횟수 카운팅
		}
		rx_data=0;					//rx_data 초기화
	}
	if(value=='r')				//정지
	{
		c=0;						//정지
		cnt=1;						//가감속 횟수 초기화
		b=0;
		dir=0;
		unstart=0;
		PORTA=0x00;					//pwm정지
	}
	if(value=='t')				//후진
	{
		c=1;						//가속
		a=70;						//start
		unstart=1;

		if(cnt<2)					//저속모드
		{
			dir=2;					//방향
		}
	}
	if(value=='y')				//좌
	{
		c=1;						//가속
		a=70;						//start
		unstart=1;

		if(cnt<2)					//저속모드
		{
			dir=3;					//방향
		}
	}
	if(value=='u')				//우
	{
		c=1;						//가속
		a=70;						//start
		unstart=1;

		if(cnt<2)					//저속모드
		{
			dir=4;					//방향
		}
	}
}

/////////////main///////////
void main()
{

	DDRA=0xff;			//모터
	
	PORTE=0xff;			//E포트 초기화

	SREG = 0x0;			//글로벌 인터럽트 비활성화

	pwm_init();			//PWM 함수
	uart_init();		//UART 함수

	SREG = 0x80;		//글로벌 인터럽트 활성화

	while(1)
	{
		control(rx_data);
		speed_up();						// 듀티 업
		speed_down();					//듀티 다운
	}
}