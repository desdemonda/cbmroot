. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

test_name=$1
tof_geom=$2
tof_digi=$3
collision_energy=$4

create_output_dir events_${test_name}/
create_result_dir ${test_name}/

nevents=500

#     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN
pars=(0    0    0   0   0    0    0       10     0  yes   no)
set_simulation_parameters ${pars}    

set_default_sts_tof_geometry

export LIT_STS_HITPRODUCER_TYPE=real
export LIT_TOF_GEOM=${tof_geom}
export LIT_TOF_DIGI=${tof_digi}
export LIT_GLOBAL_TRACKING_TYPE=branch

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=/Users/andrey/Development/cbm/d/urqmd/auau/${collision_energy}/centr/urqmd.auau.${collision_energy}.centr.0000.ftn14
    
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"all\")"
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco_qa.C(${nevents}, \"reco\")"
 
export LIT_SCRIPT=no
