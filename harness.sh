
set -e
make -f Makefile

echo "OMP Centralized Sense Reversal Barrier"
for i in {2..8}
do
	for j in 1
	do
		./omp_sense $i $j
		echo ""
	done
done

read -p "Press [Enter] key to run Dissemination Barrier..."

echo "OMP Dissemination Barrier"
for i in {2..8}
do
	for j in 1
	do
		./omp_dissemination $i $j
		echo ""
	done
done

read -p "Press [Enter] key to run Tournament Barrier..."

echo "MPI Tournament Barrier"
for i in {2..12}
do
	for j in 1
	do
		mpirun -np $i ./mpi_tournament -i $j
		echo ""
	done
done

read -p "Press [Enter] key to run MCS tree Barrier..."

echo "MPI MCS tree Barrier"
for i in {2..12}
do
	for j in 1
	do
		mpirun -np $i ./mpi_mcs -i $j
		echo ""
	done
done

read -p "Press [Enter] key to run MPI-OMP combined Barrier..."

echo "MPI-OMP Combined Barrier"
for i in {2..12}
do
	for j in {2..8}
	do
		mpirun -np $i ./mpi_omp_barrier -i $j
		echo ""
	done
done

echo "=== Done! ==="
