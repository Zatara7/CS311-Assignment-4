#!/bin/sh

##  File          : batch.sh
##  Description   : Batch script to test different cache sizes for smsasim
##                  program.
##  
##  Author        : Eric D. Kilmer <eyk5120@psu.edu>
##  Last Modified : Wed. Nov 14 10:42 EST 2013

TEST=linear
MASTER=${TEST}-output.log
WORKLOAD=${TEST}.dat

echo "Running with cache: 4096."
# Run the program and direct output to out.txt to compare against other logs
./smsasim -c 4096 $WORKLOAD > out4096.txt 2>&1
# Compare the professor's output with my output.
./verify $MASTER out4096.txt
echo "Done."

echo ""

echo "Running with cache: 2048"
./smsasim -c 2048 $WORKLOAD > out2048.txt 2>&1
./verify $MASTER out2048.txt
echo "Done."

echo ""

echo "Running with cache: 1024."
./smsasim -c 1024 $WORKLOAD > out1024.txt 2>&1
./verify $MASTER out1024.txt
echo "Done."

echo ""

echo "Running with cache: 512."
./smsasim -c  512 $WORKLOAD > out512.txt  2>&1
./verify $MASTER out512.txt
echo "Done."

echo ""

echo "Running with cache: 256."
./smsasim -c  256 $WORKLOAD > out256.txt  2>&1
./verify $MASTER out256.txt
echo "Done."

echo ""

echo "Running with cache: 128."
./smsasim -c  128 $WORKLOAD > out128.txt  2>&1
./verify $MASTER out128.txt
echo "Done."

echo ""

echo "Running with cache: 64."
./smsasim -c   64 $WORKLOAD > out64.txt   2>&1
./verify $MASTER out64.txt
echo "Done."

echo ""

echo "Running with cache: 32."
./smsasim -c   32 $WORKLOAD > out64.txt   2>&1
./verify $MASTER out64.txt
echo "Done."
