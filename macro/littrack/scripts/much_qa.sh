#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

test_name=$1
much_geo=$2
much_digi=$3
urqmd=$4

dir_name=${much_geo}_${much_digi}_${much_hit_producer_type}
create_output_dir events_${test_name}/

nevents=500

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU  URQMD    UNIGEN
pars=(0    0    0   0   0    0    15       0      0  ${urqmd} no)
set_simulation_parameters ${pars}

set_default_muon_geometry
export LIT_MUCH_GEOM=${much_geo}
export LIT_MUCH_DIGI=${much_digi}

export LIT_STS_HITPRODUCER_TYPE=real
export LIT_TRD_HITPRODUCER_TYPE=smearing

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=/Users/andrey/Development/cbm/d/urqmd/auau/25gev/centr/urqmd.auau.25gev.centr.0000.ftn14
    
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"

function run_reco() {
   export LIT_MUCH_HITPRODUCER_TYPE=$1
   export LIT_GLOBAL_TRACKING_TYPE=$2
   export LIT_GLOBAL_RECO_FILE=${LIT_DIR}/global.reco.${LIT_MUCH_HITPRODUCER_TYPE}.${LIT_GLOBAL_TRACKING_TYPE}.0000.root
   create_result_dir ${test_name}_${LIT_MUCH_HITPRODUCER_TYPE}_${LIT_GLOBAL_TRACKING_TYPE}/
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"all\")"
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco_qa.C(${nevents}, \"reco\")"
}

#run_reco simple nn
#run_reco simple branch
run_reco advanced nn
run_reco advanced branch

export LIT_SCRIPT=no
