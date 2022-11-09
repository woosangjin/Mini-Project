#define F_CPU 8000000UL
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((( F_CPU / 16) + ( USART_BAUDRATE / 2) ) / ( USART_BAUDRATE )) - 1)
#include <avr/io.h>
#include <avr/interrupt.h>

void ADC_init();
void CDS_chk();
void PWM_init();
//void PWM_run(int per);


#define rate 1
#define size 1
#define P_GAIN		47*size		
#define I_GAIN		25*size	
#define D_GAIN		0.05*size	
#define DT			0.125	
#define LED_PERSENTAGE 0
#define LED_ONTIME LED_PERSENTAGE*10

int sec_cnt = 0;
unsigned char data = 0x00;
int ADC_res = 0;
unsigned int on_time = 0;
unsigned int off_time = 0;

int mode = 0;
int led_add = 0x00;
int cnt = 0;

int adc_read =0;
int ON_OCR=0;
int OFF_OCR=0;

double save=0;

#define target_mA  200
#define target_mV   (target_mA*0.05)
#define target_adc  ((int)((target_mV/4.72)))

int state=1;
int ton=0;
void PWM_init();
void calc();
void CDS_chk();


void delay_us(unsigned int time_us)
{
	register unsigned int i;
	for(i = 0; i < time_us; i++)
	{
		 asm volatile("PUSH R0 ");
		 asm volatile("POP R0 ");
	}
}

void delay_ms(unsigned int time_ms)
{
	register unsigned int i;
	for(i = 0; i < time_ms; i++)
	{
		delay_us(250);
		delay_us(250);
		delay_us(250);
		delay_us(250);
	}
}

void uart_init()
{
	/*unsigned int ubrr = BAUD;
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)(ubrr);
	UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);	// RX,TX 사용가능, RX complete 인터럽트 사용가능
	UCSRC = 0x86;	//8 bit 셋팅, 동기모드*/
	DDRD = 0xFE;	//RX,Tx 부분 및 외부인터럽트(ID)부분
	UCSRB = 0x18;    // RXEN = 1, TXEN = 1
	UCSRC= (1 << UCSZ0 ) | (1 << UCSZ1 );
	UCSRC|=0x80;
    UBRRH = 51>>8;
	UBRRL = 51;        // 8MHz에서 9600 bps
	UCSRB |= 0x80;    // RXC interrupt 허용

}


void PWM_run(int per)
{
	/*
	int onper=0;
	int offper=0;

	
	onper=per*100;
	offper= 10000 - onper;

	PORTB|=0x01;
	delay_us(onper);
	PORTB&=0xFE;
	delay_us(offper);
	*/
	ton = per;
//	toff = 100 - per;

}

int pid(double res)
{
	static  double prev = 0;
	static  double stack = 0;
	double p,i;
	double d;
	int ret;
	p = P_GAIN * res;
	d = (((D_GAIN *(res-prev))/DT));
	prev = res;
	stack += res*DT;
	i = I_GAIN *stack;
	//i = (I_GAIN * res * DT) + stack;
	//stack += i;
	ret = p+d+i;
/*

	p = P_GAIN * res;
	d = ((unsigned int)((D_GAIN *(res-prev))/DT));
	i = (I_GAIN * res * DT) + stack;
	
	prev = res;
	stack += i;
	ret = ((int)((p+i+d)*0.526));

	return ret;
*/
	return ret;
}
void ADC_init() // 동작 확인 
{
	ADMUX = 0b00000000;	// ADC는 6번에 CDS센서가 연결되어 있고, 0번에는 R34(0.05옴) 부분에 윗단에 연결되어있어 전류측정이 가능하다.
	ADCSRA = 0b10111111;// 프리 런닝 모드이며 128분주비가 되어있다. 
	delay_us(150);		// 첫번째 셋팅후 완전히 끝나려면 25갯수의 클럭이 필요하므로 충분히 딜레이가 필요
	ADCSRA |= 0b01000000;
	delay_ms(1000);
	//만약 2개를 모두 사용할려면 프리런닝 모드가 아닌 싱글모드로 이용해야하며 실행때 마다 ADMUX를 바꾸어 주어야만 한다.
	// ADCSRA의 5번비트를 0으로 하고 싱글에서 일일히 ADC Start를 해야하며 flag비트를 확인해야만 한다.
}

