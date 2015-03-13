// -------------------------------------------------------------------------
// -----                   CbmAnaHypMix source file                    -----
// -----               Created 27/06/06  by E. Kryshen                 -----
// -------------------------------------------------------------------------
// Updated 24/01/07
// Updated 08/02/07
#include "CbmAnaHypMix.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "CbmAnaHypLambdaCandidate.h"

// -------------------------------------------------------------------------
CbmAnaHypMix::CbmAnaHypMix(Int_t nMixedEvents, Int_t iVerbose)
  : CbmAnaHypRecCascade("MixTask",iVerbose),
    fNoMixedEv(nMixedEvents)
{
}
// -------------------------------------------------------------------------

void CbmAnaHypMix::Exec(Option_t* option){
  printf("Event:%i\n",fEvent);
  fListCascades->Clear();
  fCascades=0;

  CreateDaVector();

  for (unsigned j=0;j<vDa.size();j++){
    CbmAnaHypTrack &da = vDa[j];
    for (Int_t ev=fEvent+1; ev<fEvent+1+fNoMixedEv;ev++){
      if (ev<1000) fTree->GetEntry(ev);
      else         fTree->GetEntry(ev-1000);
      for (Int_t i=0;i<fListRCLambda->GetEntriesFast();i++){
        CbmAnaHypLambdaCandidate* v0 = (CbmAnaHypLambdaCandidate*) fListRCLambda->At(i);
        CreatePair(da,*v0);
      }
    }
  }
  fTree->GetEntry(fEvent);
  fEvent++;
  vDa.clear();

}
// -------------------------------------------------------------------------

