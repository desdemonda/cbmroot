#include "LxTrackAna.h"
#include <iostream>
#include "CbmMuchGeoScheme.h"
#include "CbmMuchCluster.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "CbmStsAddress.h"
#include "CbmMuchPoint.h"
#include "TH1.h"
#include "TH2.h"
#include "TDatabasePDG.h"
#include "CbmStsTrack.h"
#include "CbmKFTrack.h"
#include "CbmKFParticle.h"
#include "TGeoManager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

ClassImp(LxTrackAna)

using namespace std;

//static Double_t xRms = 1.005;// Averaged MC
static Double_t xRms = 1.202;// Nearest hit
static Double_t xRms2 = xRms * xRms;
//static Double_t yRms = 0.9467;// Averaged MC
static Double_t yRms = 1.061;// Nearest hit
static Double_t yRms2 = yRms * yRms;
//static Double_t txRms = 0.02727;// Averaged MC
static Double_t txRms = 0.02426;// Nearest hit
static Double_t txRms2 = txRms * txRms;
//static Double_t tyRms = 0.01116;// Averaged MC
static Double_t tyRms = 0.01082;// Nearest hit
static Double_t tyRms2 = tyRms * tyRms;
static Double_t cutCoeff = 3.0;

static TH1F* muchStsBreakX = 0;
static TH1F* muchStsBreakY = 0;
static TH1F* muchStsBreakTx = 0;
static TH1F* muchStsBreakTy = 0;

static TH1F* stsMuchBreakX = 0;
static TH1F* stsMuchBreakY = 0;

static TH1F* signalChi2 = 0;
static TH1F* bgrChi2 = 0;

static TH1F* bgrInvMass = 0;
static list<LxSimpleTrack*> positiveTracks;
static list<LxSimpleTrack*> negativeTracks;
static TH1F* sigInvMass = 0;

static TH1F* nearestHitDist[LXSTATIONS] = { 0 };
static TH1F* hitsDist[LXSTATIONS] = { 0 };

static TH1F* muPlusStsTxDiff = 0;
static TH1F* muMinusStsTxDiff = 0;
static TH1F* muPlusStsXDiff = 0;
static TH1F* muMinusStsXDiff = 0;
static TH1F* muPlusVertexTxDiff = 0;
static TH1F* muMinusVertexTxDiff = 0;

static TH2F* muPlusStsBeginTxDiff2D = 0;
static TH2F* muMinusStsBeginTxDiff2D = 0;

static TH1F* deltaPhiPi = 0;

static TH1F* jPsiMuonsMomsHisto = 0;

static Double_t magnetCenterZ = 0;

static TH1F* dtxMomProductHisto = 0;

struct MomVsTxRange
{
  Double_t momLow;
  Double_t momHigh;
  Double_t txLow;
  Double_t txHigh;
};

static bool momFitTxBreak(Double_t mom, Double_t txBreak)
{
  if (mom < 3.0)
    return false;

  if (txBreak < 0)
    txBreak = -txBreak;

  Double_t inv = mom * txBreak;
  return inv > 0.18 && inv < 0.52;
}

void LxSimpleTrack::RebindMuchTrack()
{
  linkedStsTrack = 0;

  while (!linkedStsTracks.empty())
  {
    pair<LxSimpleTrack*, Double_t> trackDesc = linkedStsTracks.front();
    linkedStsTracks.pop_front();
    LxSimpleTrack* anotherMuchTrack = trackDesc.first->linkedMuchTrack.first;

    if (0 == anotherMuchTrack || trackDesc.second < trackDesc.first->linkedMuchTrack.second)
    {
      trackDesc.first->linkedMuchTrack.first = this;
      trackDesc.first->linkedMuchTrack.second = trackDesc.second;
      linkedStsTrack = trackDesc.first;

      if (0 != anotherMuchTrack)
        anotherMuchTrack->RebindMuchTrack();

      break;
    }
  }
}

LxTrackAna::LxTrackAna() : listMCTracks(0), listStsPts(0), listMuchPts(0), listMuchPixelHits(0), listMuchClusters(0),
    listMuchPixelDigiMatches(0), allTracks(), posTracks(), negTracks(), superEventTracks(0),
    superEventBrachTrack(0, 0, 0, 0, 0, 0, 0, 0), useHitsInStat(false), averagePoints(false), dontTouchNonPrimary(true),
    useChargeSignInCuts(false), buildConnectStat(false), buildBgrInvMass(false), buildSigInvMass(false), joinData(false),
    buildNearestHitDist(false), cropHits(false), buildSegmentsStat(true), segmentsAnalyzer(*this)
{
}

LxTrackAna::~LxTrackAna()
{
  Clean();
}

void LxTrackAna::Clean()
{
  for (vector<LxSimpleTrack*>::iterator i = allTracks.begin(); i != allTracks.end(); ++i)
    delete *i;

  allTracks.clear();
  posTracks.clear();
  negTracks.clear();
}

