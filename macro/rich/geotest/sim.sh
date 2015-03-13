#!/bin/sh
cd $MY_BUILD_DIR
. ./config.sh
cd -

root -l -q "./run_sim_urqmdtest.C($NEVENTS)"

root  -l -q "./run_reco_urqmdtest.C($NEVENTS)"
