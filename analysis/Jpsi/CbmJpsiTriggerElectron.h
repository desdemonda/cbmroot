/******************************************************************************
 *  $Id: CbmJpsiTriggerElectron.h,v 1.2 2006/01/26 09:58:37 hoehne Exp $
 *
 *  Class : CbmJpsiTriggerElectron
 *  Descripton: This is the class for identified electrons for real data stream.
 *
 *  Author: Alla Maevskaya
 *  E-mail: alla@inr.ru
 *****************************************************************************
 *  $Log : CbmJpsiEl.h,v $
 *****************************************************************************/

#ifndef CBM_JPSI_TRIGGER_ELECTRON_H
#define CBM_JPSI_TRIGGER_ELECTRON_H


#include "TObject.h"
#include "TVector3.h"

class CbmJpsiTriggerElectron : public TObject{


public:

    /** Constructor **/
    CbmJpsiTriggerElectron();
    CbmJpsiTriggerElectron( TVector3 momentum, Int_t pdg, Double_t ann, Double_t like, Double_t wkn ); //!constructor

    /** Destructor **/
    virtual ~CbmJpsiTriggerElectron();

    /** Accessors **/

    TVector3 GetMomentum() {return fMomentum;}
    void SetMomentum(TVector3 mom) {fMomentum = mom;}

    Int_t GetPdg() {return fPdg;}
    void SetPdg(Int_t part) {fPdg = part;}

    Double_t GetAnn() {return fAnn;}
    void SetAnn(Double_t part) {fAnn = part;}
   Double_t GetLike() {return fLike;}
    void SetLike(Double_t part) {fLike = part;}
   Double_t  GetWkn() {return fWkn;}
    void SetWkn(Double_t part) {fWkn = part;}

private:

  TVector3 fMomentum;
  Int_t fPdg;
  Double_t fAnn;
  Double_t fLike;
  Double_t fWkn;


    ClassDef(CbmJpsiTriggerElectron,3);

};

#endif
