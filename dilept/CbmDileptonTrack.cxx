/******************************************************************************
 *  $Id: CbmDileptonTrack.cxx,v 1.2 2006/01/26 09:58:37 hoehne Exp $
 *
 *  Class : CbmDileptonTrack
 *  Descripton: This is the base class for identified electrons for 
 *              dilepton pair analysis. Two classes for real data and/or
 *              simulated data will be derived from this.
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 *****************************************************************************
 *  $Log : CbmDileptonTrack.cxx,v $
 *****************************************************************************/
#include "CbmDileptonTrack.h"

ClassImp(CbmDileptonTrack)
  
CbmDileptonTrack::CbmDileptonTrack() 
: TObject(),
  fCharge(0),
  fMomentum()
{
}

CbmDileptonTrack::~CbmDileptonTrack()
{

}
