// -------------------------------------------------------------------------
// -----                     CbmEcalHit header file                    -----
// -----               Created 10/03/05  by Yu.Kharlov                 -----
// -------------------------------------------------------------------------


/**  CbmEcalHit.h
 *@author Yu.Kharlov
 **
 ** Hits in ECAL which is an total amplitude per cell
 **/


#ifndef CBMECALHIT_H
#define CBMECALHIT_H

#include "TObject.h"

class CbmEcalHit : public TObject
{
public:    
 

  /** Default constructor **/
  CbmEcalHit();

  CbmEcalHit(Int_t id, Float_t energy, Float_t psenergy, Int_t trackid, Double32_t time)
    : TObject(), fEnergy(energy), fPSEnergy(psenergy), fTime(time), fTrackId(trackid), fDetectorId(id)
  {};

  /** Destructor **/
  virtual ~CbmEcalHit();    
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt ="") const;
  
  /** Modifiers **/
  virtual void SetEnergy(Double32_t energy) {fEnergy=energy;}
  virtual void SetPSEnergy(Double32_t psenergy) {fPSEnergy=psenergy;}
  virtual void SetTime(Double32_t time) {fTime=time;}
  virtual void SetTrackId(Int_t id) {fTrackId=id;}
  virtual void SetDetectorId(Int_t id) {fDetectorId=id;}
  
  /** Accessors **/
  virtual Double_t GetEnergy() const {return fEnergy;}
  virtual Double_t GetPSEnergy() const {return fPSEnergy;}
  virtual Double_t GetTime() const {return fTime;}
  virtual Int_t GetTrackId() const {return fTrackId;}
  virtual Int_t GetDetectorId() const {return fDetectorId;}
  
protected:

  Double32_t fEnergy;    // hit amplitude
  Double32_t fPSEnergy;  // hit amplitude in PS
  Double32_t fTime;
  Int_t fTrackId;
  Int_t fDetectorId;

  ClassDef(CbmEcalHit,1)
};

#endif //CBMRICHHIT_H