void calca()
{
	//double adc_i;
	//double adc_v;


	int putt[10] = {0};
	int zz = 0;
	//ADMUX = 0b11010110;
	//ADCSRA = 0b10011111;
	//delay_us(150);
	//ADCSRA |= 0x40;
	//for(;0b00100000!=(ADCSRA&0b00100000););
	//delay_ms(20);
	//ADCSRA |= 0b01000000;
	//delay_ms(1);

	//adc_read = ADC;
	
	putt[0] = ((0b0000000001&adc_read)<<1);
	putt[1] = ((0b0000000010&adc_read));
	putt[2] = ((0b0000000100&adc_read)>>1);
	putt[3] = ((0b0000001000&adc_read)>>2);
	putt[4] = ((0b0000010000&adc_read)>>3);
	putt[5] = ((0b0000100000&adc_read)>>4);
	putt[6] = ((0b0001000000&adc_read)>>5);
	putt[7] = ((0b0010000000&adc_read)>>6);
	putt[8] = ((0b0100000000&adc_read)>>7);
	putt[9] = ((0b1000000000&adc_read)>>8);


	PORTC = 0x00;
	delay_ms(1);
	PORTC = 0x02;
	delay_ms(1);
	PORTC = 0x00;
	delay_ms(1);
	PORTC = 0x02;
	delay_ms(1);

	//UDR=0xff;
	
	while(1)
	{
		
		PORTC = putt[zz];
		delay_ms(10);
		zz++;
		if(zz==10)
		break;
	}
	PORTC = 0x02;
	delay_ms(1);
	PORTC = 0x00;
	delay_ms(1);
	PORTC = 0x02;
	delay_ms(1);
	PORTC = 0x00;
	delay_ms(1);
	//adc_v=4.7295*adc_read;

	
	//adc_i=adc_v/0.05;
	//ADCSRA &= 0b10111111;

	//return adc_read;
}


ISR(TIMER1_OVF_vect)
{
		PORTB |= 0b00000100;
		PORTB |= 0b00000001;
		//PORTB |= 0x01;
		//PORTC = 0x1e;
		//PORTB|=0x01;

}

ISR(TIMER1_COMPA_vect)
{
		//PORTB|=0x01;
		//PORTC = 0x00;
		//PORTB &=0xfe;
		PORTB &=0b11111011;
		PORTB &=0b11111110;

	//static int count=0;
	/*
	PORTB&=0xFE;
	delay_us(offper);
	PORTB|=0x01;
	delay_us(onper);
	*/
	/*
	if(!state)
	{
		if(100==count)
		{
			count =0;
			state=1;
		}
		else if(ton<count)
		{
			PORTB&=0xfe;

		}
		else
		{
			PORTB|=0x01;
		}
		count++;
	}*/


}

void civa(int pid_ret)
{
	if((LED_ONTIME + pid_ret)>1000){
		OCR1A = 1000;
		}
	else if((LED_ONTIME + pid_ret)<1)
	{
		OCR1A = 1;
		}
	else{
		OCR1A = ((LED_ONTIME+pid_ret));
	//PORTC=0b10;
		}

}



