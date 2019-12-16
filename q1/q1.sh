~/.openmpi/bin/mpicc main.c -o q1
~/.openmpi/bin/mpirun -np $1 ./q1 $2 echo "${@:3}"