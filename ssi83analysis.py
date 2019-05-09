import requests
from datetime import datetime, timedelta
import math

URL = "https://predict.stanfordssi.org/wind"

EARTH_RADIUS = 6371.0

import csv

data = list(csv.reader(open("ssi83.csv")))

data = data[:-7]
data.reverse()

import webutils

webutils.singlepathhtml(data, 'ssi83.html', 3, 4)



def angular_to_lin_distance(lat1, lat2, lon1, lon2): 
    v = math.radians(lat2 - lat1) * EARTH_RADIUS
    u = math.radians(lon2 - lon1) * EARTH_RADIUS * math.cos(math.radians(lat1))
    return u, v

sum = 0
for i in range(1, len(data)):
    lat1, lon1 = float(data[i-1][3]), float(data[i-1][4])
    lat2, lon2 = float(data[i][3]), float(data[i][4])
    u, v = angular_to_lin_distance(lat1, lat2, lon1, lon2)
    sum += math.sqrt(u**2 + v**2)
print(sum)

'''

def get_wind(timestamp, lat, lon):
    PARAMS = {'yr': timestamp.year, 'mo': timestamp.month , "day" : timestamp.day, "hr" : timestamp.hour, "mn" : timestamp.minute,
                    'alt' : 0, "lat" : lat, "lon" : lon}
    
    data = requests.get(url = URL, params = PARAMS)
    return data.json()


f = open("spot2.csv")

for line in f.readlines():
    time, lat, lon, speed = line.split(",")
    lat, lon = float(lat), float(lon)
    time = datetime.strptime(time, "%H:%M:%S")
    time = timedelta(hours=time.hour+7, minutes=time.minute, seconds=time.second) + datetime(2019, 4, 18)
    speed = float(speed)
    
    u, v = get_wind(time, lat, lon)
    meanspeed = 0
    for i in range(20):
        meanspeed += math.sqrt(u[i]**2 + v[i]**2)

    meanspeed /= 20
    print(str(time) + " speed="+str(speed) + " wind=" + str(meanspeed))
    print(speed/meanspeed)
'''