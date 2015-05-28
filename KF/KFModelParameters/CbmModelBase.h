/*
 *====================================================================
 *
 *  CBM Base Class for Models
 *  
 *  Authors: V.Vovchenko
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  Base class for models
 *
 *====================================================================
 */
 
#ifndef _CbmModelBase_h_
#define _CbmModelBase_h_
 
#include "FairTask.h"
 
class FairRootManager;
class KFParticleTopoReconstructor;

class CbmModelBase: public TObject
{
 public:
  CbmModelBase(KFParticleTopoReconstructor* tr=0):fTopoReconstructor(tr) { }
  ~CbmModelBase() { }

  virtual void ReInit(FairRootManager *fManger) { }
  virtual void Init() { }
  virtual void Exec() { }
  virtual void Finish() { }

  ClassDef(CbmModelBase,1);

 private:

  CbmModelBase(const CbmModelBase&);
  CbmModelBase& operator=(const CbmModelBase&);
  
 protected:
  KFParticleTopoReconstructor* fTopoReconstructor;

};

#endif
