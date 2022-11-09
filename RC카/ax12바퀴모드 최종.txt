#include <mega128.h>
#include <delay.h>
#include <stdio.h>

void send_tail(char line);
unsigned char i, TxData[100],TxData_W[100];           
char Sy_write []={0xff,0xff,0xfe,0x18,0x83,0x1e,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00};   

//motor 3개,line 23
char pos_tbl  []={0xff,0xff,0x00,0x05,0x03,0x1e,0x00,0x00,0x00};   //Write  _Line 9
char id_change[]={0xff,0xff,0x00,0x04,0x03,0x03,0x00,0x00};        //ID     _Line 8
char Led_On   []={0xff,0xff,0x00,0x05,0x03,0x18,0x01,0x01,0x00};   //Led on _Line 9
char CCW []     ={0xff,0xff,0x01,0x07,0x03,0x06,0x00,0x00,0x00,0x00,0x00};



void USART_Putc(char data)//USART_Tx 
{ 
     UCSR0A|=0x40; while(!(UCSR0A&0x20)); UDR0=data; 

}   

void Sy_write_Init(unsigned int pos1,unsigned int speed1,unsigned int pos2,unsigned int speed2,unsigned int pos3,unsigned int speed3,unsigned int pos4,unsigned int speed4)
{

      for(i=0;i<29;i++)TxData_W[i]=Sy_write[i];
      TxData_W[8] =pos1&0xff;TxData_W[9]=pos1>>8;
      TxData_W[10]=speed1&0xff;TxData_W[11]=speed1>>8;
      
      TxData_W[13]=pos2&0xff;TxData_W[14]=pos2>>8;
      TxData_W[15]=speed2&0xff;TxData_W[16]=speed2>>8;
     
      TxData_W[18]=pos3&0xff;TxData_W[19]=pos3>>8;
      TxData_W[20]=speed3&0xff;TxData_W[21]=speed3>>8;    
     
      TxData_W[23]=pos4&0xff;TxData_W[24]=pos4>>8;
      TxData_W[25]=speed4&0xff;TxData_W[26]=speed4>>8;    

      send_tail(28);   
      
}

void USART0_Init(void)//USART0_Init
{  
     PORTE=0x02; DDRE=0x02;
     UCSR0B=0x18; UCSR0C=0x06; UBRR0H=0x00; UBRR0L=0x00;   // 1MHz @ 16MHz
}     


void send_tail(char line) //Check_Sum_Write_mode
{
    for(i=2;i<(line-1);i++)
    {
      TxData[(line-1)]+=TxData[i];
      id_change[(line-1)]+=id_change[i];
      TxData_W[(line-1)]+=TxData_W[i];
      //CCW[(line-1)]+=CCW[i];
    }
      TxData_W[(line-1)]=~TxData_W[line-1];
      TxData[(line-1)]=~TxData[(line-1)];
      id_change[(line-1)]=~id_change[(line-1)];
      //CCW[(line-1)]   =~CCW[(line-1)];
     
     for(i=0;i<28;i++)
     {
         USART_Putc(TxData_W[i]);
     }  
      while(!(UCSR0A & 0x40)); 
  
}


void CCW_1()                        //Led on 토크 off
{
     for(i=0;i<11;i++)TxData[i]=CCW[i]; 
     send_tail(11);     //check sum 
     
     for(i=0;i<11;i++)
     {
      USART_Putc(TxData[i]);
     } 
     while(!(UCSR0A & 0x40));
}  

void main(void)                         
{   
    USART0_Init();           
   
    while (1)                   //delay_ms(500)데이터 처리 시간 필수           
    {       
    
       
 
    Sy_write_Init(0,300,0,500,500,300,300,300);		//속도 1025 넘으면 역회전
      //  CCW_1();
      //  delay_ms(1000);
        
   
    
    }   
}