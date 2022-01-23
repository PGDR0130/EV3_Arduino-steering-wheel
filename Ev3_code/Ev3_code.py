#!/usr/bin/env python3
import ev3dev2 as ev3
from ev3dev2.motor import LargeMotor, OUTPUT_A, OUTPUT_B, OUTPUT_C, OUTPUT_D, SpeedPercent, SpeedRPM
import serial
import time 

ser = serial.Serial("/dev/rfcomm0", 9600)

L1 = LargeMotor(OUTPUT_A)
L2 = LargeMotor(OUTPUT_B)


while (True) :
    L1_degree = round(L1.degrees) + 450
    if L1_degree > 900: L1_degree = 900
    elif L1_degree < 0: L1_degree = 0
    ser.write((str(L1_degree) + '\n').encode())