void PWM_init()
{
/*
	TCCR1A = (0<<WGM10)|(0<<COM1A0);
	TCCR1B = (1<<WGM12)|(1<<CS10);
	OCR1A = LED_OFFTIME;
	TIMSK = (1<<OCIE1A);
*/
/*
  	TCCR1A = 0b10000010;
    TCCR1B = 0b00011001;
	//ICR1L = 0b11011101;
	//ICR1H = 0b11111111;
	ICR1 = 65501;
    TCNT1  = 0x00;
	OCR1A  = LED_ONTIME;
	TIMSK = 0b00010100;
*/
  	TCCR1A = 0b10000010;
    TCCR1B = 0b00011011;
	//ICR1L = 0b11011101;
	//ICR1H = 0b11111111;
	ICR1 = 1000;
    TCNT1  = 0x00;
	OCR1A  = LED_ONTIME;
	TIMSK |= 0b00010100;
}
/*
ISR(TIMER2_OVF_vect)
{
	static int aaa = 0;
	if((aaa%2)==1)
	{
		PORTC = 0b10;
	}
	else if((aaa%2)==0)
	{
		PORTC = 0b00;
	}
	aaa++;

}*/
ISR(TIMER2_COMP_vect)
{
	//civa((int)(pid(target_adc-calca())));
		//PORTC|=0b00000100;
		//delay_ms(1);
		//PORTC&=0b11111011;
		sec_cnt++;
		if(sec_cnt==8)
		{
			PORTC|=0b00000100;
			delay_ms(1);
			PORTC&=0b11111011;
			civa((pid(target_adc-adc_read)));
			sec_cnt = 0;
		}
}


void timer2_init()
{
	ASSR=0x00;
	TCCR2=0b10001111;
	OCR2=122;
	TCNT2=0x00;
	TIMSK|=0x80;
}

ISR(ADC_vect)
{
	static int test = 0;
	static int cnt = 0;
	int adc = 0;
	adc = ADC;
		//	PORTC|=0x2;
			//PORTC|=0b00000010;
	//if(mode==0){
			//PORTC&=0b11111011;
		test += adc;
		cnt++;
		if (cnt>624){
			//PORTC|=0b00000100;
			adc_read = ((test / 625));
			cnt = 0;
			test=0;
			//mode=1;
			}
		//	PORTC&=0b11111011;
//	}
		//	PORTC&=0b11111101;
	//ADCSRA &= 0b10111111;
}


int main(void)
{
	cli();
	DDRB = 0x07;	// LED PWM0번포트와 나머지는 비교기부분
    DDRC = 0x1e;	//4개의 LED를 아웃풋으로 설정
	//PORTC = 0x1e;	// 4개의 LED ON
	//PORTB &=0xfd;

//	PORTB = 0x00;
	ADC_init();
	PWM_init();
	timer2_init();
//	uart_init();
//	calc();
	sei();
	//
	//OCR1A = 1000;
	while(1)
	{
	
		//if(mode==1)
		//{
			//PORTC|=0x4;
			//civa((pid(target_adc-adc_read)));
			//calca();
			delay_ms(124);
			//mode = 0;
		//	PORTC&=0b11111011;
			//delay_ms(1);
		//}
		//PORTC|=0x4;
		
//		delay_ms(1000);
//		OCR1A = 1000;
/*
		civa((int)(pid(target_adc-adc_read)));
	
		//PORTC = 0b10;
		//save=calca();
		//calca();
		//delay_ms(500);
		//civa(pid((int)(target_mA-calca())));
		//ADCSRA |= 0b01000000;
		//delay_ms(10);
		
		civa((int)(pid(target_adc-adc_read)));
		
		
		//PORTC = 0b00;
		delay_us(500);
		//OCR1A = 1000;
		*/
		/*
		calca();
		
		delay_ms(500);
		OCR1A = 200;
		calca();
		delay_ms(500);
		OCR1A = 300;
		calca();
		delay_ms(500);
		OCR1A = 400;
		calca();
		delay_ms(500);
		OCR1A = 500;
		calca();
		delay_ms(500);
		OCR1A = 600;
		calca();
		delay_ms(500);
		OCR1A = 700;
		calca();
		delay_ms(500);
		OCR1A = 800;
		calca();
		delay_ms(500);
		OCR1A = 900;
		calca();
		delay_ms(500);
		OCR1A = 1000;
		calca();
				*/
	}
	return 0;
}

