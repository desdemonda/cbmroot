#!/bin/sh
cd /data/cbmroot/buildcbm
#make
cd -    
RotMir=1

#for RX in 2 3 4  5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20;do
#for RX in 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25;do
for RY in 0;do
    for RX in 0 ;do
	#create geofile.gdml for given PMT-rotations and mirror rotation 
	root -b -q "CreateGeo/CreateGDMLfileNew.C($RX, $RY, $RotMir)"
	#root -b -q "CreateGeo/CreateGDMLfileOld.C($RX, $RY, $RotMir)"
	
	#create geofile.gdml for given PMT-rotations (mirror rotation =-1) 
	#root -b -q "CreateGeo/CreateInputForGeo.C($RX,$RY )"
    	#root -b -q "CreateGeo/create_RICH_geo_file.C($RX,$RY)"

	nEvs=10000
	#run sim and rec for given PMT-rotations, mirror rotation, 
	#and geometry case: 
	#case 0 --> old geometry with *.geo (RICH starts at 1800, Mirror tilt -1) 
	#case 1 --> gdml-geo: RICH starts at 1800, Mirror tilt -1 or 10,
	#           mirror does NOT cover full acceptance)
	#case 0 --> gdml-geo: RICH starts at 1800, Mirror tilt -1 or 10,
	#           mirror does cover full acceptance)

	#root -b -q "run_sim_geo_opt.C($nEvs,$RX, $RY, $RotMir,0)"
	#root -b -q "run_sim_geo_opt.C($nEvs,$RX, $RY, $RotMir,1)"
	root -b -q "run_sim_geo_opt.C($nEvs,$RX, $RY, $RotMir,2)"

	#root -b -q "run_reco_geo_opt.C($nEvs,$RX, $RY, $RotMir,0)"
	#root -b -q "run_reco_geo_opt.C($nEvs,$RX, $RY, $RotMir,1)"
	root -b -q "run_reco_geo_opt.C($nEvs,$RX, $RY, $RotMir,2)"	
    done
done
