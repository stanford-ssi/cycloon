import sys
sys.path.append('/mnt/e/git')
import habsim as hs
from datetime import datetime
# Note, this is simm'ed data not real data
# Name of doc refers to descent rate at nomal equil

rise = hs.Segment(2.5, stopalt=15000)
equil = hs.Segment(-1, stopalt=0) #dur=3.5
fall = hs.Segment(-2, stopalt=0)

hollister = hs.LaunchSite((36.8492, -121.432))

profile = hs.Profile([rise, equil]) #fall])

pred = hs.Prediction(profile=profile, launchsite=hollister, launchtime=datetime(2019, 7, 27, 12, 47))

plt = hs.ioutil.WebPlot()
plt.origin(*hollister.coords)

for i in range(1,21):
    traj = pred.run(model=i).split()
    plt.add(traj)

plt.save('equil1.html')