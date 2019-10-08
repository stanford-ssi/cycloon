import pandas as pd
import sys
import habsim as hs
from datetime import datetime
import numpy as np

data = pd.read_csv('ssi92/data.csv')


traj1 = hs.Trajectory()

times = []
timestamps = []
alts = []
elevs = []
rates = [0]
import time as t
from datetime import datetime

for i in range(len(data)):
    point = data.iloc[i]
    #time, lat, lon = point.transmit_time/1000, point.latitude, point.longitude
    lat, lon = point.latitude, point.longitude
    print(lat, lon)
    if lat is "nan": continue
    time = i
    traj1.data.append((time, lat, lon))
    timestamps.append(time)
    times.append(datetime.fromtimestamp(time))
    alts.append(point.altitude_gps)
    #elevs.append(hs.util.getElev((lat, lon)))


elevs = [79.0, 79.0, 80.0, 82.0, 90.0, 87.0, 87.0, 90.0, 85.0, 85.0, 80.0, 73.0, 69.0, 72.0, 118.0, 305.0, 345.0, 525.0, 175.0, 453.0, 471.0, 577.0, 189.0, 234.0, 86.0, 45.0, 27.0, 16.0, 21.0, 23.0, 25.0, 28.0, 26.0, 33.0, 34.0, 42.0, 47.0, 47.0, 35.0, 49.0, 51.0, 59.0, 63.0, 51.0, 62.0, 65.0, 71.0, 119.0, 113.0, 129.0, 171.0, 182.0, 114.0, 138.0, 165.0, 184.0, 207.0, 285.0, 267.0, 293.0, 305.0, 537.0, 493.0, 564.0, 583.0]

for i in range(1, 65):
    rates.append((alts[i] - alts[i-1])/(timestamps[i] - timestamps[i-1]))

import matplotlib.pyplot as plt
from matplotlib.dates import DateFormatter

myFmt = DateFormatter("%H:%M") 

# tell matplotlib to use the format specified above
'''
fig, ax = plt.subplots(figsize=(10,5))
ax.plot(times, elevs, label = 'Ground elevation')
ax.plot(times, alts, label='Altitude')
plt.xlabel('PDT Time')
ax.set_ylabel('Altitude (m)')
ax.set_ylim(0, 5000)
ax.legend(loc='upper right')
plt.grid()
ax2 = ax.twinx()
ax2.set_ylim(0, 50)
ax2.scatter(commandtimes, commandvals, label='Ballast events')
ax2.legend(loc='upper left')
ax2.set_ylabel('Ballast time (s)')
ax.xaxis.set_major_formatter(myFmt)

plt.savefig('ssi86profile.png')
plt.clf()
fig, ax = plt.subplots()
ax.plot(times, rates)
ax.set_ylabel('Ascent rate (m/s)')
plt.grid()
ax2 = ax.twinx()
ax2.scatter(commandtimes, commandvals)
ax2.set_ylabel('Ballast times (s)')
plt.xlabel('PDT Time')
plt.savefig('ssi86rates.png')
ax.xaxis.set_major_formatter(myFmt)
'''

print(traj1.duration(), traj1.length())

plt = hs.ioutil.WebPlot()
plt.origin(36.8492, -121.432)
plt.add([traj1])
for point in traj1:
    plt.circle(point[1], point[2], 1000)
plt.save('ssi92.html')

