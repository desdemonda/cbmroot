#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

test_name=$1
sts_geom=$2
sts_digi=$3
sts_mat_budget_file=$4
consequtive_sts_points=$5

create_output_dir events_${test_name}/
create_result_dir ${test_name}/

nevents=1000

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN
pars=(0    0    0   0   0    0    0       10     0  yes   no)
set_simulation_parameters ${pars}    

set_default_sts_geometry

export LIT_STS_HITPRODUCER_TYPE=real
export LIT_STS_GEOM=${sts_geom}
export LIT_STS_DIGI=${sts_digi}
export LIT_STS_MAT_BUDGET_FILE=${sts_mat_budget_file}
export LIT_GLOBAL_TRACKING_TYPE=branch
export LIT_CONSEQUTIVE_STS_POINTS=${consequtive_sts_points}

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=/Users/andrey/Development/cbm/d/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14
    
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"all\")"
 
export LIT_SCRIPT=no
