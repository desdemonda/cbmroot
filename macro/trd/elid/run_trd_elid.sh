export MY_BUILD_DIR=/Users/slebedev/Development/cbm/trunk/build/
export SCRIPT=yes

export RADIATOR_NAME=

for N in 0 1 2; do

   if [ ${N} = "0" ] ; then
      export RADIATOR_NAME=4mm_foam
   elif [ ${N} = "1" ]; then
      export RADIATOR_NAME=5mm_fibre
   elif [ ${N} = "2" ]; then
      export RADIATOR_NAME=4mm_f350
   fi

   xterm -hold -e ". ./sim.sh"&

done

export SCRIPT=no