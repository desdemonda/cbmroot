/** A calorimeter reconstructed particle. Momenta and calorimeter impact point
 ** inside. Chi2 for quality control. TOF information also provided in case
 ** of time measurements with calorimeter.
 ** **/
#ifndef CBMECALRECPARTICLE_H
#define CBMECALRECPARTICLE_H

#include "TObject.h"
#include "TLorentzVector.h"


class CbmEcalCluster;
class CbmEcalCell;

class CbmEcalRecParticle : public TObject
{
friend class CbmEcalReco;
friend class CbmEcalReco2;
friend class CbmEcalRecoCorr;
public:

  /** Default constructor **/
  CbmEcalRecParticle();

  /** Constructor with arguments
   *@param px,py,pz,E  4 components of LorentzVector
   **/
  CbmEcalRecParticle(Double_t px, Double_t py, 
		     Double_t pz, Double_t e,
		     Double_t x=-1111, Double_t y=-1111, Double_t z=-1111,
		     Int_t id=0, Double_t chi2=-1111, CbmEcalCluster* cl=NULL,
		     CbmEcalCell* cell=NULL, Int_t type=0);

  CbmEcalRecParticle(Int_t clnum, Int_t cellnum, Double_t px, Double_t py, 
		     Double_t pz, Double_t e,
		     Double_t x=-1111, Double_t y=-1111, Double_t z=-1111,
		     Int_t id=0, Double_t chi2=-1111, Int_t type=0);
  /** Copy constructor **/
  CbmEcalRecParticle(const CbmEcalRecParticle& obj);

  /** Destructor **/
  virtual ~CbmEcalRecParticle();

  /** Accessors **/
  TLorentzVector GetMomentum() const {return TLorentzVector(fPx,fPy,fPz,fE);}
  Double32_t Px() const {return fPx;}
  Double32_t Py() const {return fPy;}
  Double32_t Pz() const {return fPz;}
  Double32_t E() const {return fE;}
  Double32_t X() const {return fX;}
  Double32_t Y() const {return fY;}
  Double32_t Z() const {return fZ;}
  Double_t Chi2() const {return fChi2;}
  void SetChi2(Double_t chi2) {fChi2=chi2;}
  Int_t ClusterNum() const {return fClusterNum;}
  Int_t CellNum() const {return fCellNum;}
  Int_t PdgId() const {return fId;}
  Int_t CellType() const {return fType;}
  Double_t TOF() const {return fTime;}
  void SetTOF(Double32_t time) {fTime=time;}
  /** Output to screen **/
  virtual void Print(const Option_t* opt = "") const;

  Int_t MCTrack() const {return fMCTrack;}
  void SetMCTrack(Int_t num) {fMCTrack=num;}
protected:

  Double32_t fPx;  // px component of momentum 4-vector
  Double32_t fPy;  // py component of momentum 4-vector
  Double32_t fPz;  // pz component of momentum 4-vector
  Double32_t fE;   // E  component of momentum 4-vector

  /** Reconstructed position in calorimeter **/
  Double32_t fX;
  Double32_t fY;		
  Double32_t fZ;

  /** PdgId of the particle **/
  Int_t fId;

  /** Chi^2 of the cluster **/
  Double_t fChi2;

  /** Type of cell with maximum energy deposition**/
  Int_t fType;

  /** Reference to cluster **/
  Int_t fClusterNum;
  /** A seed for the photon **/
  Int_t fCellNum;
  /** A mother tracks number **/
  Int_t fMCTrack;

  /** TOF for calorimeter **/
  Double32_t fTime;

  ClassDef(CbmEcalRecParticle, 2)

};

#endif
