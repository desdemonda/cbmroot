//-----------------------------------------------------------
//-----------------------------------------------------------

// Cbm Headers ----------------------
#include "CbmKFParticleFinderPID.h"
#include "CbmKFParticleFinder.h"
#include "CbmStsTrack.h"
#include "FairRunAna.h"
#include "CbmL1PFFitter.h"
#include "L1Field.h"
#include "CbmKFVertex.h"

//KF Particle headers
#include "KFParticleTopoReconstructor.h"
#include "KFPTrackVector.h"

//ROOT headers
#include "TClonesArray.h" //to get arrays from the FairRootManager
#include "TStopwatch.h" //to measure the time
#include "TMath.h" //to calculate Prob function

//c++ and std headers
#include <iostream>
#include <cmath>
#include <vector>
using std::vector;

CbmKFParticleFinder::CbmKFParticleFinder(const char* name, Int_t iVerbose):
  FairTask(name, iVerbose), fStsTrackBranchName("StsTrack"), fTrackArray(0), fPrimVtx(0), fTopoReconstructor(0), fPVFindMode(1), fPID(0)
{
  fTopoReconstructor = new KFParticleTopoReconstructor;
  
  // set default cuts
  SetPrimaryProbCut(0.0001); // 0.01% to consider primary track as a secondary;
}

CbmKFParticleFinder::~CbmKFParticleFinder()
{
  if(fTopoReconstructor) delete fTopoReconstructor;
}

InitStatus CbmKFParticleFinder::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
  {
    Error("CbmKFParticleFinder::Init","RootManager not instantiated!");
    return kERROR;
  }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fStsTrackBranchName);
  if(fTrackArray==0)
  {
    Error("CbmKFParticleFinder::Init","track-array not found!");
    return kERROR;
  }
  
  fPrimVtx = (CbmVertex*) ioman->GetObject("PrimaryVertex");

  return kSUCCESS;
}

