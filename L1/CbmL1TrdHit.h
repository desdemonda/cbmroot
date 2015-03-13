/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  TRD hit description for L1
 *
 *====================================================================
 */

#ifndef _CbmL1TrdHit_h_
#define _CbmL1TrdHit_h_

#include "CbmKFHit.h"

struct CbmL1TrdHit //:public CbmL1KFUMeasurement
{ 

  // hit information  
  
  int iStation; // TRD station 

  // Monte Carlo information

  int MC_Point;

 static bool compareStation( const CbmL1TrdHit &a, const CbmL1TrdHit &b )
  {
    return ( a.iStation < b.iStation );
  }
};

#endif
