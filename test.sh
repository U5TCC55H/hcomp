#!/bin/bash

echo "Testing compress..."
time ./main c $1 t_output.dat

echo "Testing decompress..."
time ./main d t_output.dat t_decompressed.dat

echo
if cmp $1 t_decompressed.dat
then
    echo "Correctness test PASSED"
else
    echo "Correctness test FAILED"
fi

echo "Press any key to continue"
read
