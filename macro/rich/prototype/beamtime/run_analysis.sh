#!/bin/sh

#Macro will be running via script
export SCRIPT=yes

#Build directory of CBMROOT
export MY_BUILD_DIR=/Users/slebedev/Development/cbm/trunk/build
cd $MY_BUILD_DIR
   . ./config.sh
   cd -

analyze_run_files() {
   export OUTPUT_ROOT_FILE_ALL=${1}
   export OUTPUT_DIR=${2}
   export RUN_TITLE=${3}
   shift
   shift
   shift
   filesArray=("${@}")
   
   mkdir -p ${OUTPUT_DIR}

   ROOT_FILES=
   SPACE=" "
   export TEMP_STORAGE_DIR=$(dirname ${OUTPUT_ROOT_FILE_ALL})
   for FF in "${filesArray[@]}"; do
      export CURRENT_FILE_NAME=$(basename ${FF})
      export INPUT_HLD_FILE=${FF}
      export OUTPUT_ROOT_FILE=${TEMP_STORAGE_DIR}/${CURRENT_FILE_NAME}.tempfile.hld.root
      rm -rf ${OUTPUT_ROOT_FILE}
      ROOT_FILES=${ROOT_FILES}${SPACE}${OUTPUT_ROOT_FILE}      
      root -b -q -l "./run_analysis.C()"
#      break;
   done
   
   hadd -f -T ${OUTPUT_ROOT_FILE_ALL} ${ROOT_FILES}
   rm -rf ${ROOT_FILES}
   
   root -b -q -l "./draw_analysis.C()"
}

analyze_run_directory() {
   filesArray=( ${4}/*.hld )
   analyze_run_files ${1} ${2} ${3} ${filesArray[@]}
}

#example how to run analysis for single (or multiple files)
#filesArray=( file1.hld file2.hld file3.hld file4.hld)
#analyze_run_files outputRootFileWithHist.root outputDirWithImages/ titleOfAnalysis ${filesArray[@]}

#analyze_run_files results/te14328171010.root results/ringB1/ ringB1 /Volumes/MY PASSPORT/beamtime_cern_2014/WLS/WLS_on/nonstretched/ringB1/offset00100/1222.hld

#example how to run analysis for all .hld files in directory
#analyze_run_directory outputRootFileWithHist.root outputDirWithImages/ titleOfAnalysis directoryWithHldFiles/

analyze_run_directory results_wlson_offset250/ringB2.root results_wlson_offset250/ringB2/ ringB2_wlson_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_on/nonstretched/ringB2/offset00250/
analyze_run_directory results_wlson_offset250/ringH1.root results_wlson_offset250/ringH1/ ringH1_wlson_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_on/nonstretched/ringH1/offset00250/
analyze_run_directory results_wlson_offset250/ringG1.root results_wlson_offset250/ringG1/ ringG1_wlson_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_on/nonstretched/ringG1/offset00250/
analyze_run_directory results_wlson_offset250/ringH2.root results_wlson_offset250/ringH2/ ringH2_wlson_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_on/nonstretched/ringH2/offset00250/
analyze_run_directory results_wlson_offset250/ringB3.root results_wlson_offset250/ringB3/ ringB3_wlson_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_on/nonstretched/ringB3/offset00250/
analyze_run_directory results_wlson_offset250/ringA1.root results_wlson_offset250/ringA1/ ringA1_wlson_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_on/nonstretched/ringA1/offset00250/
analyze_run_directory results_wlson_offset250/ringB4.root results_wlson_offset250/ringB4/ ringB4_wlson_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_on/nonstretched/ringB4/offset00250/


analyze_run_directory results_wlsoff_offset250/ringB1.root results_wlsoff_offset250/ringB1/ ringB1_wlsoff_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_off/nonstretched/ringB1/offset00250/
analyze_run_directory results_wlsoff_offset250/ringH1.root results_wlsoff_offset250/ringH1/ ringH1_wlsoff_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_off/nonstretched/ringH1/offset00250/
analyze_run_directory results_wlsoff_offset250/ringG1.root results_wlsoff_offset250/ringG1/ ringG1_wlsoff_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_off/nonstretched/ringG1/offset00250/
analyze_run_directory results_wlsoff_offset250/ringH2.root results_wlsoff_offset250/ringH2/ ringH2_wlsoff_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_off/nonstretched/ringH2/offset00250/
analyze_run_directory results_wlsoff_offset250/ringB2.root results_wlsoff_offset250/ringB2/ ringB2_wlsoff_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_off/nonstretched/ringB2/offset00250/
analyze_run_directory results_wlsoff_offset250/ringA1.root results_wlsoff_offset250/ringA1/ ringA1_wlsoff_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_off/nonstretched/ringA1/offset00250/
analyze_run_directory results_wlsoff_offset250/ringB3.root results_wlsoff_offset250/ringB3/ ringB3_wlsoff_offset250 /Volumes/MY_PASSPORT/beamtime_cern_2014/WLS/WLS_off/nonstretched/ringB3/offset00250/
export SCRIPT=no
