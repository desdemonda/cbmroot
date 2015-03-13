#!/bin/bash

export NOFEVENTS=1000
export mode=2

echo "Start of full conversion simulation"
echo "-----------------------------------------------------------------------"
root -l -b -q "run_sim.C($NOFEVENTS, $mode)" || exit 11
root -l -b -q "run_reco.C($NOFEVENTS, $mode)" || exit 12
root -l -b -q "run_analysis.C($NOFEVENTS, $mode)" || exit 13
echo "-----------------------------------------------------------------------"
echo "Chain finished successfully"
