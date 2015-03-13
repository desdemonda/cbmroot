. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

test_name=$1
trd_geom=$2

create_output_dir events_${test_name}/
create_result_dir ${test_name}/

nevents=1000000

export LIT_CAVE_GEOM=cave.geo
export LIT_TRD_GEOM=${trd_geom}
export LIT_MC_FILE=${LIT_DIR}/radlength.mc.0000.root
export LIT_PAR_FILE=${LIT_DIR}/radlength.param.0000.root
export LIT_RAD_LENGTH_QA_FILE=${LIT_DIR}/radlength.qa.0000.root
    
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/radlength_sim.C(${nevents})"
${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/radlength_ana.C(${nevents})"
 
export LIT_SCRIPT=no
