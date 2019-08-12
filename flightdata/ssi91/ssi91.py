import habsim as hs
from datetime import datetime
with open('data') as data:
    points = data.read().split('\n')

processedpoints = list()

for point in points[:-1]:
    print(point)
    lat, lon, alt, time, _ = point.split('\t')
    time = datetime.strptime(time, '%Y-%m-%d %H:%M:%S %z')
    time = datetime.timestamp(time)
    processedpoints.append((time, float(lat), float(lon), float(alt)))
data = processedpoints[::-1]
traj = hs.Trajectory(data=data)
print(traj.endtime())
print(traj.duration())
print(traj.length())

plt = hs.ioutil.WebPlot()
plt.origin(36.8492, -121.432)
plt.add([traj])
for point in traj:
    plt.circle(point[1], point[2], 1000, fillOpacity=0.8)
plt.save('ssi91.html')
times, lats, lons, alts = list(zip(*traj.data))
rates = [None]
speeds = [None]
import math
def pythag(vec):
    return math.sqrt(vec[0]**2 + vec[1]**2)

for i in range(1, len(traj)):
    rates.append((alts[i] - alts[i-1])/(times[i] - times[i-1]))
    speeds.append(pythag(hs.util.angular_to_lin_distance(\
        lats[i-1], lats[i], lons[i-1], lons[i]))*1000/(times[i]-times[i-1]))

for item in list(zip(times, lats, lons, alts, rates, speeds)):
    print(item)

times = list(map(datetime.fromtimestamp, times))

import matplotlib.pyplot as plt
from matplotlib.dates import DateFormatter
myFmt = DateFormatter("%H:%M") 

fig, ax = plt.subplots()
ax.plot(times, alts)
plt.xlabel('PDT Time')
ax.set_ylabel('Altitude (m)')
ax.xaxis.set_major_formatter(myFmt)
plt.grid()
plt.ylim(bottom=0)
plt.savefig('ssi91.png')