InitStatus LxTrackAna::Init()
{
  FairRootManager* fManager = FairRootManager::Instance();
  listMCTracks = static_cast<TClonesArray*> (fManager->GetObject("MCTrack"));

  if (0 == listMCTracks)
    return kFATAL;

  listStsPts = static_cast<TClonesArray*> (fManager->GetObject("StsPoint"));

  if (0 == listStsPts)
    return kFATAL;

  listMuchPts = static_cast<TClonesArray*> (fManager->GetObject("MuchPoint"));

  if (0 == listMuchPts)
    return kFATAL;

  if (useHitsInStat)
  {
    listMuchPixelHits = static_cast<TClonesArray*> (fManager->GetObject("MuchPixelHit"));

    if (0 == listMuchPixelHits)
      return kFATAL;

    listMuchClusters = static_cast<TClonesArray*> (fManager->GetObject("MuchCluster"));

    if (0 == listMuchClusters)
      return kFATAL;

    listMuchPixelDigiMatches = static_cast<TClonesArray*> (fManager->GetObject("MuchDigiMatch"));

    if (0 == listMuchPixelDigiMatches)
      return kFATAL;
  }

  if (buildConnectStat)
  {
    muchStsBreakX = new TH1F("muchStsBreakX", "Break in prediction of X in STS", 100, -20., 20.);
    muchStsBreakX->StatOverflows();
    muchStsBreakY = new TH1F("muchStsBreakY", "Break in prediction of Y in STS", 100, -20., 20.);
    muchStsBreakY->StatOverflows();
    muchStsBreakTx = new TH1F("muchStsBreakTx", "Break in prediction of Tx in STS", 100, -0.15, 0.15);
    muchStsBreakTx->StatOverflows();
    muchStsBreakTy = new TH1F("muchStsBreakTy", "Break in prediction of Ty in STS", 100, -0.15, 0.15);
    muchStsBreakTy->StatOverflows();

    stsMuchBreakX = new TH1F("stsMuchBreakX", "Break in prediction of X in MUCH", 100, -20., 20.);
    stsMuchBreakX->StatOverflows();
    stsMuchBreakY = new TH1F("stsMuchBreakY", "Break in prediction of Y in MUCH", 100, -20., 20.);
    stsMuchBreakY->StatOverflows();

    signalChi2 = new TH1F("signalChi2", "Chi2 of signal", 100, 0., 15.);
    signalChi2->StatOverflows();
    bgrChi2 = new TH1F("bgrChi2", "Chi2 of background", 100, 0., 20.);
    bgrChi2->StatOverflows();
  }// if (buildConnectStat)

  if (buildBgrInvMass)
  {
    if (joinData)
    {
      bgrInvMass = new TH1F("bgrInvMass", "Invariant mass distribution for background", 1000, 2., 4.);
      bgrInvMass->StatOverflows();
    }
    else
    {
      superEventTracks = new TTree("SuperEventTracks", "Tracks for building a super event");
      superEventTracks->Branch("tracks", &superEventBrachTrack.px, "px/D:py:pz:e:charge");
    }
  }// if (buildBgrInvMass)

  if (buildSigInvMass)
  {
    sigInvMass = new TH1F("sigInvMass", "Invariant mass distribution for signal", 1000, 2., 4.);
    sigInvMass->StatOverflows();
  }// if (buildSigInvMass)

  if (buildNearestHitDist && useHitsInStat)
  {
    char name[32];
    char title[128];

    for (Int_t i = 0; i < LXSTATIONS; ++i)
    {
      sprintf(name, "nearestHitDist_%d", i);
      sprintf(title, "Distance from a MC point to the nearest hit at %d station", i);
      nearestHitDist[i] = new TH1F(name, title, 100, 0., 5.);
      nearestHitDist[i]->StatOverflows();
      sprintf(name, "hitsDist_%d", i);
      sprintf(title, "Distance from a MC point to the hits at %d station", i);
      hitsDist[i] = new TH1F(name, title, 100, 0., 7.);
      hitsDist[i]->StatOverflows();
    }
  }

  muPlusStsTxDiff = new TH1F("muPlusStsTxDiff", "muPlusStsTxDiff", 100, -0.2, 0.2);
  muPlusStsTxDiff->StatOverflows();
  muMinusStsTxDiff = new TH1F("muMinusStsTxDiff", "muMinusStsTxDiff", 100, -0.2, 0.2);
  muMinusStsTxDiff->StatOverflows();
  muPlusStsXDiff = new TH1F("muPlusStsXDiff", "muPlusStsXDiff", 100, -10.0, 10.0);
  muPlusStsXDiff->StatOverflows();
  muMinusStsXDiff = new TH1F("muMinusStsXDiff", "muMinusStsXDiff", 100, -10.0, 10.0);
  muMinusStsXDiff->StatOverflows();
  muPlusVertexTxDiff = new TH1F("muPlusVertexTxDiff", "muPlusVertexTxDiff", 100, -0.2, 0.2);
  muPlusVertexTxDiff->StatOverflows();
  muMinusVertexTxDiff = new TH1F("muMinusVertexTxDiff", "muMinusVertexTxDiff", 100, -0.2, 0.2);
  muMinusVertexTxDiff->StatOverflows();

  muPlusStsBeginTxDiff2D = new TH2F("muPlusStsBeginTxDiff2D", "muPlusStsBeginTxDiff2D", 100, 0., 25., 100, -0.2, 0.2);
  muPlusStsBeginTxDiff2D->StatOverflows();
  muMinusStsBeginTxDiff2D = new TH2F("muMinusStsBeginTxDiff2D", "muMinusStsBeginTxDiff2D", 100, 0., 25.0, 100, -0.2, 0.2);
  muMinusStsBeginTxDiff2D->StatOverflows();

  deltaPhiPi = new TH1F("deltaPhiPi", "deltaPhiPi", 100, 0., 1.0);
  deltaPhiPi->StatOverflows();

  jPsiMuonsMomsHisto = new TH1F("jPsiMuonsMomsHisto", "J/Psi muons momenta distribution", 200, 0., 25.);
  jPsiMuonsMomsHisto->StatOverflows();

  gGeoManager->cd("/cave_1/Magnet_container_0");
  Double_t localCoords[3] = {0., 0., 0.};
  Double_t globalCoords[3];
  gGeoManager->LocalToMaster(localCoords, globalCoords);
  magnetCenterZ = globalCoords[2];
  //cout << "magnetCenterZ = " << magnetCenterZ << endl;
  //return kFATAL;

  dtxMomProductHisto = new TH1F("dtxMomProductHisto", "Dtx x Momentum distribution", 100, -0.5, 2.5);
  dtxMomProductHisto->StatOverflows();

  segmentsAnalyzer.Init();

  return kSUCCESS;
}

static void SaveHisto(TH1* histo, const char* name)
{
  DIR* dir = opendir("configuration");

  if (dir)
    closedir(dir);
  else
    mkdir("configuration", 0700);

  char file_name[256];
  sprintf(file_name, "configuration/%s", name);
  TFile fh(file_name, "RECREATE");
  histo->Write();
  fh.Close();
  delete histo;
}

