# PID DC전압제어

  - LED에 PWM으로 전압을 공급하고 AD변환으로 전압을 센싱하여 타겟값에 도달할 수 있도록 구성
  
## 개발환경
  - 개발 tool : avr studio
  - 사용 mcu : atmega8
  - 기타 : LED
  
### 설명
  - Atmega8 mcu를 사용하여 pwm으로 led를 제어
  - adc를 이용하여 led의 전압을 센싱
  - adc로 센싱한 전압값을 pid제어기에 입력하여 계산한 수 다시 pwm으로 led를 제어한다.
  - 작업일시 2016년 3월
  - 작업기간 3개월

#### 첨부파일
  - pid.c -> 소스파일
  - 사진1.PNG -> 이미지파일
  - 사진2.PNG -> 이미지파일
