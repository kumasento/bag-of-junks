# Cache Simulator

A partial implementation for the assignment: https://jhucsf.github.io/spring2020/assign/assign03.html

Download the test data by:

```
./download-traces
```

Run by:

```
# At the project root
mkdir build
cd build
cmake ..
make 
./bin/csim 256 4 16 write-allocate write-back fifo  < ../cache-sim/data/gcc.trace
```
