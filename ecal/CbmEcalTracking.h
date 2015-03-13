#ifndef CBMECALTRACKING_H
#define CBMECALTRACKING_H

#include "FairTask.h"

class CbmEcalHitProducer;
class CbmEcalStructure;
class CbmEcalTrackExtrapolation;
class TClonesArray;

class CbmEcalTracking : public FairTask
{
public:
  /** Default constructor **/
  CbmEcalTracking();

  /** Standerd constructor **/
  CbmEcalTracking(const char* name, const Int_t verbose);

  /** Get/Set track extrapolation algorithm **/
  void SetTrackExtrapolation(CbmEcalTrackExtrapolation* e);
  CbmEcalTrackExtrapolation* GetTrackExtrapolation() const;

  /** Task initialization **/
  virtual InitStatus Init();

  /** Exec for task **/
  virtual void Exec(Option_t* opt);
  
  /** Finish task **/
  virtual void Finish();

  /** virtual destructor **/
  ~CbmEcalTracking();
private:
  Int_t fVerbose;
 
  CbmEcalStructure* fStru;
  CbmEcalTrackExtrapolation* fTrE;
  TClonesArray* fGTArray;
  TClonesArray* fTrackPar;

  CbmEcalTracking(const CbmEcalTracking&);
  CbmEcalTracking& operator=(const CbmEcalTracking&);

  ClassDef(CbmEcalTracking, 1)
};

/** Get/Set track extrapolation algorithm **/
inline void CbmEcalTracking::SetTrackExtrapolation(CbmEcalTrackExtrapolation* e)
{
  fTrE=e;
}

inline CbmEcalTrackExtrapolation* CbmEcalTracking::GetTrackExtrapolation() const
{
  return fTrE;
}

#endif
    
