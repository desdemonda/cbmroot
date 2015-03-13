// -------------------------------------------------------------------------
// -----                    CbmFieldCreator header file                  -----
// -----                Created 15/01/07  by M. Al-Turany              -----
// -------------------------------------------------------------------------


#ifndef CBMFIELDCREATOR_H
#define CBMFIELDCREATOR_H

#include "FairFieldFactory.h"

class CbmFieldPar;

class FairField;

class CbmFieldCreator : public FairFieldFactory 
{

 public:
  CbmFieldCreator();
  virtual ~CbmFieldCreator();
  virtual FairField* createFairField();
  virtual void SetParm();
  ClassDef(CbmFieldCreator,1);
  
 protected:
  CbmFieldPar* fFieldPar;
  
 private:
  CbmFieldCreator(const CbmFieldCreator&);
  CbmFieldCreator& operator=(const CbmFieldCreator&);

};
#endif //CBMFIELDCREATOR_H
