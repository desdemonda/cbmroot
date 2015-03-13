#!/bin/sh

#----------------------------------------------------------------------------------
# // 2014-07-04 - DE - script executed upon cdash tests  
# // 2014-07-04 - DE - run root macros for tests of geometry/setup
#
# run as follows:
#
# . run_test.sh sis100_electron sim
# . run_test.sh sis100_electron reco
# . run_test.sh sis100_electron digi
# . run_test.sh sis100_electron qa
# . run_test.sh sis100_electron overlap
#----------------------------------------------------------------------------------

setup=$1
opt=$2

nevents=1

if [ "${opt}" = "sim" ] ; then
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/run/run_sim.C(${nevents}, \"${setup}\")"
elif [ "${opt}" = "reco" ] ; then
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/run/run_reco.C(${nevents}, \"${setup}\")"
elif [ "${opt}" = "digi" ] ; then
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/run/run_digi.C(${nevents}, \"${setup}\")"
elif [ "${opt}" = "qa" ] ; then
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/run/run_qa.C(${nevents}, \"${setup}\")"
elif [ "${opt}" = "overlap" ] ; then
   ${ROOTSYS}/bin/root -b -q -l "${VMCWORKDIR}/macro/run/check_overlaps.C(\"${setup}\")"
fi
