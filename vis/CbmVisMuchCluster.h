// -------------------------------------------------------------------------
// -----                   CbmVisMuchCluster header file               -----
// -----                  Created 15/11/07  by E. Kryshen              -----
// -------------------------------------------------------------------------

/** CbmVisMuch
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 15.11.07
 *@version 1.0
 **
 **/


#ifndef CBMVISMUCHCLUSTER_H
#define CBMVISMUCHCLUSTER_H 1

#include <vector>

class CbmMuchCluster;
class TClonesArray;
class CbmMuchPad;
class TH2I;
class TCanvas;

using std::vector;

class CbmVisMuchCluster : public TObject{
 ClassDef(CbmVisMuchCluster,1);

 public:
  CbmVisMuchCluster(Int_t id);
  virtual ~CbmVisMuchCluster();
  void CreateHisto();
  void PrintInfo();
  void DrawHisto();
private:
  Int_t              fStationNr;
  TClonesArray*      fDigis;
  TClonesArray*      fDigiMatches;
  TClonesArray*      fClusters;

  Int_t               fClusterId;
  CbmMuchCluster*     fCluster;
  vector<CbmMuchPad*> fPads;
  vector<Double_t> fX;
  vector<Double_t> fY;
  vector<Double_t> fC;
  TCanvas* fCanvas;
  TH2I* fhChargeXY;
};


#endif
