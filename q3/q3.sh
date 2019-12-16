g++ make_graph.cpp
./a.out > graph.txt
~/.openmpi/bin/mpic++ -std=c++11 main.cpp -o q31
# echo $2
~/.openmpi/bin/mpirun -np $1 q31 < graph.txt