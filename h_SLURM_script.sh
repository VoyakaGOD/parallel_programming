#!/bin/sh
name="$1"
np="$2"
nt="$3"
shift 3
mpirun --map-by node:pe=$nt -np $np $name $@