void CbmKFParticleFinder::Exec(Option_t* opt)
{
  Int_t ntracks=0;//fTrackArray->GetEntriesFast();

  vector<CbmStsTrack> vRTracks(fTrackArray->GetEntriesFast());
  vector<int> pdg(fTrackArray->GetEntriesFast(), -1);
  vector<int> trackId(fTrackArray->GetEntriesFast(), -1);
    
  for(int iTr=0; iTr<fTrackArray->GetEntriesFast(); iTr++)
  {
    CbmStsTrack* stsTrack = ( (CbmStsTrack*) fTrackArray->At(iTr));
    const FairTrackParam* parameters = stsTrack->GetParamFirst();
    
    Double_t V[15] = {0.f}; 
    for (Int_t i=0,iCov=0; i<5; i++) 
      for (Int_t j=0; j<=i; j++,iCov++)
        V[iCov] = parameters->GetCovariance(i,j);
  
    bool ok = 1;
    ok = ok && finite(parameters->GetX());
    ok = ok && finite(parameters->GetY());
    ok = ok && finite(parameters->GetZ());
    ok = ok && finite(parameters->GetTx());
    ok = ok && finite(parameters->GetTy());
    ok = ok && finite(parameters->GetQp());
    
    for(unsigned short iC=0; iC<15; iC++)
      ok = ok && finite(V[iC]);
    ok = ok && (V[0] < 1. && V[0] > 0.)
            && (V[2] < 1. && V[2] > 0.)
            && (V[5] < 1. && V[5] > 0.)
            && (V[9] < 1. && V[9] > 0.)
            && (V[14] < 1. && V[14] > 0.);
    ok = ok && stsTrack->GetChiSq() < 10*stsTrack->GetNDF();
    if(!ok) continue;
    
    if(fPID)
    {
      if(fPID->GetPID()[iTr] == -2) continue; 
      pdg[ntracks] = fPID->GetPID()[iTr];
    }
    vRTracks[ntracks] = *stsTrack;
    trackId[ntracks] = iTr;
    
    ntracks++;
  }
  
  vRTracks.resize(ntracks);
  pdg.resize(ntracks);
  trackId.resize(ntracks);

  CbmL1PFFitter fitter;
  vector<float> vChiToPrimVtx;
  CbmKFVertex kfVertex;
  if(fPrimVtx)
    kfVertex = CbmKFVertex(*fPrimVtx);
  
  vector<L1FieldRegion> vField;
  fitter.Fit(vRTracks, pdg);
  fitter.GetChiToVertex(vRTracks, vField, vChiToPrimVtx, kfVertex, -3);
  
  KFPTrackVector tracks;
  tracks.Resize(ntracks);
  //fill vector with tracks
  for(Int_t iTr=0; iTr<ntracks; iTr++)
  {
    const FairTrackParam* parameters = vRTracks[iTr].GetParamFirst();
    float par[6] = {0.f};
    
    Double_t V[15] = {0.f}; 
    
    for (Int_t i=0,iCov=0; i<5; i++) 
      for (Int_t j=0; j<=i; j++,iCov++)
        V[iCov] = parameters->GetCovariance(i,j);
    
    float a = parameters->GetTx(), b = parameters->GetTy(), qp = parameters->GetQp();
  
    float c2 = 1.f/(1.f + a*a + b*b);
    float pq = 1.f/qp;
    float p2 = pq*pq;
    float pz = sqrt(p2*c2);
    float px = a*pz;
    float py = b*pz;
      
    float H[3] = { -px*c2, -py*c2, -pz*pq };

    par[0] = parameters->GetX();
    par[1] = parameters->GetY();
    par[2] = parameters->GetZ();
    par[3] = px;
    par[4] = py;
    par[5] = pz;

    float cxpz = H[0]*V[ 3] + H[1]*V[ 6] + H[2]*V[10];
    float cypz = H[0]*V[ 4] + H[1]*V[ 7] + H[2]*V[11];
    float capz = H[0]*V[ 5] + H[1]*V[ 8] + H[2]*V[12];
    float cbpz = H[0]*V[ 8] + H[1]*V[ 9] + H[2]*V[13];
    float cpzpz = H[0]*H[0]*V[5] +H[1]*H[1]*V[9] + H[2]*H[2]*V[14] 
      + 2*( H[0]*H[1]*V[8] +H[0]*H[2]*V[12] +H[1]*H[2]*V[13]);

    float cov[21];
    cov[ 0] = V[0];
    cov[ 1] = V[1];
    cov[ 2] = V[2];
    cov[ 3] = 0.f;
    cov[ 4] = 0.f;
    cov[ 5] = 0.f;
    cov[ 6] = V[3]*pz + a*cxpz; 
    cov[ 7] = V[4]*pz + a*cypz; 
    cov[ 8] = 0.f;
    cov[ 9] = V[5]*pz*pz + 2.f*a*pz*capz + a*a*cpzpz;
    cov[10] = V[6]*pz+b*cxpz; 
    cov[11] = V[7]*pz+b*cypz; 
    cov[12] = 0.f;
    cov[13] = V[8]*pz*pz + a*pz*cbpz + b*pz*capz + a*b*cpzpz;
    cov[14] = V[9]*pz*pz + 2.f*b*pz*cbpz + b*b*cpzpz;
    cov[15] = cxpz; 
    cov[16] = cypz; 
    cov[17] = 0.f;
    cov[18] = capz*pz + a*cpzpz;
    cov[19] = cbpz*pz + b*cpzpz;
    cov[20] = cpzpz;
  
    Int_t q = 0;
    if(qp>0.f)
      q = 1;
    if(qp<0.f)
      q=-1;
    
    float field[10];
    int entrSIMD = iTr % fvecLen;
    int entrVec  = iTr / fvecLen;
    field[0] = vField[entrVec].cx0[entrSIMD];
    field[1] = vField[entrVec].cx1[entrSIMD];
    field[2] = vField[entrVec].cx2[entrSIMD];
    field[3] = vField[entrVec].cy0[entrSIMD];
    field[4] = vField[entrVec].cy1[entrSIMD];
    field[5] = vField[entrVec].cy2[entrSIMD];
    field[6] = vField[entrVec].cz0[entrSIMD];
    field[7] = vField[entrVec].cz1[entrSIMD];
    field[8] = vField[entrVec].cz2[entrSIMD];
    field[9] = vField[entrVec].z0[entrSIMD];
    
    for(Int_t iP=0; iP<6; iP++)
      tracks.SetParameter(par[iP], iP, iTr);
    for(Int_t iC=0; iC<21; iC++)
      tracks.SetCovariance(cov[iC], iC, iTr);
    for(Int_t iF=0; iF<10; iF++)
      tracks.SetFieldCoefficient(field[iF], iF, iTr);
    tracks.SetId(trackId[iTr], iTr);
    tracks.SetPDG(pdg[iTr], iTr);
    tracks.SetQ(q, iTr);

    if(fPVFindMode == 0)
    {    
      if(vChiToPrimVtx[iTr] < 3)
        tracks.SetPVIndex(0, iTr);
      else
        tracks.SetPVIndex(-1, iTr);
    }
    else
      tracks.SetPVIndex(-1, iTr);
  }
    
  TStopwatch timer;
  timer.Start();
  
  fTopoReconstructor->Init(tracks);
  if(fPVFindMode == 0)
  {
    KFPVertex primVtx_tmp;
    primVtx_tmp.SetXYZ(0,0,0);
    primVtx_tmp.SetCovarianceMatrix( 0,0,0,0,0,0 );
    primVtx_tmp.SetNContributors( 0 );
    primVtx_tmp.SetChi2( -100 );

    vector<int> pvTrackIds;
    KFVertex pv(primVtx_tmp);
    fTopoReconstructor->AddPV(pv, pvTrackIds);
  }
  else if(fPVFindMode == 1)
    fTopoReconstructor->ReconstructPrimVertex();
  else if(fPVFindMode == 2)
    fTopoReconstructor->ReconstructPrimVertex(0);
  
//   {
//     KFPVertex primVtx_tmp;
//     primVtx_tmp.SetXYZ(kfVertex.GetRefX(), kfVertex.GetRefY(), kfVertex.GetRefZ());
//     primVtx_tmp.SetCovarianceMatrix( kfVertex.GetCovMatrix()[0], kfVertex.GetCovMatrix()[1], 
//                                      kfVertex.GetCovMatrix()[2], kfVertex.GetCovMatrix()[3], 
//                                      kfVertex.GetCovMatrix()[4], kfVertex.GetCovMatrix()[5] );
//     primVtx_tmp.SetNContributors( kfVertex.GetRefNTracks() );
//     primVtx_tmp.SetChi2( kfVertex.GetRefChi2() );
// 
//     vector<short int> pvTrackIds;
//     KFVertex pv(primVtx_tmp);
//     fTopoReconstructor->AddPV(pv, pvTrackIds);
//   }

  fTopoReconstructor->SortTracks();
  fTopoReconstructor->ReconstructParticles();
  
  timer.Stop();
  fTopoReconstructor->SetTime(timer.RealTime());
}

double CbmKFParticleFinder::InversedChi2Prob(double p, int ndf) const
{
  double epsilon = 1.e-14;
  double chi2Left = 0.f;
  double chi2Right = 10000.f;
  
  double probLeft = p - TMath::Prob(chi2Left, ndf);
  
  double chi2Centr = (chi2Left+chi2Right)/2.f;
  double probCentr = p - TMath::Prob( chi2Centr, ndf);
  
  while( TMath::Abs(chi2Right-chi2Centr)/chi2Centr > epsilon )
  {
    if(probCentr * probLeft > 0.f)
    {
      chi2Left = chi2Centr;
      probLeft = probCentr;
    }
    else
    {
      chi2Right = chi2Centr;
    }
    
    chi2Centr = (chi2Left+chi2Right)/2.f;
    probCentr = p - TMath::Prob( chi2Centr, ndf);
  }
  
  return chi2Centr;
}

void CbmKFParticleFinder::SetPrimaryProbCut(float prob)
{ 
  fTopoReconstructor->SetChi2PrimaryCut( InversedChi2Prob(prob, 2) );
}

ClassImp(CbmKFParticleFinder);