static void BuildInvMass(list<LxSimpleTrack*>& pTracks, list<LxSimpleTrack*>& nTracks, TH1* histo)
{
  for (list<LxSimpleTrack*>::iterator i = pTracks.begin(); i != pTracks.end(); ++i)
  {
    LxSimpleTrack* posTrack = *i;

    for (list<LxSimpleTrack*>::iterator j = nTracks.begin(); j != nTracks.end(); ++j)
    {
      LxSimpleTrack* negTrack = *j;
      Double_t E12 = posTrack->e + negTrack->e;
      Double_t px12 = posTrack->px + negTrack->px;
      Double_t py12 = posTrack->py + negTrack->py;
      Double_t pz12 = posTrack->pz + negTrack->pz;
      Double_t m122 = E12 * E12 - px12 * px12 - py12 * py12 - pz12 * pz12;
      Double_t m12 = m122 > 1.e-20 ? sqrt(m122) : 0.;
      histo->Fill(m12);
    }
  }
}

static void BuildInvMass2(list<CbmStsTrack*>& stsTracks, TH1* histo)
{
  for (list<CbmStsTrack*>::iterator i = stsTracks.begin(); i != stsTracks.end(); ++i)
  {
    CbmStsTrack* posTrack = *i;
    //CbmStsTrack t1(*posTrack);
    //extFitter.DoFit(&t1, 13);
    //Double_t chi2Prim = extFitter.GetChiToVertex(&t1, fPrimVtx);
    const FairTrackParam* t1param = posTrack->GetParamFirst();
    //extFitter.Extrapolate(&t1, fPrimVtx->GetZ(), &t1param);
    CbmKFTrack muPlus(*posTrack);
    Double_t t1Qp = t1param->GetQp();
    list<CbmStsTrack*>::iterator j = i;
    ++j;

    for (; j != stsTracks.end(); ++j)
    {
      CbmStsTrack* negTrack = *j;
      //CbmStsTrack t2(*negTrack);
      //extFitter.DoFit(&t2, 13);
      //chi2Prim = extFitter.GetChiToVertex(&t2, fPrimVtx);
      const FairTrackParam* t2param = negTrack->GetParamLast();
      //extFitter.Extrapolate(&t2, fPrimVtx->GetZ(), &t2param);
      Double_t t2Qp = t2param->GetQp();

      if (t1Qp * t2Qp >= 0)
        continue;

      CbmKFTrack muMinus(*negTrack);
      vector<CbmKFTrackInterface*> kfData;

      if (t1Qp > 0)
      {
        kfData.push_back(&muPlus);
        kfData.push_back(&muMinus);
      }
      else
      {
        kfData.push_back(&muMinus);
        kfData.push_back(&muPlus);
      }

      //CbmKFParticle DiMu;
      //DiMu.Construct(kfData, 0);
      //DiMu.TransportToDecayVertex();
      //Double_t m, merr;
      //DiMu.GetMass(m, merr);
      //histo->Fill(m);
    }
  }
}

void LxTrackAna::FinishTask()
{
  TFile* curFile = TFile::CurrentFile();

  if (buildConnectStat)
  {
    SaveHisto(muchStsBreakX, "muchStsBreakX.root");
    SaveHisto(muchStsBreakY, "muchStsBreakY.root");
    SaveHisto(muchStsBreakTx, "muchStsBreakTx.root");
    SaveHisto(muchStsBreakTy, "muchStsBreakTy.root");

    SaveHisto(stsMuchBreakX, "stsMuchBreakX.root");
    SaveHisto(stsMuchBreakY, "stsMuchBreakY.root");

    SaveHisto(signalChi2, "signalChi2.root");
    SaveHisto(bgrChi2, "bgrChi2.root");
  }// if (buildConnectStat)

  if (buildBgrInvMass)
  {
    if (joinData)
    {
      TFile fh("tracks_tree.root");
      superEventTracks = static_cast<TTree*> (fh.Get("SuperEventTracks"));
      //LxSimpleTrack st(0, 0, 0, 0, 0, 0, 0, 0);
      //superEventTracks->SetBranchAddress("tracks", &st.px);
      CbmStsTrack* st = new CbmStsTrack;
      superEventTracks->SetBranchAddress("tracks", &st);
      list<CbmStsTrack*> stsTracks;
      Int_t nEnt = superEventTracks->GetEntries();

      for (Int_t i = 0; i < nEnt; ++i)
      {
        superEventTracks->GetEntry(i);
        //LxSimpleTrack* t = new LxSimpleTrack(0, 0, 0, 0, st.px, st.py, st.pz, st.e);
        //t->charge = st.charge;
        CbmStsTrack* t = new CbmStsTrack(*st);
        stsTracks.push_back(t);
      }

      BuildInvMass2(stsTracks, bgrInvMass);
      SaveHisto(bgrInvMass, "bgrInvMass.root");

      for (list<CbmStsTrack*>::iterator i = stsTracks.begin(); i != stsTracks.end(); ++i)
        delete *i;
    }
    else
    {
      TFile fh("tracks_tree.root", "RECREATE");
      superEventTracks->Write();
      fh.Close();
      delete superEventTracks;
    }
  }// if (buildBgrInvMass)

  if (buildSigInvMass)
    SaveHisto(sigInvMass, "sigInvMass.root");

  if (buildNearestHitDist && useHitsInStat)
  {
    char fileName[32];

    for (Int_t i = 0; i < LXSTATIONS; ++i)
    {
      sprintf(fileName, "%s.root", nearestHitDist[i]->GetName());
      SaveHisto(nearestHitDist[i], fileName);
      sprintf(fileName, "%s.root", hitsDist[i]->GetName());
      SaveHisto(hitsDist[i], fileName);
    }
  }

  SaveHisto(muPlusStsTxDiff, "muPlusStsTxDiff.root");
  SaveHisto(muMinusStsTxDiff, "muMinusStsTxDiff.root");
  SaveHisto(muPlusStsXDiff, "muPlusStsXDiff.root");
  SaveHisto(muMinusStsXDiff, "muMinusStsXDiff.root");
  SaveHisto(muPlusVertexTxDiff, "muPlusVertexTxDiff.root");
  SaveHisto(muMinusVertexTxDiff, "muMinusVertexTxDiff.root");

  SaveHisto(muPlusStsBeginTxDiff2D, "muPlusStsBeginTxDiff2D.root");
  SaveHisto(muMinusStsBeginTxDiff2D, "muMinusStsBeginTxDiff2D.root");

  SaveHisto(deltaPhiPi, "deltaPhiPi.root");

  SaveHisto(jPsiMuonsMomsHisto, "jPsiMuonsMomsHisto.root");
  SaveHisto(dtxMomProductHisto, "dtxMomProductHisto.root");

  segmentsAnalyzer.Finish();

  TFile::CurrentFile() = curFile;
  FairTask::FinishTask();
}

