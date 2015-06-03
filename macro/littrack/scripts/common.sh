#!/bin/sh

# Function sets simulation parameters
function set_simulation_parameters() {
    pars=$1
    # number of embedded muons from FairBoxGenerator
    export LIT_NOF_MUONS_PLUS=${pars[0]}
    # number of embedded muons from FairBoxGenerator
    export LIT_NOF_MUONS_MINUS=${pars[1]}
    # number of embedded electrons from FairBoxGenerator
    export LIT_NOF_ELECTRONS=${pars[2]}
    # number of embedded positrons from FairBoxGenerator
    export LIT_NOF_POSITRONS=${pars[3]}
    # number of embedded pions from FairBoxGenerator
    export LIT_NOF_PIONS_PLUS=${pars[4]}
    # number of embedded pions from FairBoxGenerator
    export LIT_NOF_PIONS_MINUS=${pars[5]}
    # number of embedded J/Psi decayed to mu+ and mu- 
    export LIT_NOF_JPSI_TO_MUONS=${pars[6]}
    # number of embedded J/Psi decayed to e+ and e- 
    export LIT_NOF_JPSI_TO_ELECTRONS=${pars[7]}
    # number of generated Au ions
    export LIT_NOF_AU_IONS=${pars[8]}
    # If "yes" than UrQMD will be used as background
    export LIT_URQMD=${pars[9]}
    # If "yes" than CbmUnigenGenerator will be used instead of FairUrqmdGenerator
    export LIT_UNIGEN=${pars[10]}
    # If "yes" than PLUTO will be used
    export LIT_PLUTO=${pars[11]}
}



# Function sets default file names using a specified DIR and file number
function set_default_file_names() {
    dir=$1
    XXXX=$2
    export LIT_MC_FILE=${dir}/mc.${XXXX}.root
    export LIT_PAR_FILE=${dir}/param.${XXXX}.root
    export LIT_MVD_DELTA_FILE=${dir}/mc.delta.${XXXX}.root
    export LIT_GLOBAL_RECO_FILE=${dir}/global.reco.${XXXX}.root
    export LIT_GLOBAL_HITS_FILE=${dir}/global.hits.${XXXX}.root
    export LIT_GLOBAL_TRACKS_FILE=${dir}/global.tracks.${XXXX}.root
    export LIT_GLOBAL_TRACKS_IDEAL_FILE=${dir}/global.tracks.ideal.${XXXX}.root
    export LIT_MVD_RECO_FILE=${dir}/mvd.reco.${XXXX}.root
    export LIT_PROP_ANA_FILE=${dir}/propagation.ana.${XXXX}.root
    export LIT_RICH_FILE=${dir}/rich.reco.${XXXX}.root
    export LIT_ELID_FILE=${dir}/elid.qa.${XXXX}.root
    export LIT_FIELD_QA_FILE=${dir}/field.qa.${XXXX}.root
    export LIT_FIELDAPR_QA_FILE=${dir}/fieldapr.qa.${XXXX}.root
    export LIT_ANALYSIS_FILE=${dir}/analysis.${XXXX}.root
    export LIT_QA_FILE=${dir}/qa.${XXXX}.root
}



