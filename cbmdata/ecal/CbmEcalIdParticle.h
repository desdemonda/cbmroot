// -------------------------------------------------------------------------
// -----                 CbmEcalIdParticle header file                 -----
// -----                 Created 2009.03.13 by //Dr.Sys                -----
// -------------------------------------------------------------------------

/**  CbmEcalIdParticle.h
 *@author //Dr.Sys
 ** Identified particle in ECAL, characterized by reconstructed 4-momentum
 **/

#ifndef CBMECALIDPARTICLE_H
#define CBMECALIDPARTICLE_H

#include "TObject.h"
#include "TLorentzVector.h"


class CbmEcalCluster;
class CbmEcalCell;

class CbmEcalIdParticle : public TObject
{
public:

  /** Default constructor **/
  CbmEcalIdParticle();

  /** Constructor with arguments
   *@param px,py,pz,E  4 components of LorentzVector
   **/
  CbmEcalIdParticle(CbmEcalCell* cell,
                    Double_t x, Double_t y, Double_t z,
		    Double_t e, Double_t pse, Double_t shape,
                    Int_t tracknum, Double_t prob, Int_t mctrack=-1111); 

  CbmEcalIdParticle(Int_t cell,
                    Double_t x, Double_t y, Double_t z,
		    Double_t e, Double_t pse, Double_t shape,
                    Int_t tracknum, Double_t prob, Int_t mctrack=-1111); 
  /** Copy constructor **/
  CbmEcalIdParticle(const CbmEcalIdParticle& obj);

  /** Destructor **/
  virtual ~CbmEcalIdParticle();

  /** Accessors **/
  Double32_t E() const {return fE;}
  Double32_t PSE() const {return fPSE;}
  Double32_t Shape() const {return fShape;}
  Double32_t X() const {return fX;}
  Double32_t Y() const {return fY;}
  Double32_t Z() const {return fZ;}
  Double32_t EProb() const {return fEProb;}// Probability  to be an electron
  Int_t CellNum() const {return fCellNum;}
  virtual void Print(const Option_t* opt = "") const;

  Int_t Track() const {return fTrack;}
  Int_t MCTrack() const {return fMCTrack;}
  void SetMCTrack(Int_t tr) {fMCTrack=tr;} 
protected:
  /** Reconstructed position in calorimeter **/
  Double32_t fX;
  Double32_t fY;		
  Double32_t fZ;

  Double32_t fE;	// E in cluster
  Double32_t fPSE;	// Energy in PS
  Double32_t fShape;	// 3x3 Energy/2x2 Energy

  /** A seed for the photon **/
  Int_t fCellNum;
  /** A mother track number **/
  Int_t fTrack;

  Double32_t fEProb;	// Probability of track to be an electron

  /** A mother MC track **/
  Int_t fMCTrack;

  ClassDef(CbmEcalIdParticle, 2)
};

#endif
