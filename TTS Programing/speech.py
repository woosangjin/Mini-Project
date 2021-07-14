# -*- coding: utf-8 -*-
"""
Created on Wed Jul 14 15:16:17 2021

@author: WSJ
"""
#TTS 프로그래밍
#현재 시간을 입력받아 speech할 수 있다.

#!pip install playsound     #모듈설치필요
#!pip install playsound     #모듈설치필요
from datetime import datetime #시간을 관리하는 모듈

time = datetime.now()   #시간데이터를 받아와 time변수에 저장

#시각화
print("오늘은 {}년 {}월 {}일 입니다.".format(time.year,time.month,time.day))
print("현재 시간은 {}시 {}분 입니다.".format(time.hour,time.minute))

from gtts import gTTS #구글에서 만든 스피치모듈
import playsound #mp3파일을 제어하는 모듈
import os #directory제어 모듈

#청각화
#하나의 변수에 저장하여 코드 간편화
sound = "오늘은 {}년 {}월 {}일 입니다. 현재시간은 {}시 {}분 입니다.".format(time.year,time.month,time.day,time.hour,time.minute)

#저장된 문자열을 MP3로 변환하여 저장
tts=gTTS(text=sound,lang='ko')  #문자열에 한국어 음성 합성
tts.save("time.mp3")   #MP3파일로 저장

playsound.playsound("time.mp3",True)    #playsound 함수를 호출하여 sound변수에 저장되어있는 문자열을 speech

os.remove("time.mp3")   #실행후 mp3파일 제거