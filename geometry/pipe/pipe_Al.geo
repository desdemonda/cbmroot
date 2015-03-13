// Realistic shape of the Al beampipe with 2.3 degree opening half angle.
// Pipe wall is 0.6 mm, window is concave sphere with 0.7 mm thickness
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
aluminium
6
0. 360.
 -50.    24.8   25.5 
  25.    24.8   25.5 
  35.   129.8  130.5
 211.55 129.8  130.5
 211.55 110.15 130.5
 212.25 110.   130.5

0. 0. 0. 
1.  0.  0.  0.  1.  0.  0.  0.  1
//

pipevac1
cave
PCON
vacuum
6
0. 360.  
 -50.     0.    24.8   
  25.     0.    24.8  
  35.     0.   129.8
 199.36   0.   129.8
 199.37  54.32 129.8
 211.55 110.15 129.8
0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
//

pipe2
cave
SPHE
aluminium
 500.    500.7
 167.3   179.08
   0.    360.

0. 0. 700.
1.  0.  0.  0. 1.  0.  0. 0. 1
//
      
pipe3
cave
PCON 
aluminium
9
0. 360.
 199.37  7.41  8.04
 200.07  7.43  8.02
 230.    8.64  9.24
 230.    8.64  9.64
 240.    9.04 10.04
 240.    9.04  9.64
1598.   62.86 63.46 
1598.   62.86 89.26 
1600.   63.66 89.26

0. 0. 0. 
1.  0.  0.  0.  1.  0.  0.  0.  1
//

pipevac3
cave
PCON
vacuum
6
0. 360.
 199.37 0.  7.41
 200.07 0.  7.43
 230.   0.  8.64
 240.   0.  9.04
1598.   0. 62.86
1600.   0. 63.66

0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
//