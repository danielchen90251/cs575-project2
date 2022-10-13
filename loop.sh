#!/bin/bash

clear
for t in 1 2 4 8 12 16 20 24 32
do
  for n in 256 512 1024 2048 3072 4096 5120 6144
  do
    g++ -O3 proj02.cpp -DNUMT=$t -DNUMNODES=$n -o proj2 -lm -fopenmp
    ./proj2
  done
done