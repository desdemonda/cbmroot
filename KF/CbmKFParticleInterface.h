//-----------------------------------------------------------
//-----------------------------------------------------------

#ifndef CbmKFParticleInterface_HH
#define CbmKFParticleInterface_HH

#include "TObject.h"

class CbmStsTrack;
class KFParticle;

class CbmKFParticleInterface : public TObject {
 public:

  // Constructors/Destructors ---------
  CbmKFParticleInterface() {};
  ~CbmKFParticleInterface() {};
  
  static void SetKFParticleFromStsTrack(CbmStsTrack *track, KFParticle* particle, Int_t pdg = 211);
  
 private:  
   
  const CbmKFParticleInterface& operator = (const CbmKFParticleInterface&);
  CbmKFParticleInterface(const CbmKFParticleInterface&);
  
  ClassDef(CbmKFParticleInterface,1);
};

#endif
