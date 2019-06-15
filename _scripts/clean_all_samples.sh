#!/bin/bash

# find . -name Makefile -exec bash -c 'make -C "${1%/*}" clean' -- {} \;

COMMAND=""
echo $COMMAND

PROJECTS_DIRS="HAL_RT_samples HAL_samples RT_samples Tests HAL_setup"
for d in $PROJECTS_DIRS; do
    MK_DIRS=`find $d -name Makefile -exec bash -c 'echo ${1%/*}' -- {} \;`
    
    for mk in $MK_DIRS; do
        make clean -j8 -C $mk
    done
done
