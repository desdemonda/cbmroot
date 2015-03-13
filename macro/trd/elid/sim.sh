#!/bin/sh


export MY_BUILD_DIR=/Users/slebedev/Development/cbm/trunk/build/
export SCRIPT=yes

cd $MY_BUILD_DIR
. ./config.sh
cd -

for M in 0 1 2 3 4; do
   export ID_METHOD=${M}
   
   for N in 6 7 8 9 10 11 12; do
      export NOF_TRD_LAYERS=${N}
      export OUTPUT_DIR=results/${RADIATOR_NAME}/${ID_METHOD}/${NOF_TRD_LAYERS}/
      mkdir -p ${OUTPUT_DIR}
      root -b -q "./trd_elid_train.C"
   
   done
done

export SCRIPT=no



