#!/bin/sh
cd $MY_BUILD_DIR
. ./config.sh
cd -

root -b -q "./run_ascii_generator.C($NEVENTS)"

root -b -q "./run_sim.C($NEVENTS)"

root -l "./run_reco.C($NEVENTS)"

