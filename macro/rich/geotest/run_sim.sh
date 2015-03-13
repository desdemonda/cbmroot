#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MY_BUILD_DIR=/Users/slebedev/Development/cbm/trunk/build/

export RESULT_DIR=results/

# Geometries
export CAVE_GEOM=cave.geo
export TARGET_GEOM=target/target_au_250mu.geo
export PIPE_GEOM=pipe/pipe_standard.geo
export STS_GEOM=sts/sts_v12b.geo.root
export RICH_GEOM=rich/rich_v08a.geo
export FIELD_MAP=field_v12a
export MAGNET_GEOM=magnet/magnet_v12a.geo
export FIELD_MAP_SCALE=1.0

export STS_MATERIAL_BUDGET_FILE_NAME=/sts/sts_matbudget_v12b.root
export STS_DIGI_FILE=/sts/sts_v13c_std.digi.par

#number of events for each thread
export NEVENTS=200

for Z in 1; do
   export DIR=/Users/slebedev/Development/cbm/data/simulations/rich/urqmdtest/al/

   export RESULT_DIR=results_urqmdtest_pipe_1_al_1/
   mkdir -p ${RESULT_DIR}

   export RICH_GEOM=rich/rich_v13c_pipe_1_al_1.root
   export MC_FILE=${DIR}/25gev.centr.mc.pipe_1_al_1.root
   export PAR_FILE=${DIR}/25gev.centr.param.pipe_1_al_1.root
   export RECO_FILE=${DIR}/25gev.centr.reco.pipe_1_al_1.root
   
  
  # ./sim.sh
   xterm -hold -e ". ./sim.sh"&
 #  bsub -q batch -J mc.$Z.$XXXX.run -o $DIR/log/$XXXX.log -N sh ./sim.sh
   

done
export SCRIPT=no
