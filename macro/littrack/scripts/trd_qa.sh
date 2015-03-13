#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

test_name=$1
trd_geom=$2
trd_digi=$3
trd_norm=$4
collision_energy=$5

create_output_dir events_${test_name}/

nevents=300

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN
pars=(0    0    0   0   0    0    0       10     0  yes   no)
set_simulation_parameters ${pars}    

set_default_electron_geometry

export LIT_STS_HITPRODUCER_TYPE=real
export LIT_TRD_GEOM=${trd_geom}
export LIT_TRD_DIGI=${trd_digi}
export LIT_GLOBAL_TRACKING_TYPE=branch
export LIT_NORM_TRD_POINTS=$trd_norm
export LIT_NORM_TRD_HITS=$trd_norm

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=/Users/andrey/Development/cbm/d/urqmd/auau/${collision_energy}/centr/urqmd.auau.${collision_energy}.centr.0000.ftn14
    
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"

function run_reco() {
   export LIT_TRD_HITPRODUCER_TYPE=$1
   export LIT_GLOBAL_TRACKING_TYPE=$2
   export LIT_GLOBAL_RECO_FILE=${LIT_DIR}/global.reco.${LIT_TRD_HITPRODUCER_TYPE}.${LIT_GLOBAL_TRACKING_TYPE}.0000.root
   export LIT_QA_FILE=${LIT_DIR}/qa.${LIT_TRD_HITPRODUCER_TYPE}.${LIT_GLOBAL_TRACKING_TYPE}.0000.root
   create_result_dir ${test_name}_${LIT_TRD_HITPRODUCER_TYPE}_${LIT_GLOBAL_TRACKING_TYPE}/
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"all\")"
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco_qa.C(${nevents}, \"reco\")"
}

run_reco smearing nn
run_reco smearing branch
run_reco digi nn
run_reco digi branch
run_reco clustering nn
run_reco clustering branch

export LIT_SCRIPT=no