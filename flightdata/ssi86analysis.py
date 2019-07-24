import pandas
from webutils import *

data = pandas.read_csv("ssi86.csv")

ilat = data["iridium_latitude"]
ilon = data["iridium_longitude"]

time = data["Human Time"]

cep = data["iridium_cep"]
pathcache = list()
for i in range(len(cep)):
    if cep[i] < 10 and cep[i] > 0:
        pathcache.append((ilat[i],ilon[i],time[i],cep[i]))

## As hours --- only works if time_step goes evenly into one hour
slat, slon, __, __ = pathcache[0]

path = "ssi86i.html"
f = open(path, "w")

f.write(part1 + str(slat) + "," + str(slon))
f.write(part2)
for lat, lon, time, cep in pathcache:
    f.write(get_marker_string(lat, lon, str(cep), time))
f.write(get_path_string(pathcache, "#000000", 0, 1))
f.write(part3short)
#f.write(text_output)
f.write(part4 + part5)