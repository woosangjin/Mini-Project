# 보드 GO

  - 몸의 기울기를 센싱하여 기동하는 전동스케이트보드
  - [보드GO팀 유튜브영상](https://www.youtube.com/watch?v=NZgVnm5m07w)
  
## 개발환경
  - 개발 tool : avr studio (스케이트보드), arduino ide(자이로센서)
  - 사용 mcu : atmega128(스케이트보드), arduino micro(자이로센서)
  - 기타 : bldc모터, esc(bldc 모터드라이버) hc06(블루투스모듈), mpu6050(자이로센서), 압력센서
  
  
### 설명
  - atmega128로 압력센서 2개의 값을 입력받음(압력센서 2개는 양발로 하나씩 밟는상태)
  - arduino micro로 자이로센서의 값을 입력받음(자이로센서는 사용자의 상체에 부착)
  - arduino micro에서 받은 자이로센서값을 블루투스 모듈을 통해 atmega128로 tx
  - atmega128에서 자이로센서값과 압력센서2개의 값을 연산하여 몸의 기울기를 계산하여 bldc모터를 제어한다.
 
#### 첨부파일
  - 최종PPT자료.pptx -> 프로젝트 종료후 발표자료
  - 자이로센서.c -> arduino micro 소스코드(몸에 부착된 part)
  - 전동보드 기획자료 -> 프로젝트 기획 발표자료
  - 최종.c -> atmega128 소스코드(보드 part)
