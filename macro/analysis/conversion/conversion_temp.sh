#!/bin/bash

# this macro makes a copy of the run_sim.C, run_reco.C and run_analysis.C with a different name (just added a timestamp there)
# and executes these copies. after finishing all three steps the copies will be deleted after each step.
# the aim of this was to be able to simultaneously work/modify the three run_*.C macros, while another analysis is still running

export NOFEVENTS=10000
export mode=1

DATE=$(date +"%H%M%S")

SIM="run_sim_"$DATE".C"
REC="run_reco_"$DATE".C"
ANA="run_analysis_"$DATE".C"

SIM2="run_sim_"$DATE
REC2="run_reco_"$DATE
ANA2="run_analysis_"$DATE



cp run_sim.C $SIM
cp run_reco.C $REC
cp run_analysis.C $ANA

sed -i "s/run_sim/$SIM2/" $SIM
sed -i "s/run_reco/$REC2/" $REC
sed -i "s/run_analysis/$ANA2/" $ANA

echo "Start of full conversion simulation"
echo "-----------------------------------------------------------------------"
root -l -b -q "$SIM($NOFEVENTS, $mode)" || exit 11
rm -f $SIM
root -l -b -q "$REC($NOFEVENTS, $mode)" || exit 12
rm -f $REC
root -l -b -q "$ANA($NOFEVENTS, $mode)" || exit 13
rm -f $ANA
echo "-----------------------------------------------------------------------"
echo "Chain finished successfully"




