//*********************************************************
// TRD Jul10 geometry by DE
//*********************************************************
// stations located at 4500 / 6750 / 9000 mm in z
// with frontside of radiators of the 1st layer  
// v01 - 12 mm gas thickness		            
// v02 -  6 mm gas thickness (standard)          
// v03 - tilted, 6 mm gas thickness              
// v04 - tilted, aligned, 6 mm gas thickness     
// v05 - tilted layer pairs, 6 mm gas thickness  
//*********************************************************
//*********************************************************
// This is the geometry file of the following configuration
// Number of stations: 3
// Number of layers per station: 4
//*********************************************************
// Inner Radius of station 1: 240.000000
// Outer Radius of station 1: 3600.000000
// z-position at the middle of the station 1: 5500.000000
//*********************************************************
// Inner Radius of station 2: 240.000000
// Outer Radius of station 2: 4600.000000
// z-position at the middle of the station 2: 7750.000000
//*********************************************************
// Inner Radius of station 3: 490.000000
// Outer Radius of station 3: 5600.000000
// z-position at the middle of the station 3: 10000.000000
//*********************************************************
//*********************************
trd1
cave
PGON
air
2
45 360 4
-1000.000000 240.000000 4600.000000 
1000.000000 240.000000 4600.000000 
0. 0. 7750.0
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1001
trd1
BOX
air
500.000000 -500.000000 -247.500000
500.000000 500.000000 -247.500000
-500.000000 500.000000 -247.500000
-500.000000 -500.000000 -247.500000
500.000000 -500.000000 247.500000
500.000000 500.000000 247.500000
-500.000000 500.000000 247.500000
-500.000000 -500.000000 247.500000
-2000.000000 0.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1radiator
trd1mod1#1001
BOX
pefoam20
480.000000 -480.000000 -180.000000
480.000000 480.000000 -180.000000
-480.000000 480.000000 -180.000000
-480.000000 -480.000000 -180.000000
480.000000 -480.000000 180.000000
480.000000 480.000000 180.000000
-480.000000 480.000000 180.000000
-480.000000 -480.000000 180.000000
0.000000 0.000000 -67.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1gas
trd1mod1#1001
BOX
TRDgas
480.000000 -480.000000 -6.000000
480.000000 480.000000 -6.000000
-480.000000 480.000000 -6.000000
-480.000000 -480.000000 -6.000000
480.000000 -480.000000 6.000000
480.000000 480.000000 6.000000
-480.000000 480.000000 6.000000
-480.000000 -480.000000 6.000000
0.000000 0.000000 118.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1padplane
trd1mod1#1001
BOX
goldcoatedcopper
480.000000 -480.000000 -0.015000
480.000000 480.000000 -0.015000
-480.000000 480.000000 -0.015000
-480.000000 -480.000000 -0.015000
480.000000 -480.000000 0.015000
480.000000 480.000000 0.015000
-480.000000 480.000000 0.015000
-480.000000 -480.000000 0.015000
0.000000 0.000000 124.514999
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1mylar
trd1mod1#1001
BOX
mylar
480.000000 -480.000000 -0.750000
480.000000 480.000000 -0.750000
-480.000000 480.000000 -0.750000
-480.000000 -480.000000 -0.750000
480.000000 -480.000000 0.750000
480.000000 480.000000 0.750000
-480.000000 480.000000 0.750000
-480.000000 -480.000000 0.750000
0.000000 0.000000 125.279999
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1electronics
trd1mod1#1001
BOX
goldcoatedcopper
480.000000 -480.000000 -0.035000
480.000000 480.000000 -0.035000
-480.000000 480.000000 -0.035000
-480.000000 -480.000000 -0.035000
480.000000 -480.000000 0.035000
480.000000 480.000000 0.035000
-480.000000 480.000000 0.035000
-480.000000 -480.000000 0.035000
0.000000 0.000000 126.065002
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1frame1#1
trd1mod1#1001
BOX
G10
500.000000 -10.000000 -186.800003
500.000000 10.000000 -186.800003
-500.000000 10.000000 -186.800003
-500.000000 -10.000000 -186.800003
500.000000 -10.000000 186.800003
500.000000 10.000000 186.800003
-500.000000 10.000000 186.800003
-500.000000 -10.000000 186.800003
0.000000 490.000000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1frame1#2
trd1mod1#1001
0.000000 -490.000000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1frame2#1
trd1mod1#1001
BOX
G10
10.000000 -480.000000 -186.800003
10.000000 480.000000 -186.800003
-10.000000 480.000000 -186.800003
-10.000000 -480.000000 -186.800003
10.000000 -480.000000 186.800003
10.000000 480.000000 186.800003
-10.000000 480.000000 186.800003
-10.000000 -480.000000 186.800003
490.000000 0.000000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1frame2#2
trd1mod1#1001
-490.000000 0.000000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1003
trd1
-2000.000000 1000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1004
trd1
-2000.000000 -1000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1013
trd1
-2000.000000 2000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1014
trd1
-2000.000000 -2000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1041
trd1
-2000.000000 3000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1042
trd1
-2000.000000 -3000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
