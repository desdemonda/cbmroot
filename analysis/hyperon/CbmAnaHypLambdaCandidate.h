#ifndef CBMHYPANALAMBDACANDIDATE_H
#define CBMHYPANALAMBDACANDIDATE_H

#include "TObject.h"
#include "CbmAnaHypPair.h"
#include "CbmStsTrack.h"
class CbmAnaHypLambdaCandidate : public CbmAnaHypPair
{

 public:

  CbmAnaHypLambdaCandidate();
  CbmAnaHypLambdaCandidate(CbmAnaHypPair v0, CbmStsTrack track);
  virtual ~CbmAnaHypLambdaCandidate();

  CbmStsTrack track;

  ClassDef(CbmAnaHypLambdaCandidate,1);

};

#endif
