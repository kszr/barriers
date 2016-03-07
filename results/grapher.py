import matplotlib.pyplot as plt
import numpy as np
from pylab import *
import os
import re

"""
Plotting MPI MCS and Tournament Barriers.
"""
mcs_vals = []
file = open("aggregate_results/mpi_mcs_final.txt", "r")
for line in file:
    p = re.compile("([0-9]+): ([0-9]+.[0-9]+)")
    m = p.search(line)
    if m:
        mcs_vals.append(float(m.groups()[1]))
mcs_results = np.array(mcs_vals)

tournament_vals = []
file = open("aggregate_results/mpi_tournament_final.txt", "r")
for line in file:
    p = re.compile("([0-9]+): ([0-9]+.[0-9]+)")
    m = p.search(line)
    if m:
        tournament_vals.append(float(m.groups()[1]))
tournament_results = np.array(tournament_vals)

x = np.arange(2.0, 13.0)
plt.plot(x, mcs_results, "o-", label="MCS")
plt.plot(x, tournament_results, "s-", label="Tournament")
plt.xlabel("Number of Processors")
plt.ylabel(r"Time ($\mu$s)")
plt.legend(("MCS","Tournament"), loc="upper left")
plt.savefig("graphs/MCS_Tourney.png")


"""
Plotting MPI-OMP with 12 threads per process against MPI MCS and MPI Tournament.
"""
mpi_omp_vals = []
file = open("aggregate_results/mpi_omp_final.txt", "r")
for line in file:
    p = re.compile("([0-9]+): 12: ([0-9]+.[0-9]+)")
    m = p.search(line)
    if m:
        mpi_omp_vals.append(float(m.groups()[1]))
mpi_omp_results = np.array(mpi_omp_vals)

plt.cla()
x = np.arange(2.0, 9.0)
plt.plot(x, np.array(mcs_vals[0:7]), "o-", label="MCS")
plt.plot(x, np.array(tournament_vals[0:7]), "s-", label="Tournament")
plt.plot(x, mpi_omp_results, "^-", label="MPI OMP")
plt.xlabel("Number of Processors")
plt.ylabel(r"Time ($\mu$s)")
plt.legend(("MCS", "Tournament", "MPI-OMP"), loc="best")
plt.savefig("graphs/MPI_OMP.png")


"""
Plotting OMP Sense and Dissemination Barriers
"""
omp_diss_vals = []
file = open("aggregate_results/omp_dissemination_final.txt", "r")
for line in file:
    p = re.compile("([0-9]+): ([0-9]+.[0-9]+)")
    m = p.search(line)
    if m:
        omp_diss_vals.append(float(m.groups()[1]))
omp_diss_results = np.array(omp_diss_vals)

omp_sense_vals = []
file = open("aggregate_results/omp_sense_final.txt", "r")
for line in file:
    p = re.compile("([0-9]+): ([0-9]+.[0-9]+)")
    m = p.search(line)
    if m:
        omp_sense_vals.append(float(m.groups()[1]))
omp_sense_results = np.array(omp_sense_vals)

plt.cla()
x = np.arange(2.0, 9.0)
plt.plot(x, omp_diss_results, "o-", label="Dissemination")
plt.plot(x, omp_sense_results, "s-", label="Sense Reversal")
plt.xlabel("Number of Threads")
plt.ylabel(r"Time ($\mu$s)")
plt.legend(("Dissemination", "Sense Reversal"), loc="best")
plt.savefig("graphs/Dissemination_Sense.png")