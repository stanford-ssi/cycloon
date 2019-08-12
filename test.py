from datetime import datetime, timedelta
import habsim as hs

hs.util.checkServer()

'''
asc = hs.Segment(5, stopalt=29000)
equil = hs.Segment(0, dur=3)
desc = hs.Segment(-10, stopalt=0)
floating = hs.Segment(0, dur=30, coeff=1)
'''
hollister = hs.LaunchSite((36.8492, -121.432))

profile = hs.ControlledProfile(50, 5)
profile.initialize(2000, 5000, 30000, seed=[79.0, 5000])



pred = hs.Prediction(profile=profile,launchsite=hollister, step=240)
pred.run(model=1)



plt = hs.ioutil.WebPlot()
plt.origin(*hollister.coords)
plt.add(pred.split(), hs.ioutil.RandomColorGenerator())

time = datetime.now().timestamp() - 100

targettraj = list()
for i in range(101):
    targettraj.append((time + i * 3600, 30.7 + i*0.1, -92.7-i*0.2))

hours = [targettraj[i][0] for i in range(101)]
lats = [targettraj[i][1] for i in range(101)]
lons = [targettraj[i][2] for i in range(101)]

for i in range(1):
    pred.run(model=1)
    closest = hs.util.optimize_step(pred, hs.StaticTarget(40.7, -92.7), 20, decreasing_weights=False)
    print(profile)
    print(closest)

plt.add(pred.split(), hs.ioutil.RandomColorGenerator())
#plt.add([targettraj])
plt.marker(40.7, -92.7)
plt.save("test.html") 

print(pred.trajectory.startpoint())
print(pred.trajectory.endpoint())
