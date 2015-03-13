#ifndef CBMECALTRACKEXTRAPOLATION_H
#define CBMECALTRACKEXTRAPOLATION_H

#include "TNamed.h"

class TClonesArray;
class CbmGlobalTrack;
class FairTrackParam;
class CbmEcalStructure;

class CbmEcalTrackExtrapolation : public TNamed
{
public:
  CbmEcalTrackExtrapolation(Int_t verb=0): TNamed("EcalTrackExtrapolation","Tracking for calorimeter"), fVerbose(verb), fStr(NULL), fMap(NULL) {};
  virtual ~CbmEcalTrackExtrapolation() {delete fMap;}
  virtual void Init(); 
  virtual void Finish() {};
  virtual Int_t DoExtrapolate(TClonesArray* gTrackArr, TClonesArray* fTrackParam)=0;
  /** Set verbosity level **/
  inline void SetVerbose(Int_t verbose) {fVerbose=verbose;}
  inline void SetStructure(CbmEcalStructure* str) {fStr=str;}
  inline CbmEcalStructure* GetStructure() const {return fStr;}
  inline Short_t* Map() {return fMap;} 
protected:
  inline Int_t Verbose() const {return fVerbose;}
  inline CbmEcalStructure* Str() const {return fStr;}
private:
  Int_t fVerbose;
  CbmEcalStructure* fStr;
  // A map: track at calorimeter surface -> global track
  Short_t* fMap;			//!

  CbmEcalTrackExtrapolation(const CbmEcalTrackExtrapolation&);
  CbmEcalTrackExtrapolation& operator=(const CbmEcalTrackExtrapolation&);

  ClassDef(CbmEcalTrackExtrapolation, 1)
};

#endif
