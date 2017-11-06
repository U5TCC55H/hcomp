#!/bin/bash

echo "Testing compress..."
time ./main c t_input.dat t_output.dat

echo "Testing decompress..."
time ./main d t_output.dat t_decompressed.dat

echo
if cmp t_input.dat t_decompressed.dat
then
    echo "Correctness test PASSED"
else
    echo "Correctness test FAILED"
fi
