import pandas as pd
import sys
from datetime import datetime
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.dates import DateFormatter
import time as t
from datetime import datetime, timedelta

data = pd.read_csv('rock7.csv')
times = []
alts = []
bmpalts = []
rates = [0]

locs = []

for i in range(214, 45, -1):
    point = data.iloc[i]
    time = point["Date Time (UTC)"]
    time = datetime.strptime(time, '%d/%b/%Y %H:%M:%S') - timedelta(hours=7)
    times.append(time)
    lat, lon = point.lat, point.lon
    locs.append((lat, lon))
    alt = point.alt_gps
    bmp_alt = point.alt_bmp
    alts.append(alt)
    bmpalts.append(bmp_alt)

for i in range(1, len(bmpalts)):
    dalt = bmpalts[i]-bmpalts[i-1]
    dt = (times[i]-times[i-1]).seconds
    rates.append(dalt/dt)

plt.scatter(bmpalts, rates)
plt.grid()
plt.xlabel('Altitude (m)')
plt.ylabel('Ascent rate (m/s)')
#plt.ylim(-3, 10)
plt.savefig('rates.png')

myFmt = DateFormatter("%H:%M") 

# tell matplotlib to use the format specified above

fig, ax = plt.subplots(figsize=(10,5))
ax.plot(times, alts, label='GPS Altitude')
ax.plot(times, bmpalts, label='BMP Altitude')
plt.xlabel('PDT Time')
plt.ylim(bottom=0)
ax.set_ylabel('Altitude (m)')
ax.legend(loc='upper right')
plt.grid()
ax.xaxis.set_major_formatter(myFmt)

for l in list(zip(times, alts, locs)): print(l)

plt.savefig('profile.png')
plt.clf()

import habsim as hs
lats, lons = list(zip(*locs))
data = list(zip(times,lats,lons))
traj1 = hs.Trajectory(data=data)

print(traj1.duration(), traj1.length())
exit()

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

'''