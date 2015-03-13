/** CbmMuchDigitizeGem.cxx
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 01.05.11
 *@version 2.0
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 19.03.07
 *@version 1.0
 **
 ** CBM task class for digitizing MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchDigi out of CbmMuchPoint.
 **/

// Includes from MUCH
#include "CbmMuchDigitizeGem.h"
#include "CbmMuchPoint.h"
#include "CbmMuchSector.h"
#include "CbmMuchStation.h"
#include "CbmMuchModuleGem.h"
#include "CbmMuchModuleGemRadial.h"
#include "CbmMuchModuleGemRectangular.h"
#include "CbmMuchPad.h"
#include "CbmMuchPadRadial.h"
#include "CbmMuchPadRectangular.h"
#include "CbmMuchSectorRadial.h"
#include "CbmMuchSectorRectangular.h"
#include "CbmMuchDigi.h"

// Includes from base
#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmMCEpoch.h"
#include "CbmMCBuffer.h"
#include "CbmDaqBuffer.h"

// Includes from ROOT
#include "TObjArray.h"
#include "TDatabasePDG.h"
#include "TFile.h"
#include "TRandom.h"
#include "TChain.h"

#include <vector>
using std::map;

// -------------------------------------------------------------------------
CbmMuchDigitizeGem::CbmMuchDigitizeGem(const char* digiFileName) 
  : FairTask("MuchDigitizeGem",1),
    fgDeltaResponse(),
    fAlgorithm(1),
    fGeoScheme(CbmMuchGeoScheme::Instance()),
    fDigiFile(digiFileName),
    fPoints(NULL),
    fMCTracks(NULL),
    fDigis(NULL),
    fDigiMatches(NULL),
    fNFailed(0),
    fNOutside(0),
    fNMulti(0),
    fNADCChannels(256),
    fQMax(500000),
    fQThreshold(10000),
    fMeanNoise(1500),
    fSpotRadius(0.05),
    fMeanGasGain(1e4),
    fDTime(3),
    fDeadPadsFrac(0),
    fTimer(),
    fDaq(0),
    fMcChain(NULL),
    fDeadTime(400),
    fDriftVelocity(100),
    fPeakingTime(20),
    fRemainderTime(40),
    fTimeBinWidth(1),
    fNTimeBins(200),
    fNdigis(0),
  fTOT(0),
    fTotalDriftTime(0.4/fDriftVelocity*10000) // 40 ns
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchDigitizeGem::~CbmMuchDigitizeGem() {
  if (fDigis) {
    fDigis->Delete();
    delete fDigis;
  }
  if (fDigiMatches) {
    fDigiMatches->Delete();
    delete fDigiMatches;
  }
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchDigitizeGem::Init() {
  FairRootManager* ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("Init", "No FairRootManager");

  // Initialize GeoScheme
  TFile* oldfile=gFile;
  TFile* file=new TFile(fDigiFile);
  TObjArray* stations = (TObjArray*) file->Get("stations");
  file->Close();
  file->Delete();
  gFile=oldfile;
  fGeoScheme->Init(stations);
  // Determine drift volume width
  Double_t driftVolumeWidth = 0.4; // cm - default
  for (Int_t i=0;i<fGeoScheme->GetNStations();i++){
    CbmMuchStation* station = fGeoScheme->GetStation(i);
    if (station->GetNLayers()<=0) continue;
    CbmMuchLayerSide* side = station->GetLayer(0)->GetSide(0);
    if (side->GetNModules()<=0) continue;
    CbmMuchModule* module = side->GetModule(0);
    if (module->GetDetectorType()!=1 && module->GetDetectorType()!=3) continue;
    driftVolumeWidth = module->GetSize().Z();
    break;
  }
  fTotalDriftTime = driftVolumeWidth/fDriftVelocity*10000; // [ns];
  
  if (!fDaq){
    // Get input array of MuchPoints
    fPoints = (TClonesArray*) ioman->GetObject("MuchPoint");
    // Get input array of MC tracks
    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    // Register output array MuchDigi
    fDigis = new TClonesArray("CbmMuchDigi", 1000);
    ioman->Register("MuchDigi", "Digital response in MUCH", fDigis, kTRUE);
    // Register output array MuchDigiMatches
    fDigiMatches = new TClonesArray("CbmMuchDigiMatch", 1000);
    ioman->Register("MuchDigiMatch", "Digi Match in MUCH", fDigiMatches, kTRUE);
  } else {
    if ( ! ( CbmMCBuffer::Instance() && CbmDaqBuffer::Instance() ) ) {
      fLogger->Fatal(MESSAGE_ORIGIN, "No MCBuffer or DaqBuffer present!");
      return kFATAL;
    } 
    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  }

  // Set response on delta function
  Int_t nShapeTimeBins=Int_t(gkResponsePeriod/gkResponseBin);
  fgDeltaResponse.Set(nShapeTimeBins);
  for (Int_t i=0;i<fgDeltaResponse.GetSize();i++){
    Double_t time = i*gkResponseBin;
    if (time<=fPeakingTime) fgDeltaResponse[i]=time/fPeakingTime;
    else fgDeltaResponse[i] = exp(-(time-fPeakingTime)/fRemainderTime); 
  }

  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmMuchDigitizeGem::Exec(Option_t* opt) {
  // get current event to revert back at the end of exec
  Int_t currentEvent = FairRootManager::Instance()->GetInTree()->GetBranch("MCTrack")->GetReadEntry();
  fTimer.Start();
  fNdigis = 0;
  Int_t nPoints=0;
  
  if (!fDaq){
    fDigis->Clear();
    fDigiMatches->Delete();
    CbmMCBuffer::Instance()->Clear();
    CbmMCBuffer::Instance()->Fill(fPoints,kMUCH,0,0);
    CbmMCBuffer::Instance()->SetEndOfRun();

    // Timur: to fix problem with cleanup between events <
    vector<CbmMuchModule*> modules = fGeoScheme->GetModules();

    for (vector<CbmMuchModule*>::iterator i = modules.begin(); i != modules.end(); ++i)
    {
      CbmMuchModuleGemRadial* module = static_cast<CbmMuchModuleGemRadial*> (*i);
      if (module->GetDetectorType() == 2) continue; //AZ - skip straws

      vector<CbmMuchPad*> pads = module->GetPads();

      for (vector<CbmMuchPad*>::iterator j = pads.begin(); j != pads.end(); ++j)
      {
        CbmMuchDigiMatch* match = (*j)->GetMatch();
        match->ClearLinks();
      }
    }
    // Timur >
  }

  const CbmMuchPoint* point = dynamic_cast<const CbmMuchPoint*>(CbmMCBuffer::Instance()->GetNextPoint(kMUCH));
  while (point) {
    nPoints++;
    ExecPoint(point);
    point = dynamic_cast<const CbmMuchPoint*>(CbmMCBuffer::Instance()->GetNextPoint(kMUCH));
  }

  // Add remaining digis
  vector<CbmMuchModule*> modules = fGeoScheme->GetModules();
  for (Int_t im=0;im<modules.size();im++){
    if (modules[im]->GetDetectorType()!=1 && modules[im]->GetDetectorType()!=3) continue;
    CbmMuchModuleGem* module = (CbmMuchModuleGem*) modules[im];
    vector<CbmMuchPad*> pads = module->GetPads();
    for (Int_t ip=0;ip<pads.size();ip++) AddDigi(pads[ip]);
  }

  fTimer.Stop();
  gLogger->Info(MESSAGE_ORIGIN,"MuchDigitizeGem: %5.2f s, %i points, %i digis",fTimer.RealTime(),nPoints,fNdigis);

  // revert branch to "current event"
  FairRootManager::Instance()->GetInTree()->GetBranch("MCTrack")->GetEntry(currentEvent);
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchDigitizeGem::Finish(){
  if (fDaq) Exec("");
}
// -------------------------------------------------------------------------


// ------- Private method ExecAdvanced -------------------------------------
Bool_t CbmMuchDigitizeGem::ExecPoint(const CbmMuchPoint* point) {
  // TODO workaround to extract point index - to be reconsidered
  Int_t iPoint = point->GetLink(0).GetIndex();
  TVector3 v1,v2,dv;
  point->PositionIn(v1);
  point->PositionOut(v2);
  dv = v2-v1;

  Int_t detectorId = point->GetDetectorID();
  CbmMuchModule* module = fGeoScheme->GetModuleByDetId(detectorId);
  
  if (fAlgorithm==0){    // Simple digitization
    TVector3 v = 0.5*(v1+v2);
    CbmMuchPad* pad = 0;
    if (module->GetDetectorType()==1){
      CbmMuchModuleGemRectangular* module1 = (CbmMuchModuleGemRectangular*) module;
      pad = module1->GetPad(v[0],v[1]);
      if (pad) printf("x0=%f,y0=%f\n",pad->GetX(),pad->GetY());
    } else if (module->GetDetectorType()==3){
      CbmMuchModuleGemRadial* module3 = (CbmMuchModuleGemRadial*) module;
      pad = module3->GetPad(v[0],v[1]);
    }
    if (!pad) return kFALSE;
    AddCharge(pad,fQMax,iPoint,point->GetTime(),0);
    return kTRUE;
  }
  
  // Start of advanced digitization
  Int_t nElectrons = Int_t(GetNPrimaryElectronsPerCm(point)*dv.Mag());
  if (nElectrons<0) return kFALSE;
  
  Double_t time = point->GetTime();
  
  if (module->GetDetectorType()==1) {
    CbmMuchModuleGemRectangular* module1 = (CbmMuchModuleGemRectangular*) module;
    map<CbmMuchSector*,Int_t> firedSectors;
    for (Int_t i=0;i<nElectrons;i++) {
      Double_t aL = gRandom->Rndm();
      Double_t driftTime = (1-aL)*fTotalDriftTime;
      TVector3 ve = v1 + dv*aL;
      UInt_t ne   = GasGain();
      Double_t x  = ve.X();
      Double_t y  = ve.Y();
      Double_t x1 = x-fSpotRadius;
      Double_t x2 = x+fSpotRadius;
      Double_t y1 = y-fSpotRadius;
      Double_t y2 = y+fSpotRadius;
      Double_t s  = 4*fSpotRadius*fSpotRadius;
      firedSectors[module1->GetSector(x1,y1)]=0;
      firedSectors[module1->GetSector(x1,y2)]=0;
      firedSectors[module1->GetSector(x2,y1)]=0;
      firedSectors[module1->GetSector(x2,y2)]=0;
      for (map<CbmMuchSector*, Int_t>::iterator it = firedSectors.begin(); it!= firedSectors.end(); it++) {
        CbmMuchSector* sector  = (*it).first;
        if (!sector) continue;
        for (Int_t iPad=0;iPad<sector->GetNChannels();iPad++){
          CbmMuchPad* pad = sector->GetPadByChannelIndex(iPad);
          Double_t xp0 = pad->GetX();
          Double_t xpd = pad->GetDx()/2.;
          Double_t xp1 = xp0-xpd;
          Double_t xp2 = xp0+xpd;
          if (x1>xp2 || x2<xp1) continue;
          Double_t yp0 = pad->GetY();
          Double_t ypd = pad->GetDy()/2.;
          Double_t yp1 = yp0-ypd;
          Double_t yp2 = yp0+ypd;
          if (y1>yp2 || y2<yp1) continue;
          Double_t lx = x1>xp1 ? (x2<xp2 ? x2-x1 : xp2-x1) : x2-xp1;
          Double_t ly = y1>yp1 ? (y2<yp2 ? y2-y1 : yp2-y1) : y2-yp1;
          AddCharge(pad,UInt_t(ne*lx*ly/s),iPoint,time,driftTime);
        }
      } // loop fired sectors
      firedSectors.clear();
    }
  }
  
  if (module->GetDetectorType()==3) {
    CbmMuchModuleGemRadial* module3 = (CbmMuchModuleGemRadial*) module;
    CbmMuchSectorRadial* sFirst = (CbmMuchSectorRadial*) module3->GetSectorByIndex(0);
    CbmMuchSectorRadial* sLast  = (CbmMuchSectorRadial*) module3->GetSectorByIndex(module3->GetNSectors()-1);

    Double_t rMin = sFirst->GetR1();
    Double_t rMax = sLast->GetR2();

    for (Int_t i=0;i<nElectrons;i++) {
      Double_t aL   = gRandom->Rndm();
      Double_t driftTime = -1;
      while(driftTime < 0) driftTime = (1-aL)*fTotalDriftTime + gRandom->Gaus(0, fDTime);
      TVector3 ve   = v1 + dv*aL;
      UInt_t ne     = GasGain();
      Double_t r    = ve.Perp();
      Double_t phi  = ve.Phi();
      Double_t r1   = r-fSpotRadius;
      Double_t r2   = r+fSpotRadius;
      Double_t phi1 = phi-fSpotRadius/r;
      Double_t phi2 = phi+fSpotRadius/r;
      if (r1<rMin && r2>rMin) {
        AddCharge(sFirst,UInt_t(ne*(r2-rMin)/(r2-r1)),iPoint,time,driftTime,phi1,phi2);
        continue;
      }  
      if (r1<rMax && r2>rMax) {
        AddCharge(sLast,UInt_t(ne*(rMax-r1)/(r2-r1)),iPoint,time,driftTime,phi1,phi2);
        continue;
      }

      CbmMuchSectorRadial* s1 = module3->GetSectorByRadius(r1);
      CbmMuchSectorRadial* s2 = module3->GetSectorByRadius(r2);
      if (s1==s2) AddCharge(s1,ne,iPoint,time,driftTime,phi1,phi2); 
      else {
        AddCharge(s1,UInt_t(ne*(s1->GetR2()-r1)/(r2-r1)),iPoint,time,driftTime,phi1,phi2);
        AddCharge(s2,UInt_t(ne*(r2-s2->GetR1())/(r2-r1)),iPoint,time,driftTime,phi1,phi2);
      }
    }
  }

  return kTRUE;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Int_t CbmMuchDigitizeGem::GasGain() {
  Double_t gasGain = -fMeanGasGain * TMath::Log(1 - gRandom->Rndm());
  if (gasGain < 0.) gasGain = 1e6;
  return (Int_t) gasGain;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchDigitizeGem::Sigma_n_e(Double_t Tkin, Double_t mass) {
  Double_t logT;
  TF1 fPol6("fPol6","pol6",-5,10);
  if (mass < 0.1) {
    logT = log(Tkin * 0.511 / mass);
    if (logT > 9.21034)    logT = 9.21034;
    if (logT < min_logT_e) logT = min_logT_e;
    return fPol6.EvalPar(&logT,sigma_e);
  } else if (mass >= 0.1 && mass < 0.2) {
    logT = log(Tkin * 105.658 / mass);
    if (logT > 9.21034)    logT = 9.21034;
    if (logT < min_logT_mu) logT = min_logT_mu;
    return fPol6.EvalPar(&logT,sigma_mu);
  } else {
    logT = log(Tkin * 938.272 / mass);
    if (logT > 9.21034)    logT = 9.21034;
    if (logT < min_logT_p) logT = min_logT_p;
    return fPol6.EvalPar(&logT,sigma_p);
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmMuchDigitizeGem::MPV_n_e(Double_t Tkin, Double_t mass) {
  Double_t logT;
  TF1 fPol6("fPol6","pol6",-5,10);
  if (mass < 0.1) {
    logT = log(Tkin * 0.511 / mass);
    if (logT > 9.21034)    logT = 9.21034;
    if (logT < min_logT_e) logT = min_logT_e;
    return fPol6.EvalPar(&logT,mpv_e);
  } else if (mass >= 0.1 && mass < 0.2) {
    logT = log(Tkin * 105.658 / mass);
    if (logT > 9.21034)    logT = 9.21034;
    if (logT < min_logT_mu) logT = min_logT_mu;
    return fPol6.EvalPar(&logT,mpv_mu);
  } else {
    logT = log(Tkin * 938.272 / mass);
    if (logT > 9.21034)    logT = 9.21034;
    if (logT < min_logT_p) logT = min_logT_p;
    return fPol6.EvalPar(&logT,mpv_p);
  }
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Double_t CbmMuchDigitizeGem::GetNPrimaryElectronsPerCm(const CbmMuchPoint* point){
  Int_t trackId = point->GetTrackID();
  Int_t eventId = point->GetEventID();
  if (trackId < 0) return -1;


  /* Commented out on request of A. Senger from 22.01.2014
  if (fDaq && eventId!=FairRootManager::Instance()->GetInTree()->GetBranch("MCTrack")->GetReadEntry())
      FairRootManager::Instance()->GetInTree()->GetBranch("MCTrack")->GetEntry(eventId);
  */
  CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(trackId);
  
  if (!mcTrack) return -1;
  Int_t pdgCode = mcTrack->GetPdgCode();

  TParticlePDG *particle = TDatabasePDG::Instance()->GetParticle(pdgCode);
  // Assign proton hypothesis for unknown particles
  if (!particle) particle = TDatabasePDG::Instance()->GetParticle(2212);
  if (TMath::Abs(particle->Charge()) < 0.1) return -1;

  Double_t m = particle->Mass();
  TLorentzVector p;
  p.SetXYZM(point->GetPx(),point->GetPy(),point->GetPz(),m);
  Double_t Tkin = p.E()-m; // kinetic energy of the particle
  Double_t sigma = CbmMuchDigitizeGem::Sigma_n_e(Tkin,m); // sigma for Landau distribution
  Double_t mpv   = CbmMuchDigitizeGem::MPV_n_e(Tkin,m);   // most probable value for Landau distr.
  Double_t n = gRandom->Landau(mpv, sigma);
  while (n > 5e4) n = gRandom->Landau(mpv, sigma); // restrict Landau tail to increase performance
  return m<0.1 ? n/l_e : n/l_not_e;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Bool_t CbmMuchDigitizeGem::AddCharge(CbmMuchSectorRadial* s,UInt_t ne, Int_t iPoint, Double_t time, Double_t driftTime, 
    Double_t phi1, Double_t phi2){
  CbmMuchPadRadial* pad1 = s->GetPadByPhi(phi1);
  CbmMuchPadRadial* pad2 = s->GetPadByPhi(phi2);
  if (pad1==pad2) 
    AddCharge(pad1,ne,iPoint,time,driftTime);
  else {
    Double_t phi = pad1 ? pad1->GetPhi2() : pad2 ? pad2->GetPhi1() : 0;
    UInt_t pad1_ne = UInt_t(ne*(phi-phi1)/(phi2-phi1));
    AddCharge(pad1,pad1_ne   ,iPoint,time,driftTime);
    AddCharge(pad2,ne-pad1_ne,iPoint,time,driftTime);
  }
  return kFALSE;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuchDigitizeGem::AddCharge(CbmMuchPad* pad, UInt_t charge, Int_t iPoint, Double_t time, Double_t driftTime){
  if (!pad) return;
  CbmMuchDigiMatch* match = pad->GetMatch();
  CbmMuchDigi* digi = pad->GetDigi();
  
  if (match->GetNofLinks()==0) {
    digi->SetTime(time);
    match->SetDeadTime(fDeadTime);
  }
  if (time>digi->GetTime()+match->GetDeadTime()) {
    AddDigi(pad);
    digi->SetTime(time);
    match->SetDeadTime(fDeadTime);
  }
  match->AddCharge(iPoint,charge,time+driftTime,fgDeltaResponse,time);
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
Bool_t CbmMuchDigitizeGem::AddDigi(CbmMuchPad* pad) {
  CbmMuchDigiMatch* match = pad->GetMatch();
  CbmMuchDigi* digi = pad->GetDigi();
  match->AddNoise(fMeanNoise);
  Double_t  max_charge = match->GetMaxCharge();
  Double_t t1 = match->GetTimeStamp(fQThreshold);
  Double_t  dt = match->GetTimeOverThreshold(fQThreshold);

  // Check for threshold 
  if (t1<0) {
    match->Reset();
    return kFALSE;
  }
  
  
  Int_t adc = 0;
  Double_t nBinsInNs = 1;
  if (fTOT) adc = Int_t(dt/nBinsInNs);             // if time over threshold
  else      adc = max_charge*fNADCChannels/fQMax;  // if max amplitude
  // if overflow
  if (adc >= (1<<12)) adc = (1<<12) - 1;

  digi->SetAdc(adc);
  digi->SetTime(t1);
  gLogger->Debug1(MESSAGE_ORIGIN,"Pad: sector=%i channel=%i",pad->GetSectorIndex(),pad->GetChannelIndex());
  gLogger->Debug1(MESSAGE_ORIGIN,"New digi: sector=%i channel=%i",CbmMuchAddress::GetSectorIndex(digi->GetAddress()),CbmMuchAddress::GetChannelIndex(digi->GetAddress()));
  
  if (fDaq){
    CbmMuchDigi* digLight = new CbmMuchDigi(digi,match);
    CbmDaqBuffer::Instance()->InsertData(digLight);
  } else {
    new ((*fDigis)[fDigis->GetEntriesFast()]) CbmMuchDigi(digi);
    new ((*fDigiMatches)[fDigiMatches->GetEntriesFast()]) CbmMuchDigiMatch(match);
  }
  match->Reset();
  fNdigis++;
  return kTRUE;
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchDigitizeGem)






