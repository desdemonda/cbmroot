/******************************************************************************
 *  $Id: CbmDileptonTrackReal.cxx,v 1.2 2006/01/26 09:58:37 hoehne Exp $
 *
 *  Class : CbmDileptonTrackReal
 *  Descripton: This is the class for identified electrons for real data stream.
 *
 *  Author: Supriya Das
 *  E-mail: S.Das@gsi.de
 *****************************************************************************
 *  $Log : CbmDileptonTrackReal.cxx,v $
 *****************************************************************************/

#include "CbmDileptonTrackReal.h"

ClassImp(CbmDileptonTrackReal)
  
CbmDileptonTrackReal::CbmDileptonTrackReal() 
: CbmDileptonTrack(),
  indexGlobal(-99),
  fChiPrimary(0.),
  isRICH(kFALSE),
  isTRD(kFALSE),
  isTOF(kFALSE)
{
  fCharge     = -99;
}

CbmDileptonTrackReal::CbmDileptonTrackReal(Int_t i, Short_t charge, TVector3 mom, Double_t chi, Bool_t isRich, Bool_t isTrd, Bool_t isTof)
  : CbmDileptonTrack(),
    indexGlobal(i),
    fChiPrimary(chi),
    isRICH(isRich),
    isTRD(isTrd),
    isTOF(isTof)
{
    fCharge     = charge;
    fMomentum   = mom;
}

CbmDileptonTrackReal::~CbmDileptonTrackReal()
{

}
