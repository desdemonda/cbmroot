#!/bin/sh

. ${VMCWORKDIR}/macro/littrack/scripts/common.sh

export LIT_SCRIPT=yes

#all, sim, reco
setup=$1
opt=$2

if [ "${opt}" = "all" ] ; then
   create_output_dir events_${setup}/
   create_result_dir results_${setup}/
else
   export LIT_DIR=events_${setup}/
   export LIT_RESULT_DIR=results_${setup}/
fi

nevents=5

if [ "${setup}" = "electron" ] ; then
   #     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN PLUTO
   pars=(0    0    0   0   0    0    0       10      0  yes   yes   no)
   set_simulation_parameters ${pars}
   set_default_electron_geometry
elif [ "${setup}" = "muon" ] ; then
   #     NMU+ NMU- NE- NE+ NPI+ NPI- NJPSIMU NJPSIE AU URQMD UNIGEN PLUTO
   pars=(0    0    0   0   0    0    10       0      0  yes   yes   no)
   set_simulation_parameters ${pars}
   set_default_muon_geometry
fi

export LIT_STS_HITPRODUCER_TYPE=real
export LIT_TRD_HITPRODUCER_TYPE=smearing
export LIT_MUCH_HITPRODUCER_TYPE=advanced
export LIT_GLOBAL_TRACKING_TYPE=branch

set_default_file_names ${LIT_DIR} 0000

export LIT_URQMD_FILE=${VMCWORKDIR}/input/urqmd.auau.25gev.centr.root

if [ "${opt}" = "all" ] ; then
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"all\")"
elif [ "${opt}" = "sim" ] ; then
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_sim.C(${nevents})"
elif [ "${opt}" = "reco" ] ; then
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/littrack/global_reco.C(${nevents}, \"all\")"
fi

export LIT_SCRIPT=no