// Our goal here is to investigate various properties of Monte Carlo tracks derivable from points of there intersections
// with detector stations. At the same time in MUCH we use not the Monte Carlo points but hits corresponding to them.
// -- This is done to make the statistical properties more realistic.
void LxTrackAna::Exec(Option_t* opt)
{
  Clean();

  Int_t nEnt = listMCTracks->GetEntries();
  cout << "There are: " << nEnt << " of MC tracks" << endl;

  for (Int_t i = 0; i < nEnt; ++i)
  {
    CbmMCTrack* mct = static_cast<CbmMCTrack*> (listMCTracks->At(i));
    LxSimpleTrack* t = new LxSimpleTrack(mct->GetPdgCode(), mct->GetMotherId(), mct->GetP(), mct->GetPt(),
        mct->GetPx(), mct->GetPy(), mct->GetPz(), mct->GetEnergy());
    allTracks.push_back(t);
  }

  nEnt = listStsPts->GetEntries();
  cout << "There are: " << nEnt << " of STS MC points" << endl;

  for (Int_t i = 0; i < nEnt; ++i)
  {
    CbmStsPoint* stsPt = static_cast<CbmStsPoint*> (listStsPts->At(i));
    Int_t mcTrackId = stsPt->GetTrackID();
    LxSimpleTrack* track = allTracks[mcTrackId];
    TVector3 xyzI, xyzO;
    stsPt->Position(xyzI);
    stsPt->PositionOut(xyzO);
    TVector3 xyz = .5 * (xyzI + xyzO);
    TVector3 pxyzI, pxyzO;
    stsPt->Momentum(pxyzI);
    stsPt->MomentumOut(pxyzO);
    TVector3 pxyz = .5 * (pxyzI + pxyzO);
    LxSimplePoint point(xyz.X(), xyz.Y(), xyz.Z(), pxyz.X() / pxyz.Z(), pxyz.Y() / pxyz.Z());
    Int_t stationNr = CbmStsAddress::GetElementId(stsPt->GetDetectorID(), kStsStation);
    track->stsPoints[stationNr].push_back(point);
  }

  nEnt = listMuchPts->GetEntries();
  cout << "There are: " << nEnt << " of MUCH MC points" << endl;

  for (Int_t i = 0; i < nEnt; ++i)
  {
    CbmMuchPoint* mcPoint = static_cast<CbmMuchPoint*> (listMuchPts->At(i));
    Int_t mcTrackId = mcPoint->GetTrackID();
    LxSimpleTrack* track = allTracks[mcTrackId];
    Int_t stationNr = CbmMuchGeoScheme::GetStationIndex(mcPoint->GetDetectorId());
    Int_t layerNr = CbmMuchGeoScheme::GetLayerIndex(mcPoint->GetDetectorId());
    TVector3 xyzI, xyzO;
    mcPoint->Position(xyzI);
    mcPoint->PositionOut(xyzO);
    TVector3 xyz = .5 * (xyzI + xyzO);
    TVector3 pxyzI, pxyzO;
    mcPoint->Momentum(pxyzI);
    mcPoint->MomentumOut(pxyzO);
    TVector3 pxyz = .5 * (pxyzI + pxyzO);
    LxSimplePoint point(xyz.X(), xyz.Y(), xyz.Z(), pxyz.X() / pxyz.Z(), pxyz.Y() / pxyz.Z());

    if (useHitsInStat)
      track->muchMCPts[stationNr][layerNr].push_back(point);
    else
      track->muchPoints[stationNr][layerNr].push_back(point);
  }

  if (useHitsInStat)
  {
    nEnt = listMuchPixelHits->GetEntries();
    cout << "There are: " << nEnt << " of MUCH pixel hits" << endl;

    for (Int_t i = 0; i < nEnt; ++i)
    {
      CbmMuchPixelHit* mh = static_cast<CbmMuchPixelHit*> (listMuchPixelHits->At(i));

      Int_t stationNumber = CbmMuchGeoScheme::GetStationIndex(mh->GetAddress());
      Int_t layerNumber = CbmMuchGeoScheme::GetLayerIndex(mh->GetAddress());
      TVector3 pos, err;
      mh->Position(pos);
      mh->PositionError(err);
      Double_t x = pos.X();
      Double_t y = pos.Y();
      Double_t z = pos.Z();

      if (x != x || y != y || z != z)// Test for NaN
        continue;

      Int_t clusterId = mh->GetRefId();
      CbmMuchCluster* cluster = static_cast<CbmMuchCluster*> (listMuchClusters->At(clusterId));
      Int_t nDigis = cluster->GetNofDigis();

      for (Int_t j = 0; j < nDigis; ++j)
      {
        CbmMuchDigiMatch* digiMatch = static_cast<CbmMuchDigiMatch*> (listMuchPixelDigiMatches->At(cluster->GetDigi(j)));
        Int_t nMCs = digiMatch->GetNofLinks();

        for (Int_t k = 0; k < nMCs; ++k)
        {
          const CbmLink& lnk = digiMatch->GetLink(k);
          Int_t mcIndex = lnk.GetIndex();
          CbmMuchPoint* mcPoint = static_cast<CbmMuchPoint*> (listMuchPts->At(mcIndex));
          Int_t mcTrackId = mcPoint->GetTrackID();
          LxSimpleTrack* track = allTracks[mcTrackId];
          Int_t stationNr = CbmMuchGeoScheme::GetStationIndex(mcPoint->GetDetectorId());
          Int_t layerNr = CbmMuchGeoScheme::GetLayerIndex(mcPoint->GetDetectorId());
          LxSimplePoint point(x, y, z, 0, 0);
          track->muchPoints[stationNr][layerNr].push_back(point);
        }
      }
    }

    nEnt = listMuchClusters->GetEntries();
    cout << "There are: " << nEnt << " of MUCH clusters" << endl;

    nEnt = listMuchPixelDigiMatches->GetEntries();
    cout << "There are: " << nEnt << " of MUCH pixel digi matches" << endl;
  }//if (useHitsInStat)

  if (averagePoints)
    AveragePoints();

  if (buildConnectStat)
    BuildStatistics();

  //Connect(false);
  Connect(true);

  if (buildSigInvMass)
    BuildInvMass(posTracks, negTracks, sigInvMass);

  if (buildSegmentsStat)
    segmentsAnalyzer.BuildStatistics();
}

