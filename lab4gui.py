import serial
import numpy as np
from time import sleep
from matplotlib import pyplot as plt
from drawnow import *

import time

ser = serial.Serial(port='COM3', baudrate=9600,timeout=0)
    
print(ser.name)  
plt.ion()

def makeFig(): 
    plt.ylim(0,10)                                 
    plt.title('JOYSTICK CONTROL')      
    plt.grid(True)                                 
    plt.ylabel('Y AXIS')  
    plt.xlabel('X AXIS')                        
    plt.plot(X_Val,Y_Val,'rx-')       
    plt.xlim(0,10)

while True:
	#data = ser.read(9999)
    	#if len(data) > 0:
       	#	print 'Got:', data

	while (ser.inWaiting()==0): #Wait 
        	pass #do nothing
   	data = ser.readline() 
    	dataArray = data.split(',')   
	try:    	
	   X_Val = float( dataArray[0])           
    	   Y_Val = float( dataArray[1])   
	except ValueError,e:
           print "error" 
 	#Xaxis.append(X_Val)
    	#Yaxis.append(Y_Val)     	 
	#print X_Val
	drawnow(makeFig)                      
   	plt.pause(.000001)
		
	

ser.close()

