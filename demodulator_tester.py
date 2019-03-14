import math
import os

amplitude = 100


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

file = open("test.txt", "w")

for i in range(1000):
    file.write(str(0) + "\n")
for i in range(1000):
    file.write(str(int((math.cos(i*2*math.pi/8) + 1* math.cos(i*2*math.pi/4)) * amplitude))  + "\n")