#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

test_name=$1
detector_setup=$2
main_detector_geometry=$3
collision=$4

# Parse collision
collision_arr=(${collision//_/ })
system=${collision_arr[0]}
energy=${collision_arr[1]}
collision_type=${collision_arr[2]}

create_output_dir events_${test_name}/

nevents=400

if [ "${detector_setup}" = "electron" ] ; then
    #     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN PLUTO
    pars=(0    0    0   0   0    0    0       10     0  yes   no     no)
    set_default_electron_geometry
    export LIT_TRD_GEOM=trd/trd_${main_detector_geometry}.geo.root
    export LIT_TRD_DIGI=${VMCWORKDIR}/parameters/trd/trd_${main_detector_geometry}.digi.par
elif [ "${detector_setup}" = "muon" ] ; then
    #     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN PLUTO
    pars=(0    0    0   0   0    0    10       0     0  yes   no     no)
    set_default_muon_geometry
    export LIT_MUCH_GEOM=much/much_${main_detector_geometry}.geo
    export LIT_MUCH_DIGI=${VMCWORKDIR}/parameters/much/much_${main_detector_geometry}.digi.root
else
    echo "Error! Detector setup unknown! Must be electron or muon!"
    exit 2
fi

set_simulation_parameters ${pars} 

export LIT_STS_HITPRODUCER_TYPE=real
export LIT_TRD_HITPRODUCER_TYPE=smearing
export LIT_MUCH_HITPRODUCER_TYPE=advanced

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=/data.local2/andrey/tests/urqmd/${system}/${energy}/${collision_type}/urqmd.${system}.${energy}.${collision_type}.0001.ftn14

$ROOTSYS/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
$ROOTSYS/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"hits\")"

# Test different global tracking algorithms
# Branching algorithm
export LIT_GLOBAL_TRACKING_TYPE=branch
export LIT_GLOBAL_TRACKS_FILE=${LIT_DIR}/global.tracks.branch.0000.root
export LIT_QA_FILE=${test_name}_${LIT_GLOBAL_TRACKING_TYPE}/qa.root
create_result_dir ${test_name}_${LIT_GLOBAL_TRACKING_TYPE}/
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"tracking\")"
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco_qa.C(${nevents}, \"ht\")"
# Nearest neighbor algorithm
export LIT_GLOBAL_TRACKING_TYPE=nn
export LIT_GLOBAL_TRACKS_FILE=${LIT_DIR}/global.tracks.nn.0000.root
export LIT_QA_FILE=${test_name}_${LIT_GLOBAL_TRACKING_TYPE}/qa.root
create_result_dir ${test_name}_${LIT_GLOBAL_TRACKING_TYPE}/
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"tracking\")"
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco_qa.C(${nevents}, \"ht\")"
# Nearest neighbor parallel algorithm
#export LIT_GLOBAL_TRACKING_TYPE=nn_parallel
#export LIT_GLOBAL_TRACKS_FILE=${LIT_DIR}/global.tracks.nn_parallel.0000.root
#export LIT_QA_FILE=${test_name}_${LIT_GLOBAL_TRACKING_TYPE}/qa.root
#create_result_dir ${test_name}_${LIT_GLOBAL_TRACKING_TYPE}/
#${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"tracking\")"
#${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco_qa.C(${nevents}, \"ht\")"

# Run summary report generator
export LIT_STUDY_OUTPUT_DIR=${test_name}_summary/
rm -rf ${LIT_STUDY_OUTPUT_DIR}
mkdir ${LIT_STUDY_OUTPUT_DIR}
export LIT_NOF_STUDIES=2
export LIT_STUDY_NAME1=branch
export LIT_FILE_NAME1=${test_name}_branch/qa.root
export LIT_STUDY_NAME2=nn
export LIT_FILE_NAME2=${test_name}_nn/qa.root
#export LIT_STUDY_NAME3=nn_parallel
#export LIT_FILE_NAME3=${test_name}_nn_parallel/qa.root
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/qa_study_report.C"

export LIT_SCRIPT=no
