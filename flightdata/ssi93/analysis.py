import pandas as pd
import sys
from datetime import datetime
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.dates import DateFormatter
import time as t
from datetime import datetime, timedelta

data = pd.read_csv('rockblock.csv')

# apparent landing 14
# launch 70
# 23 true landing

times = []
alts = []
bmpalts = []
rates = [0]

locs = []
ballast_times = []
ballasts = []


for i in range(70, 20, -1):
    point = data.iloc[i]
    time = point["Date Time (UTC)"]
    time = datetime.strptime(time, '%d/%b/%Y %H:%M:%S') - timedelta(hours=7)
    if point.Direction == 'MT': 
        ballast_times.append(time)
        ballasts.append(int(str(point.Payload[:2]), 16))
        continue
    
    times.append(time)
    lat, lon = point.lat, point.lon
    locs.append((lat, lon))
    alt = point.gpsalt
    bmp_alt = point.bmpalt
    alts.append(alt)
    bmpalts.append(bmp_alt)

for i in range(1, len(bmpalts)):
    dalt = bmpalts[i]-bmpalts[i-1]
    dt = (times[i]-times[i-1]).seconds
    rates.append(dalt/dt)

plt.plot(bmpalts, rates)
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


ax2 = ax.twinx()
ax2.scatter(ballast_times, ballasts)
ax2.set_ylabel('Ballast times (s)')
ax2.xaxis.set_major_formatter(myFmt)

for l in list(zip(times, alts, locs)): print(l)

plt.savefig('profile.png')
plt.clf()

import habsim as hs

#processedpoints.append((time, float(lat), float(lon), float(alt)))
#data = processedpoints[::-1]

lats, lons = list(zip(*locs))
data = list(zip(times,lats,lons))
traj1 = hs.Trajectory(data=data)

print(traj1.duration(), traj1.length())

plt = hs.ioutil.WebPlot()
plt.origin(36.8492, -121.432)
plt.add([traj1])
for point in traj1:
    plt.circle(point[1], point[2], 1000)
plt.save('ssi93.html')

