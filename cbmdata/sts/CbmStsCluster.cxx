/**
 * \file CbmStsCluster.cxx
 * \author V.Friese <v.friese@gsi.de>
 * \since 28.08.06
 *
 * Updated 25/06/2008 by R. Karabowicz.
 * Updated 04/03/2014 by A. Lebedev <andrey.lebedev@gsi.de>
 **/
#include "CbmStsCluster.h"

CbmStsCluster::CbmStsCluster()
    : CbmCluster()
    , fMean(0.)
    , fMeanError(0.)
    , fCharge(0.)
    , fChannelMean(0.)
    , fChannelMeanSq(0.)
    , fTime(0.)
    , fIndex(-1)
    , fSectorNr(0)
{

}

CbmStsCluster::~CbmStsCluster()
{
}

ClassImp(CbmStsCluster)