static inline void AveragePoints(list<LxSimplePoint>& points)
{
  if (points.empty() || points.size() == 1)
    return;

  Double_t x = 0;
  Double_t y = 0;
  Double_t z = 0;
  Double_t tx = 0;
  Double_t ty = 0;

  for (list<LxSimplePoint>::iterator i = points.begin(); i != points.end(); ++i)
  {
    LxSimplePoint p = *i;
    x += p.x;
    y += p.y;
    z += p.z;
    tx += p.tx;
    ty += p.ty;
  }

  x /= points.size();
  y /= points.size();
  z /= points.size();
  tx /= points.size();
  ty /= points.size();
  LxSimplePoint averagedPoint(x, y, z, tx, ty);
  points.clear();
  points.push_back(averagedPoint);
}

// If hits are used in statistics average MC points in MUCH.
static inline void AveragePoints(LxSimpleTrack* track, bool useHitsInStat)
{
  for (Int_t i = 0; i < LXSTSSTATIONS; ++i)
    AveragePoints(track->stsPoints[i]);

  if (useHitsInStat)
  {
    for (Int_t i = 0; i < LXSTATIONS; ++i)
    {
      for (Int_t j = 0; j < LXLAYERS; ++j)
        AveragePoints(track->muchMCPts[i][j]);
    }
  }
  else
  {
    for (Int_t i = 0; i < LXSTATIONS; ++i)
    {
      for (Int_t j = 0; j < LXLAYERS; ++j)
        AveragePoints(track->muchPoints[i][j]);
    }
  }
}

void LxTrackAna::AveragePoints()
{
  for (vector<LxSimpleTrack*>::iterator i = allTracks.begin(); i != allTracks.end(); ++i)
    ::AveragePoints(*i, useHitsInStat);
}

static Int_t maxTracks = 0;
static Int_t maxMuchPts1 = 0;
static Int_t maxMuchPts0 = 0;
static Int_t maxStsPts7 = 0;
static Int_t maxStsPts6 = 0;

static inline void BuildStatistics(LxSimpleTrack* track)
{
  if (track->muchPoints[1][LXMIDDLE].size() > maxMuchPts1)
    maxMuchPts1 = track->muchPoints[1][LXMIDDLE].size();

  if (track->muchPoints[0][LXMIDDLE].size() > maxMuchPts0)
    maxMuchPts0 = track->muchPoints[0][LXMIDDLE].size();

  if (track->stsPoints[7].size() > maxStsPts7)
    maxStsPts7 = track->stsPoints[7].size();

  if (track->stsPoints[6].size() > maxStsPts6)
    maxStsPts6 = track->stsPoints[6].size();

  jPsiMuonsMomsHisto->Fill(track->p);

  for (list<LxSimplePoint>::iterator j = track->muchPoints[1][LXMIDDLE].begin(); j != track->muchPoints[1][LXMIDDLE].end(); ++j)
  {
    LxSimplePoint muchPt1 = *j;

    for (list<LxSimplePoint>::iterator k = track->muchPoints[0][LXMIDDLE].begin(); k != track->muchPoints[0][LXMIDDLE].end(); ++k)
    {
      LxSimplePoint muchPt0 = *k;
      Double_t diffZMuch = muchPt0.z - muchPt1.z;
      Double_t txMuch = (muchPt0.x - muchPt1.x) / diffZMuch;
      Double_t tyMuch = (muchPt0.y - muchPt1.y) / diffZMuch;

      Double_t diffZMag = magnetCenterZ - muchPt0.z;
      Double_t magX = muchPt0.x + txMuch * diffZMag;
      Double_t magTx = magX / magnetCenterZ;
      Double_t diffMagTx = txMuch - magTx;

      if (-13 == track->pdgCode)
        dtxMomProductHisto->Fill(diffMagTx * track->p);
      else if (13 == track->pdgCode)
        dtxMomProductHisto->Fill(-diffMagTx * track->p);

      if (-13 == track->pdgCode)
      {
        Double_t txDiff = txMuch - muchPt0.x / muchPt0.z;
        muPlusVertexTxDiff->Fill(txDiff);

        if (!track->stsPoints[0].empty() && !track->stsPoints[1].empty())
        {
          LxSimplePoint p0 = track->stsPoints[0].front();
          LxSimplePoint p1 = track->stsPoints[1].front();
          muPlusStsBeginTxDiff2D->Fill(track->p, txMuch - (p1.x - p0.x) / (p1.z - p0.z));
          muMinusStsBeginTxDiff2D->Fill(track->p, (p1.x - p0.x) / (p1.z - p0.z) - txMuch);
          deltaPhiPi->Fill(track->p * (txMuch - (p1.x - p0.x) / (p1.z - p0.z)));
        }
      }
      else if (13 == track->pdgCode)
      {
        Double_t txDiff = txMuch - muchPt0.x / muchPt0.z;
        muMinusVertexTxDiff->Fill(txDiff);

        if (!track->stsPoints[0].empty() && !track->stsPoints[1].empty())
        {
          LxSimplePoint p0 = track->stsPoints[0].front();
          LxSimplePoint p1 = track->stsPoints[1].front();
          muMinusStsBeginTxDiff2D->Fill(track->p, txMuch - (p1.x - p0.x) / (p1.z - p0.z));
          muPlusStsBeginTxDiff2D->Fill(track->p, (p1.x - p0.x) / (p1.z - p0.z) - txMuch);
          deltaPhiPi->Fill(track->p * ((p1.x - p0.x) / (p1.z - p0.z) - txMuch));
        }
      }

      for (list<LxSimplePoint>::iterator l = track->stsPoints[7].begin(); l != track->stsPoints[7].end(); ++l)
      {
        LxSimplePoint stsPt7 = *l;
        Double_t diffZ = stsPt7.z - muchPt0.z;
        Double_t extX = muchPt0.x + txMuch * diffZ;
        Double_t extY = muchPt0.y + tyMuch * diffZ;
        Double_t dx = stsPt7.x - extX;
        Double_t dy = stsPt7.y - extY;
        muchStsBreakX->Fill(dx);
        muchStsBreakY->Fill(dy);

        if (-13 == track->pdgCode)
        {
          Double_t extXmu = muchPt0.x + (txMuch - 0.00671) * diffZ;
          muPlusStsXDiff->Fill(stsPt7.x - extXmu);
        }
        else if (13 == track->pdgCode)
        {
          Double_t extXmu = muchPt0.x + (txMuch + 0.00691) * diffZ;
          muMinusStsXDiff->Fill(stsPt7.x - extXmu);
        }

        for (list<LxSimplePoint>::iterator m = track->stsPoints[6].begin(); m != track->stsPoints[6].end(); ++m)
        {
          LxSimplePoint stsPt6 = *m;
          Double_t diffZSts = stsPt6.z - stsPt7.z;
          Double_t txSts = (stsPt6.x - stsPt7.x) / diffZSts;
          Double_t tySts = (stsPt6.y - stsPt7.y) / diffZSts;
          //muchStsBreakX->Fill(dx);
          //muchStsBreakY->Fill(dy);
          Double_t dtx = txSts - txMuch;
          Double_t dty = tySts - tyMuch;
          muchStsBreakTx->Fill(dtx);
          muchStsBreakTy->Fill(dty);

          if (-13 == track->pdgCode)
            muPlusStsTxDiff->Fill(dtx);
          else if (13 == track->pdgCode)
            muMinusStsTxDiff->Fill(dtx);

          Double_t chi2 = dx * dx / xRms2 + dy * dy / yRms2 + dtx * dtx / txRms2 + dty * dty / tyRms2;

          if (0 > track->motherId && (13 == track->pdgCode || -13 == track->pdgCode))// JPsi
            signalChi2->Fill(chi2);
          else
            bgrChi2->Fill(chi2);

          diffZ = muchPt0.z - stsPt7.z;
          extX = stsPt7.x + txSts * diffZ;
          extY = stsPt7.y + tySts * diffZ;
          dx = muchPt0.x - extX;
          dy = muchPt0.y - extY;
          stsMuchBreakX->Fill(dx);
          stsMuchBreakY->Fill(dy);
        }
      }
    }
  }
}

