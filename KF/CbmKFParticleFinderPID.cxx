//-----------------------------------------------------------
//-----------------------------------------------------------

// Cbm Headers ----------------------
#include "CbmKFParticleFinderPID.h"
#include "CbmStsTrack.h"
#include "CbmMCTrack.h"
#include "CbmTrackMatchNew.h"
#include "CbmTofHit.h"
#include "CbmGlobalTrack.h"
#include "CbmTrdTrack.h"
#include "CbmRichRing.h"

#include "FairRunAna.h"

//ROOT headers
#include "TClonesArray.h"

//c++ and std headers
#include <iostream>

CbmKFParticleFinderPID::CbmKFParticleFinderPID(const char* name, Int_t iVerbose):
  FairTask(name, iVerbose), fStsTrackBranchName("StsTrack"), fGlobalTrackBranchName("GlobalTrack"), 
  fTofBranchName("TofHit"), fMCTracksBranchName("MCTrack"), fTrackMatchBranchName("StsTrackMatch"), fTrdBranchName ("TrdTrack"), fRichBranchName ("RichRing"),
  fTrackArray(0), fGlobalTrackArray(0), fTofHitArray(0), fMCTrackArray(0), fTrackMatchArray(0), fTrdTrackArray(0), fRichRingArray(0),
  fPIDMode(0), fSisMode(0), fTrdPIDMode(0), fRichPIDMode(0), fPID(0)
{
}

CbmKFParticleFinderPID::~CbmKFParticleFinderPID()
{
}

InitStatus CbmKFParticleFinderPID::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
  {
    Error("CbmKFParticleFinderPID::Init","RootManager not instantiated!");
    return kERROR;
  }
  
  // Get sts tracks
  fTrackArray=(TClonesArray*) ioman->GetObject(fStsTrackBranchName);
  if(fTrackArray==0)
  {
    Error("CbmKFParticleFinderPID::Init","track-array not found!");
    return kERROR;
  }
  
  if(fPIDMode==2)
  {
    // Get global tracks
    fGlobalTrackArray=(TClonesArray*) ioman->GetObject(fGlobalTrackBranchName);
    if(fGlobalTrackArray==0)
    {
      Error("CbmKFParticleFinderPID::Init","global track array not found!");
      return kERROR;
    }
  
    // Get ToF hits
    fTofHitArray=(TClonesArray*) ioman->GetObject(fTofBranchName);
    if(fTofHitArray==0)
    {
      Error("CbmKFParticleFinderPID::Init","track-array not found!");
      return kERROR;
    }
    
    if(fTrdPIDMode > 0)
    {
      fTrdTrackArray = (TClonesArray*) ioman->GetObject(fTrdBranchName);
      if(fTrdTrackArray==0)
      {
        Error("CbmKFParticleFinderPID::Init","TRD track-array not found!");
        return kERROR;
      }
    }
    
    if(fRichPIDMode>0)
    {  
      fRichRingArray = (TClonesArray*) ioman->GetObject(fRichBranchName);
      if(fRichRingArray == 0)
      {
        Error("CbmKFParticleFinderPID::Init","Rich ring array not found!");
        return kERROR;
      }
    }
  }
  
  if(fPIDMode==1)
  {
    //MC Tracks
    fMCTrackArray=(TClonesArray*) ioman->GetObject(fMCTracksBranchName);
    if(fMCTrackArray==0)
    {
      Error("CbmKFParticleFinderPID::Init","mc track array not found!");
      return kERROR;
    }
  
    //Track match
    fTrackMatchArray=(TClonesArray*) ioman->GetObject(fTrackMatchBranchName);
    if(fTrackMatchArray==0)
    {
      Error("CbmKFParticleFinderPID::Init","track match array not found!");
      return kERROR;
    }
  }
  
  return kSUCCESS;
}

