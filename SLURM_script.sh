#!/bin/sh
name="$1"
np="$2"
shift 2
mpirun -np $np $name $@
