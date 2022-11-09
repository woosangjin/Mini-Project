#include <UTFT.h>
#include <DueTimer.h>

extern uint8_t SmallFont[];
#define BUFFER_SIZE 1000

UTFT myGLCD(1,38,39,40,41);

uint16_t ADC_record_real[300] = {0};
uint16_t ADC_record[300] = {0};
uint16_t ADC_record_pre[300] = {0};
float ADC_temp;
float ADC_input_averg = 0, ADC_off=0;
float ADC_min=0, ADC_max =0;
volatile char state = 0x01;
unsigned int hz_scale_real = 1;
unsigned int hz_scale = 1;
float hz_choice_lcd_print[] = {500, 100, 1, 10 ,100};
unsigned int test_int = 1;
int text = 0;
int text2 = 0;
float voltage_choice_lcd_print[] = {10 , 7.5 , 5 , 2.5 , 1 , 0.5};
int voltage_scale = 1;
float record_voltage_trans = 0;
unsigned int textssss=0;
int cnt = 0;
int start_su = 0;
int secondhandler_cnt = 0;
float ADC_input_averg_real=0;

int secondhandler_stat(int a)
{
  static int temp = 0;
  if(a==2);
  else
    temp = a;
  return temp;
}
void firstHandler(){
      static int i = 0;
      if(i == 300)
      {
        Timer4.stop();
        secondhandler_stat(1);
        i=0;
      }
      else
      ADC_record_real[i++] = analogRead(A0);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  //Timer3.attachInterrupt(firstHandler).start(100); // Every 500ms
  //Timer4.attachInterrupt(secondHandler).start(500000);
  //Timer3.attachInterrupt(firstHandler).start((int)(hz_choice_lcd_print[hz_scale]*1000)/270);
  //Timer4.attachInterrupt(secondHandler).start((int)(hz_choice_lcd_print[hz_scale]*1000)/270);
  //Timer3.attachInterrupt(firstHandler).setFrequency(10000).start();
  //Timer5.attachInterrupt(thirdHandler).setFrequency(10);
  pinMode(12,OUTPUT);
  pinMode(A0,INPUT);
// Setup the LCD
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  myGLCD.clrScr();
  graph_init();
  adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST); 
  REG_ADC_MR = 0x00000000;
  analogReadResolution(12); 
  Serial.print(REG_ADC_MR,HEX);
  pinMode(3, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(3), Stop_adc, FALLING); //Stop 
  ADC_offset();
  analogWrite(11,230);
  //Timer3.attachInterrupt(thirdHandler).setFrequency(2).start();
  Timer4.attachInterrupt(firstHandler).setFrequency(54000000).start();
}
void loop() {
  if(secondhandler_stat(2)>0)
  {
    draw();
    secondhandler_stat(0);
    ADC_draw2();
    data_print();
    ADC_analog_read_and_volt_scale_choice(voltage_scale);
    choice_Hz();
  }
}
void choice_Hz()
{
   hz_scale= hz_scale_real;
  switch(hz_scale)
  {
    case 0 :Timer4.setFrequency(155000).start(); break;
    case 1 :Timer4.setFrequency(32400).start(); break;
    case 2 :Timer4.setFrequency(3240).start(); break;
    case 3 :Timer4.setFrequency(324).start(); break;
    case 4 :Timer4.setFrequency(32).start(); break;
  }
}

void ADC_analog_read_and_volt_scale_choice(unsigned voltage_choice)
{
  int x_place = 0;
  float max_measure_volt = voltage_choice_lcd_print[voltage_choice];
  float measure_volt_step = (max_measure_volt / 5);
  myGLCD.setColor(255,255,255);
  myGLCD.printNumF(max_measure_volt,1,x_place,9);
  myGLCD.printNumF(measure_volt_step*4,1,x_place,((int)(211-9)/5)+9);
  myGLCD.printNumF(measure_volt_step*3,1,x_place,(((int)(211-9)/5)*2)+9);
  myGLCD.printNumF(measure_volt_step*2,1,x_place,(((int)(211-9)/5)*3)+9);
  myGLCD.printNumF(measure_volt_step,1,x_place,(((int)(211-9)/5)*4)+9);
  myGLCD.print("0",x_place,211);
  switch(voltage_choice)
  {
    case 0 : record_voltage_trans=0.5; break;
    case 1 : record_voltage_trans=0.75; break;
    case 2 : record_voltage_trans=1;break;
    case 3 : record_voltage_trans=2; break;
    case 4 : record_voltage_trans=5; break;
    case 5 : record_voltage_trans=10; break;
  }
}

void ADC_draw2()
{
  if(state==1)
  {
    ADC_min = (ADC_min < ADC_temp) ? ADC_min : ADC_temp;
    ADC_max = (ADC_max > ADC_temp) ? ADC_max : ADC_temp;
    ADC_max = ADC_max*1.225/1000;
    ADC_min = ADC_min*1.225/1000;
  }
}

void graph_init()
{
  myGLCD.setColor(0,0,255);
  myGLCD.print("Project Lab 3",80, 2);
  myGLCD.setColor(255,0,0); //(red,green,blue)
  myGLCD.print("v:",3 , 228);
  myGLCD.print("ms:",60 , 228);
  myGLCD.print("max:",130 , 228);
  myGLCD.print("min:",210 , 228);
  myGLCD.setBackColor(0,0,0);   //255,255,255¸é Èò»ö
  myGLCD.setColor(255,255,255);
  myGLCD.fillRect(9,14,311,220);
}

void draw()
{
  
  if(state==1){
    myGLCD.setColor(255,255,255);
    myGLCD.fillRect(24,14,296,216);
    myGLCD.setColor(0,0,0);
    for(int i = 0 ; i<300; i++)
    {
   ADC_temp = ADC_record_real[i]-ADC_off;
   ADC_input_averg += ADC_temp;
   ADC_record[i] = 217-((ADC_temp*0.0488*record_voltage_trans));
   ADC_record_pre[i] = ADC_record[i];
   
   if(i>0)
   {
   myGLCD.drawLine(i+24,ADC_record[i-1],i+25,ADC_record[i]);
   myGLCD.drawPixel(i+25,ADC_record[i]);
   } 
  }
    ADC_input_averg *= 0.0186;
    ADC_input_averg /=4096; 
    ADC_input_averg_real = ADC_input_averg;
    ADC_input_averg = 0;  
}
}
void data_print()
{
  if(state==1){
  myGLCD.setColor(255,255,255);   
  myGLCD.printNumF(ADC_input_averg_real,2,18,228);
  myGLCD.printNumF(hz_choice_lcd_print[hz_scale],1,86,228);
  myGLCD.printNumF(ADC_max,3,161,228);
  myGLCD.printNumF(ADC_min,3,245,228);
  }
}

void ADC_offset()
{
  for(int i =0; i<100;i++)
  ADC_off +=analogRead(A0);
  ADC_off /= 150;
}

void Stop_adc()
{
  state ^=0x01;
}
