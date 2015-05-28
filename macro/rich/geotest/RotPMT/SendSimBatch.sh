#!/bin/sh

nEvs=10
GeoCase=5
PtNotP=1
MomMin=0
MomMax=4

export SCRIPT=yes

export N_EVS=$nEvs
export GEO_CASE=$GeoCase
export PT_NOT_P=$PtNotP
export MOM_MIN=$MomMin
export MOM_MAX=$MomMax

for RotMir in 1 ;do
    export ROTMIR=$RotMir
    for RX in 0 5 10 15 20 25 30 35 40 45 50 ;do
#1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50;do
	export PMT_ROTX=$RX
	#for RY in 0 5 10 15 20 25 30 35 40 45 50 ;do
	for RY in 0;do
#1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50;do
	export PMT_ROTY=$RY

	    root -b -q "CreateGeo/CreateGDMLfileNew.C($RX, $RY, $RotMir)"
	    root -b -q "Import_GDML_Export_ROOT.c($RX, $RY, $RotMir)"

	    #root -b -l -q "Run_Sim_Display.C($nEvs,  $RX, $RY, $RotMir )"
	    root -b -l -q "Run_Sim_GeoOpt_Batch.C($nEvs,  $RX, $RY, $RotMir )"
	    root -b -l -q "Run_Reco_GeoOpt_Batch.C($nEvs,  $RX, $RY, $RotMir )"
	done
    done
done
