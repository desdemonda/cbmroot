// -------------------------------------------------------------------------
// -----              CbmAnaHypSelectLambda header file                -----
// -----               Created 07/02/05  by E. Kryshen                 -----
// -------------------------------------------------------------------------

/** @class CbmAnaHypSelectLambda.h
 ** @author E.Kryshen  <e.kryshen@gsi.de>
 **
 ** This task loops over reconstructed tracks and reconstructs lambda
 ** decays using impact parameter constraint, given in the macro.
 **
 ** Reconstructed lambda candidates are stored to the file of
 ** CbmStSTrack objects.
 */

#ifndef CBMANAHYPSELECTLAMBDA_H
#define CBMANAHYPSELECTLAMBDA_H

#include "CbmAnaHypRecLambda.h"
class TClonesArray;

class CbmAnaHypSelectLambda : public CbmAnaHypRecLambda
{
public:
  CbmAnaHypSelectLambda();
  CbmAnaHypSelectLambda(const char *name, Int_t iVerbose);
  virtual ~CbmAnaHypSelectLambda(){};
  InitStatus Init();
  virtual void Exec(Option_t* option);

  void SetCutB1  (Double_t b1  ) {fCutB1  =b1;}
  void SetCutB2  (Double_t b2  ) {fCutB2  =b2;}
  void SetCutChi (Double_t chi ) {fCutChi =chi; }
  void SetCutPca (Double_t pca ) {fCutPca =pca; }
  void SetCutDca (Double_t dca ) {fCutDca =dca; }
  void SetCutMmin(Double_t mMin) {fCutMmin=mMin;}
  void SetCutMmax(Double_t mMax) {fCutMmax=mMax;}
  void SetCutPcaMax(Double_t pcaMax) {fCutPcaMax=pcaMax;}

protected:
  Double_t fCutB1;
  Double_t fCutB2;
  Double_t fCutChi;
  Double_t fCutDca;
  Double_t fCutPca;
  Double_t fCutPcaMax;
  Double_t fCutMmin;
  Double_t fCutMmax;
  TClonesArray* fListRCLambda;

 private:

  CbmAnaHypSelectLambda(const CbmAnaHypSelectLambda&);
  CbmAnaHypSelectLambda operator=(const CbmAnaHypSelectLambda&);

  ClassDef(CbmAnaHypSelectLambda,1);
};

#endif
