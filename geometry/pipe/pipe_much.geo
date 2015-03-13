//  $Id: pipe_newvac.geo,v 1.2 2006/02/03 14:16:13 hoehne Exp $

//  History of cvs commits:
//  $Log: pipe_newvac.geo,v $
//  Revision 1.2  2006/02/03 14:16:13  hoehne
//  update because of slightly modified rich.geo
//
//  Revision 1.1  2005/09/20 09:27:49  friese
//  Modified beam pipe in sts
//
//  - Revision   Johann M. Heuser, 21.7.2005
//  - new geometry for the vacuum section: barrel-like shape
//  - diameter at z=1700: reduced from
//
// Revision for Much: continuous pipe
//----------------------------------------------------------
pipe1
cave
PCON 
carbon
9
0. 360.
 -50.  25.  25.5
  25.  25.  25.5
  35. 130. 130.5
 260. 130. 130.5
 270.  10.  10.5
1500.  30.  30.5
1700.  85.  85.5
6000. 300. 300.5
11400.  300. 300.5
0. 0. 0.
1.  0.  0.  0.  1.  0.  0.  0.  1
//
pipevac1
cave
PCON
vacuum
9
0. 360.  
 -50.  0.  25.   
  25.  0.  25.  
  35.  0. 130.
 260.  0. 130.   
 270.  0.  10.
1500.  0.  30.  
1700   0.  85.
6000. 0. 300.
11400. 0. 300.
0. 0. 0.
1.  0.  0.  0. 1.  0.  0. 0. 1
//
