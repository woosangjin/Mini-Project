#define  F_CPU 16000000UL

#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

unsigned char oc=100;				//�񱳰�
unsigned char pwm = 20;				//��Ƽ ����
unsigned char cnt=1;		//����,����	ī����

unsigned int a=70;					//�ʱ�ӵ�
unsigned int b=0;					//�񱳰�
unsigned int c=0;					//0:����1:���� 2:���� 
unsigned int dir=0;					//���� 1����	2����	3��	4��

unsigned char rx_data;

//////////����/////////
ISR(USART0_RX_vect)			//rx ���ͷ�Ʈ
{
	rx_data = UDR0;
}
//////////����//////////

////////pwm///////////////
ISR(TIMER0_COMP_vect)		//pwm ���� ���� �� ����
{
	if(c>=1)						//����
	{
		if(pwm>oc)
		{
			PORTA=0x00;
		}
		else
		{
			if(dir==1)				//����
			{
				PORTA=0x11;
			}
			if(dir==2)				//����
			{
				PORTA=0x33;
			}
			if(dir==3)				//��
			{
				PORTA=0x31;
			}
			if(dir==4)				//��
			{
				PORTA=0x13;
			}
		}
		if(++pwm>200)				//��Ƽ ���� �κ�
		{
			pwm=20;
		}
	}
}

void pwm_init()
{
/////////////Ÿ�̸�//////////	
	TCCR0 |= 0x0A; 		//CTC��� ���������� = CK/8
	TCNT0 = 0x0;		//ī���� �������� Ŭ����
	OCR0 = 20;			//1usec
	TIMSK |= 0x02;		// output compare match ���ͷ�Ʈ Ȱ��ȭ
	TIFR = 0x00;		//��� ���ͷ�Ʈ �÷��� Ŭ����
////////////Ÿ�̸�/////////////
}

///////////uart////////////
void uart_init()
{
	UBRR0H = 103>>8;		//baud rate = 9600
	UBRR0L = 103;			//baud rate = 9600
	UCSR0A = 0x00;			// ���ۼӵ� 1��
	UCSR0B |=  0x90;		//rx complete ���ͷ�Ʈ Ȱ��ȭ,rx���
	UCSR0C |= 0x06;			// parity x, 1 stop bit, 8 data bits

	rx_data = UDR0;			//UDR0 �����
}

///////////uart/////////////
void speed_up()					//����
{
	if((c==1)&&(1<=cnt<5))		//cnt�� �ӵ� ī��Ʈ
	{
		for(oc=b;oc<a;oc++)
		{
			_delay_ms(1);
			if(oc>=a-1)
			{
				b=a+1;			//��Ƽ �ʱ�ȭ����
			}
		}
	}
}
void speed_down()				//����
{
	if((c==2)&&(1<=cnt<5))		//cnt�� �ӵ� ī��Ʈ
	{
		for(oc=a;oc>b;oc--)
		{
			_delay_ms(1);
			if(oc>=b-1)
			{
				a=b+2;			//��Ƽ �ʱ�ȭ ����
			}
		}
	}
}

unsigned char control(unsigned char value)
{

	unsigned static int unstart=0;				//���ӵ��� ���� ����

	if(value=='q')				//���
	{
		c=1;						//����
		a=70;						//start
		unstart=1;

		if(cnt<2)					//���Ӹ��
		{
			dir=1;						//����
		}
	}

	if(value=='w')				//����
	{
		if(unstart==1)				//���ӵ� ���� ����
		{
			if(cnt<5)				//4�ܺ���
			{
				c=1;					//����
				a=a+20;					//���� ���� ����
				cnt++;					//Ƚ�� ī����
			}
		}
		rx_data=0;					//rx_data �ʱ�ȭ
	}

	if(value=='e')				//����
	{
		if(cnt>1)
		{
			c=2;						//����
			b=b-20;						//���� ���� ����
			cnt--;						//Ƚ�� ī����
		}
		rx_data=0;					//rx_data �ʱ�ȭ
	}
	if(value=='r')				//����
	{
		c=0;						//����
		cnt=1;						//������ Ƚ�� �ʱ�ȭ
		b=0;
		dir=0;
		unstart=0;
		PORTA=0x00;					//pwm����
	}
	if(value=='t')				//����
	{
		c=1;						//����
		a=70;						//start
		unstart=1;

		if(cnt<2)					//���Ӹ��
		{
			dir=2;					//����
		}
	}
	if(value=='y')				//��
	{
		c=1;						//����
		a=70;						//start
		unstart=1;

		if(cnt<2)					//���Ӹ��
		{
			dir=3;					//����
		}
	}
	if(value=='u')				//��
	{
		c=1;						//����
		a=70;						//start
		unstart=1;

		if(cnt<2)					//���Ӹ��
		{
			dir=4;					//����
		}
	}
}

/////////////main///////////
void main()
{

	DDRA=0xff;			//����
	
	PORTE=0xff;			//E��Ʈ �ʱ�ȭ

	SREG = 0x0;			//�۷ι� ���ͷ�Ʈ ��Ȱ��ȭ

	pwm_init();			//PWM �Լ�
	uart_init();		//UART �Լ�

	SREG = 0x80;		//�۷ι� ���ͷ�Ʈ Ȱ��ȭ

	while(1)
	{
		control(rx_data);
		speed_up();						// ��Ƽ ��
		speed_down();					//��Ƽ �ٿ�
	}
}