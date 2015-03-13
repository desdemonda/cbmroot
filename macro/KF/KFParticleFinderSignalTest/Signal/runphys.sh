#!/bin/sh

rm -rf Efficiency.txt
root -l -b -q "physSignal.C("$1")"      2>&1 | tee phys.log

