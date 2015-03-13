// Realistic shape of the Be beampipe with 2.3 degree opening half angle.
// Pipe wall is 0.5 mm, window is convex sphere with 0.5 mm thickness
// Made by Andrey Chernogorov.
//  description of shape PCON:
//  - material
//  - number of points
//  - azimuthal angle from xxx to yyy   [deg]
//  - point 1: z  inner-radius  outer-radius [mm]
//  - ...
//  - point n 
//----------------------------------------------------------

pipe1
cave
PCON 
beryllium
6
0. 360.
 -50.    25.   25.5 
  25.    25.   25.5 
  35.   130.   130.5
 187.76 130.   130.5
 187.76 110.   130.5
 188.26 110.11 130.5

0. 0. 0. 
1.  0.  0.  0.  1.  0.  0.  0.  1
//

pipevac1
cave
PCON
vacuum
7
0. 360.  
 -50.     0.    25.   
  25.     0.    25.  
  35.     0.   130.
 187.75   0.   130.
 187.75   0.   110.
 197.49   0.    50.
 200.     0.     0.
0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
//

pipe2
cave
SPHE
beryllium
 500.    500.5
    .922  12.708
   0.    360.

0. 0. -300.
1.  0.  0.  0. 1.  0.  0. 0. 1
//
      
pipe3
cave
PCON 
beryllium
9
0. 360.
 199.94  7.53  8.04
 200.42  7.55  8.05
 230.    8.74  9.24
 230.    8.74  9.64
 240.    9.14 10.04
 240.    9.14  9.64
1598.   62.96 63.46 
1598.   62.96 89.26 
1600.   63.76 89.26

0. 0. 0. 
1.  0.  0.  0.  1.  0.  0.  0.  1
//

pipevac3
cave
PCON
vacuum
6
0. 360.
 199.94 0.  7.53
 200.42 0.  7.55
 230.   0.  8.64
 240.   0.  9.04
1598.   0. 62.86
1600.   0. 63.66

0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
//