# Calorimeter located in 1.5 m from target after STS.
# Can be used as a first absorber layer of MUCH system.
# Full calorimeter length is 20 X_0.
# Consists of 70 layers of 1.5 mm scintillator and 1 mm tungsten
# (see absorber line) without preshower.
# 
# March 2010, //Dr.Sys
XPos=0		#Position of ECal center	[cm]
YPos=0		#Position of ECal center	[cm]
ZPos=150	#Position of ECal start		[cm]
NLayers=70 	#Number of layers		
ModuleSize=12.5	#Module size			[cm]
Lead=.1		#Absorber thickness in layer	[cm]
Scin=.15	#Scintillator thickness in layer[cm]
Tyveec=.006	#Tyveec thickness in layer	[cm]
PSLead=0.00	#Lead thickness in preshower	[cm]
PSScin=0.00	#Scintillator thickness in layer[cm]
PSGap=0.0	#Gap between ecal and PS	[cm]
CellSize=0.5	#Cell size in ECAL simulation	[cm]
# ECAL will be divided on cells with size CellSize x CellSize.
# CbmEcalCell responsible for ECAL cells assembled from such a cells.
# Such way allows to construct ECALs with different granularities at analysis stage from one Geant simulated input.
# Only restriction the size of all CbmEcalCells should be multiply to CellSize.
EcalZSize=20.0	#Z size of ECAL container	[cm]
ECut=100e-6	#Geant cuts CUTGAM CUTELE BCUTE BCUTM DCUTE [GeV]
HCut=300e-6	#Geant cuts CUTNEU CUTHAD CUTMUO DCUTM PPCUTM [GeV]
FastMC=0	#0 for full MC (with showers in ECAL), 1 for fast MC (only hits at sensitive plane before ECAL)
absorber=ECALWolfram   #Material of the absorber
structure
#Zero for no module here. ECALs with rectangular hole only can be constructed.
#Number means number of divisions of ECAL module in both directions: vertical and horizontal.
#So 2 states for module with 4 cells in it.
#        1         2         3         
#234567890123456789012345678901234567890
55555555555555555555 # 1
55555555555555555555 # 2
55555555555555555555 # 3
55555555555555555555 # 4
55555555555555555555 # 5
55555555555555555555 # 6
55555555555555555555 # 7
55555555500555555555 # 8
55555555500555555555 # 9
55555555555555555555 #10
55555555555555555555 #11
55555555555555555555 #12
55555555555555555555 #13
55555555555555555555 #14
55555555555555555555 #15
55555555555555555555 #16

