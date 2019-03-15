import math
import os

amplitude = 1000


#for i in range(1000):
#    file.write(str(0) + "\n")
'''
periods = [3.5, 3.8, 3.9, 3.95, 4, 4.05, 4.1, 4.2 , 4.5, 5, 6, 7, 7.5, 7.8, 7.9, 7.95, 8, 8.05, 8.1, 8.2, 8.5, 9, 10]

for period in periods:
    file = open("test.txt", "w")
    for i in range(100000):
        file.write(str(int(math.cos(i*2*math.pi/period) * amplitude)) + "\n")
    file.close()
    os.system("./demodulator test.txt > " + str(64) + "onebit" + str(period))
'''
'''
file = open("test.txt", "w")

for i in range(62):
    file.write(str(0) + "\n")
for j in range(2):
    for i in range(64 * 8):
        file.write(str(int(math.sin(i*2*math.pi/8)    ))  + "\n")
    for i in range(64 * 8):
        file.write(str(int(math.sin(i*2*math.pi/4)   ))  + "\n")

for j in range(10):
    file.write(str(int(math.cos(i*2*math.pi/8) + math.cos(i*2*math.pi/8) / 8))  + "\n")


'''
file = open("test.txt", "w")
'''

for k in range(1, 32):
    file = open("test.txt", "w")

    for i in range(k):
        file.write(str(0) + "\n")

    for j in range(1):
        for i in range(64 * 8):
            file.write(str(int(math.sin(i*2*math.pi/8)  * amplitude  ))  + "\n")
        for i in range(64 * 8):
            file.write(str(int(math.sin(i*2*math.pi/4)  * amplitude ))  + "\n")

    for j in range(10):
        file.write(str(int(math.cos(i*2*math.pi/8) + math.cos(i*2*math.pi/8) / 8))  + "\n")


    file.close()
    os.system("./demodulator test.txt > offset" + str(k))

'''

for i in range(15):
    file.write(str(0) + "\n")

for i in range(64 * 8):
    file.write(str(int(amplitude + math.sin(i*2*math.pi/8)   * amplitude ))  + "\n")


message = 'EE26N IS THE BEST INTROSEM EVER'


for char in message:
    char = ord(char)
    for i in range(8):
        mask = 0x80
        
        if mask & char:
            for i in range(64 * 8):
                file.write(str(int(amplitude + math.sin(i*2*math.pi/4)   * amplitude ))  + "\n")
    
        else:
            for i in range(64 * 8):
                file.write(str(int(amplitude + math.sin(i*2*math.pi/8)  * amplitude ))  + "\n")
        
        char <<= 1

for i in range(5000):
    file.write(str(int(amplitude + math.sin(i*2*math.pi/8) * amplitude ))  + "\n")
