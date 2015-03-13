#!/bin/bash
#
##   a sim job with 1000 events runs for about 03:30 h
#

CBMPATH=/data6/cbm/software/cbmroot_trunk_20140702/macro/trd/maf

## NEVENTS can be between 1 and 1000 - (limit is number of events in urqmd file)
NEVENTS=1000
#//NEVENTS=1

# NURQMD can be between 0 and 399 - (number of urqmd files)
NURQMDSTART=0
NURQMDEND=149
#NURQMDEND=399

for i in `seq -f %04.0f $NURQMDSTART $NURQMDEND` 
do
  echo -n "$i " 

  if [ -s /data6/cbm/software/sim/test.params.$i.root ] 
  then
    echo exists
  else
    j=`expr $i + 0`
#     echo $j

#   qsub -q "long"   -d $CBMPATH/.. -e $CBMPATH/output -o $CBMPATH/output -v NEVENTS=$NEVENTS,NURQMD=$j "$CBMPATH/maf_run_sim.sh";
    qsub -q "medium" -d $CBMPATH/.. -e $CBMPATH/output -o $CBMPATH/output -v NEVENTS=$NEVENTS,NURQMD=$j "$CBMPATH/maf_run_sim.sh";
#   qsub -q "short"  -d $CBMPATH/.. -e $CBMPATH/output -o $CBMPATH/output -v NEVENTS=$NEVENTS,NURQMD=$i "$CBMPATH/maf_run_sim.sh";
    sleep 5
  fi 

done 
