#!/bin/sh

# Pathes and parameters
test_dir=/data.local2/andrey/tests/cdash_tests
build_dir=${test_dir}/cbmroot_build_dec13

function checkout_cbmroot
{
    dir=$1
    rm -rf ${dir}/*
    cd ${dir}/
    svn co  https://subversion.gsi.de/fairroot/cbmroot/trunk cbmroot
    cp -R /data.local2/andrey/tests/fieldmaps/fieldmaps/field_v* ${dir}/cbmroot/input/
    cp /data.local2/andrey/tests/cdash_config/Dart.cfg ${dir}/cbmroot/Dart.cfg
}

function build_cbmroot 
{
    src_dir=$1
    build_dir=$2
    simpath=$3
    export SIMPATH=${simpath}
    mkdir -p ${build_dir}
    cd ${build_dir}
    cmake ${src_dir}
    . ${build_dir}/config.sh
    make -j8
}

function copy_results_to_web
{
    src_dir=$1
    web_dir=/u/andrey/web-docs/tests
    day=`date +%y-%m-%d`
    rm -rf ${web_dir}/${day}
    mkdir -p ${web_dir}/${day}
    cp -R ${src_dir} ${web_dir}/${day}/
    chmod -R u+rwX,g+rx,o+rx ${web_dir}/${DAY}/
}

# Run test without CDash
if [ "$1" = "build" ] ; then
    checkout_cbmroot ${test_dir}
    build_cbmroot ${test_dir}/cbmroot ${build_dir} /data.local2/fairsoft/dec13/

    # Run tests
    . ${VMCWORKDIR}/macro/littrack/nightly_tests/run_test_electron.sh
    . ${VMCWORKDIR}/macro/littrack/nightly_tests/run_test_muon.sh
    
    copy_results_to_web "${test_dir}/cbmroot/macro/littrack/nightly_tests/littrack*"
elif [ "$1" = "cdash" ] ; then 
    checkout_cbmroot ${test_dir}
    
    # Patch CbmRoot_test.cmake
    rm ${test_dir}/cbmroot/CbmRoot_test.cmake
    cp /data.local2/andrey/tests/patch/CbmRoot_test.cmake ${test_dir}/cbmroot/CbmRoot_test.cmake
    
    # Run tests with CDash
    ${test_dir}/cbmroot/Dart.sh Experimental ${test_dir}/cbmroot/Dart.cfg
    
    copy_results_to_web "${build_dir}/macro/littrack/nightly_tests/littrack*"
elif [ "$1" = "local" ] ; then
    . ./run_test_electron.sh
    . ./run_test_muon.sh
    
    copy_results_to_web "./littrack*"
else
    echo 'Error! Option $1 is not supported!'
    exit 2
fi
