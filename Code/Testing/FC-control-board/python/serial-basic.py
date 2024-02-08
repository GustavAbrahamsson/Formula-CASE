# %% import
import numpy as np
import matplotlib.pyplot as plt

import pygame
import serial

def setup_serial():
    # Setup serial communication
    ser = serial.Serial()
    ser.baudrate = 115200
    ser.port = 'COM5'
    ser.close()
    ser.open()
    return ser

ser = setup_serial()

values = []

for i in range(20):
    # Read binary values from serial
    values.append(ser.read(1))
    # Print values
    print(values[i])


ser.close()

#%%
import struct
ser_values = values
# ser_values consists of many seperate bytes
# Convert to one long byte string
ser_values = b''.join(ser_values)
# Index of search string
search_string = b'Sending values\r\n'
index = ser_values.find(search_string)
index = index + len(search_string)
# Slice off the first part
ser_values = ser_values[index:]
# Keep first 4 bytes
ser_values = ser_values[:4]
ser_values

float_value = struct.unpack('f', ser_values)
float_value


#%%
int_values = [int.from_bytes(value, byteorder='big', signed=True) for value in values]
int_values

