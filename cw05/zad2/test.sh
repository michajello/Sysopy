#!/bin/bash
N=1000000
K=100
R=600
FIFO=cos

for (( i = 0; i < 5; i++ )); do
    ./slave.out $FIFO $N $K 1>/dev/null &
     sleep 0.1
done
./master.out $FIFO $R
