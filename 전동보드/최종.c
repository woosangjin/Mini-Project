#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile unsigned char rx_data;
int sel=0;

ISR(USART0_RX_vect)      //자이로값수신 인터럽트
{
	rx_data = UDR0;
}

void adc_init()             //압력센서
{
	ADMUX = 0x20;	//AREF,ADC Left
	ADCSRA = 0x84;	//prescaler:16
}

void pwm_init()           //모터제어
{
	TCCR1A = 0x82;	//14Fast PWM
	TCCR1B = 0x1A;		//CLK/8
	TCCR1C = 0x00;
	ICR1 = 39999;		//16000000/N*(1+39999)=50hz

	TCNT1 = 0x0000;
}

void uart_init()              //bluetooth통신
{
	UBRR0H = 103>>8;
	UBRR0L = 103;
	UCSR0A |= 0x00;
	UCSR0B |= 0x98;
	UCSR0C = 0x06;
	
	rx_data = UDR0;	
}

int main()
{
	int fsr_num1 = 0, fsr_num2 = 0, i = 0;
	int result=0, fsr_sensor[] = {1,2};
	int speed =1200;
	
	DDRB = 0xff;
	DDRG = 0xff;
	DDRF = 0X00;

	PORTE=0xff;

	SREG = 0x0;

	pwm_init();
	uart_init();
	adc_init();

	SREG = 0x80;
	
	while(1)
	{		
		OCR1A=1000;	//bldc시동
		
		if(rx_data=='a')
		{
			ADCSRA = ADCSRA | 0x60;

			if(fsr_sensor[i] == 1 )	//ADC0
			{
				ADMUX = 0x20;
				fsr_num1 = ADCH;
				ADMUX = 0x21;

			}
			else if(fsr_sensor[i] == 2)	//ADC1
			{
				ADMUX = 0x21;
				fsr_num2 = ADCH;
				ADMUX = 0x20;
			}

			result = fsr_num1 - fsr_num2;
			if(i < sizeof(fsr_sensor)-1) i++;
			else i = 0;
			
			sel=((fsr_num1>10)&(fsr_num2>10));	//한쪽발만 올렸을때 출발방지
			
			if(sel==1)
			{
				if(result>5)
				{
					OCR1A=speed=speed+10;
					_delay_ms(25);
					if(speed>=2400)
					{
						speed=2400;
					}
					if(rx_data=='b')
					{
						OCR1A=speed=speed-30;									
					}
				}
				if(result<0)
				{
					OCR1A=speed=speed-10;
					_delay_ms(10);
					if(speed<=1000)
					{
						speed=1000;
					}
				}
			}
		}
	}
}
