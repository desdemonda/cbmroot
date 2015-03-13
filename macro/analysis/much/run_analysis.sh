#!/bin/bash

export DIR=`expr $1`
export FILENO=`expr $2`
export NEVENTS=`expr $3`
export RANSEED=`expr $4`

echo "Start of full polarization simulation"
echo "-----------------------------------------------------------------------"
root -l -b -q "much_sim.C(\"$DIR\",$FILENO,$NEVENTS,$RANSEED)" || exit 11
root -l -b -q "sts_reco.C(\"$DIR\",$FILENO,$NEVENTS)"          || exit 12
root -l -b -q "much_hits.C(\"$DIR\",$FILENO,$NEVENTS)"         || exit 13
root -l -b -q "much_reco.C(\"$DIR\",$FILENO,$NEVENTS)"         || exit 14
root -l -b -q "much_pol.C(\"$DIR\",$FILENO,$NEVENTS)"          || exit 15
echo "-----------------------------------------------------------------------"
echo "Chain finished successfully"