void CbmKFParticleFinderPID::Exec(Option_t* opt)
{ 
  fPID.clear();
  
  Int_t nTracks=fTrackArray->GetEntriesFast();
  fPID.resize(nTracks, -1);

  if(fPIDMode==1)
    SetMCPID();
  if(fPIDMode==2)
    SetRecoPID(); 
}

void CbmKFParticleFinderPID::Finish()
{
}

void CbmKFParticleFinderPID::SetMCPID()
{
  Int_t nMCTracks = fMCTrackArray->GetEntriesFast();
  Int_t ntrackMatches = fTrackMatchArray->GetEntriesFast();
  for(int iTr=0; iTr<ntrackMatches; iTr++)
  {
    fPID[iTr] = -2;
    CbmTrackMatchNew* stsTrackMatch = (CbmTrackMatchNew*) fTrackMatchArray->At(iTr);
    if(stsTrackMatch -> GetNofLinks() == 0) continue;
    Float_t bestWeight = 0.f;
    Float_t totalWeight = 0.f;
    Int_t mcTrackId = -1;
    for(int iLink=0; iLink<stsTrackMatch -> GetNofLinks(); iLink++)
    {
      totalWeight += stsTrackMatch->GetLink(iLink).GetWeight();
      if( stsTrackMatch->GetLink(iLink).GetWeight() > bestWeight)
      {
        bestWeight = stsTrackMatch->GetLink(iLink).GetWeight();
        mcTrackId = stsTrackMatch->GetLink(iLink).GetIndex();
      }
    }
    if(bestWeight/totalWeight < 0.7) continue;
    if(mcTrackId >= nMCTracks || mcTrackId < 0)
    {
      std::cout << "Sts Matching is wrong!    StsTackId = " << mcTrackId << " N mc tracks = " << nMCTracks << std::endl;
      continue;
    }
    CbmMCTrack *cbmMCTrack = (CbmMCTrack*)fMCTrackArray->At(mcTrackId);
    if(!(TMath::Abs(cbmMCTrack->GetPdgCode()) == 11 ||
         TMath::Abs(cbmMCTrack->GetPdgCode()) == 13 ||
         TMath::Abs(cbmMCTrack->GetPdgCode()) == 211 ||
         TMath::Abs(cbmMCTrack->GetPdgCode()) == 321 ||
         TMath::Abs(cbmMCTrack->GetPdgCode()) == 2212 ||
         TMath::Abs(cbmMCTrack->GetPdgCode()) == 1000010020 ||
         TMath::Abs(cbmMCTrack->GetPdgCode()) == 1000010030 ||
         TMath::Abs(cbmMCTrack->GetPdgCode()) == 1000020030 ||
         TMath::Abs(cbmMCTrack->GetPdgCode()) == 1000020040 ) )
      continue;
    fPID[iTr] = cbmMCTrack->GetPdgCode();
  }
}