static inline void BuildNearestHitStat(LxSimpleTrack* track, bool cropHits)
{
  static Double_t maxDist = 0;
  static Double_t mcX = 0;
  static Double_t hitX = 0;
  static Double_t mcY = 0;
  static Double_t hitY = 0;

  static Double_t maxMinDist = 0;
  static Double_t mcMinX = 0;
  static Double_t hitMinX = 0;
  static Double_t mcMinY = 0;
  static Double_t hitMinY = 0;
  static Int_t nMinHits = 0;

  for (Int_t i = 0; i < LXSTATIONS; ++i)
  {
    if (track->muchMCPts[i][LXMIDDLE].empty() || track->muchPoints[i][LXMIDDLE].empty())
      continue;

    LxSimplePoint mcPoint = track->muchMCPts[i][LXMIDDLE].front();// We assume the MC points were averaged and there can be at most one point.
    Double_t minDist = -1;
    Double_t mcMinX0 = 0;
    Double_t hitMinX0 = 0;
    Double_t mcMinY0 = 0;
    Double_t hitMinY0 = 0;
    Double_t hitMinZ0 = 0;
    Int_t nMinHits0 = 0;

    for (list<LxSimplePoint>::iterator j = track->muchPoints[i][LXMIDDLE].begin(); j != track->muchPoints[i][LXMIDDLE].end(); ++j)
    {
      LxSimplePoint hitPoint = *j;
      Double_t dx = hitPoint.x - mcPoint.x;
      Double_t dy = hitPoint.y - mcPoint.y;
      Double_t dist = sqrt(dx * dx + dy * dy);

      if (track->motherId < 0 && (13 == track->pdgCode || -13 == track->pdgCode))
        hitsDist[i]->Fill(dist);

      if (minDist > dist || minDist < 0)
      {
        minDist = dist;

        mcMinX0 = mcPoint.x;
        hitMinX0 = hitPoint.x;
        mcMinY0 = mcPoint.y;
        hitMinY0 = hitPoint.y;
        hitMinZ0 = hitPoint.z;
        nMinHits0 = track->muchPoints[i][LXMIDDLE].size();
      }

      if (maxDist < dist)
      {
        maxDist = dist;
        mcX = mcPoint.x;
        hitX = hitPoint.x;
        mcY = mcPoint.y;
        hitY = hitPoint.y;
      }
    }// for (list<LxSimplePoint>::iterator j = track->muchPoints[i].begin(); j != track->muchPoints[i].end(); ++j)

    if (minDist >= 0)
    {
      if (track->motherId < 0 && (13 == track->pdgCode || -13 == track->pdgCode))
        nearestHitDist[i]->Fill(minDist);

      if (cropHits)
      {
        track->muchPoints[i][LXMIDDLE].clear();
        LxSimplePoint point(hitMinX0, hitMinY0, hitMinZ0, 0, 0);
        track->muchPoints[i][LXMIDDLE].push_back(point);
      }

      if (maxMinDist < minDist)
      {
        maxMinDist = minDist;
        mcMinX = mcMinX0;
        hitMinX = hitMinX0;
        mcMinY = mcMinY0;
        hitMinY = hitMinY0;
        nMinHits = nMinHits0;
      }
    }
  }

  cout << "BuildNearestHitStat: maxDist=" << maxDist << " MC x=" << mcX << " Hit x=" << hitX << " MC y=" <<
      mcY << " Hit y=" << hitY << endl;
  cout << "BuildNearestHitStat: maxMinDist=" << maxMinDist << " MC x=" << mcMinX << " Hit x=" << hitMinX <<
      " MC y=" << mcMinY << " Hit y=" << hitMinY << " n hits=" << nMinHits << endl;
}

