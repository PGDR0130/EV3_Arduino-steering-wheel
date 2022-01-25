#!/usr/bin/env python3
import ev3dev2 as ev3 
from ev3dev2.motor import LargeMotor, OUTPUT_A, OUTPUT_B, OUTPUT_C, OUTPUT_D, SpeedPercent, SpeedRPM
import serial
import time
import os
from ev3dev2.auto import Button





blue_check = False
try:
    # auto bind bluetooth device 'maker' is the password 
    os.system('echo maker | sudo -S rfcomm bind 0 FC:A8:9A:00:50:64')
    ser = serial.Serial("/dev/rfcomm0", 9600)
    blue_check = True
    print("Arduino connected")
except serial.serialutil.SerialException:
    print("Arduino not connected")
    blue_check = False

"""
L2 --
   --R2
L1 --
   --R1
(wheel)
"""
R1 = LargeMotor(OUTPUT_A)
R2 = LargeMotor(OUTPUT_B)
L1 = LargeMotor(OUTPUT_C)
L2 = LargeMotor(OUTPUT_D)


while (True) :
    L1_speed = L1.speed
    R1_speed = R1.speed
    # print(L1_speed, ' ', R1_speed)

    if(-10 < L1_speed and L1_speed < 10 and -10 < R1_speed and R1_speed < 10):
        L1.stop_action = 'brake'
        R1.stop_action = 'brake'
    else:
        R1.stop()
        L1.stop()    
        L1.stop_action = 'coast'
        R1.stop_action = 'coast'

    # print(L1.state, R1.state)

    # rest angle when press ev3 enter button
    if(Button().enter):
        R1.reset()
        L1.reset()

    # degree update
    L1_degree = round(L1.degrees) + 450
    R1_degree = round(-R1.degrees) + 450
    avg_degree = (L1_degree + R1_degree)//2
    if avg_degree > 900: 
        avg_degree = 900

    elif avg_degree < 0: 
        avg_degree = 0
    if blue_check == True :
        ser.write((str(L1_degree) + '\n').encode())
    print('degree : ',avg_degree)