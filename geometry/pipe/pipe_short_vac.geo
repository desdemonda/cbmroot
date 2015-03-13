//  $Id: pipe_standard.geo,v 1.1 2006/06/09 14:04:21 hoehne Exp $
//
//  pipe_standard.geo (Claudia Hoehne)
//  -----------------
//  this is a copy of pipe_newvac.ego, Revision 1.3 (Johann heuser)
//  with changes as follows:
//  - remove strong increase of beam pipe towards RICH to a continous increase
//    towards end of RICH
//  - keep constant radius from end of RICH detector
//  -> at z=1600mm (start of RICH): inner radius 32mm
//  -> at z=4500mm (end of RICH): inner radius 200mm
//  -> keep this radius till the ECAL
//
//  History of cvs commits:
//  $Log: pipe_standard.geo,v $
//  Revision 1.1  2006/06/09 14:04:21  hoehne
//  new standard version for beampipe
//  copy of pipe_newvac.geo v.1.3 with changes as follows:
//  - continuous increase of beam pipe radius from end of STS to end of RICH
//  - keep r=2000mm from end of RICH on
//  - pipe for RICH of length 2.9m (1600mm < z < 4500mm) cut off
//
//  Revision 1.3  2006/05/24 15:46:45  heuser
//  // Revision 1.3  Johann M. Heuser, 22.5.2006
//  // new geometry for the vacuum section:
//  //    * barrel-like shape with 13 cm radius between z=3.5cm and z=24cm.
//  //    * inclined downstream window "end plate" between z=24 and z=27 cm,
//  //      thickness 500 micron.
//  //  - diameter at z=1700: 8.5 cm to match the beam pipe in the RICH.
//
//
//  Revision 1.2  2005/07/12 12:54:28  kharlov
//  Cut off a chunk of pipe inside RICH
//
//  - Revision 1.3  Johann M. Heuser, 22.5.2006
//  - new geometry for the vacuum section: 
//    * barrel-like shape with 13 cm radius between z=3.5cm and z=24cm.
//    * inclined downstream window "end plate" between z=24 and z=27 cm, 
//      thickness 500 micron.
//  - diameter at z=1700: 8.5 cm to match the beam pipe in the RICH.
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
6
0. 360.
 -50.  25.  25.5 
  25.  25.  25.5 
  35. 130. 130.5
 110. 130. 130.5
 120.  5.  5.5
1600.  22. 22.5

0. 0. 0. 
1.  0.  0.  0.  1.  0.  0.  0.  1
//
pipevac1
cave
PCON
vacuum
6
0. 360.  
 -50.  0.  25.   
  25.  0.  25.  
  35.  0. 130.
 110.  0. 130.   
 120.  0.  5.
1600.  0.  22.

0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
//

pipe2
cave
PCON 
carbon
2
0. 360.
3500. 0. 200.5
10999. 0. 200.5
0. 0. 0.
1.  0.  0.  0.  1.  0.  0.  0.  1

pipevac2
pipe2
PCON
vacuum
2
0. 360.
3500. 0. 200.
10999. 0. 200.
0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
//

pipe3
cave
PCON 
carbon
2
0. 360.
11001. 0. 300.5 
11400. 0. 300.5
0. 0. 0. 
1.  0.  0.  0.  1.  0.  0.  0.  1

pipevac3
pipe3
PCON
vacuum
2
0. 360.
11001. 0. 300.
11400. 0. 300.
0. 0. 0. 
1.  0.  0.  0. 1.  0.  0. 0. 1
