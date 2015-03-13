#!/bin/bash
. /data6/cbm/software/cbmroot_trunk_20140702/build/config.sh > /dev/null
nEvents=$NEVENTS
urqmd=$NURQMD
echo "root -l -b -q run_reco_maf.C'('$nEvents,$urqmd')'"
#echo "nice root -l -b -q run_reco_maf.C'('$nEvents,$urqmd')' &"  > myreco
echo "root -l -b -q run_reco_maf.C'('$nEvents,$urqmd')'"  > mymafreco
. mymafreco
