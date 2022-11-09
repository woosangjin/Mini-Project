# 스마트폰으로 제어하는 RC탱크
  - 시중에서 판매중인 유선 RC에 AT계열 MCU를 사용하여 스마트폰으로 제어할 수 있도록 구성

## 개발환경
  - 개발tool : avr studio
  - 사용 mcu : attiny2313
  - 기타 : 블루투스모듈, dc모터드라이버, 보조배터리
  
### 설명
  - 소형 mcu인 attiny2313을 사용하여 간단히 회로를 직접구성
  - 시리얼통신으로 블루투스 모듈을 제어하여 스마트폰과 연동
  - pwm 2port를 사용하여 실제 탱크처럼 기동할 수 있도록 구성 
  - 스마트폰 앱은 앱스토어에서 다운받아 사용
  - 작업일시 2016년 7월
  - 작업기간 3일

### 첨부파일
  - RC.c -> 최종소스파일
  - 사진1 -> 이미지파일
  - 사진2 -> 이미지파일