# Function sets the default muon geometry
function set_default_muon_geometry() {
    export LIT_CAVE_GEOM=cave.geo
    export LIT_PIPE_GEOM=pipe/pipe_v13c.geo.root
    export LIT_SHIELD_GEOM=much/shield_v13f.geo
    export LIT_MVD_GEOM=
    export LIT_STS_GEOM=sts/sts_v13d.geo.root
    export LIT_STS_DIGI=${VMCWORKDIR}/parameters/sts/sts_v13d_std.digi.par
    export LIT_STS_MAT_BUDGET_FILE=${VMCWORKDIR}/parameters/sts/sts_matbudget_v13d.root
    export LIT_MUCH_GEOM=much/much_v13f.geo
    export LIT_MUCH_DIGI=${VMCWORKDIR}/parameters/much/much_v13f.digi.root
    export LIT_RICH_GEOM=
    export LIT_TRD_GEOM=
    export LIT_TRD_DIGI=
    export LIT_TOF_GEOM=tof/tof_v13-5e.geo.root
    export LIT_TOF_DIGI=${VMCWORKDIR}/parameters/tof/tof_v13-5e.digi.par
    export LIT_ECAL_GEOM=
    export LIT_FIELD_MAP=field_v12b
    export LIT_MAGNET_GEOM=magnet/magnet_v12b.geo.root
    
    export LIT_CONSEQUTIVE_STS_POINTS=1
    export LIT_NORM_STS_POINTS=4
    export LIT_NORM_TRD_POINTS=0
    export LIT_NORM_MUCH_POINTS=16
    export LIT_NORM_TOF_POINTS=1
    export LIT_NORM_TRD_HITS=0
    export LIT_NORM_MUCH_HITS=15
    export LIT_NORM_TOF_HITS=1
}



# Function sets the default electron geometry
function set_default_electron_geometry() {
    export LIT_CAVE_GEOM=cave.geo
    export LIT_PIPE_GEOM=pipe/pipe_v14y.geo.root
    export LIT_SHIELD_GEOM=
    export LIT_MVD_GEOM=
    export LIT_STS_GEOM=sts/sts_v13d.geo.root
    export LIT_STS_DIGI=${VMCWORKDIR}/parameters/sts/sts_v13d_std.digi.par
    export LIT_STS_MAT_BUDGET_FILE=${VMCWORKDIR}/parameters/sts/sts_matbudget_v13d.root
    export LIT_MUCH_GEOM=
    export LIT_MUCH_DIGI=
    export LIT_RICH_GEOM=rich/rich_v14a.root
    export LIT_TRD_GEOM=trd/trd_v15a_3e.geo.root
    export LIT_TRD_DIGI=${VMCWORKDIR}/parameters/trd/trd_v15a_3e.digi.par
    export LIT_TOF_GEOM=tof/tof_v13-5d.geo.root
    export LIT_TOF_DIGI=${VMCWORKDIR}/parameters/tof/tof_v13-5d.digi.par
    export LIT_ECAL_GEOM=
    export LIT_FIELD_MAP=field_v12b
    export LIT_MAGNET_GEOM=magnet/magnet_v12b.geo.root
    
    export LIT_CONSEQUTIVE_STS_POINTS=1
    export LIT_NORM_STS_POINTS=4
    export LIT_NORM_TRD_POINTS=6
    export LIT_NORM_MUCH_POINTS=0
    export LIT_NORM_TOF_POINTS=1
    export LIT_NORM_TRD_HITS=6
    export LIT_NORM_MUCH_HITS=0
    export LIT_NORM_TOF_HITS=1
}



# Function set default mvd geometry
function set_default_mvd_geometry() {
    export LIT_CAVE_GEOM=cave.geo
    export LIT_PIPE_GEOM=pipe/pipe_v14l.root
    export LIT_SHIELD_GEOM=
    export LIT_MVD_GEOM=mvd/mvd_v14b.geo.root
    export LIT_STS_GEOM=sts/sts_v13d.geo.root
    export LIT_STS_DIGI=${VMCWORKDIR}/parameters/sts/sts_v13d_std.digi.par
    export LIT_STS_MAT_BUDGET_FILE=${VMCWORKDIR}/parameters/sts/sts_matbudget_v13d.root
    export LIT_MUCH_GEOM=
    export LIT_MUCH_DIGI=
    export LIT_RICH_GEOM=
    export LIT_TRD_GEOM=
    export LIT_TRD_DIGI=
    export LIT_TOF_GEOM=
    export LIT_TOF_DIGI=
    export LIT_ECAL_GEOM=
    export LIT_FIELD_MAP=field_v12b
    export LIT_MAGNET_GEOM=magnet/magnet_v12b.geo.root
    
    export LIT_CONSEQUTIVE_STS_POINTS=1
    export LIT_NORM_STS_POINTS=4
    export LIT_NORM_TRD_POINTS=0
    export LIT_NORM_MUCH_POINTS=0
    export LIT_NORM_TOF_POINTS=0
    export LIT_NORM_TRD_HITS=0
    export LIT_NORM_MUCH_HITS=0
    export LIT_NORM_TOF_HITS=0
}

