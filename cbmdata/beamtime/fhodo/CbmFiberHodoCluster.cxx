/**
 * \file CbmFiberHodoCluster.cxx
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \brief Data Container for TRD clusters.
 */
#include "CbmFiberHodoCluster.h"

CbmFiberHodoCluster::CbmFiberHodoCluster()
  : CbmCluster()
  , fTime(0.)
  , fMean(0.)
  , fMeanError(0.)
  , fCharge(0.)
{
}

 CbmFiberHodoCluster::~CbmFiberHodoCluster(){
}

ClassImp(CbmFiberHodoCluster)
