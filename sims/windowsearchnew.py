import requests 
from datetime import datetime, timedelta
from webutils import *
import os
URL = "http://predict.stanfordssi.org/singlepredicth"
duration = 72 ## hours
step = 240 ## seconds


import urllib
import json


locations = {"MillCreek" :(35.9832,-121.5),
    "Limantour": (38.024,-122.88),
    "BigSur": (36.2988, -121.9035),
    "ElJarro": (37.0127, -122.222),
    "AnoNuevo": (37.1046, -122.3249),
    "PigeonPoint":  (37.179, -122.39),
    "Custom" : (35.21682	,-121.04582),
    "Hollister" : (36.8492, -121.432),
    "SantaCruz" : (36.9451, -122.0259)
}


def main(location, coeff):

    start = requests.get(url = "http://predict.stanfordssi.org/which").text
    start = datetime.strptime(start, "%Y%m%d%H")
    #start = datetime(2019, 4, 28, 9+7)
  
    #start = datetime(2019, 4, 20, 7+11, 2)
    time = start
    #end1 = datetime(2019, 4, 19, 18)
    interval1 = timedelta(hours = 6)
    end2 = datetime(2019, 5, 28, 14+7)
    #interval2 = timedelta(hours = 24)
    
    #times = [time]
    
    times = list()
    '''
    while time <= end1:
        times.append(time)
        time = time + interval1
    '''
    while time <= end2:
        times.append(time)
        time = time + interval1
    
    
    # cycloon
    for time in times:
        print( str(time) + " with floating coeffient " + str(coeff))
        paths = list()
        for n in range(1,21):
            path = predict(time, location, coeff, n)
            paths.append([path])
        generate_html(paths, "res/", time.strftime("%Y%m%d%H%M") + "_" + str(coeff), requests.get(url = "http://predict.stanfordssi.org/which").text, time.strftime("%Y%m%d%H%M"), 6, 240)

    '''
    # spaceshot
    print(times)
    for time in times:
        print(str(time))
        paths = list()
        for n in range(1,21):
            print(n)
            duration1 = 29000 / 3.7 / 3600
            rise = predict(time, "Hollister", 1, n, rate=3.7, alt=79, duration=duration1)
            locations["Custom"]=rise[-1][1:3]
            duration2 = 0.5 #(21+7 - time.hour - duration1)
            equil = predict(time+timedelta(hours=duration1), "Custom", 1, n, alt=29000, duration=duration2)
            locations["Custom"]=equil[-1][1:3]
            fall = predict(time+timedelta(hours=duration1 + duration2), "Custom", 1, n, rate = -15, alt=29000, duration=1)
            
            #paths.append((rise, equil))
            paths.append((rise, equil, fall))
        generate_html(paths, "res/spaceshot/", time.strftime("%Y%m%d%H%M"), requests.get(url = "http://predict.stanfordssi.org/which").text, time.strftime("%Y%m%d%H%M"), 0, 240)
    '''


def predict(timestamp, location, drift_coeff, model, rate = 0, alt=0, duration=72, step=step):
    
    URL = 'https://predict.stanfordssi.org/singlepredicth?yr=' + str(timestamp.year) + "&mo=" + str(timestamp.month) + "&day=" +str(timestamp.day)+ "&hr=" +str(timestamp.hour) + "&mn=" + str(timestamp.minute) + "&rate=" +str(rate)+ "&coeff=" + str(drift_coeff) + "&model=" +str(model)+ "&alt=" + str(alt) + \
        "&lat=" + str(locations[location][0]) + "&lon=" + str(locations[location][1]) + "&dur=" + str(duration) + "&step=" + str(step)
    #os.system("wget \"" + URL + "\"")
    #print(URL)
    return json.load(urllib.request.urlopen(URL))
    '''
    
    PARAMS = {'yr': timestamp.year, 'mo': timestamp.month , "day" : timestamp.day, "hr" : timestamp.hour, "mn" : timestamp.minute,
                    "rate" : rate, 'coeff' : drift_coeff, 'model' : model, 'alt' : alt, "lat" : locations[location][0], "lon" : locations[location][1],
                    "dur" : duration, "step": step}
    print(PARAMS)
    data = requests.get(url = URL, params = PARAMS)
    return data.json()
    '''

def generate_html(pathcache, folder, filename, model_timestamp, sim_timestamp, marker_interval, timestep):
    ## As hours --- only works if time_step goes evenly into one hour
    __, slat, slon, __, __, __ = pathcache[0][0][0]

    path = folder + "/" + filename + ".html"
    f = open(path, "w")

    f.write(part1 + str(slat) + "," + str(slon))
    f.write(part2)

    text_output = "Model time: " + model_timestamp + ", launchtime: " + sim_timestamp + "<br/><br/>"

    marker_interval_in_waypoints = marker_interval * 3600 / timestep
    sum = 0
    lengths = list()

    colors = ["#000000", "#FF0000", "#0000FF"]

    for i in range(len(pathcache)):
        for k in range(len(pathcache[i])):
            path = pathcache[i][k]
            sum += len(path)-1
            lengths.append(float((str("%.2f" % ((len(path)-1) * timestep / 3600)))))
            pathstring = ""

            for j in range(len(path)):
                timestamp, lat, lon, alt, u, v = path[j]
                if marker_interval != 0:
                    if (j % marker_interval_in_waypoints == 0 and j != 0) or j == len(path)-1:
                        f.write(get_marker_string(lat, lon, str(i+1), str(j * timestep / 3600) + "h"))
            
                pathstring = pathstring + str("%.2f" % (j * timestep / 3600)) + "h " + ",Loc=" + str("%.5f" % lat)+ "," + str("%.5f" % lon)+ \
                ",u=" + str("%.3f" % u)+ ",v=" + str("%.3f" % v)+  "<br>\n"
        
            text_output = text_output + "Model number: " + str (i+1) + "<br/>" + pathstring + "<br/><br/>"

            f.write(get_path_string(path, colors[k]))
    lengths.sort(reverse=True)
    print(lengths, end=" ")
    print("Average " + str("%.2f" % (sum * timestep / 3600/ 20)))
    f.write(part3short)
    f.write(text_output)
    f.write(part4 + part5)

if __name__ == "__main__":
    import sys
    main(sys.argv[1], float(sys.argv[2]))