# Function set default sts geometry
function set_default_sts_geometry() {
    export LIT_CAVE_GEOM=cave.geo
    export LIT_PIPE_GEOM=pipe/pipe_v13a.geo.root
    export LIT_SHIELD_GEOM=
    export LIT_MVD_GEOM=
    export LIT_STS_GEOM=sts/sts_v13d.geo.root
    export LIT_STS_DIGI=${VMCWORKDIR}/parameters/sts/sts_v13d_std.digi.par
    export LIT_STS_MAT_BUDGET_FILE=${VMCWORKDIR}/parameters/sts/sts_matbudget_v13d.root
    export LIT_MUCH_GEOM=
    export LIT_MUCH_DIGI=
    export LIT_RICH_GEOM=
    export LIT_TRD_GEOM=
    export LIT_TRD_DIGI=
    export LIT_TOF_GEOM=
    export LIT_TOF_DIGI=
    export LIT_ECAL_GEOM=
    export LIT_FIELD_MAP=field_v12b
    export LIT_MAGNET_GEOM=magnet/magnet_v12b.geo.root
    
    export LIT_CONSEQUTIVE_STS_POINTS=1
    export LIT_NORM_STS_POINTS=4
    export LIT_NORM_TRD_POINTS=0
    export LIT_NORM_MUCH_POINTS=0
    export LIT_NORM_TOF_POINTS=0
    export LIT_NORM_TRD_HITS=0
    export LIT_NORM_MUCH_HITS=0
    export LIT_NORM_TOF_HITS=0
}



# Function set default sts geometry
function set_default_sts_tof_geometry() {
    export LIT_CAVE_GEOM=cave.geo
    export LIT_PIPE_GEOM=pipe/pipe_v13a.geo.root
    export LIT_SHIELD_GEOM=
    export LIT_MVD_GEOM=
    export LIT_STS_GEOM=sts/sts_v13d.geo.root
    export LIT_STS_DIGI=${VMCWORKDIR}/parameters/sts/sts_v13d_std.digi.par
    export LIT_STS_MAT_BUDGET_FILE=${VMCWORKDIR}/parameters/sts/sts_matbudget_v13d.root
    export LIT_MUCH_GEOM=
    export LIT_MUCH_DIGI=
    export LIT_RICH_GEOM=
    export LIT_TRD_GEOM=
    export LIT_TRD_DIGI=
    export LIT_TOF_GEOM=tof/tof_v13b.geo.root
    export LIT_TOF_DIGI=${VMCWORKDIR}/parameters/tof/tof_v13b.digi.par
    export LIT_ECAL_GEOM=
    export LIT_FIELD_MAP=field_v12b
    export LIT_MAGNET_GEOM=magnet/magnet_v12b.geo.root
    
    export LIT_CONSEQUTIVE_STS_POINTS=1
    export LIT_NORM_STS_POINTS=4
    export LIT_NORM_TRD_POINTS=0
    export LIT_NORM_MUCH_POINTS=0
    export LIT_NORM_TOF_POINTS=1
    export LIT_NORM_TRD_HITS=0
    export LIT_NORM_MUCH_HITS=0
    export LIT_NORM_TOF_HITS=1
}



# Function exports and creates output directories for LIT_DIR
function create_output_dir()
{
    export LIT_DIR=$1
    rm -r -f ${LIT_DIR}
    mkdir -p ${LIT_DIR}
}



# Function exports and creates output directories for LIT_RESULT_DIR
function create_result_dir()
{
    export LIT_RESULT_DIR=$1
    rm -r -f ${LIT_RESULT_DIR}
    mkdir -p ${LIT_RESULT_DIR}
}

