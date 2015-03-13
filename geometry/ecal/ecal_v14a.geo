# Distance from calorimeter to target is 7.5 m.
# Total thickness of absorber is 22 X_0. (No preshower)
# 124 layers of lead and scintillator.
# Thickness of lead and scintillator tiles is 1 mm.
# Only modules with 4 and 16 channel are used.
# Total number of modules: 1088. Channels: 4352
# 	4 channel modules (type 2 ): 1088 (4352 channels).
# CbmEcalDetailed class should be used for transport.
# By //Dr.Sys 2014.05.17
XPos=0		#Position of ECal center	[cm]
YPos=0		#Position of ECal center	[cm]
ZPos=750	#Position of ECal start		[cm]
NLayers=124	#Number of layers		
ModuleSize=12.0	#Module size			[cm]
Lead=.1		#Absorber thickness in layer	[cm]
Scin=.1		#Scintillator thickness in layer[cm]
Tyveec=.006	#Tyveec thickness in layer	[cm]
PSLead=0.00	#Lead thickness in preshower	[cm]
PSScin=0.00	#Scintillator thickness in layer[cm]
PSGap=0.0	#Gap between ecal and PS	[cm]
CellSize=3.0	#Cell size in ECAL simulation	[cm]
HoleRadius=.075 #Radius of hole in the calorimeter		[cm]
FiberRadius=.06 #Radius of fiber				[cm]
Steel=0.01	#Thickness of steel tapes			[cm]
TileEdging=0.01	#Thickness of white coating near tiles edges 	[cm]
CF[1]=1		#Is there clear fiber in cell type 1
CF[2]=1		#Is there clear fiber in cell type 2
CF[4]=1		#Is there clear fiber in cell type 4
NH[1]=12	#Number of holes for cell type 1
NH[2]=6		#Number of holes for cell type 2
NH[4]=4		#Number of holes for cell type 4
LightMap[1]=$VMCWORKDIR/parameters/ecal/12x12.lightmap # Light collection
LightMap[2]=$VMCWORKDIR/parameters/ecal/06x06.lightmap # efficiency maps for
LightMap[4]=$VMCWORKDIR/parameters/ecal/03x03.lightmap # different modules
# Be as compatible to CbmEcal in physics as possible 
usesimplegeo=1	#Use simplified geometry
ecalversion=1	#Use CbmEcalDetailed for coordinate calculation
# ECAL will be divided on cells with size CellSize x CellSize.
# CbmEcalCell responsible for ECAL cells assembled from such a cells.
# Such way allows to construct ECALs with different granularities at analysis stage from one Geant simulated input.
# Only restriction the size of all CbmEcalCells should be multiply to CellSize.
EcalZSize=30.0	#Z size of ECAL container	[cm]
ECut=100e-6	#Geant cuts CUTGAM CUTELE BCUTE BCUTM DCUTE [GeV]
HCut=300e-6	#Geant cuts CUTNEU CUTHAD CUTMUO DCUTM PPCUTM [GeV]
FastMC=0	#0 for full MC (with showers in ECAL), 1 for fast MC (only hits at sensitive plane before ECAL)
absorber=Lead   #Material of the absorber
structure
#Zero for no module here. ECALs with rectangular hole only can be constructed.
#Number means number of divisions of ECAL module in both directions: vertical and horizontal.
#So 2 states for module with 4 cells in it.
#         1         2         3     
#12345678901234567890123456789012345
2222222222222222222222222222222222 #1
2222222222222222222222222222222222 #2
2222222222222222222222222222222222 #3
2222222222222222222222222222222222 #4
2222222222222222222222222222222222 #5
2222222222222222222222222222222222 #6
2222222222222222222222222222222222 #7
2222222222222222222222222222222222 #8
2222222222222222222222222222222222 #9
2222222222222222222222222222222222 #10
2222222222222222222222222222222222 #11
2222222222222222222222222222222222 #12
2222222222222222222222222222222222 #13
2222222222222222222222222222222222 #14
2222222222222222222222222222222222 #15
2222222222222222222222222222222222 #16
0000000000000000000000000000000000 #17 1
0000000000000000000000000000000000 #18 2
0000000000000000000000000000000000 #19 3
0000000000000000000000000000000000 #20 4
0000000000000000000000000000000000 #21 5
0000000000000000000000000000000000 #22 6
0000000000000000000000000000000000 #23 7
0000000000000000000000000000000000 #24 8
0000000000000000000000000000000000 #25 9
0000000000000000000000000000000000 #26 0
0000000000000000000000000000000000 #27 10
0000000000000000000000000000000000 #28 11
0000000000000000000000000000000000 #29 12
0000000000000000000000000000000000 #30 13
0000000000000000000000000000000000 #31 14
0000000000000000000000000000000000 #32 15
0000000000000000000000000000000000 #33 16
0000000000000000000000000000000000 #34 17
0000000000000000000000000000000000 #35 18
2222222222222222222222222222222222 #39 14
2222222222222222222222222222222222 #40 13
2222222222222222222222222222222222 #41 12
2222222222222222222222222222222222 #42 11
2222222222222222222222222222222222 #43 10
2222222222222222222222222222222222 #44 9
2222222222222222222222222222222222 #45 8
2222222222222222222222222222222222 #46 7
2222222222222222222222222222222222 #47 6
2222222222222222222222222222222222 #48 5
2222222222222222222222222222222222 #49 4
2222222222222222222222222222222222 #50 3
2222222222222222222222222222222222 #51 2
2222222222222222222222222222222222 #52 1
2222222222222222222222222222222222 #43 2
2222222222222222222222222222222222 #54 1























