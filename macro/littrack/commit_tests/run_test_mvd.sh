#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

setup=$1
opt=$2

if [ "${opt}" = "all" ] ; then
   create_output_dir events_mvd_${setup}/
   create_result_dir results_mvd_${setup}/
else
   export LIT_DIR=events_mvd_${setup}/
   export LIT_RESULT_DIR=results_mvd_${setup}/
fi

nevents=5

set_default_mvd_geometry

export LIT_STS_HITPRODUCER_TYPE=real
   
export LIT_NOF_MVD_DELTA_EVENTS=0
   
set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=${VMCWORKDIR}/input/urqmd.auau.25gev.centr.root

function run_sim() {
   # Generate file for delta electrons
   if [ "${setup}" = "delta" ] ; then
       #     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN
       pars=(0    0    0   0   0    0    0       0      1  no    no)
       set_simulation_parameters ${pars}
       export LIT_MC_FILE=${LIT_DIR}/mc.delta.0000.root
       export LIT_PAR_FILE=${LIT_DIR}/param.delta.0000.root
       ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(20)"
       export LIT_NOF_MVD_DELTA_EVENTS=10
   fi
  
   #     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN
   pars=(0    0    0   0   0    0    0       10      0  yes   yes)
   set_simulation_parameters ${pars}   

   export LIT_MC_FILE=${LIT_DIR}/mc.0000.root
   export LIT_PAR_FILE=${LIT_DIR}/param.0000.root
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
}

if [ "${opt}" = "all" ] ; then
   run_sim
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/mvd_reco.C(${nevents})"
elif [ "${opt}" = "sim" ] ; then
   run_sim
elif [ "${opt}" = "reco" ] ; then
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/mvd_reco.C(${nevents})"
fi


export LIT_SCRIPT=no