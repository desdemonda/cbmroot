=#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MY_BUILD_DIR=/Users/slebedev/Development/cbm/trunk/build

# Geometries
export CAVE_GEOM=cave.geo
export RICH_GEOM=rich/prototype/RichPrototype-1Mir-Standard.geo;

export MIN_MOMENTUM=7;
export MAX_MOMENTUM=7;

export NOF_NOISE_HITS_IN_RICH=0
export RICH_COLLECTION_EFF=1.0
export SIGMA_ERROR_RICH=0.06
export CROSSTALK_RICH=0.02
export PMT_TYPE=6
# choose ring finder: "ideal" or "hough_prototype"
export RING_FINDER=hough_prototype

#number of events for each thread
export NEVENTS=100

for Z in 1; do

   export DIR=/Users/slebedev/Development/cbm/data/simulations/richprototype/

   export IN_ASCII_FILE=${DIR}/epi.ascii.dat
   export MC_FILE=${DIR}/mc.root
   export PAR_FILE=$DIR/param.root
   export RECO_FILE=$DIR/reco.root
   
  . ./sim.sh
  
  # xterm -hold -e ". ./sim.sh"&
   
done
export SCRIPT=no
