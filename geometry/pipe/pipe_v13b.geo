// pipe for SIS100/SIS300 electron setup
// by DE, 20131007
//
//  pipe_v13d.geo 
//  -----------------
//  this is a copy of pipe_standard.geo, 
//  with changes as follows:
//               - reduce diameter of segment 3, now starting at 4500 mm
//                 to 33 cm to avoid overlap with tof
//
//  description of shape PCON:
//  - material
//  - number of points
//  - azimuthal angle from xxx to yyy   [deg]
//  - point 1: z  inner-radius  outer-radius [mm]
//  - ...
//  - point n 
//  
//----------------------------------------------------------


pipe1
cave
PCON 
carbon
8
0. 360.
 -50.   25.  25.5 
  25.   25.  25.5 
  35.  130. 130.5
 239.5 130. 130.5
 240.  128. 130.5
 269.5  10.  12.5
 270.   10.  10.5 
1600.   32.  32.5

0. 0. 0. 
1.  0.  0.  0.  1.  0.  0.  0.  1
//

pipevac1
cave
PCON
vacuum
8
0. 360.  
 -50.  0.  25.   
  25.  0.  25.  
  35.  0. 130.
 239.5 0. 130.
 240.  0. 128.
 269.5 0.  10.
 270.  0.  10.
1600.  0.  32.

0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
//

pipe2
cave
PCON 
carbon
2
0. 360.
3500. 200. 200.5
6000. 200. 200.5
0. 0. 0.
1.  0.  0.  0.  1.  0.  0.  0.  1

pipevac2
pipe2
PCON
vacuum
2
0. 360.
3500. 0. 200.
6000. 0. 200.
0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
//

pipe3
cave
PCON 
carbon
2
0. 360.
6000. 149. 149.5
10000. 149. 149.5
0. 0. 0. 
1.  0.  0.  0.  1.  0.  0.  0.  1

pipevac3
pipe3
PCON
vacuum
2
0. 360.
6000. 0. 149.
10000. 0. 149.
0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
