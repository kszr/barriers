import matplotlib.pyplot as plt
import numpy as np
from pylab import *

mcs_results = np.array([373.811875, 391.1341667, 482.776875, 598.8355, 617.4433333, 647.8669643, 691.6609375, 728.1423611, 781.37075, 796.1086364, 824.9725])
tournament_results = np.array([393.65375, 414.0233333, 442.249375, 446.78625, 489.05, 484.1308929, 581.51875, 549.6709722, 554.69775, 549.9723864, 576.2260417])

x = np.arange(2.0, 13.0)


plt.plot(x, mcs_results, "o-", label="MCS")
plt.plot(x, tournament_results, "s-", label="Tournament")
plt.xlabel("Number of Processors")
plt.ylabel("Time in Microsecs")
legend(('MCS','Tournament'))
plt.savefig("MCS_Tourney.png")