/******************************************************************************
 *  $Id: CbmJpsiTriggerElectron.cxx,v 1.2 2006/01/26 09:58:37 hoehne Exp $
 *
 *  Class : CbmJpsiTriggerElectron
 *  Descripton: This is the base class for identified electrons for 
 *              dilepton pair analysis. Two classes for real data and/or
 *              simulated data will be derived from this.
 *
 *  Author: Alla maevskaya
 *  E-mail: alla@inr.ru
 *****************************************************************************
 *  $Log : CbmJpsiTriggerElectron.cxx,v $
 *****************************************************************************/
#include "CbmJpsiTriggerElectron.h"
#include "TVector3.h"

ClassImp(CbmJpsiTriggerElectron)
  
CbmJpsiTriggerElectron::CbmJpsiTriggerElectron():TObject()
{
  fPdg=-1;
  fLike=-1;
  fAnn=-2;
  fWkn=-1;
}
CbmJpsiTriggerElectron::CbmJpsiTriggerElectron( TVector3 momentum, Int_t pdg,Double_t ann, Double_t like, Double_t wkn):TObject()
{
  fMomentum = momentum;
  fPdg = pdg;
  fLike=like;
  fAnn=ann;
  fWkn=wkn;
}

CbmJpsiTriggerElectron::~CbmJpsiTriggerElectron()
{

}
