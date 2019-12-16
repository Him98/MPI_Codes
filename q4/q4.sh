g++ make_graph_wo_src.cpp
./a.out > graph.txt
~/.openmpi/bin/mpic++ -std=c++11 main.cpp -o q4
# echo $2
~/.openmpi/bin/mpirun -np $1 q4 graph.txt