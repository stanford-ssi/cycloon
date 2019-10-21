import pandas as pd
import sys
from datetime import datetime
import numpy as np
from collections import defaultdict

data = pd.read_csv('data.csv')

commandtimes = []
commandvals = []

lines = open('commands.txt').readlines()

ballasts = defaultdict(int)
vents = defaultdict(int)

time = None
for line in lines:
    try:
        time = datetime.strptime('Jun 1 2019 ' + line[-9:].strip(), '%b %d %Y %I:%M %p')
    except:
        if len(line) == 3: # ballast
            val = int(line, 16)
            ballasts[time] += val
        elif len(line) == 5: # vent
            val = int(line, 16)
            vents[time] += val
            
times = []
timestamps = []
alts = []
rates = []
import time as t
from datetime import datetime

for i in range(120, 68, -1):
    point = data.iloc[i]
    time = point.transmit_time/1000
    datetime = datetime.fromtimestamp(time)
    times.append(datetime)

    timestamps.append(time)
    
    alts.append(point.altitude_barometer)



for i in range(1, len(alts)):
    rates.append((alts[i] - alts[i-1])/(timestamps[i] - timestamps[i-1]))
midpoints = [timestamps[i+1]/2 + timestamps[i]/2 for i in range(len(times)-1)]
midpoints = [datetime.fromtimestamp(time) for time in midpoints]

ballasttimes, ballastvals = list(zip(*ballasts.items()))
venttimes, ventvals = list(zip(*vents.items()))
print(vents)


import matplotlib.pyplot as plt
from matplotlib.dates import DateFormatter

myFmt = DateFormatter("%H:%M") 

# tell matplotlib to use the format specified above


fig, ax = plt.subplots(figsize=(10,5))
ax.xaxis.set_major_formatter(myFmt)
ax.plot(times, alts, label='Altitude')
plt.xlabel('PDT Time')
ax.set_ylabel('Altitude (m)')
plt.grid()

ax.set_ylim(0, 25000)

ax2 = ax.twinx()

ax2.scatter(ballasttimes, ballastvals, label='Ballast events')

ax2.scatter(venttimes, ventvals, label='Vent events')
ax2.legend(loc='upper left')
ax2.set_ylabel('Events (s)')
plt.savefig('ssi87profile.png')
plt.clf()

fig, ax = plt.subplots(figsize=(10,5))
ax.plot(midpoints, rates)
ax.set_ylabel('Ascent rate (m/s)')
plt.xlabel('PDT Time')
ax.xaxis.set_major_formatter(myFmt)
plt.grid()
ax2 = ax.twinx()
times, ballastvals = list(zip(*ballasts.items()))
ax2.scatter(times, ballastvals, label='Ballast events')
times, ventvals = list(zip(*vents.items()))
ax2.scatter(times, ventvals, label='Vent events')
ax2.legend(loc='upper left')
ax2.set_ylabel('Events (s)')
plt.savefig('rates.png')
plt.clf()