void CbmKFParticleFinderPID::SetRecoPID()
{
  if (NULL == fGlobalTrackArray) { Fatal("KF Particle Finder", "No GlobalTrack array!"); }
  if (NULL == fTofHitArray) { Fatal("KF Particle Finder", "No TOF hits array!"); }

  const Double_t m2TOF[7] = { 0.885, 0.245, 0.019479835, 0., 3.49, 7.83, 1.95};
  
  Double_t sP[7][5];
  if(fSisMode == 0) //SIS-100
  {
    Double_t sPLocal[7][5] = { {0.056908,-0.0470572,0.0216465,-0.0021016,8.50396e-05},
                               {0.00943075,-0.00635429,0.00998695,-0.00111527,7.77811e-05},
                               {0.00176298,0.00367263,0.00308013,0.000844013,-0.00010423},
                               {0.00218401, 0.00152391, 0.00895357, -0.000533423, 3.70326e-05},
                               {0.261491, -0.103121, 0.0247587, -0.00123286, 2.61731e-05},
                               {0.657274, -0.22355, 0.0430177, -0.0026822, 7.34146e-05},
                               {0.116525, -0.045522,0.0151319, -0.000495545, 4.43144e-06}  }; 
    for(Int_t iSp=0; iSp<7; iSp++)
      for(Int_t jSp=0; jSp<5; jSp++)
        sP[iSp][jSp] = sPLocal[iSp][jSp];
  }
  
  if(fSisMode == 1) //SIS-300
  {
    Double_t sPLocal[7][5] = { {0.0337428,-0.013939,0.00567602,-0.000202229,4.07531e-06},
                               {0.00717827,-0.00257353, 0.00389851,-9.83097e-05, 1.33011e-06},
                               {0.001348,0.00220126,0.0023619,7.35395e-05,-4.06706e-06},
                               {0.00142972, 0.00308919, 0.00326995, 6.91715e-05, -2.44194e-06},
                               {0.261491, -0.103121, 0.0247587, -0.00123286, 2.61731e-05},  //TODO tune for SIS300
                               {0.657274, -0.22355, 0.0430177, -0.0026822, 7.34146e-05},
                               {0.116525, -0.045522,0.0151319, -0.000495545, 4.43144e-06}  }; 
    for(Int_t iSp=0; iSp<7; iSp++)
      for(Int_t jSp=0; jSp<5; jSp++)
        sP[iSp][jSp] = sPLocal[iSp][jSp];
  }

  const Int_t PdgHypo[7] = {2212, 321, 211, -11, 1000010020, 1000010030, 1000020030};

  for (Int_t igt = 0; igt < fGlobalTrackArray->GetEntriesFast(); igt++) {
    const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(fGlobalTrackArray->At(igt));

    Int_t stsTrackIndex = globalTrack->GetStsTrackIndex();
    if( stsTrackIndex<0 ) continue;

    Bool_t isElectronTRD = 0;
    Bool_t isElectronRICH = 0;
    Bool_t isElectron = 0;

    CbmStsTrack* cbmStsTrack = (CbmStsTrack*) fTrackArray->At(stsTrackIndex);
    const FairTrackParam *stsPar = cbmStsTrack->GetParamFirst();
    TVector3 mom;
    stsPar->Momentum(mom);

    Double_t p = mom.Mag();
    Int_t q = stsPar->GetQp() > 0 ? 1 : -1;
    
    if (fRichPIDMode == 1)
    {
      if(fRichRingArray)
      {
        Int_t richIndex = globalTrack->GetRichRingIndex();
        if (richIndex > -1)
        {
          CbmRichRing* richRing = (CbmRichRing*)fRichRingArray->At(richIndex);
          if (richRing)
          {
            Double_t axisA = richRing->GetAaxis();
            Double_t axisB = richRing->GetBaxis();
            Double_t dist = richRing->GetDistance();
            
            Double_t fMeanA = 4.95;
            Double_t fMeanB = 4.54;
            Double_t fRmsA = 0.30;
            Double_t fRmsB = 0.22;
            Double_t fRmsCoeff = 3.5;
            Double_t fDistCut = 1.;


        //            if(fElIdAnn->DoSelect(richRing, p) > -0.5) isElectronRICH = 1;
            if (p<5.5){
              if ( fabs(axisA-fMeanA) < fRmsCoeff*fRmsA &&
              fabs(axisB-fMeanB) < fRmsCoeff*fRmsB && 
              dist < fDistCut) isElectronRICH = 1;
            }
            else {
              ///3 sigma
              // Double_t polAaxis = 5.80008 - 4.10118 / (momentum - 3.67402);
              // Double_t polBaxis = 5.58839 - 4.75980 / (momentum - 3.31648);
              // Double_t polRaxis = 5.87252 - 7.64641/(momentum - 1.62255);
              ///2 sigma          
              Double_t polAaxis = 5.64791 - 4.24077 / (p - 3.65494);
              Double_t polBaxis = 5.41106 - 4.49902 / (p - 3.52450);
              //Double_t polRaxis = 5.66516 - 6.62229/(momentum - 2.25304);
              if ( axisA < (fMeanA + fRmsCoeff*fRmsA) &&
              axisA > polAaxis &&
              axisB < (fMeanB + fRmsCoeff*fRmsB) && 
              axisB > polBaxis &&
              dist < fDistCut) isElectronRICH = 1;
            }
          }
        }
      }
    }

    if (fTrdPIDMode == 1)
    {
      if(fTrdTrackArray)
      {
        Int_t trdIndex = globalTrack->GetTrdTrackIndex();
        if (trdIndex > -1)
        {
          CbmTrdTrack* trdTrack = (CbmTrdTrack*)fTrdTrackArray->At(trdIndex);
          if (trdTrack)
          {
            if (trdTrack->GetPidWkn() > 0.635)
              isElectronTRD = 1;
          }
        }
      }
    }

    if (fTrdPIDMode == 2)
    {
      if(fTrdTrackArray)
      {
        Int_t trdIndex = globalTrack->GetTrdTrackIndex();
        if (trdIndex > -1)
        {
          CbmTrdTrack* trdTrack = (CbmTrdTrack*)fTrdTrackArray->At(trdIndex);
          if (trdTrack)
          {
            if (trdTrack->GetPidANN() > 0.9)
              isElectronTRD = 1;
          }
        }
      }
    }

    Double_t l = globalTrack->GetLength();// l is calculated by global tracking
    if(fSisMode==0) //SIS-100
      if( !((l>500.) && (l<900.)) ) continue;
    if(fSisMode==1) //SIS 300
      if( !((l>700.) && (l<1500.)) ) continue;
      
    Double_t time;
    Int_t tofHitIndex = globalTrack->GetTofHitIndex();
    if (tofHitIndex >= 0) {
      const CbmTofHit* tofHit = static_cast<const CbmTofHit*>(fTofHitArray->At(tofHitIndex));
      if(!tofHit) continue;
      time = tofHit->GetTime();
    }
    else
      continue;

    if(fSisMode==0) //SIS-100
      if( !((time>16.) && (time<42.)) ) continue;
    if(fSisMode==1) //SIS 300
      if( !((time>26.) && (time<52.)) ) continue;

    Double_t m2 = p*p*(1./((l/time/29.9792458)*(l/time/29.9792458))-1.);

    Double_t sigma[7];
    Double_t dm2[7];

    for(int iSigma=0; iSigma<7; iSigma++)
    {
      sigma[iSigma] = sP[iSigma][0] + sP[iSigma][1]*p + sP[iSigma][2]*p*p + sP[iSigma][3]*p*p*p + sP[iSigma][4]*p*p*p*p;
      dm2[iSigma] = fabs(m2 - m2TOF[iSigma])/sigma[iSigma];
    }

    int iPdg=2;
    Double_t dm2min = dm2[2];
    
    if( p>1.5 )
    {
      if ( isElectronRICH && isElectronTRD ) isElectron = 1;
      if ( fRichPIDMode == 0 && isElectronTRD ) isElectron = 1;
      if ( fTrdPIDMode == 0 && isElectronRICH ) isElectron = 1;
    }
    else
      if( isElectronRICH ) isElectron = 1;
    
    if(isElectron)
    {
      if(dm2[3] > 3.)
        isElectron = 0;
    }
    
    if(!isElectron)
    {
      if(p>12.) continue;
      
      for(int jPDG=0; jPDG<7; jPDG++)
      {
        if(jPDG==3) continue;
        if(dm2[jPDG] < dm2min) { iPdg = jPDG; dm2min = dm2[jPDG]; }
      }

      if(dm2min > 2) iPdg=-1;

      if(iPdg > -1)
        fPID[stsTrackIndex] = q*PdgHypo[iPdg];
    }
    else
      fPID[stsTrackIndex] = q*PdgHypo[3];
  }
}

ClassImp(CbmKFParticleFinderPID);