void LxTrackAna::BuildStatistics()
{
  if (allTracks.size() > maxTracks)
    maxTracks = allTracks.size();

  for (vector<LxSimpleTrack*>::iterator i = allTracks.begin(); i != allTracks.end(); ++i)
  {
    LxSimpleTrack* track = *i;

    if (track->muchPoints[0][LXMIDDLE].empty() ||  track->muchPoints[1][LXMIDDLE].empty() || track->muchPoints[5][LXMIDDLE].empty())
      continue;

    if (buildNearestHitDist && useHitsInStat)
      ::BuildNearestHitStat(track, cropHits);// Hits can be cropped (only the nearest to MC hit is left) here!

    if (track->motherId > -1 || (13 != track->pdgCode && -13 != track->pdgCode))
      continue;

    ::BuildStatistics(track);
  }

  cout << "Statistics is built maxTracks=" << maxTracks << " maxMuchPts1=" << maxMuchPts1 << " maxMuchPts0=" << maxMuchPts0
      << " maxStsPts7=" << maxStsPts7 << " maxStsPts6=" << maxStsPts6 << endl;
}

void LxTrackAna::Connect(bool useCuts)
{
  static Int_t jpsiTrackCount = 0;
  static Int_t jpsiTrackCountCutted = 0;
  static Int_t jpsiMatchedCount = 0;
  static Int_t jpsiMatchedCountCutted = 0;

  static Int_t otherTrackCount = 0;
  static Int_t otherTrackCountCutted = 0;
  static Int_t otherMatchedCount = 0;
  static Int_t otherMatchedCountCutted = 0;

  for (vector<LxSimpleTrack*>::iterator i = allTracks.begin(); i != allTracks.end(); ++i)
  {
    LxSimpleTrack* muchTrack = *i;

    if (muchTrack->muchPoints[0][LXMIDDLE].empty() || muchTrack->muchPoints[1][LXMIDDLE].empty() || muchTrack->muchPoints[5][LXMIDDLE].empty())
      continue;

    for (list<LxSimplePoint>::iterator j = muchTrack->muchPoints[1][LXMIDDLE].begin(); j != muchTrack->muchPoints[1][LXMIDDLE].end(); ++j)
    {
      LxSimplePoint muchPt1 = *j;

      for (list<LxSimplePoint>::iterator k = muchTrack->muchPoints[0][LXMIDDLE].begin(); k != muchTrack->muchPoints[0][LXMIDDLE].end(); ++k)
      {
        LxSimplePoint muchPt0 = *k;
        Double_t diffZMuch = muchPt0.z - muchPt1.z;
        Double_t txMuch = (muchPt0.x - muchPt1.x) / diffZMuch;
        Double_t txMuchVertex = muchPt0.x / muchPt0.z;
        Double_t tyMuch = (muchPt0.y - muchPt1.y) / diffZMuch;
        Connect(muchTrack, muchPt0, txMuch, tyMuch, useCuts);
      }// for (list<LxSimplePoint>::iterator k = muchTrack->muchPoints[0].begin(); k != muchTrack->muchPoints[0].end(); ++k)
    }// for (list<LxSimplePoint>::iterator j = muchTrack->muchPoints[1].begin(); j != muchTrack->muchPoints[1].end(); ++j)

    if (0 > muchTrack->motherId && (13 == muchTrack->pdgCode || -13 == muchTrack->pdgCode))// JPsi
    {
      if (useCuts)
      {
        ++jpsiTrackCountCutted;

        if (muchTrack == muchTrack->linkedStsTrack)
          ++jpsiMatchedCountCutted;

        if (buildSigInvMass && 0 != muchTrack->linkedStsTrack)
        {
          if (muchTrack->linkedStsTrack->charge > 0)
            posTracks.push_back(muchTrack->linkedStsTrack);
          else if (muchTrack->linkedStsTrack->charge < 0)
            negTracks.push_back(muchTrack->linkedStsTrack);
        }
      }
      else
      {
        ++jpsiTrackCount;

        if (muchTrack == muchTrack->linkedStsTrack)
          ++jpsiMatchedCount;
      }
    }
    else// Background track handled here.
    {
      if (useCuts)
      {
        ++otherTrackCountCutted;

        if (muchTrack == muchTrack->linkedStsTrack || 0 == muchTrack->linkedStsTrack)
          ++otherMatchedCountCutted;

        if (buildBgrInvMass && !joinData && 0 != muchTrack->linkedStsTrack)
        {
          superEventBrachTrack.px = muchTrack->linkedStsTrack->px;
          superEventBrachTrack.py = muchTrack->linkedStsTrack->py;
          superEventBrachTrack.pz = muchTrack->linkedStsTrack->pz;
          superEventBrachTrack.e = muchTrack->linkedStsTrack->e;
          superEventBrachTrack.charge = muchTrack->linkedStsTrack->charge;
          superEventTracks->Fill();
        }
      }
      else
      {
        ++otherTrackCount;

        if (muchTrack == muchTrack->linkedStsTrack || 0 == muchTrack->linkedStsTrack)
          ++otherMatchedCount;
      }
    }
  }// for (vector<LxSimpleTrack*>::iterator i = allTracks.begin(); i != allTracks.end(); ++i)

  if (useCuts)
  {
    Double_t efficiency = jpsiMatchedCountCutted * 100;
    efficiency /= jpsiTrackCountCutted;
    cout << "J/psi (with cuts) connection efficiency = " << efficiency << "% ( " << jpsiMatchedCountCutted << " / " << jpsiTrackCountCutted << " )" << endl;
    efficiency = otherMatchedCountCutted * 100;
    efficiency /= otherTrackCountCutted;
    cout << "Others (with cuts) connection efficiency = " << efficiency << "% ( " << otherMatchedCountCutted << " / " << otherTrackCountCutted << " )" << endl;
  }
  else
  {
    Double_t efficiency = jpsiMatchedCount * 100;
    efficiency /= jpsiTrackCount;
    cout << "J/psi (without cuts) connection efficiency = " << efficiency << "% ( " << jpsiMatchedCount << " / " << jpsiTrackCount << " )" << endl;
    efficiency = otherMatchedCount * 100;
    efficiency /= otherTrackCount;
    cout << "Others (without cuts) connection efficiency = " << efficiency << "% ( " << otherMatchedCount << " / " << otherTrackCount << " )" << endl;
  }
}

