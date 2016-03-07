import os
import re

"""
MPI Tournament results extracted below. All results in microseconds.
"""
tournament_map = {}
dir_tournament = "mpi_tournament_results"
tournament_files = os.listdir(dir_tournament)
for path in tournament_files:
    file = open(dir_tournament + "/" + path, "r")
    for line in file:
        p = re.compile("took (0.[0-9]*)s to run with ([0-9]*) processors")
        m = p.search(line)
        if m:
            try:
                tournament_map[int(m.groups()[1])] = (float(m.groups()[0])+tournament_map[int(m.groups()[1])][0], 1+int(tournament_map[int(m.groups()[1])][1]))
            except KeyError:
                tournament_map[int(m.groups()[1])] = (float(m.groups()[0]),1)

file = open("mpi_tournament_final.txt", "w")
file.write("MPI Tournament Results\n")
file.write("(num processors: time in microseconds)\n")
for key in tournament_map:
    file.write(str(key) + ": " + str(10000*float(tournament_map[key][0])/int(tournament_map[key][1])));
    file.write("\n");
file.close()

"""
MPI MCS results extracted below. All results in microseconds.
"""
mcs_map = {}
dir_mcs = "mpi_mcs_results"
mcs_files = os.listdir(dir_mcs)
for path in mcs_files:
    file = open(dir_mcs + "/" + path, "r")
    for line in file:
        p = re.compile("took (0.[0-9]*)s to run with ([0-9]*) processors")
        m = p.search(line)
        if m:
            try:
                mcs_map[int(m.groups()[1])] = (float(m.groups()[0])+mcs_map[int(m.groups()[1])][0], 1+int(mcs_map[int(m.groups()[1])][1]))
            except KeyError:
                mcs_map[int(m.groups()[1])] = (float(m.groups()[0]),1)

file = open("mpi_mcs_final.txt", "w")
file.write("MPI MCS Results\n")
file.write("(num processors: time in microseconds)\n")
for key in mcs_map:
    file.write(str(key) + ": " + str(10000*float(mcs_map[key][0])/int(mcs_map[key][1])));
    file.write("\n");
file.close()
