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

analyze_run_files /Users/slebedev/Development/cbm/data/cern_beamtime_2014/te14328171010.root results_te14328171010/ results_te14328171010 /Users/slebedev/Development/cbm/data/cern_beamtime_2014/te14328171010.hld


#example how to run analysis for all .hld files in directory
#analyze_run_directory outputRootFileWithHist.root outputDirWithImages/ titleOfAnalysis directoryWithHldFiles/

#analyze_run_directory all_test_file.root recoRR/ testRun dirWithHld


export SCRIPT=no