void LxTrackAna::Connect(LxSimpleTrack* muchTrack, LxSimplePoint muchPt0, Double_t txMuch, Double_t tyMuch,
    bool useCuts)
{
  for (vector<LxSimpleTrack*>::iterator l = allTracks.begin(); l != allTracks.end(); ++l)
  {
    LxSimpleTrack* stsTrack = *l;

    if (stsTrack->p < 3.0 || stsTrack->pt < 1.0)
      continue;

    Int_t m0 = 0;
    Int_t n0 = -1;

    for (; m0 < LXSTSSTATIONS - 1; ++m0)
    {
      if (!stsTrack->stsPoints[m0].empty())
      {
        n0 = m0 + 1;

        for (; n0 <= m0 + 2 && n0 < LXSTSSTATIONS; ++n0)
        {
          if (!stsTrack->stsPoints[n0].empty())
            break;
        }
      }

      if (n0 <= m0 + 2)
        break;
    }

    Int_t m = LXSTSSTATIONS - 1;
    Int_t n = -1;

    for (; m > 0; --m)
    {
      if (!stsTrack->stsPoints[m].empty())
      {
        n = m - 1;

        for (; n >= m - 2 && n >= 0; --n)
        {
          if (!stsTrack->stsPoints[n].empty())
            break;
        }
      }

      if (n >= m - 2)
        break;
    }

    if (n >= 0 && m > n && n >= m - 2)
    {
      if (m0 >= LXSTSSTATIONS - 1 || n0 >= LXSTSSTATIONS || m0 >= n0)
      {
        m0 = n;
        n0 = m;
      }

      LxSimplePoint stsPtM0 = stsTrack->stsPoints[m0].front();
      LxSimplePoint stsPtN0 = stsTrack->stsPoints[n0].front();
      Double_t txSts0 = (stsPtN0.x - stsPtM0.x) / (stsPtN0.z - stsPtM0.z);

      for (list<LxSimplePoint>::iterator o = stsTrack->stsPoints[m].begin(); o != stsTrack->stsPoints[m].end(); ++o)
      {
        LxSimplePoint stsPtM = *o;
        Double_t deltaZ = stsPtM.z - muchPt0.z;
        Double_t extX = muchPt0.x + txMuch * deltaZ;
        Double_t extY = muchPt0.y + tyMuch * deltaZ;
        Double_t dx = stsPtM.x - extX;
        Double_t dy = stsPtM.y - extY;

        if (dx < 0)
          dx = -dx;

        if (dy < 0)
          dy = -dy;

        if (useCuts && (dx > xRms * cutCoeff || dy > yRms * cutCoeff))
          continue;

        for (list<LxSimplePoint>::iterator p = stsTrack->stsPoints[n].begin(); p != stsTrack->stsPoints[n].end(); ++p)
        {
          LxSimplePoint stsPtN = *p;
          Double_t diffZSts = stsPtN.z - stsPtM.z;
          Double_t txSts = (stsPtN.x - stsPtM.x) / diffZSts;
          Double_t tySts = (stsPtN.y - stsPtM.y) / diffZSts;
          Double_t dtx = txSts - txMuch;
          Double_t dty = tySts - tyMuch;

          Double_t stsCharge = TDatabasePDG::Instance()->GetParticle(stsTrack->pdgCode)->Charge();
          Double_t muchCharge = txMuch - txSts0;
          bool chargesSignsTheSame = (stsCharge > 0 && muchCharge > 0) || (stsCharge < 0 && muchCharge < 0);

          if (dtx < 0)
            dtx = -dtx;

          if (dty < 0)
            dty = -dty;

          if (useCuts && ((useChargeSignInCuts && (!chargesSignsTheSame || !momFitTxBreak(stsTrack->p, muchCharge))) || dtx > txRms * cutCoeff || dty > tyRms * cutCoeff))
            continue;

          Double_t chi2 = dx * dx / xRms2 + dy * dy / yRms2 + dtx * dtx / txRms2 + dty * dty / tyRms2;
          stsTrack->charge = stsCharge;

          if (0 == stsTrack->linkedMuchTrack.first || chi2 < stsTrack->linkedMuchTrack.second)
          {
            list<pair<LxSimpleTrack*, Double_t> >::iterator r = muchTrack->linkedStsTracks.begin();

            for (; r != muchTrack->linkedStsTracks.end() && r->second <= chi2; ++r)
              ;

            pair<LxSimpleTrack*, Double_t> trackDesc(stsTrack, chi2);
            muchTrack->linkedStsTracks.insert(r, trackDesc);
          }
        }
      }
    }
  }// for (vector<LxSimpleTrack*>::iterator l = allTracks.begin(); l != allTracks.end(); ++l)

  if (!muchTrack->linkedStsTracks.empty())
  {
    pair<LxSimpleTrack*, Double_t> trackDesc = muchTrack->linkedStsTracks.front();
    muchTrack->linkedStsTracks.pop_front();
    LxSimpleTrack* anotherMuchTrack = trackDesc.first->linkedMuchTrack.first;
    trackDesc.first->linkedMuchTrack.first = muchTrack;
    trackDesc.first->linkedMuchTrack.second = trackDesc.second;
    muchTrack->linkedStsTrack = trackDesc.first;

    if (0 != anotherMuchTrack)
      anotherMuchTrack->RebindMuchTrack();
  }
}
