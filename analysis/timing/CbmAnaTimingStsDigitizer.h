/** CbmAnaTimingStsDigitizer.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-07-07
 **
 **/

#ifndef CbmAnaTimingStsDigitizer_H_
#define CbmAnaTimingStsDigitizer_H_ 1

#include "FairTask.h"
#include "TString.h"
class TClonesArray;
class CbmMCEpoch;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;
class TH1D;

class CbmAnaTimingStsDigitizer : public FairTask{
public:
  CbmAnaTimingStsDigitizer();
  CbmAnaTimingStsDigitizer(const char* name, TString histoName);
  virtual ~CbmAnaTimingStsDigitizer();
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void Finish();
  virtual void SetParContainers();
  void SetStationId(Int_t id) { fStationId = id; }
  void SetSectorId(Int_t id)  { fSectorId = id; }
  void SetSideId(Int_t id)    { fSideId = id; }
  void SetChannelId(Int_t id) { fChannelId = id; }
  void SetEpoch(Bool_t epoch) { fEpoch = epoch; }
  
private:
  CbmGeoStsPar*     fGeoPar;      /** Geometry parameters **/
  CbmStsDigiPar*    fDigiPar;     /** Digitisation parameters **/
  CbmStsDigiScheme* fDigiScheme;  /** Digitisation scheme **/
  Int_t         fEvent;             //!
  TString       fHistoName;
  TClonesArray* fMcTracks;         //!
  TClonesArray* fStsPoints;        //!
  TClonesArray* fStsDigis;         //!
  TClonesArray* fStsDigiMatches;   //!
  CbmMCEpoch*   fMcEpoch;          //!
  Int_t fStationId;
  Int_t fSectorId;
  Int_t fSideId;
  Int_t fChannelId;
  Bool_t fEpoch;
  TH1D* fPointsTimeAll;
  TH1D* fPointsTimeSector;
  TH1D* fDigiTimeAll;
  TH1D* fDigiTimeSector;
  TH1D* fDigiVsSector;
  ClassDef(CbmAnaTimingStsDigitizer,1)
};

#endif
