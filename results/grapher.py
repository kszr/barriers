import matplotlib.pyplot as plt
import numpy as np
from pylab import *
import os
import re

mcs_vals = []
file = open("mpi_mcs_final.txt", "r")
for line in file:
    p = re.compile("([0-9]+): ([0-9]+.[0-9]+)")
    m = p.search(line)
    if m:
        mcs_vals.append(float(m.groups()[1]))
mcs_results = np.array(mcs_vals)

tournament_vals = []
file = open("mpi_tournament_final.txt", "r")
for line in file:
    p = re.compile("([0-9]+): ([0-9]+.[0-9]+)")
    m = p.search(line)
    if m:
        tournament_vals.append(float(m.groups()[1]))
tournament_results = np.array(tournament_vals)


"""
Plotting MPI MCS and Tournament Barriers.
"""

x = np.arange(2.0, 13.0)
plt.plot(x, mcs_results, "o-", label="MCS")
plt.plot(x, tournament_results, "s-", label="Tournament")
plt.xlabel("Number of Processors")
plt.ylabel(r"Time in $\mu$secs")
plt.legend(("MCS","Tournament"), loc="upper left")
plt.savefig("MCS_Tourney.png")

mpi_omp_vals = []
file = open("mpi_omp_final.txt", "r")
for line in file:
    p = re.compile("([0-9]+): 12: ([0-9]+.[0-9]+)")
    m = p.search(line)
    if m:
        mpi_omp_vals.append(float(m.groups()[1]))
mpi_omp_results = np.array(mpi_omp_vals)

"""
Plotting MPI-OMP with 12 threads per process against MPI MCS and MPI Tournament.
"""
plt.cla()
x = np.arange(2.0, 9.0)
plt.plot(x, np.array(mcs_vals[0:7]), "o-", label="MCS")
plt.plot(x, np.array(tournament_vals[0:7]), "s-", label="Tournament")
plt.plot(x, mpi_omp_results, "^-", label="MPI OMP")
plt.xlabel("Number of Processors")
plt.ylabel(r"Time in $\mu$secs")
plt.legend(("MCS", "Tournament", "MPI-OMP"), loc="upper left")
plt.savefig("MPI_OMP.png")