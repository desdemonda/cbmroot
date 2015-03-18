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
300.000000 -300.000000 -247.500000
300.000000 300.000000 -247.500000
-300.000000 300.000000 -247.500000
-300.000000 -300.000000 -247.500000
300.000000 -300.000000 247.500000
300.000000 300.000000 247.500000
-300.000000 300.000000 247.500000
-300.000000 -300.000000 247.500000
600.000000 0.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1radiator
trd1mod1#1001
BOX
pefoam20
285.000000 -285.000000 -180.000000
285.000000 285.000000 -180.000000
-285.000000 285.000000 -180.000000
-285.000000 -285.000000 -180.000000
285.000000 -285.000000 180.000000
285.000000 285.000000 180.000000
-285.000000 285.000000 180.000000
-285.000000 -285.000000 180.000000
0.000000 0.000000 -67.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1gas
trd1mod1#1001
BOX
TRDgas
285.000000 -285.000000 -6.000000
285.000000 285.000000 -6.000000
-285.000000 285.000000 -6.000000
-285.000000 -285.000000 -6.000000
285.000000 -285.000000 6.000000
285.000000 285.000000 6.000000
-285.000000 285.000000 6.000000
-285.000000 -285.000000 6.000000
0.000000 0.000000 118.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1padplane
trd1mod1#1001
BOX
goldcoatedcopper
285.000000 -285.000000 -0.015000
285.000000 285.000000 -0.015000
-285.000000 285.000000 -0.015000
-285.000000 -285.000000 -0.015000
285.000000 -285.000000 0.015000
285.000000 285.000000 0.015000
-285.000000 285.000000 0.015000
-285.000000 -285.000000 0.015000
0.000000 0.000000 124.514999
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1mylar
trd1mod1#1001
BOX
mylar
285.000000 -285.000000 -0.750000
285.000000 285.000000 -0.750000
-285.000000 285.000000 -0.750000
-285.000000 -285.000000 -0.750000
285.000000 -285.000000 0.750000
285.000000 285.000000 0.750000
-285.000000 285.000000 0.750000
-285.000000 -285.000000 0.750000
0.000000 0.000000 125.279999
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1electronics
trd1mod1#1001
BOX
goldcoatedcopper
285.000000 -285.000000 -0.035000
285.000000 285.000000 -0.035000
-285.000000 285.000000 -0.035000
-285.000000 -285.000000 -0.035000
285.000000 -285.000000 0.035000
285.000000 285.000000 0.035000
-285.000000 285.000000 0.035000
-285.000000 -285.000000 0.035000
0.000000 0.000000 126.065002
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1frame1#1
trd1mod1#1001
BOX
G10
300.000000 -7.500000 -186.800003
300.000000 7.500000 -186.800003
-300.000000 7.500000 -186.800003
-300.000000 -7.500000 -186.800003
300.000000 -7.500000 186.800003
300.000000 7.500000 186.800003
-300.000000 7.500000 186.800003
-300.000000 -7.500000 186.800003
0.000000 292.500000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1frame1#2
trd1mod1#1001
0.000000 -292.500000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1frame2#1
trd1mod1#1001
BOX
G10
7.500000 -285.000000 -186.800003
7.500000 285.000000 -186.800003
-7.500000 285.000000 -186.800003
-7.500000 -285.000000 -186.800003
7.500000 -285.000000 186.800003
7.500000 285.000000 186.800003
-7.500000 285.000000 186.800003
-7.500000 -285.000000 186.800003
292.500000 0.000000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1frame2#2
trd1mod1#1001
-292.500000 0.000000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1002
trd1
-600.000000 -0.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1003
trd1
0.000000 600.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1004
trd1
-0.000000 -600.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1005
trd1
600.000000 600.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1006
trd1
600.000000 -600.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1007
trd1
-600.000000 -600.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod1#1008
trd1
-600.000000 600.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1001
trd1
BOX
air
300.000000 -300.000000 -247.500000
300.000000 300.000000 -247.500000
-300.000000 300.000000 -247.500000
-300.000000 -300.000000 -247.500000
300.000000 -300.000000 247.500000
300.000000 300.000000 247.500000
-300.000000 300.000000 247.500000
-300.000000 -300.000000 247.500000
1200.000000 0.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2radiator
trd1mod2#1001
BOX
pefoam20
285.000000 -285.000000 -180.000000
285.000000 285.000000 -180.000000
-285.000000 285.000000 -180.000000
-285.000000 -285.000000 -180.000000
285.000000 -285.000000 180.000000
285.000000 285.000000 180.000000
-285.000000 285.000000 180.000000
-285.000000 -285.000000 180.000000
0.000000 0.000000 -67.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2gas
trd1mod2#1001
BOX
TRDgas
285.000000 -285.000000 -6.000000
285.000000 285.000000 -6.000000
-285.000000 285.000000 -6.000000
-285.000000 -285.000000 -6.000000
285.000000 -285.000000 6.000000
285.000000 285.000000 6.000000
-285.000000 285.000000 6.000000
-285.000000 -285.000000 6.000000
0.000000 0.000000 118.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2padplane
trd1mod2#1001
BOX
goldcoatedcopper
285.000000 -285.000000 -0.015000
285.000000 285.000000 -0.015000
-285.000000 285.000000 -0.015000
-285.000000 -285.000000 -0.015000
285.000000 -285.000000 0.015000
285.000000 285.000000 0.015000
-285.000000 285.000000 0.015000
-285.000000 -285.000000 0.015000
0.000000 0.000000 124.514999
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2mylar
trd1mod2#1001
BOX
mylar
285.000000 -285.000000 -0.750000
285.000000 285.000000 -0.750000
-285.000000 285.000000 -0.750000
-285.000000 -285.000000 -0.750000
285.000000 -285.000000 0.750000
285.000000 285.000000 0.750000
-285.000000 285.000000 0.750000
-285.000000 -285.000000 0.750000
0.000000 0.000000 125.279999
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2electronics
trd1mod2#1001
BOX
goldcoatedcopper
285.000000 -285.000000 -0.035000
285.000000 285.000000 -0.035000
-285.000000 285.000000 -0.035000
-285.000000 -285.000000 -0.035000
285.000000 -285.000000 0.035000
285.000000 285.000000 0.035000
-285.000000 285.000000 0.035000
-285.000000 -285.000000 0.035000
0.000000 0.000000 126.065002
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2frame1#1
trd1mod2#1001
BOX
G10
300.000000 -7.500000 -186.800003
300.000000 7.500000 -186.800003
-300.000000 7.500000 -186.800003
-300.000000 -7.500000 -186.800003
300.000000 -7.500000 186.800003
300.000000 7.500000 186.800003
-300.000000 7.500000 186.800003
-300.000000 -7.500000 186.800003
0.000000 292.500000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2frame1#2
trd1mod2#1001
0.000000 -292.500000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2frame2#1
trd1mod2#1001
BOX
G10
7.500000 -285.000000 -186.800003
7.500000 285.000000 -186.800003
-7.500000 285.000000 -186.800003
-7.500000 -285.000000 -186.800003
7.500000 -285.000000 186.800003
7.500000 285.000000 186.800003
-7.500000 285.000000 186.800003
-7.500000 -285.000000 186.800003
292.500000 0.000000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2frame2#2
trd1mod2#1001
-292.500000 0.000000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1002
trd1
-1200.000000 -0.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1003
trd1
0.000000 1200.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1004
trd1
-0.000000 -1200.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1005
trd1
1200.000000 600.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1006
trd1
1200.000000 -600.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1007
trd1
-1200.000000 -600.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1008
trd1
-1200.000000 600.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1009
trd1
600.000000 1200.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1010
trd1
-600.000000 1200.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1011
trd1
-600.000000 -1200.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1012
trd1
600.000000 -1200.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1013
trd1
1200.000000 1200.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1014
trd1
-1200.000000 1200.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1015
trd1
-1200.000000 -1200.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod2#1016
trd1
1200.000000 -1200.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1001
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
3000.000000 0.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3radiator
trd1mod3#1001
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
trd1mod3gas
trd1mod3#1001
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
trd1mod3padplane
trd1mod3#1001
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
trd1mod3mylar
trd1mod3#1001
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
trd1mod3electronics
trd1mod3#1001
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
trd1mod3frame1#1
trd1mod3#1001
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
trd1mod3frame1#2
trd1mod3#1001
0.000000 -490.000000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3frame2#1
trd1mod3#1001
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
trd1mod3frame2#2
trd1mod3#1001
-490.000000 0.000000 -60.699997
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1002
trd1
-3000.000000 -0.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1003
trd1
3000.000000 1000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1004
trd1
3000.000000 -1000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1005
trd1
-3000.000000 -1000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1006
trd1
-3000.000000 1000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1007
trd1
0.000000 2000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1008
trd1
-0.000000 -2000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1009
trd1
1000.000000 2000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1010
trd1
-1000.000000 2000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1011
trd1
-1000.000000 -2000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1012
trd1
1000.000000 -2000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1013
trd1
3000.000000 2000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1014
trd1
3000.000000 -2000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1015
trd1
-3000.000000 -2000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1016
trd1
-3000.000000 2000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1017
trd1
4000.000000 2000.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1018
trd1
4000.000000 1000.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1019
trd1
4000.000000 0.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1020
trd1
4000.000000 -1000.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1021
trd1
4000.000000 -2000.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1022
trd1
-4000.000000 -2000.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1023
trd1
-4000.000000 -1000.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1024
trd1
-4000.000000 0.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1025
trd1
-4000.000000 1000.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1026
trd1
-4000.000000 2000.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1027
trd1
5000.000000 2000.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1028
trd1
5000.000000 1000.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1029
trd1
5000.000000 0.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1030
trd1
5000.000000 -1000.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1031
trd1
5000.000000 -2000.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1032
trd1
-5000.000000 -2000.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1033
trd1
-5000.000000 -1000.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1034
trd1
-5000.000000 0.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1035
trd1
-5000.000000 1000.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1036
trd1
-5000.000000 2000.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1037
trd1
5000.000000 3000.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1038
trd1
5000.000000 -3000.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1039
trd1
4000.000000 3000.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1040
trd1
4000.000000 -3000.000000 -752.500000
1.000000 0.000000 0.000000 0.000000 1.000000 0.000000 -0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1041
trd1
3000.000000 3000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1042
trd1
3000.000000 -3000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1043
trd1
1000.000000 3000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1044
trd1
1000.000000 -3000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1045
trd1
0.000000 3000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1046
trd1
-5000.000000 -3000.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1047
trd1
-5000.000000 3000.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1048
trd1
-4000.000000 -3000.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1049
trd1
-4000.000000 3000.000000 -752.500000
1.000000 0.000000 -0.000000 0.000000 1.000000 0.000000 0.000000 0.000000 1.000000
//*********************************
//*********************************
trd1mod3#1050
trd1
-3000.000000 -3000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1051
trd1
-3000.000000 3000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1052
trd1
-1000.000000 -3000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1053
trd1
-1000.000000 3000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************
//*********************************
trd1mod3#1054
trd1
-0.000000 -3000.000000 -752.500000
1.  0.  0.  0.  1.  0.  0.  0.  1.
//*********************************