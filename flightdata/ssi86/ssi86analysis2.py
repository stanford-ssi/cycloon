import pandas as pd
import sys
sys.path.append('/mnt/e/git')
import habsim as hs
from datetime import datetime
import numpy as np

data = pd.read_csv('ssi86.csv')

commandtimes = []
commandvals = []

with open('ssi86commands.txt') as f:
    time = None
    for line in f:
        try: 
            time = line.split('[')[1].split(']')[0]
            time = datetime.strptime('May 18 2019 ' + time, '%b %d %Y %I:%M %p')
            #print(time)
        except:
            try:
                command = int(line.split('`')[1], 16)
                commandtimes.append(time)
                commandvals.append(command)
            except:
                continue

traj1 = hs.Trajectory()

times = []
timestamps = []
alts = []
elevs = []
rates = [0]
import time as t
from datetime import datetime

for i in range(65):
    point = data.iloc[i]
    time, lat, lon = point.transmit_time/1000, point.latitude, point.longitude
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


print(traj1.duration(), traj1.length())

plt = hs.ioutil.WebPlot()
plt.origin(36.8492, -121.432)
plt.add([traj1])
plt.save('ssi86a.html')

traj2 = hs.Trajectory()

traj2.data = []
print(traj2)
for i in range(210, 280):
    
    sliced = data.iloc[i-5:i+5]

    lats, ceps, lons = sliced['iridium_latitude'].tolist(), sliced['iridium_cep'].tolist(), sliced['iridium_longitude'].tolist()

    lat = np.average(lats, weights=np.divide(1, ceps))
    lon = np.average(lons, weights=np.divide(1, ceps))
    time = data.iloc[i].transmit_time/1000
    traj2.data.append((time, lat, lon))
    '''
    prev, curr, fut = data.iloc[i-1], data.iloc[i], data.iloc[i+1]
    prevcep, currcep, futcep = prev.iridium_cep, curr.iridium_cep, fut.iridium_cep
    if currcep < 4:
        time = curr.transmit_time/1000
        denom = 1/prevcep + 1/currcep + 1/futcep
        lat = (1/prevcep * prev.iridium_latitude + 1/currcep * curr.iridium_latitude + 1/futcep * fut.iridium_latitude)/denom
        lon = (1/prevcep * prev.iridium_longitude + 1/currcep * curr.iridium_longitude + 1/futcep * fut.iridium_longitude)/denom
        print(time, lat, lon)
        
        traj2.data.append((time, lat, lon))
    '''

plt2 = hs.ioutil.WebPlot()
plt2.origin(38.3481, -120.6653)
plt2.add([traj1])
plt2.add([traj2], ['#FF0000'])
plt2.save('ssi86b.html')
print(traj2.duration(), traj2.length())
'''

import numpy as np
import matplotlib.pyplot as plt

# Create some mock data
t = np.arange(0.01, 10.0, 0.01)
data1 = np.exp(t)
data2 = np.sin(2 * np.pi * t)

fig, ax1 = plt.subplots()

color = 'tab:red'
ax1.set_xlabel('time (s)')
ax1.set_ylabel('exp', color=color)
ax1.plot(t, data1, color=color)
ax1.tick_params(axis='y', labelcolor=color)

ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis

color = 'tab:blue'
ax2.set_ylabel('sin', color=color)  # we already handled the x-label with ax1
ax2.plot(t, data2, color=color)
ax2.tick_params(axis='y', labelcolor=color)

fig.tight_layout()  # otherwise the right y-label is slightly clipped
plt.show()
'''