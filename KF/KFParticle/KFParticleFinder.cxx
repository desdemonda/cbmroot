//----------------------------------------------------------------------------
// Implementation of the KFParticle class
// .
// @author  I.Kisel, I.Kulakov, M.Zyzak
// @version 1.0
// @since   20.08.13
// 
// 
//  -= Copyright &copy ALICE HLT and CBM L1 Groups =-
//____________________________________________________________________________

#include "KFParticleFinder.h"

//for particle finding
#include <map>
using std::map;
using std::vector;

#include "KFParticleDatabase.h"
#include <iomanip>

KFParticleFinder::KFParticleFinder():
  fNPV(-1),fNThreads(1),fCutCharmPt(0.2f),fCutCharmChiPrim(6.f),fCutLVMPt(0.2f),fCutLVMP(1.0f),fCutJPsiPt(1.0f),
  fD0(0), fD0bar(0), fD04(0), fD04bar(0), fDPlus(0), fDMinus(0), fLPi(0), fLPiPIndex(0), fEmcClusters(0)
{
  //Cuts
  //track + track
  //chi2_prim         chi2_geo          ldl
  fCuts2D[0] = 3.f; fCuts2D[1] = 3.f; fCuts2D[2] = 5.f; 
  //tracks to select primary and secondary particles
  //mass              chi2_topo          ldl
#ifdef PANDA_STT
  fSecCuts[0] = 3.f; fSecCuts[1] = -3.f; fSecCuts[2] = 10.f;
#else  
  fSecCuts[0] = 3.f; fSecCuts[1] = 5.f; fSecCuts[2] = 10.f;
#endif
  //track + particle
  //                ldl          chi2_topo                        chi2_geo
  fCutsTrackV0[0][0] = 10;     fCutsTrackV0[0][1] = 5;        fCutsTrackV0[0][2] = 6;  //Xi
  fCutsTrackV0[1][0] = 10;     fCutsTrackV0[1][1] = 3;        fCutsTrackV0[1][2] = 3;  //Omega, charm, H0, Sigma+
  fCutsTrackV0[2][0] = -100.;  fCutsTrackV0[2][1] = 10000.;   fCutsTrackV0[2][2] = 3;  //resonances
  
  //charm
  //chi2               l/dl                  chi2_topo
  fCutsCharm[0] = 3.f; fCutsCharm[1] = 10.f; fCutsCharm[2] = 3.f; //D0 -> pi+ K-
  
  //cuts on particles reconstructed from short-lived particles
  //ldl,                      chi2_topo                 chi2_geo
  //H0 -> Lambda Lambda, Xi0 -> Lambda pi0
  fCutsPartPart[0][0] =  10;  fCutsPartPart[0][1] = 3;  fCutsPartPart[0][2] = 3;
  //Sigma0 -> Lambda Gamma, pi0 -> Gamma Gamma, K* -> K pi0, Sigma*0 -> Lambda pi0, Xi* -> Xi pi0
  fCutsPartPart[1][0] = -10;  fCutsPartPart[1][1] = 3;  fCutsPartPart[1][2] = 3;  
}

void KFParticleFinder::FindParticles(KFPTrackVector *vRTracks, kfvector_float* ChiToPrimVtx,
                                     vector<KFParticle>& Particles, KFParticleSIMD* PrimVtx, int nPV)
{
  //* Finds particles (K0s and Lambda) from a given set of tracks
// std::cout << "kfp size  " <<  sizeof(KFParticle) << std::endl;
  fNPV = nPV;
  
  const int nPartPrim = vRTracks[2].NPions() * vRTracks[3].NKaons() + 
                        vRTracks[3].NPions() * vRTracks[2].NKaons() + 
                        vRTracks[2].NKaons() * vRTracks[3].NKaons() + 
                        vRTracks[2].NKaons() * vRTracks[3].NProtons() + 
                        vRTracks[3].NKaons() * vRTracks[2].NProtons() + 
                        vRTracks[2].NElectrons() * vRTracks[3].NElectrons() + 
                        vRTracks[2].NMuons() * vRTracks[3].NMuons();

  const int nPart = vRTracks[0].NPions() * vRTracks[1].NPions() +
                    vRTracks[0].NPions() * vRTracks[1].NProtons() +
                    vRTracks[1].NPions() * vRTracks[0].NProtons() + nPartPrim;
  
//std::cout << "NPart estim " << nPart << std::endl;
//   Particles.reserve(vRTracks.size() + nPart);

  fD0.clear();
  fD0bar.clear();
  fD04.clear();
  fD04bar.clear();
  fDPlus.clear();
  fDMinus.clear();
  fLPi.clear();
  fLPiPIndex.clear();
  fK0PiPlus.clear();
  fK0PiMinusIndex.clear();
  fK0PiPi.clear();
  
  int nEmcClusters = 0;
  if(fEmcClusters)
    nEmcClusters = fEmcClusters->Size();
  
  vector<KFParticle> vGammaPrimEmc;
  
  Particles.reserve(nPart+vRTracks[0].Size()+vRTracks[1].Size()+vRTracks[2].Size()+vRTracks[3].Size() + nEmcClusters);
  {
    KFPTrack kfTrack;
    for(int iV=0; iV<4; iV++)
    {
      for(unsigned short iTr=0; iTr < vRTracks[iV].Size(); iTr++)
      {
        vRTracks[iV].GetTrack(kfTrack, iTr);
        KFParticle tmp(kfTrack, vRTracks[iV].PDG()[iTr]);
        tmp.SetPDG(vRTracks[iV].PDG()[iTr]);//vTrackPDG[iTr]);
        tmp.SetId(Particles.size());
        vRTracks[iV].SetId(Particles.size(),iTr);
        tmp.AddDaughterId( kfTrack.Id() );
#ifdef NonhomogeneousField
        for(int iF=0; iF<10; iF++)
          tmp.SetFieldCoeff( vRTracks[iV].FieldCoefficient(iF)[iTr], iF);
#endif
        Particles.push_back(tmp);
      }
    }
    
    if(fEmcClusters)
    {
      KFParticleSIMD tmpGammaSIMD;
      KFParticle tmpGamma;
      
      for(unsigned short iEmc=0; iEmc < fEmcClusters->Size(); iEmc += float_vLen)
      {
        const unsigned short NClustersVec = (iEmc + float_vLen < fEmcClusters->Size()) ? float_vLen : (fEmcClusters->Size() - iEmc);
        tmpGammaSIMD.Load(*fEmcClusters, iEmc, PrimVtx[0]);
        for(int iV=0; iV<NClustersVec; iV++)
        {
          tmpGammaSIMD.GetKFParticle(tmpGamma, iV);
          tmpGamma.SetPDG(22); //gamma pdg
          tmpGamma.SetId(Particles.size());
          tmpGamma.CleanDaughtersId();
          tmpGamma.AddDaughterId(fEmcClusters->Id()[iEmc+iV]);
          Particles.push_back(tmpGamma);
          vGammaPrimEmc.push_back(tmpGamma);
        }
      }
    }
  }

  vector< vector<KFParticle> > vXiPrim(fNPV);
  vector< vector<KFParticle> > vXiBarPrim(fNPV);

  vector< vector<KFParticle> > vXiStarPrim(fNPV);
  vector< vector<KFParticle> > vXiStarBarPrim(fNPV);

  vector< vector<KFParticle> > vV0Prim[4]; // Ks, Lambda, Lambda_bar, gamma
  for(int iV0Prim = 0; iV0Prim<4; iV0Prim++)
    vV0Prim[iV0Prim].resize(fNPV);
  vector<KFParticle> vV0Sec[4]; //Ks, Lambda, Lambda_bar, gamma
  
  vector< vector<KFParticle> > vPi0Prim(fNPV);
  vector<KFParticle> vPi0Sec;
  
  Find2DaughterDecay(vRTracks, ChiToPrimVtx,
                     Particles, PrimVtx, fCuts2D,
                     fSecCuts, vV0Prim, vV0Sec);
  
  for(int iPV=0; iPV<fNPV; iPV++ )
  {
    ExtrapolateToPV(vV0Prim[0][iPV],PrimVtx[iPV]);
    ExtrapolateToPV(vV0Prim[1][iPV],PrimVtx[iPV]);
    ExtrapolateToPV(vV0Prim[2][iPV],PrimVtx[iPV]);
    ExtrapolateToPV(vV0Prim[3][iPV],PrimVtx[iPV]);
  }
  
  //Xi- -> Lambda pi-, Omega- -> Lambda K-
  FindTrackV0Decay(vV0Sec[1], 3122, vRTracks[1], -1, vRTracks[1].FirstPion(), vRTracks[1].LastKaon(),
                   Particles, PrimVtx, -1, &(ChiToPrimVtx[1]), &vXiPrim);
  //Xi+ -> Lambda pi+, Omega+ -> Lambda K+
  FindTrackV0Decay(vV0Sec[2], -3122, vRTracks[0], 1, vRTracks[0].FirstPion(), vRTracks[0].LastKaon(),
                   Particles, PrimVtx, -1, &(ChiToPrimVtx[0]), &vXiBarPrim);
  for(int iPV=0; iPV<fNPV; iPV++ )
  {
    ExtrapolateToPV(vXiPrim[iPV],PrimVtx[iPV]);
    ExtrapolateToPV(vXiBarPrim[iPV],PrimVtx[iPV]);
  }
  
  //K*+ -> K0 pi+
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(vV0Prim[0][iPV], 310, vRTracks[2], 1, vRTracks[2].FirstPion(), vRTracks[2].LastPion(),
                     Particles, PrimVtx, iPV, 0);
  //K*- -> K0 pi-
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(vV0Prim[0][iPV], 310, vRTracks[3], -1, vRTracks[3].FirstPion(), vRTracks[3].LastPion(),
                     Particles, PrimVtx, iPV, 0);
  //Sigma*+ -> Lambda pi+
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(vV0Prim[1][iPV], 3122, vRTracks[2], 1, vRTracks[2].FirstPion(), vRTracks[2].LastPion(),
                     Particles, PrimVtx, iPV, 0);
  //Sigma*- -> Lambda pi-, Xi*- -> Lambda K-
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(vV0Prim[1][iPV], 3122, vRTracks[3], -1, vRTracks[3].FirstPion(), vRTracks[3].LastKaon(),
                     Particles, PrimVtx, iPV, 0);
  //Sigma*+_bar -> Lambda_bar pi-
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(vV0Prim[2][iPV], -3122, vRTracks[3], -1, vRTracks[3].FirstPion(), vRTracks[3].LastPion(),
                      Particles, PrimVtx, iPV, 0);
  //Sigma*-_bar -> Lambda_bar pi+, Xi*+ -> Lambda_bar + K+
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(vV0Prim[2][iPV], -3122, vRTracks[2], 1, vRTracks[2].FirstPion(), vRTracks[2].LastKaon(),
                      Particles, PrimVtx, iPV, 0);
  //Xi*0 -> Xi- pi+
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(vXiPrim[iPV], 3312, vRTracks[2], 1, vRTracks[2].FirstPion(), vRTracks[2].LastPion(),
                      Particles, PrimVtx, iPV, 0, &vXiStarPrim);
  //Xi*0_bar -> Xi+ pi-
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(vXiBarPrim[iPV], -3312, vRTracks[3], -1, vRTracks[3].FirstPion(), vRTracks[3].LastPion(),
                      Particles, PrimVtx, iPV, 0, &vXiStarBarPrim);
  //Omega*- -> Xi- pi+ K-
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(vXiStarPrim[iPV], 3324, vRTracks[3], -1, vRTracks[3].FirstKaon(), vRTracks[3].LastKaon(),
                      Particles, PrimVtx, iPV, 0);
  //Omega*+ -> Xi+ pi- K+
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(vXiStarBarPrim[iPV], -3324, vRTracks[2], 1, vRTracks[2].FirstKaon(), vRTracks[2].LastKaon(),
                      Particles, PrimVtx, iPV, 0);
  // Charm
  //LambdaC -> pi+ K- p, Ds+ -> pi+ K- K+, D+ -> pi+ K- pi+
  FindTrackV0Decay(fD0, 421, vRTracks[0], 1, vRTracks[0].FirstPion(), vRTracks[0].LastProton(),
                    Particles, PrimVtx, -1, &(ChiToPrimVtx[0]));
  //LambdaC_bar -> pi- K+ p-, Ds- -> pi- K+ K-, D- -> pi- K+ pi-
  FindTrackV0Decay(fD0bar, -421, vRTracks[1], -1, vRTracks[1].FirstPion(), vRTracks[1].LastProton(),
                    Particles, PrimVtx, -1, &(ChiToPrimVtx[1]));    
  //D0->pi+ K- pi+ pi-
  FindTrackV0Decay(fDPlus, 411, vRTracks[1], -1, vRTracks[1].FirstPion(), vRTracks[1].LastPion(),
                    Particles, PrimVtx, -1, &(ChiToPrimVtx[1]));
  //D0_bar->pi- K+ pi- pi+
  FindTrackV0Decay(fDMinus, -411, vRTracks[0], 1, vRTracks[0].FirstPion(), vRTracks[0].LastPion(),
                    Particles, PrimVtx, -1, &(ChiToPrimVtx[0]));    
  //D0 -> pi+ K-
  SelectParticles(Particles,fD0,PrimVtx,fCutsCharm[2],fCutsCharm[1],
                  KFParticleDatabase::Instance()->GetD0Mass(), KFParticleDatabase::Instance()->GetD0MassSigma(), fSecCuts[0]);
  //D0_bar -> pi+ K-
  SelectParticles(Particles,fD0bar,PrimVtx,fCutsCharm[2],fCutsCharm[1],
                  KFParticleDatabase::Instance()->GetD0Mass(), KFParticleDatabase::Instance()->GetD0MassSigma(), fSecCuts[0]);
  //D*+->D0 pi+
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(fD0, 421, vRTracks[2], 1, vRTracks[2].FirstPion(), vRTracks[2].LastPion(),
                      Particles, PrimVtx, iPV, 0);
  //D*- -> D0_bar pi-
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(fD0, -421, vRTracks[3], -1, vRTracks[3].FirstPion(), vRTracks[3].LastPion(),
                      Particles, PrimVtx, iPV, 0);
  //D0 -> pi+ K- pi+ pi-
  SelectParticles(Particles,fD04,PrimVtx,fCutsCharm[2],fCutsCharm[1],
                  KFParticleDatabase::Instance()->GetD0Mass(), KFParticleDatabase::Instance()->GetD0MassSigma(), fSecCuts[0]);
  //D0_bar -> pi- K+ pi- pi+
  SelectParticles(Particles,fD04bar,PrimVtx,fCutsCharm[2],fCutsCharm[1],
                  KFParticleDatabase::Instance()->GetD0Mass(), KFParticleDatabase::Instance()->GetD0MassSigma(), fSecCuts[0]);
  //D*+->D0 pi+
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(fD04, 429, vRTracks[2], 1, vRTracks[2].FirstPion(), vRTracks[2].LastPion(),
                      Particles, PrimVtx, iPV, 0);
  //D0*- -> D0_bar pi-
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(fD04bar, -429, vRTracks[3], -1, vRTracks[3].FirstPion(), vRTracks[3].LastPion(),
                      Particles, PrimVtx, iPV, 0);
  //D+
  SelectParticles(Particles,fDPlus,PrimVtx,fCutsCharm[2],fCutsCharm[1],
                  KFParticleDatabase::Instance()->GetDPlusMass(), KFParticleDatabase::Instance()->GetDPlusMassSigma(), fSecCuts[0]);
  //D-
  SelectParticles(Particles,fDMinus,PrimVtx,fCutsCharm[2],fCutsCharm[1],
                  KFParticleDatabase::Instance()->GetDPlusMass(), KFParticleDatabase::Instance()->GetDPlusMassSigma(), fSecCuts[0]);
  //D*0->D+ pi-
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(fDPlus, 411, vRTracks[3], -1, vRTracks[3].FirstPion(), vRTracks[3].LastPion(),
                      Particles, PrimVtx, iPV, 0);
  //D*0_bar->D- pi+
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(fDMinus, -411, vRTracks[2], 1, vRTracks[2].FirstPion(), vRTracks[2].LastPion(),
                      Particles, PrimVtx, iPV, 0);

  //H0 -> Lambda Lambda
  CombinePartPart(vV0Sec[1], vV0Sec[1], Particles, PrimVtx, fCutsPartPart[0], -1, 3000, 1, 1);
  //H0 -> Lambda p pi-
  FindTrackV0Decay(fLPi, 3002, vRTracks[0], 1, vRTracks[0].FirstProton(), vRTracks[0].LastProton(),
                    Particles, PrimVtx, -1);
  //Sigma0 -> Lambda Gamma
  for(int iPV=0; iPV<fNPV; iPV++)
    CombinePartPart(vV0Prim[3][iPV], vV0Prim[1][iPV], Particles, PrimVtx, fCutsPartPart[1], iPV, 3212);
  //Sigma0_bar -> Lambda_bar Gamma
  for(int iPV=0; iPV<fNPV; iPV++)
    CombinePartPart(vV0Prim[3][iPV], vV0Prim[2][iPV], Particles, PrimVtx, fCutsPartPart[1], iPV, -3212);
  //pi0 -> gamma gamma
  const float& mPi0 = KFParticleDatabase::Instance()->GetPi0Mass();
  const float& mPi0Sigma = KFParticleDatabase::Instance()->GetPi0MassSigma();
  CombinePartPart(vV0Sec[3], vV0Sec[3], Particles, PrimVtx, fCutsPartPart[1], -1, 111, 1, 0, &vPi0Prim, &vPi0Sec, mPi0, mPi0Sigma);
  for(int iPV=0; iPV<fNPV; iPV++)
  {
    CombinePartPart(vV0Prim[3][iPV], vV0Prim[3][iPV], Particles, PrimVtx, fCutsPartPart[1], iPV, 111, 1, 0, &vPi0Prim, &vPi0Sec, mPi0, mPi0Sigma);
    CombinePartPart(vV0Sec[3],       vV0Prim[3][iPV], Particles, PrimVtx, fCutsPartPart[1],  -1, 111, 0, 0, &vPi0Prim, &vPi0Sec, mPi0, mPi0Sigma);
  }
  for(int iPV=0; iPV<fNPV; iPV++ )
    ExtrapolateToPV(vPi0Prim[iPV],PrimVtx[iPV]);
  //eta -> pi0 pi0 pi0
  //TODO implement this
  //Sigma+ -> p pi0
  FindTrackV0Decay(vPi0Sec, 111, vRTracks[0],  1, vRTracks[0].FirstProton(), vRTracks[0].LastProton(),
                    Particles, PrimVtx, -1);
  //Sigma+_bar -> p- pi0
  FindTrackV0Decay(vPi0Sec, 111, vRTracks[1], -1, vRTracks[1].FirstProton(), vRTracks[1].LastProton(),
                    Particles, PrimVtx, -1);
  //Xi0 -> Lambda pi0
  CombinePartPart(vPi0Sec, vV0Sec[1], Particles, PrimVtx, fCutsPartPart[0], -1, 3322);
  //Xi0_bar -> Lambda_bar pi0
  CombinePartPart(vPi0Sec, vV0Sec[2], Particles, PrimVtx, fCutsPartPart[0], -1, -3322);
  //K*+ -> K+ pi0
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(vPi0Prim[iPV], 111, vRTracks[2],  1, vRTracks[2].FirstKaon(), vRTracks[2].LastKaon(),
                      Particles, PrimVtx, -1);
  //K*- -> K- pi0
  for(int iPV=0; iPV<fNPV; iPV++)
    FindTrackV0Decay(vPi0Prim[iPV], 111, vRTracks[3], -1, vRTracks[3].FirstKaon(), vRTracks[3].LastKaon(),
                      Particles, PrimVtx, -1);
  //K*0 -> K0 pi0
  for(int iPV=0; iPV<fNPV; iPV++)
    CombinePartPart(vPi0Prim[iPV], vV0Prim[0][iPV], Particles, PrimVtx, fCutsPartPart[1], iPV, 100313, 0, 1);    
  //Sigma*0 -> Lambda pi0
  for(int iPV=0; iPV<fNPV; iPV++)
    CombinePartPart(vPi0Prim[iPV], vV0Prim[1][iPV], Particles, PrimVtx, fCutsPartPart[1], iPV, 3214, 0, 1);       
  //Sigma*0_bar -> Lambda_bar pi0
  for(int iPV=0; iPV<fNPV; iPV++)
    CombinePartPart(vPi0Prim[iPV], vV0Prim[2][iPV], Particles, PrimVtx, fCutsPartPart[1], iPV, -3214, 0, 1);       
  //Xi*- -> Xi- pi0
  for(int iPV=0; iPV<fNPV; iPV++)
    CombinePartPart(vPi0Prim[iPV], vXiPrim[iPV], Particles, PrimVtx, fCutsPartPart[1], iPV, 3314, 0, 1);   
  //Xi*+ -> Xi+ pi0
  for(int iPV=0; iPV<fNPV; iPV++)
    CombinePartPart(vPi0Prim[iPV], vXiBarPrim[iPV], Particles, PrimVtx, fCutsPartPart[1], iPV, -3314, 0, 1);  

  //reconstruct particles with daughters
  if(fEmcClusters)
  {
    //pi0 -> gamma gamma, EMC
    vector< vector<KFParticle> > vPi0PrimEmc(1);
    vector<KFParticle> vPi0SecEmc;
    vector< vector<KFParticle> > vD0PrimEmc(1);
    CombinePartPart(vGammaPrimEmc, vGammaPrimEmc, Particles, PrimVtx, fCutsPartPart[1], 0, 111, 1, 0, &vPi0PrimEmc, &vPi0SecEmc, mPi0, mPi0Sigma);
        
    //D+ -> K0 pi+
    FindTrackV0Decay(vV0Sec[0],    310, vRTracks[0],  1, vRTracks[0].FirstPion(), vRTracks[0].LastPion(), Particles, PrimVtx, -1/*, &(ChiToPrimVtx[0])*/);
    //D0 -> K0 pi+ pi-
    FindTrackV0Decay(fK0PiPlus, 100411, vRTracks[1], -1, vRTracks[1].FirstPion(), vRTracks[1].LastPion(), Particles, PrimVtx, -1/*, &(ChiToPrimVtx[0])*/);
    //D0 -> K0 pi+ pi- pi0
    CombinePartPart(fK0PiPi, vPi0PrimEmc[0], Particles, PrimVtx, fCutsPartPart[1], -1, 428, 0, 0, &vD0PrimEmc, 0, 
                    KFParticleDatabase::Instance()->GetD0Mass(), 0.025);
    
    for(int iPV=0; iPV<1; iPV++ )
    {
      ExtrapolateToPV(vPi0PrimEmc[iPV],PrimVtx[iPV]);
      ExtrapolateToPV(vD0PrimEmc[iPV],PrimVtx[iPV]);
    }
    //D0* -> D0 pi0
    CombinePartPart(vD0PrimEmc[0], vPi0PrimEmc[0], Particles, PrimVtx, fCutsPartPart[1], 0, 10428);
  }
}

void KFParticleFinder::ExtrapolateToPV(vector<KFParticle>& vParticles, KFParticleSIMD& PrimVtx)
{
  KFParticle* parts[float_vLen];
  KFParticle tmpPart[float_vLen];
  
  for(unsigned short iv=0; iv<float_vLen; iv++)
    parts[iv] = &tmpPart[iv];
    
  for(unsigned int iL=0; iL<vParticles.size(); iL += float_vLen)
  {

    unsigned int nPart = vParticles.size();
    unsigned int nEntries = (iL + float_vLen < nPart) ? float_vLen : (nPart - iL);

    
    for(unsigned short iv=0; iv<nEntries; iv++)
      tmpPart[iv] = vParticles[iL+iv];

    KFParticleSIMD tmp(parts,nEntries);

    tmp.TransportToPoint(PrimVtx.Parameters());

    for(unsigned int iv=0; iv<nEntries; iv++)
    {
      tmp.GetKFParticle(vParticles[iL+iv], iv);
    }
  }
}

float_v KFParticleFinder::GetChi2BetweenParticles(KFParticleSIMD &p1, KFParticleSIMD &p2)
{
  const float_v& x1 = p1.GetX();
  const float_v& y1 = p1.GetY();
  const float_v& z1 = p1.GetZ();

  const float_v& x2 = p2.GetX();
  const float_v& y2 = p2.GetY();
  const float_v& z2 = p2.GetZ();

  const float_v dx = x1 - x2;
  const float_v dy = y1 - y2;
  const float_v dz = z1 - z2;

  const float_v& c0 = p1.GetCovariance(0) + p2.GetCovariance(0);
  const float_v& c1 = p1.GetCovariance(1) + p2.GetCovariance(1);
  const float_v& c2 = p1.GetCovariance(2) + p2.GetCovariance(2);
  const float_v& c3 = p1.GetCovariance(3) + p2.GetCovariance(3);
  const float_v& c4 = p1.GetCovariance(4) + p2.GetCovariance(4);
  const float_v& c5 = p1.GetCovariance(5) + p2.GetCovariance(5);

  const float_v r2 = dx*dx + dy*dy + dz*dz;
  const float_v err2 = c0*dx*dx + c2*dy*dy + c5*dz*dz + 2.f*( c1*dx*dy + c3*dx*dz + c4*dy*dz );

  return (r2*r2/err2);
}

inline void KFParticleFinder::ConstructV0(KFPTrackVector* vTracks,
                                          int iTrTypePos,
                                          int iTrTypeNeg,
                                          uint_v& idPosDaughters,
                                          uint_v& idNegDaughters,
                                          int_v& daughterPosPDG,
                                          int_v& daughterNegPDG,
                                          KFParticleSIMD& mother,
                                          KFParticle& mother_temp,
                                          const unsigned short NTracks,
                                          vector<float_v>& l,
                                          vector<float_v>& dl,
                                          vector<KFParticle>& Particles,
                                          KFParticleSIMD* PrimVtx,
                                          const float* cuts,
                                          const int_v& pvIndex,
                                          const float* secCuts,
                                          const float_v& massMotherPDG,
                                          const float_v& massMotherPDGSigma,
                                          KFParticleSIMD& motherPrimSecCand,
                                          int& nPrimSecCand,
                                          vector< vector<KFParticle> >* vMotherPrim,
                                          vector<KFParticle>* vMotherSec
                                         )
{
  float_m isPrimary(pvIndex>-1);
  int_v trackId;
  KFParticleSIMD posDaughter(vTracks[iTrTypePos],idPosDaughters, daughterPosPDG);
  trackId.gather( &(vTracks[iTrTypePos].Id()[0]), idPosDaughters );
  posDaughter.SetId(trackId);

  KFParticleSIMD negDaughter(vTracks[iTrTypeNeg],idNegDaughters, daughterNegPDG);
  trackId.gather( &(vTracks[iTrTypeNeg].Id()[0]), idNegDaughters );
  negDaughter.SetId(trackId);

  float_v vtxGuess[3] = {0.f};
  float_v vtxErrGuess[3] = {0.f};
  if( !(isPrimary.isFull()) )
  {
    negDaughter.GetVertexApproximation(posDaughter, vtxGuess, vtxErrGuess);
  }
  if(!(isPrimary.isEmpty()))
  {
    int iPV=pvIndex[0];
    if( (pvIndex == iPV).isFull() )
    {
      vtxGuess[0](isPrimary) = PrimVtx[iPV].X();
      vtxGuess[1](isPrimary) = PrimVtx[iPV].Y();
      vtxGuess[2](isPrimary) = PrimVtx[iPV].Z();
    
      vtxErrGuess[0](isPrimary) = 100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[0]);
      vtxErrGuess[1](isPrimary) = 100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[2]);
      vtxErrGuess[2](isPrimary) = 100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[5]);
    }
    else
    {
      for(int iV=0; iV<float_vLen; iV++)
      {
        if(!isPrimary[iV]) continue;
        if(iV >= NTracks) continue;
        
        iPV = pvIndex[iV];
        vtxGuess[0][iV] = PrimVtx[iPV].X()[0];
        vtxGuess[1][iV] = PrimVtx[iPV].Y()[0];
        vtxGuess[2][iV] = PrimVtx[iPV].Z()[0];
    
        vtxErrGuess[0][iV] = 100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[0][0]);
        vtxErrGuess[1][iV] = 100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[2][0]);
        vtxErrGuess[2][iV] = 100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[5][0]);        
      }
    }
  }

  mother.SetVtxGuess(vtxGuess[0], vtxGuess[1], vtxGuess[2]);
  mother.SetVtxErrGuess(vtxErrGuess[0], vtxErrGuess[1], vtxErrGuess[2]);   

  const KFParticleSIMD* vDaughtersPointer[2] = {&negDaughter, &posDaughter};
  if(isPrimary.isFull())
    mother.Construct(vDaughtersPointer, 2, 0, -1, 0, 1);
  else
    mother.Construct(vDaughtersPointer, 2, 0);

  float_m saveParticle(int_v::IndexesFromZero() < int(NTracks));
  float_v chi2Cut = cuts[1];
  float_v ldlCut  = cuts[2];
  if( !(float_m(abs(mother.PDG()) == 421)).isEmpty() )
  {
    chi2Cut( float_m(abs(mother.PDG()) == 421) ) = fCutsCharm[0];
    ldlCut( float_m(abs(mother.PDG()) == 421) )= fCutsCharm[1];
  }
  saveParticle &= (mother.Chi2()/static_cast<float_v>(mother.NDF()) < chi2Cut );
  saveParticle &= KFPMath::Finite(mother.GetChi2());
  saveParticle &= (mother.GetChi2() > 0.0f);
  saveParticle &= (mother.GetChi2() == mother.GetChi2());

  if( saveParticle.isEmpty() ) return;
  
  float_v lMin(1.e8f);
  float_v ldlMin(1.e8f);
  float_m isParticleFromVertex(false);

  for(int iP=0; iP<fNPV; iP++)
  {
    float_m isParticleFromVertexLocal;
    mother.GetDistanceToVertexLine(PrimVtx[iP], l[iP], dl[iP], &isParticleFromVertexLocal);
    isParticleFromVertex |= isParticleFromVertexLocal;
    float_v ldl = (l[iP]/dl[iP]);
    lMin( (l[iP] < lMin) && saveParticle) = l[iP];
    ldlMin( (ldl < ldlMin) && saveParticle) = ldl;
  }
//   saveParticle &= (float_m(!isPrimary) && ldlMin > ldlCut) || isPrimary;
  saveParticle &= (lMin < 200.f);

  KFParticleSIMD motherTopo;
    ldlMin = 1.e8f;
  for(int iP=0; iP<fNPV; iP++)
  {
    motherTopo = mother;
    motherTopo.SetProductionVertex(PrimVtx[iP]);
    motherTopo.GetDecayLength(l[iP], dl[iP]);
    float_v ldl = (l[iP]/dl[iP]);
    ldlMin( (ldl < ldlMin) && saveParticle) = ldl;
  }
  saveParticle &= ( (float_m(!isPrimary) && ldlMin > ldlCut) || float_m(isPrimary) );
  
//         if(isPrimary && (float(ldlMin > 3) )) continue;
  saveParticle &= (float_m(!isPrimary) && isParticleFromVertex) || isPrimary;

  if( saveParticle.isEmpty() ) return;
  
  float_m isK0     = saveParticle && (mother.PDG() == 310);
  float_m isLambda = saveParticle && (abs(mother.PDG()) == 3122);
  float_m isGamma  = saveParticle && (mother.PDG() == 22);

  float_m saveMother(false);
  
  if( !(isK0.isEmpty()) || !(isLambda.isEmpty()) || !(isGamma.isEmpty()))
  { 
    float_v mass, errMass;

    mother.GetMass(mass, errMass);
    saveMother = saveParticle;
    saveMother &= (abs(mass - massMotherPDG)/massMotherPDGSigma) < secCuts[0];
    saveMother &= ((ldlMin > secCuts[2]) && !isGamma) || isGamma;
    saveMother &= (isK0 || isLambda || isGamma);
  }

  for(int iv=0; iv<NTracks; iv++)
  {
    if(!saveParticle[iv]) continue;
  
    mother.GetKFParticle(mother_temp, iv);
    if( mother.PDG()[iv] == 421 ) 
    {
      fD0.push_back(mother_temp);
      continue;
    }
    if( mother.PDG()[iv] == -421 ) 
    {
      fD0bar.push_back(mother_temp);
      continue;
    }
    int motherId = Particles.size();
    mother_temp.SetId(Particles.size());
    Particles.push_back(mother_temp);
    
    if( mother.PDG()[iv] == 22 && isPrimary[iv] )
    {
      float negPt2 = negDaughter.Px()[iv]*negDaughter.Px()[iv] + negDaughter.Py()[iv]*negDaughter.Py()[iv];
      float posPt2 = posDaughter.Px()[iv]*posDaughter.Px()[iv] + posDaughter.Py()[iv]*posDaughter.Py()[iv];

      if( (negPt2 >fCutLVMPt*fCutLVMPt) && (posPt2 >fCutLVMPt*fCutLVMPt) )
      {
        mother_temp.SetPDG(100113);
        mother_temp.SetId(Particles.size());
        Particles.push_back(mother_temp);
        
        if( (negPt2 >fCutJPsiPt*fCutJPsiPt) && (posPt2 >fCutJPsiPt*fCutJPsiPt) )
        {
          mother_temp.SetPDG(443);
          mother_temp.SetId(Particles.size());
          Particles.push_back(mother_temp);
        }
      }  
    }

    if( mother.PDG()[iv] == 200113 )
    {
      float negPt2 = negDaughter.Px()[iv]*negDaughter.Px()[iv] + negDaughter.Py()[iv]*negDaughter.Py()[iv];
      float posPt2 = posDaughter.Px()[iv]*posDaughter.Px()[iv] + posDaughter.Py()[iv]*posDaughter.Py()[iv];
      
      if( (negPt2 >fCutJPsiPt*fCutJPsiPt) && (posPt2 >fCutJPsiPt*fCutJPsiPt) )
      {
        mother_temp.SetPDG(100443);
        mother_temp.SetId(Particles.size());
        Particles.push_back(mother_temp);
      }  
    }
    
    if(saveMother[iv])
    {
      mother.SetId(motherId);
      motherPrimSecCand.SetOneEntry(nPrimSecCand,mother,iv);

      nPrimSecCand++;
      if(nPrimSecCand==float_vLen)
      {
        SaveV0PrimSecCand(motherPrimSecCand,nPrimSecCand,mother_temp,PrimVtx,secCuts,vMotherPrim,vMotherSec);
        nPrimSecCand = 0;
      }
    }
  }
}

inline void KFParticleFinder::SaveV0PrimSecCand(KFParticleSIMD& mother,
                                                int& NParticles,
                                                KFParticle& mother_temp,
                                                KFParticleSIMD* PrimVtx,
                                                const float* secCuts,
                                                vector< vector<KFParticle> >* vMotherPrim,
                                                vector<KFParticle>* vMotherSec)
{
  KFParticleSIMD motherTopo;
  float_v massMotherPDG, massMotherPDGSigma;
  
  float_m isSec(false);
  float_m isPrim(false);
  vector<int> iPrimVert[float_vLen];

  KFParticleDatabase::Instance()->GetMotherMass(mother.PDG(),massMotherPDG,massMotherPDGSigma);
  
  float_m isK0        = (mother.PDG() == 310);
  float_m isLambda    = (abs(mother.PDG()) == 3122);
  float_m isGamma     = (mother.PDG() == 22);

  int_v arrayIndex(-1); //for saving primary candidates; 

  arrayIndex(mother.PDG() ==   310) = 0;
  arrayIndex(mother.PDG() ==  3122) = 1;
  arrayIndex(mother.PDG() == -3122) = 2;
  arrayIndex(mother.PDG() ==    22) = 3;

  float_m isPrimaryPart(false);

  for(int iP=0; iP< fNPV; iP++)
  {
    motherTopo = mother;
    motherTopo.SetProductionVertex(PrimVtx[iP]);
    
    const float_v& motherTopoChi2Ndf = motherTopo.GetChi2()/motherTopo.GetNDF();
    const float_m isPrimaryPartLocal = ( motherTopoChi2Ndf < secCuts[1] );
    if(isPrimaryPartLocal.isEmpty()) continue;
    isPrimaryPart |= isPrimaryPartLocal;
    for(int iV=0; iV<float_vLen; iV++)
    {
      if(isPrimaryPartLocal[iV])
        iPrimVert[iV].push_back(iP);
    }
  }
  
  isPrim |= ( ( isPrimaryPart ) && (isK0 || isLambda || isGamma) );
  isSec  |= ( (!isPrimaryPart ) && (isK0 || isLambda || isGamma) );
  
  mother.SetNonlinearMassConstraint(massMotherPDG);

  for(int iv=0; iv<NParticles; iv++)
  { 
    if(isPrim[iv] || isSec[iv])
    {  
      mother.GetKFParticle(mother_temp, iv);
      
      if(isPrim[iv] )
      {
        for(unsigned int iP = 0; iP<iPrimVert[iv].size(); iP++)
          vMotherPrim[arrayIndex[iv]][iPrimVert[iv][iP]].push_back(mother_temp);
      }
      
      if(isSec[iv] )
        vMotherSec[arrayIndex[iv]].push_back(mother_temp);
    }
  }
}

void KFParticleFinder::Find2DaughterDecay(KFPTrackVector* vTracks, kfvector_float* ChiToPrimVtx,
                                          vector<KFParticle>& Particles,
                                          KFParticleSIMD* PrimVtx,
                                          const float* cuts,
                                          const float* secCuts,
                                          vector< vector<KFParticle> >* vMotherPrim,
                                          vector<KFParticle>* vMotherSec )
{ 
  KFParticle mother_temp;
  KFParticleSIMD mother;
  vector<float_v> l(fNPV), dl(fNPV);

  KFParticleSIMD daughterNeg, daughterPos;

    
  // for secondary V0
  unsigned short nBufEntry = 0;
  float_v dS;
  uint_v idNegDaughters;
  uint_v idPosDaughters;
  int_v daughterPosPDG(-1);
  int_v daughterNegPDG(-1);
    
  int_v pvIndexMother(-1);
  
  float_v massMotherPDG(Vc::Zero), massMotherPDGSigma(Vc::Zero);
  int_v V0PDG(Vc::Zero);

  KFParticleSIMD motherPrimSecCand;
  int nPrimSecCand =0;
  
  int trTypeIndexPos[2] = {0,2};
  int trTypeIndexNeg[2] = {1,3};

  for( int iTrTypeNeg = 0; iTrTypeNeg<2; iTrTypeNeg++)
  {
    KFPTrackVector& negTracks = vTracks[ trTypeIndexNeg[iTrTypeNeg] ];
    
    for(int iTrTypePos=0; iTrTypePos<2; iTrTypePos++)
    {
      KFPTrackVector& posTracks = vTracks[ trTypeIndexPos[iTrTypePos] ];
      int_v negTracksSize = negTracks.Size();
      int nPositiveTracks = posTracks.Size();
      
      //track categories
      int nTC = 5;
      int startTCPos[5] = {0};
      int endTCPos[5] = {0};
      int startTCNeg[5] = {0};
      int endTCNeg[5] = {0};
      
      if((iTrTypeNeg == 0) && (iTrTypePos == 0))
      {
        // Secondary particles
        nTC = 5;
        // e-
        startTCPos[0] = 0; endTCPos[0] = posTracks.LastElectron();
        startTCNeg[0] = 0; endTCNeg[0] = negTracks.LastElectron(); 
        //mu-
        startTCPos[1] = 0; endTCPos[1] = 0;
        startTCNeg[1] = 0; endTCNeg[1] = 0; 
        //pi- + ghosts
        startTCPos[2] = posTracks.FirstPion(); endTCPos[2] = nPositiveTracks;
        startTCNeg[2] = negTracks.FirstPion(); endTCNeg[2] = negTracks.LastPion();        
        //K-
        startTCPos[3] = posTracks.FirstPion(); endTCPos[3] = posTracks.LastPion();
        startTCNeg[3] = negTracks.FirstKaon(); endTCNeg[3] = negTracks.LastKaon();  
        //p-
        startTCPos[4] = posTracks.FirstPion(); endTCPos[4] = posTracks.LastPion();
        startTCNeg[4] = negTracks.FirstProton(); endTCNeg[4] = negTracksSize[0];  
      }
      
      if( iTrTypeNeg != iTrTypePos )
      {
        //Mixed particles - only gamma -> e+ e-
        nTC = 1;
        startTCPos[0] = 0; endTCPos[0] = posTracks.LastElectron();
        startTCNeg[0] = 0; endTCNeg[0] = negTracks.LastElectron();     
      }
      
      if((iTrTypeNeg == 1) && (iTrTypePos == 1))
      {
        //primary particles
        nTC = 5;
        // e-
        startTCPos[0] = 0; endTCPos[0] = posTracks.LastElectron();
        startTCNeg[0] = 0; endTCNeg[0] = negTracks.LastElectron(); 
        //mu-
        startTCPos[1] = posTracks.FirstMuon(); endTCPos[1] = posTracks.LastMuon();
        startTCNeg[1] = negTracks.FirstMuon(); endTCNeg[1] = negTracks.LastMuon(); 
        //pi- + ghosts
        startTCPos[2] = posTracks.FirstKaon(); endTCPos[2] = posTracks.LastKaon();
        startTCNeg[2] = negTracks.FirstPion(); endTCNeg[2] = negTracks.LastPion();        
        //K-
        startTCPos[3] = posTracks.FirstPion(); endTCPos[3] = nPositiveTracks;
        startTCNeg[3] = negTracks.FirstKaon(); endTCNeg[3] = negTracks.LastKaon();  
        //p-
        startTCPos[4] = posTracks.FirstKaon(); endTCPos[4] = posTracks.LastKaon();
        startTCNeg[4] = negTracks.FirstProton(); endTCNeg[4] = negTracksSize[0];      
      }
      
      for(int iTC=0; iTC<nTC; iTC++)
      {
        for(unsigned short iTrN=startTCNeg[iTC]; iTrN < endTCNeg[iTC]; iTrN += float_vLen)
        {
          const unsigned short NTracksNeg = (iTrN + float_vLen < negTracks.Size()) ? float_vLen : (negTracks.Size() - iTrN);

          int_v negInd = int_v::IndexesFromZero() + int(iTrN);

          int_v negPDG = reinterpret_cast<const int_v&>(negTracks.PDG()[iTrN]);
          int_v negPVIndex = reinterpret_cast<const int_v&>(negTracks.PVIndex()[iTrN]);
          
          int_v trackPdgNeg = negPDG;
          int_m activeNeg = (negPDG != -1);
          
          if( !((negPDG == -1).isEmpty()) )
          {
            trackPdgNeg(negPVIndex<0 && (negPDG == -1) ) = -211;
                
            activeNeg |= int_m(negPVIndex < 0) && int_m(negPDG == -1) ;
          }
          
          activeNeg &= (int_v::IndexesFromZero() < int(NTracksNeg));
              
          daughterNeg.Load(negTracks, iTrN, negPDG);

          float_v chiPrimNeg(Vc::Zero);
          float_v chiPrimPos(Vc::Zero);
          
          if( (iTrTypeNeg == 0) && (iTrTypePos == 0) )
            chiPrimNeg = reinterpret_cast<const float_v&>( ChiToPrimVtx[trTypeIndexNeg[iTrTypeNeg]][iTrN]);
          
          for(unsigned short iTrP=startTCPos[iTC]; iTrP < endTCPos[iTC]; iTrP += float_vLen)
          {
            const unsigned short NTracks = (iTrP + float_vLen < nPositiveTracks) ? float_vLen : (nPositiveTracks - iTrP);

            const int_v& posPDG = reinterpret_cast<const int_v&>(posTracks.PDG()[iTrP]);
            const int_v& posPVIndex = reinterpret_cast<const  int_v&>(posTracks.PVIndex()[iTrP]);              
            const int_m& isPosSecondary = (posPVIndex < 0);

            daughterPos.Load(posTracks, iTrP, posPDG);
            
            if( (iTrTypeNeg == 0) && (iTrTypePos == 0) )
              chiPrimPos = reinterpret_cast<const float_v&>( ChiToPrimVtx[trTypeIndexPos[iTrTypePos]][iTrP]);
            
            for(int iRot = 0; iRot<float_vLen; iRot++)
            {
              if(iRot>0)
              {
                negPDG = negPDG.rotated(1);
                negPVIndex = negPVIndex.rotated(1);
                negInd = negInd.rotated(1);
                trackPdgNeg = trackPdgNeg.rotated(1);
              
                daughterNeg.Rotate();
                chiPrimNeg = chiPrimNeg.rotated(1);

                activeNeg = ( (negPDG != -1) || ( (negPVIndex < 0) && (negPDG == -1) ) ) && (negInd < negTracksSize);
              }
              const int_m& isSecondary = int_m( negPVIndex < 0 ) && isPosSecondary;
              const int_m& isPrimary   = int_m( negPVIndex >= 0 ) && (!isPosSecondary);
            
              float_m closeDaughters = activeNeg && (int_v::IndexesFromZero() < int(NTracks));

              float_v ds(Vc::Zero), dsPos(Vc::Zero);
              if(!( (iTrTypePos == 1) && (iTrTypeNeg == 1) ) )
              { 
                float_v par1[8], cov1[36], par2[8], cov2[36];
                daughterNeg.GetDStoParticle(daughterPos, ds, dsPos);
                daughterNeg.Transport(ds,par1,cov1);
                daughterPos.Transport(dsPos,par2,cov2);
          
                const float_v& dx = par1[0] - par2[0];
                const float_v& dy = par1[1] - par2[1];
                const float_v& dz = par1[2] - par2[2];
                const float_v& r2 = dx*dx + dy*dy + dz*dz;
                
                const float_v vtx[3] = {(par1[0] + par2[0])/2.f,
                                        (par1[1] + par2[1])/2.f,
                                        (par1[2] + par2[2])/2.f, };
        
                daughterNeg.CorrectErrorsOnS(par1, vtx, cov1);
                daughterPos.CorrectErrorsOnS(par2, vtx, cov2);
                
                const float_v cov[6] = {cov1[0]+cov2[0],
                                        cov1[1]+cov2[1],
                                        cov1[2]+cov2[2],
                                        cov1[3]+cov2[3],
                                        cov1[4]+cov2[4],
                                        cov1[5]+cov2[5] };
                const float_v& err2 = cov[0]*dx*dx + cov[2]*dy*dy + cov[5]*dz*dz + 2.f*( cov[1]*dx*dy + cov[3]*dx*dz + cov[4]*dy*dz );
  
                closeDaughters &= (r2*r2/err2) < float_v(3.f);
//                 closeDaughters &= (r2 < float_v(1.f));
              }
              if(closeDaughters.isEmpty()) continue;
              
              
              int_v trackPdgPos[2];
              int_m active[2];

              int nPDGPos = 2;
              
              active[0] = (posPDG != -1);
              active[0] &= ((isPrimary && (posPVIndex == negPVIndex)) || !(isPrimary));

              active[1] = int_m(false);
              
              trackPdgPos[0] = posPDG;
              
              if( (posPDG == -1).isEmpty() )
              {
                nPDGPos = 1;
              }
              else
              {
                trackPdgPos[0](isSecondary && posPDG == -1) = 211;
                trackPdgPos[1] = 2212;
                
                active[0] |= isSecondary && int_m(posPDG == -1);
                active[1]  = isSecondary && int_m(posPDG == -1);
              }

              active[0] &= closeDaughters;
              active[1] &= closeDaughters;

              for(int iPDGPos=0; iPDGPos<nPDGPos; iPDGPos++)
              {
                if(active[iPDGPos].isEmpty()) continue;
                
                //detetrmine a pdg code of the mother particle
                
                int_v motherPDG(-1);
                
                if(iTC==0)
                  motherPDG(                (abs(trackPdgPos[iPDGPos])==   11) && int_m(abs(trackPdgNeg) ==   11) ) =    22; //gamma -> e+ e-
                else if(iTC==1)
                  motherPDG( isPrimary   && (abs(trackPdgPos[iPDGPos])==   13) && int_m(abs(trackPdgNeg) ==   13) ) =   200113; //rho -> mu+ mu-
                else if(iTC==2)
                {
                  motherPDG( isSecondary && (abs(trackPdgPos[iPDGPos])==  211) && int_m(abs(trackPdgNeg) ==  211) ) =   310; //K0 -> pi+ pi-
                  motherPDG( isSecondary && (abs(trackPdgPos[iPDGPos])== 2212) && int_m(abs(trackPdgNeg) ==  211) ) =  3122; //Lambda -> p pi-
                  motherPDG( isSecondary && (abs(trackPdgPos[iPDGPos])==  321) && int_m(abs(trackPdgNeg) ==  211) ) =  -421; //D0_bar -> pi- K+        
                  motherPDG( isPrimary   && (abs(trackPdgPos[iPDGPos])==  321) && int_m(abs(trackPdgNeg) ==  211) ) =   313; //K*0 -> K+ pi-
  //               motherPDG( isPrimary   && (abs(trackPdgPos[iPDGPos])==  211) && int_m(abs(trackPdgNeg) ==  211) ) =   113; //rho -> pi+ pi-
                }
                else if(iTC==3)
                {
                  motherPDG( isSecondary && (abs(trackPdgPos[iPDGPos])==  211) && int_m(abs(trackPdgNeg) ==  321) ) =   421; //D0 -> pi+ K-
                  motherPDG( isPrimary   && (abs(trackPdgPos[iPDGPos])==  211) && int_m(abs(trackPdgNeg) ==  321) ) =  -313; //K*0_bar -> K- pi+
                  motherPDG( isPrimary   && (abs(trackPdgPos[iPDGPos])== 2212) && int_m(abs(trackPdgNeg) ==  321) ) =  3124; //Lambda* -> p K-
                  motherPDG( isPrimary   && (abs(trackPdgPos[iPDGPos])==  321) && int_m(abs(trackPdgNeg) ==  321) ) =   333; //phi -> K+ K-
                }
                else if(iTC==4)
                {
                  motherPDG( isSecondary && (abs(trackPdgPos[iPDGPos])==  211) && int_m(abs(trackPdgNeg) == 2212) ) = -3122; //Lambda_bar -> p- pi+
                  motherPDG( isPrimary   && (abs(trackPdgPos[iPDGPos])==  321) && int_m(abs(trackPdgNeg) == 2212) ) = -3124; //Lambda*_bar -> p- K+
                }

                if( (iTrTypeNeg == 0) && (iTrTypePos == 0) )
                {
                  active[iPDGPos] &= int_m(chiPrimNeg > fCuts2D[0]) && int_m(chiPrimPos > fCuts2D[0]);
                }
                
                active[iPDGPos] &= (motherPDG != -1);
                if(active[iPDGPos].isEmpty()) continue;

                if( !((abs(motherPDG) == 421).isEmpty()) )
                {
                  const float_v& ptNeg2 = daughterNeg.Px()*daughterNeg.Px() + daughterNeg.Py()*daughterNeg.Py();
                  const float_v& ptPos2 = daughterPos.Px()*daughterPos.Px() + daughterPos.Py()*daughterPos.Py();
                  active[iPDGPos] &= ( (abs(motherPDG) == 421) && 
                                      int_m(ptNeg2 >= fCutCharmPt*fCutCharmPt) && 
                                      int_m(ptPos2 >= fCutCharmPt*fCutCharmPt) &&
                                      int_m(chiPrimNeg > fCutCharmChiPrim) && int_m(chiPrimPos > fCutCharmChiPrim) )
                                    || (!(abs(motherPDG) == 421));
                }
                if( !((abs(motherPDG) == 200113).isEmpty()) )
                {
                  const float_v& ptNeg2 = daughterNeg.Px()*daughterNeg.Px() + daughterNeg.Py()*daughterNeg.Py();
                  const float_v& ptPos2 = daughterPos.Px()*daughterPos.Px() + daughterPos.Py()*daughterPos.Py();
                  const float_v& pNeg2 = ptNeg2 + daughterNeg.Pz()*daughterNeg.Pz();
                  const float_v& pPos2 = ptPos2 + daughterPos.Pz()*daughterPos.Pz();
                  
                  active[iPDGPos] &= ( (abs(motherPDG) == 200113) && 
                                      int_m(ptNeg2 >= fCutLVMPt*fCutLVMPt) && 
                                      int_m(ptPos2 >= fCutLVMPt*fCutLVMPt) && 
                                      int_m(pNeg2 >= fCutLVMP*fCutLVMP) && 
                                      int_m(pPos2 >= fCutLVMP*fCutLVMP)) 
                                    || (!(abs(motherPDG) == 200113));
                }
                
                if(active[iPDGPos].isEmpty()) continue;

                for(int iV=0; iV<NTracks; iV++)
                {
                  if(!(active[iPDGPos][iV])) continue;
                  

                  idPosDaughters[nBufEntry] = iTrP+iV;
                  idNegDaughters[nBufEntry] = negInd[iV];
                  
                  daughterPosPDG[nBufEntry] = trackPdgPos[iPDGPos][iV];
                  daughterNegPDG[nBufEntry] = trackPdgNeg[iV];
                  
                  pvIndexMother[nBufEntry] = isPrimary[iV] ? negPVIndex[iV] : -1;
                  
                  V0PDG[nBufEntry] = motherPDG[iV];
                  
                  dS[nBufEntry] = ds[iV];

                  nBufEntry++;

                  if(nBufEntry == float_vLen)
                  {
                    KFParticleDatabase::Instance()->GetMotherMass(V0PDG,massMotherPDG,massMotherPDGSigma);
                    mother.SetPDG( V0PDG );
                    ConstructV0(vTracks, trTypeIndexPos[iTrTypePos], trTypeIndexNeg[iTrTypeNeg],                
                                idPosDaughters, idNegDaughters, daughterPosPDG, daughterNegPDG,
                                mother, mother_temp,
                                nBufEntry, l, dl, Particles, PrimVtx,
                                cuts, pvIndexMother, secCuts, massMotherPDG,
                                massMotherPDGSigma, motherPrimSecCand, nPrimSecCand, vMotherPrim, vMotherSec);
                    nBufEntry = 0; 
                  }
                }//iV
              }//iPDGPos
            }//iRot
          }//iTrP
        }//iTrN
        
        if( nBufEntry>0 )
        {
          for(int iV=nBufEntry; iV<float_vLen; iV++)
          {
            idPosDaughters[iV] = idPosDaughters[0];
            idNegDaughters[iV] = idNegDaughters[0];
          }

          KFParticleDatabase::Instance()->GetMotherMass(V0PDG,massMotherPDG,massMotherPDGSigma);
          mother.SetPDG( V0PDG );

          ConstructV0(vTracks, trTypeIndexPos[iTrTypePos], trTypeIndexNeg[iTrTypeNeg],              
                      idPosDaughters, idNegDaughters, daughterPosPDG, daughterNegPDG,
                      mother, mother_temp,
                      nBufEntry, l, dl, Particles, PrimVtx,
                      cuts, pvIndexMother, secCuts, massMotherPDG,
                      massMotherPDGSigma, motherPrimSecCand, nPrimSecCand, vMotherPrim, vMotherSec);
          nBufEntry = 0; 
        }
        
        if(nPrimSecCand>0)
        {
          SaveV0PrimSecCand(motherPrimSecCand,nPrimSecCand,mother_temp,PrimVtx,secCuts,vMotherPrim,vMotherSec);
          nPrimSecCand = 0;
        }
      }//iTC
    }//iTrTypeNeg
  }//iTrTypePos
}

void KFParticleFinder::ConstructTrackV0Cand(KFPTrackVector& vTracks,
                                            uint_v& idTracks,
                                            int_v& trackPDG,
                                            KFParticle* vV0[],
                                            KFParticleSIMD& mother,
                                            KFParticleSIMD* motherTopo,
                                            KFParticle& mother_temp,
                                            const unsigned short nElements,
                                            std::vector<float_v>& l,
                                            std::vector<float_v>& dl,
                                            std::vector<KFParticle>& Particles,
                                            KFParticleSIMD* PrimVtx,
                                            const float_v* cuts,
                                            const int_v& pvIndex,
                                            const float_v& massMotherPDG,
                                            const float_v& massMotherPDGSigma,
                                            std::vector< std::vector<KFParticle> >* vMotherPrim,
                                            std::vector<KFParticle>* vMotherSec)
{
  float_m isPrimary(pvIndex>-1);
  
  int_v trackId( &(vTracks.Id()[0]), idTracks );
  KFParticleSIMD V0(vV0,nElements);
  KFParticleSIMD track(vTracks, idTracks, trackPDG);
  track.SetId(trackId);
  
  float_v vtxGuess[3];
  float_v vtxErrGuess[3];
  if( !(isPrimary.isFull()) )
  {
    V0.GetVertexApproximation(track, vtxGuess, vtxErrGuess);
  }
  if(!(isPrimary.isEmpty()))
  {
    int iPV=pvIndex[0];
    if( (pvIndex == iPV).isFull() )
    {
      vtxGuess[0](isPrimary) = PrimVtx[iPV].X();
      vtxGuess[1](isPrimary) = PrimVtx[iPV].Y();
      vtxGuess[2](isPrimary) = PrimVtx[iPV].Z();
    
      vtxErrGuess[0](isPrimary) = 100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[0]);
      vtxErrGuess[1](isPrimary) = 100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[2]);
      vtxErrGuess[2](isPrimary) = 100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[5]);
    }
    else
    {
      for(int iV=0; iV<float_vLen; iV++)
      {
        if(!isPrimary[iV]) continue;
        if(iV >= nElements) continue;
        iPV = pvIndex[iV];
        vtxGuess[0][iV] = PrimVtx[iPV].X()[0];
        vtxGuess[1][iV] = PrimVtx[iPV].Y()[0];
        vtxGuess[2][iV] = PrimVtx[iPV].Z()[0];
    
        vtxErrGuess[0][iV] = 100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[0][0]);
        vtxErrGuess[1][iV] = 100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[2][0]);
        vtxErrGuess[2][iV] = 100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[5][0]);        
      }
    }
  }
  mother.SetVtxGuess(vtxGuess[0], vtxGuess[1], vtxGuess[2]);
  mother.SetVtxErrGuess(vtxErrGuess[0], vtxErrGuess[1], vtxErrGuess[2]);  
    
  const KFParticleSIMD* vDaughtersPointer[2] = {&V0, &track};
  if(isPrimary.isFull())
    mother.Construct(vDaughtersPointer, 2, 0, -1, 0, 1);
  else
    mother.Construct(vDaughtersPointer, 2, 0);
  
  float_m active(int_v::IndexesFromZero() < int(nElements));
  
  float_m saveParticle(active);
  saveParticle &= (mother.Chi2()/static_cast<float_v>(mother.NDF()) < cuts[2] );
  saveParticle &= KFPMath::Finite(mother.GetChi2());
  saveParticle &= (mother.GetChi2() > 0.0f);
  saveParticle &= (mother.GetChi2() == mother.GetChi2());

  if( saveParticle.isEmpty() ) { return; }

  int_m isSameTrack(false);
  for(unsigned short iD=0; iD<V0.DaughterIds().size(); iD++)
    isSameTrack |= ( int_v(V0.DaughterIds()[iD]) == int_v(trackId) );
  
  saveParticle &= ( !static_cast<float_m>(isSameTrack));
  if( saveParticle.isEmpty() ) { return; }        
      
  float_v lMin(1.e8f);
  float_v ldlMin(1.e8f);
  float_m isParticleFromVertex(false);

  for(int iP=0; iP<fNPV; iP++)
  {
    float_m isParticleFromVertexLocal;
    mother.GetDistanceToVertexLine(PrimVtx[iP], l[iP], dl[iP], &isParticleFromVertexLocal);
    isParticleFromVertex |= isParticleFromVertexLocal;
    float_v ldl = (l[iP]/dl[iP]);
    lMin( (l[iP] < lMin) && active) = l[iP];
    ldlMin( (ldl < ldlMin) && active) = ldl;
  }
//   saveParticle &= ( (float_m(!isPrimary) && ldlMin > cuts[0]) || float_m(isPrimary) );
  saveParticle &= (lMin < 200.f);
// 
// //         if(isPrimary && (float(ldlMin > 3) )) continue;
  saveParticle &= ((float_m(!isPrimary) && isParticleFromVertex) || float_m(isPrimary) );
  if( saveParticle.isEmpty() ) { return; }

  float_m isSameParticle = (abs(mother.PDG()) ==    4122) ||
                           (abs(mother.PDG()) ==     411) ||
                           (abs(mother.PDG()) ==     431) ||
                           (abs(mother.PDG()) ==     429) ||
                           (abs(mother.PDG()) == 1003334) ||
                           (abs(mother.PDG()) ==    3001) ||
                           isPrimary;
  if(!((isSameParticle).isFull()))
  {
    float_m isParticleFromVertexLocal;
    float_v l1, dl1;
    V0.GetDistanceToVertexLine(mother, l1, dl1, &isParticleFromVertexLocal);
    
    saveParticle &= ( isSameParticle || ((!isSameParticle) && isParticleFromVertexLocal));
    if( saveParticle.isEmpty() ) { return; }
  }
   
  ldlMin = 1.e8f;
  for(int iP=0; iP<fNPV; iP++)
  {
    motherTopo[iP] = mother;
    motherTopo[iP].SetProductionVertex(PrimVtx[iP]);
    motherTopo[iP].GetDecayLength(l[iP], dl[iP]);
    float_v ldl = (l[iP]/dl[iP]);
    ldlMin( (ldl < ldlMin) && active) = ldl;

  }
  saveParticle &= ( (float_m(!isPrimary) && ldlMin > cuts[0]) || float_m(isPrimary) );

  vector<int> iPrimVert[float_vLen];
  float_m isPrimaryPart(false);

  for(int iP=0; iP<fNPV; iP++)
  {
    const float_v& motherTopoChi2Ndf = motherTopo[iP].GetChi2()/motherTopo[iP].GetNDF();
    const float_m isPrimaryPartLocal = ( motherTopoChi2Ndf < cuts[1] );
    isPrimaryPart |= isPrimaryPartLocal;
    for(int iV=0; iV<float_vLen; iV++)
    {
      if(isPrimaryPartLocal[iV])
        iPrimVert[iV].push_back(iP);
    }
  }
      
  for(unsigned int iv=0; iv<nElements; iv++)
  {
    if(!saveParticle[iv]) continue; 
    
    mother.GetKFParticle(mother_temp, iv);
    if( mother.PDG()[iv] == 3312 )
    {
      fLPi.push_back(mother_temp);
      fLPiPIndex.push_back( V0.DaughterIds()[1][iv] );
    }
    
    if(  mother.PDG()[iv] == 100411 )
    {
      fK0PiPlus.push_back(mother_temp);
      fK0PiMinusIndex.push_back( V0.DaughterIds()[0][iv] );
      continue;
    }
    
    // reset daughter ids for 3- and 4-particle decays
    if( (abs(mother.PDG()[iv]) == 411)  ||
        (abs(mother.PDG()[iv]) == 427) ||
        (abs(mother.PDG()[iv]) == 429) ||
        (abs(mother.PDG()[iv]) == 431)  ||
        (abs(mother.PDG()[iv]) == 4122) ||
        (abs(mother.PDG()[iv]) == 1003334) ||
        (abs(mother.PDG()[iv]) == 3001) )
    {
      mother_temp.CleanDaughtersId();
      for(int iD=0; iD < vV0[iv]->NDaughters(); iD++)
        mother_temp.AddDaughterId( vV0[iv]->DaughterIds()[iD] );
      mother_temp.AddDaughterId(trackId[iv]);
    }
    
    if( mother.PDG()[iv] == 411 ) 
    {
      fDPlus.push_back(mother_temp);
      continue;
    }
    if( mother.PDG()[iv] == -411 ) 
    {
      fDMinus.push_back(mother_temp);
      continue;
    }
    
    if( mother.PDG()[iv] == 427 )
    {
      fK0PiPi.push_back(mother_temp);
      continue;
    }
    
    if( mother.PDG()[iv] == 429 ) 
    {
      fD04.push_back(mother_temp);
      continue;
    }
    if( mother.PDG()[iv] == -429 ) 
    {
      fD04bar.push_back(mother_temp);
      continue;
    }
    
    if(!(isPrimaryPart[iv]))
    {
      if( vMotherSec )
      {
        float mass, errMass;
        mother_temp.GetMass(mass, errMass);
        if(abs(mother.PDG()[0]) == 3324)
        {
          vMotherSec->push_back(mother_temp);
        }
        else
        {    
          mother_temp.SetNonlinearMassConstraint(massMotherPDG[0]);

          if( (fabs(mass - massMotherPDG[0])/massMotherPDGSigma[0]) <= 3 )
          vMotherSec->push_back(mother_temp);
        }
      }
      continue;
    }
    
    mother_temp.SetId(Particles.size());
    Particles.push_back(mother_temp);

    if(vMotherPrim)
    {
      if( !((abs(mother.GetPDG()[iv]) == 3312) || (abs(mother.GetPDG()[iv]) == 3324))) continue;
      float mass, errMass;

      mother_temp.GetMass(mass, errMass);
      if(abs(mother.PDG()[iv]) == 3324)
      {
        for(unsigned int iP=0; iP<iPrimVert[iv].size(); iP++)
          (*vMotherPrim)[iPrimVert[iv][iP]].push_back(mother_temp);
      }
      else
      {
        mother_temp.SetNonlinearMassConstraint(massMotherPDG[0]);

        if( (fabs(mass - massMotherPDG[0])/massMotherPDGSigma[0]) <= 3 )
          for(unsigned int iP=0; iP<iPrimVert[iv].size(); iP++)
            (*vMotherPrim)[iPrimVert[iv][iP]].push_back(mother_temp);
      }
    }
  }
}

void KFParticleFinder::FindTrackV0Decay(vector<KFParticle>& vV0,
                                        const int V0PDG,
                                        KFPTrackVector& vTracks,
                                        const int q,
                                        const int firstTrack,
                                        const int lastTrack,
                                        vector<KFParticle>& Particles,    
                                        KFParticleSIMD* PrimVtx,
                                        int v0PVIndex,
                                        kfvector_float* ChiToPrimVtx,
                                        vector< vector<KFParticle> >* vMotherPrim,
                                        vector<KFParticle>* vMotherSec)
{
  if( (vV0.size() < 1) || ((lastTrack-firstTrack) < 1) ) return;
    
  KFParticle mother_temp;

  KFParticle* v0Pointer[float_v::Size];
  
  KFParticleSIMD mother, track;
  KFParticleSIMD *motherTopo = new KFParticleSIMD[fNPV];
  
  vector<float_v> l(fNPV), dl(fNPV);

  float_v cuts[3];
  
  // for secondary V0
  unsigned short nBufEntry = 0;
  float_v dS;
  uint_v idTrack;
  int_v trackPDGMother(-1);
    
  int_v pvIndexMother(-1);
  
  float_v massMotherPDG(Vc::Zero), massMotherPDGSigma(Vc::Zero);
  int_v motherParticlePDG(Vc::Zero);

//   Particles.reserve(Particles.size() + vV0.size());

  bool isCharm = ((abs(V0PDG) == 421) || (abs(V0PDG) == 411) || (abs(V0PDG) == 429)) && (v0PVIndex<0);

  for(unsigned short iV0=0; iV0 < vV0.size(); iV0++)
  {    
    int iNegDaughter = vV0[iV0].DaughterIds()[0];
    int iPosDaughter = vV0[iV0].DaughterIds()[1];
    
    for(unsigned short iTr=firstTrack; iTr < lastTrack; iTr += float_vLen)
    {
      const unsigned short NTracks = (iTr + float_vLen < lastTrack) ? float_vLen : (lastTrack - iTr);

      const int_v& trackPDG = reinterpret_cast<const int_v&>(vTracks.PDG()[iTr]);
      const int_v& trackPVIndex = reinterpret_cast<const  int_v&>(vTracks.PVIndex()[iTr]);
      
      const int_m& isTrackSecondary = (trackPVIndex < 0);
      const int_m& isSecondary = int_m( v0PVIndex < 0 ) && isTrackSecondary;
      const int_m& isPrimary   = int_m( v0PVIndex >= 0 ) && (!isTrackSecondary);
      const int_m& isSamePV = (isPrimary && (v0PVIndex == trackPVIndex)) || !(isPrimary);

      float_m closeDaughters = float_m(isSamePV) && float_m(int_v::IndexesFromZero() < int(NTracks));

      if(v0PVIndex < 0)
      {
        KFParticleSIMD v0(vV0[iV0]);
        track.Load(vTracks, iTr, trackPDG);
     
        float_v dsV0, dsTrack;
        float_v par1[8], cov1[36], par2[8], cov2[36];
        v0.GetDStoParticle(track, dsV0, dsTrack);
        v0.Transport(dsV0,par1,cov1);
        track.Transport(dsTrack,par2,cov2);
  
        const float_v& dx = par1[0] - par2[0];
        const float_v& dy = par1[1] - par2[1];
        const float_v& dz = par1[2] - par2[2];
        const float_v& r2 = dx*dx + dy*dy + dz*dz;
        
        const float_v vtx[3] = {(par1[0] + par2[0])/2.f,
                                (par1[1] + par2[1])/2.f,
                                (par1[2] + par2[2])/2.f, };

        v0.CorrectErrorsOnS(par1, vtx, cov1);
        track.CorrectErrorsOnS(par2, vtx, cov2);
        
        const float_v cov[6] = {cov1[0]+cov2[0],
                                cov1[1]+cov2[1],
                                cov1[2]+cov2[2],
                                cov1[3]+cov2[3],
                                cov1[4]+cov2[4],
                                cov1[5]+cov2[5] };
        const float_v& err2 = cov[0]*dx*dx + cov[2]*dy*dy + cov[5]*dz*dz + 2.f*( cov[1]*dx*dy + cov[3]*dx*dz + cov[4]*dy*dz );
                
        closeDaughters &= ( (r2 < float_v(1.f)) && (r2*r2/err2) < float_v(3.f) && isSecondary);
      }
      if(closeDaughters.isEmpty()) continue;
      
      int_v trackPdgPos[2];
      int_m active[2];

      int nPDGPos = 2;
      
      active[0] = int_m(closeDaughters);
      active[1] = (trackPDG == -1) && isSecondary && closeDaughters;
      
      trackPdgPos[0] = trackPDG;
      
      if( (trackPDG == -1).isEmpty() || abs(V0PDG ==  421) )
      {
        nPDGPos = 1;
      }
      else
      {
        trackPdgPos[0](trackPDG == -1) = q*211;
        trackPdgPos[1](isSecondary) = q*321;
      }
      
      for(int iPDGPos=0; iPDGPos<nPDGPos; iPDGPos++)
      {
        
        if(active[iPDGPos].isEmpty()) continue;
        
        //detetrmine a pdg code of the mother particle
        
        int_v motherPDG(-1);
        
        if( V0PDG == 3122 )
        {
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  211) ) =  3312;
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  321) ) =  3334;
          motherPDG( isPrimary   && int_m(trackPdgPos[iPDGPos] ==  211) ) =  3224; 
          motherPDG( isPrimary   && int_m(trackPdgPos[iPDGPos] ==  -211) ) =  3114;
          motherPDG( isPrimary   && int_m(trackPdgPos[iPDGPos] == -321) ) =   1003314; 
        }
        else if( V0PDG == -3122 )
        {
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  321) ) = -3334;
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  211) ) = -3312;
          motherPDG( isPrimary   && int_m(trackPdgPos[iPDGPos] ==  -211) ) =  -3224; 
          motherPDG( isPrimary   && int_m(trackPdgPos[iPDGPos] ==   211) ) =  -3114;
          motherPDG( isPrimary   && int_m(trackPdgPos[iPDGPos] ==  321) ) =  -1003314; 
        }
        else if( V0PDG == 310)
        {
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] ==  211) ) =  323; 
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] ==  -211) ) =  -323; 
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  211) ) =  100411;
        }
        else if( V0PDG == 3312 )
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] ==  211) ) =  3324; 
        else if( V0PDG == -3312)
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] == -211) ) = -3324; 
        else if( V0PDG == 3324 )
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] == -321) ) =  1003334; 
        else if( V0PDG == -3324 )
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] ==  321) ) = -1003334;         
        else if(V0PDG ==  421)
        {
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  211) ) =  411;
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  321) ) =  431;
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) == 2212) ) =  4122;
          const int_v& id = reinterpret_cast<const int_v&>(vTracks.Id()[iTr]);
          int_m isDMeson = isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  211);
          active[iPDGPos] &= (!(isDMeson)) || (isDMeson && ( id > iPosDaughter) );
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] ==   211) ) =   10411; 
        }
        else if(V0PDG == -421)
        {
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  211) ) = -411;
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  321) ) = -431;
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) == 2212) ) = -4122;
          const int_v& id = reinterpret_cast<const int_v&>(vTracks.Id()[iTr]);
          int_m isDMeson = isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  211);
          active[iPDGPos] &= (!(isDMeson)) || (isDMeson && ( id > iNegDaughter) );
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] ==  -211) ) =  -10411; 
        }
        else if(V0PDG == 411)
        {
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  211) ) = 429;
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] ==  -211) ) =   10421; 
        }
        else if(V0PDG == -411)
        {
          motherPDG( isSecondary && int_m(abs(trackPdgPos[iPDGPos]) ==  211) ) = -429;
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] ==   211) ) =  -10421; 
        }
        else if(V0PDG == 429)      
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] ==   211) ) =   20411; 
        else if(V0PDG == -429)
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] ==  -211) ) =  -20411; 
        else if( V0PDG == 3002 )
        {
          const int_v& id = reinterpret_cast<const int_v&>(vTracks.Id()[iTr]);
          int_m isSameProton = (id == fLPiPIndex[iV0]);
          motherPDG( isSecondary && int_m(trackPdgPos[iPDGPos] == 2212) && (!isSameProton)) =  3001; 
        }
        else if( V0PDG == 100411 )
        {
          const int_v& id = reinterpret_cast<const int_v&>(vTracks.Id()[iTr]);
          int_m isSamePiMinus = (id == fK0PiMinusIndex[iV0]);
          motherPDG( isSecondary && int_m(trackPdgPos[iPDGPos] == -211) && (!isSamePiMinus)) =  427; 
        }
        else if( V0PDG == 111 )
        {
          motherPDG( isSecondary && int_m(trackPdgPos[iPDGPos] == 2212) ) =  3222; 
          motherPDG( isSecondary && int_m(trackPdgPos[iPDGPos] == -2212) ) =  -3222; 
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] == 321) ) =  100323; 
          motherPDG( isPrimary && int_m(trackPdgPos[iPDGPos] == -321) ) =  -100323; 
        }        
        active[iPDGPos] &= (motherPDG != -1);
        
        if(ChiToPrimVtx)
          active[iPDGPos] &= ( ( abs(motherPDG) != 3334) ||
                             ( abs(motherPDG) == 3334 && reinterpret_cast<const float_v&>((*ChiToPrimVtx)[iTr]) > 7 ) );
        
        if(active[iPDGPos].isEmpty()) continue;
        
        if(isCharm)
        {
          const float_v& trackPt = track.Px()*track.Px() + track.Py()*track.Py();
          active[iPDGPos] &= int_m(trackPt >= fCutCharmPt*fCutCharmPt) && int_m(reinterpret_cast<const float_v&>((*ChiToPrimVtx)[iTr]) > fCutCharmChiPrim );
        }
                  
        for(int iV=0; iV<NTracks; iV++)
        {
          if(!(active[iPDGPos][iV])) continue;
          

          idTrack[nBufEntry] = iTr+iV;
          v0Pointer[nBufEntry] = &vV0[iV0];
          
          trackPDGMother[nBufEntry] = trackPdgPos[iPDGPos][iV];
          
          pvIndexMother[nBufEntry] = v0PVIndex;
          
          float massMother, massMotherSigma;
          KFParticleDatabase::Instance()->GetMotherMass(motherPDG[iV],massMother,massMotherSigma);

          massMotherPDG[nBufEntry] = massMother;
          massMotherPDGSigma[nBufEntry] = massMotherSigma;
          motherParticlePDG[nBufEntry] = motherPDG[iV];
                    
          int motherType = 0;

          switch (abs(motherPDG[iV]))
          {
            case   3312: motherType = 0; break; //Xi
            case   3334: motherType = 1; break; //Omega
            case   4122: motherType = 1; break; //LambdaC
            case    431: motherType = 1; break; //Ds+-
            case    411: motherType = 1; break; //D+-
            case    428: motherType = 1; break; //D0
            case    429: motherType = 1; break; //D0
            case   3001: motherType = 1; break; //H0
            case   3222: motherType = 1; break; //Sigma+
            default:   motherType = 2; break; //resonances
          }
          for(int iCut=0; iCut<3; iCut++)
            cuts[iCut][nBufEntry] = fCutsTrackV0[motherType][iCut];

          nBufEntry++;

          if(nBufEntry == float_vLen)
          {
            mother.SetPDG( motherParticlePDG );
            ConstructTrackV0Cand(vTracks,   
                                 idTrack, trackPDGMother, v0Pointer,
                                 mother, motherTopo, mother_temp,
                                 nBufEntry, l, dl, Particles, PrimVtx,
                                 cuts, pvIndexMother, massMotherPDG,
                                 massMotherPDGSigma, vMotherPrim, vMotherSec);
            nBufEntry = 0; 
          }
        }//iV
      }//iPDGPos
    }//iTr
  }
  
  if(nBufEntry > 0)
  {
    for(int iV=nBufEntry; iV<float_vLen; iV++)
      idTrack[iV] = idTrack[0];
    
    mother.SetPDG( motherParticlePDG );
    ConstructTrackV0Cand(vTracks,  
                          idTrack, trackPDGMother, v0Pointer,
                          mother, motherTopo, mother_temp,
                          nBufEntry, l, dl, Particles, PrimVtx,
                          cuts, pvIndexMother, massMotherPDG,
                          massMotherPDGSigma, vMotherPrim, vMotherSec);
    nBufEntry = 0; 
  }
  if(motherTopo) delete [] motherTopo;
}

void KFParticleFinder::SelectParticles(vector<KFParticle>& Particles,
                                       vector<KFParticle>& vCandidates,
                                       KFParticleSIMD* PrimVtx,
                                       const float& cutChi2Topo,
                                       const float& cutLdL,
                                       const float& mass,
                                       const float& massErr,
                                       const float& massCut)
{
  KFParticle* cand[float_vLen];
  int nCand = vCandidates.size();
  
  vector<KFParticle> newCandidates;
  vector<float_v> l(fNPV), dl(fNPV);

  for(unsigned short iC=0; iC < nCand; iC += float_vLen)
  {
    unsigned int nEntries = (iC + float_vLen < nCand) ? float_vLen : (nCand - iC);

    for(unsigned short iv=0; iv<nEntries; iv++)
      cand[iv] = &vCandidates[iC+iv];

    KFParticleSIMD mother(cand,nEntries);
    
    float_m saveParticle(int_v::IndexesFromZero() < int(nEntries));

    float_v lMin(1.e8f);
    float_v ldlMin(1.e8f);
    float_m isParticleFromVertex(false);

    for(int iP=0; iP<fNPV; iP++)
    {
      float_m isParticleFromVertexLocal;
      mother.GetDistanceToVertexLine(PrimVtx[iP], l[iP], dl[iP], &isParticleFromVertexLocal);
      isParticleFromVertex |= isParticleFromVertexLocal;
      float_v ldl = (l[iP]/dl[iP]);
      lMin( (l[iP] < lMin) && saveParticle) = l[iP];
      ldlMin( (ldl < ldlMin) && saveParticle) = ldl;
    }
    saveParticle &= ldlMin > cutLdL;
    saveParticle &= (lMin < 200.f);
    saveParticle &= isParticleFromVertex;
    if( saveParticle.isEmpty() ) continue;

    KFParticleSIMD* candTopo = new KFParticleSIMD[fNPV];

    for(int iP=0; iP<fNPV; iP++)
    {
      candTopo[iP] = mother;
      candTopo[iP].SetProductionVertex(PrimVtx[iP]);
    }
    
    for(unsigned int iv=0; iv<nEntries; iv++)
    {
      if(!saveParticle[iv]) continue;
      
      bool isPrimary = 0;
      for(int iP=0; iP<fNPV; iP++)
      {
        if( !(KFPMath::Finite(candTopo[iP].GetChi2())[iv]) ) continue;
        if(!(candTopo[iP].GetChi2()[iv] > 0.0f)) continue;
        if(!(candTopo[iP].GetChi2()[iv]==candTopo[iP].GetChi2()[iv])) continue;
      
        if(float(candTopo[iP].GetChi2()[iv])/float(candTopo[iP].GetNDF()[iv]) <= cutChi2Topo )
          isPrimary = 1;
      }
      if(!isPrimary)
        continue;

      vCandidates[iC+iv].SetId(Particles.size());
      Particles.push_back(vCandidates[iC+iv]);
      
      float m, dm;
      vCandidates[iC+iv].GetMass(m,dm);
      if( (fabs(m - mass)/massErr) > massCut ) continue;

      vCandidates[iC+iv].SetNonlinearMassConstraint(mass);
      newCandidates.push_back(vCandidates[iC+iv]);
    }
    if(candTopo) delete [] candTopo;
  }
  
  vCandidates = newCandidates;
}

void KFParticleFinder::CombinePartPart(vector<KFParticle>& particles1,
                                       vector<KFParticle>& particles2,
                                       vector<KFParticle>& Particles,
                                       KFParticleSIMD* PrimVtx,
                                       const float* cuts,
                                       int iPV,
                                       const int MotherPDG,
                                       bool isSameInputPart,
                                       bool saveOnlyPrimary,
                                       vector< vector<KFParticle> >* vMotherPrim,
                                       vector<KFParticle>* vMotherSec,
                                       float massMotherPDG,
                                       float massMotherPDGSigma)
{
  if( (particles1.size() ==  0) || (particles2.size() ==  0) ) return;
  KFParticle mother_temp;
  KFParticleSIMD mother;
  KFParticleSIMD *motherTopo = new KFParticleSIMD[fNPV];
  mother.SetPDG( MotherPDG );

  vector<float_v> l(fNPV), dl(fNPV);

  KFParticle* tmpPart2[float_vLen];
  int nPart2 = particles2.size();

  bool isPrimary = (iPV >= 0);
  
  for(unsigned short iP1=0; iP1 < particles1.size(); iP1++)
  {
    KFParticleSIMD vDaughters[2] = {KFParticleSIMD(particles1[iP1]), KFParticleSIMD()};

    unsigned short startIndex=0;
    if(isSameInputPart) startIndex=iP1+1;
    for(unsigned short iP2=startIndex; iP2 < nPart2; iP2 += float_vLen)
    {
      unsigned int nElements = (iP2 + float_vLen < nPart2) ? float_vLen : (nPart2 - iP2);
      float_m active(int_v::IndexesFromZero() < int(nElements));

      for(unsigned short iv=0; iv<nElements; iv++)
        tmpPart2[iv] = &particles2[iP2+iv];

      vDaughters[1] = KFParticleSIMD(tmpPart2,nElements);

//       if( reconstructPi0 )
//       {
//         int indexOffset = fEmcClusters->Id()[0];
//         uint_v gammaIndex1( (unsigned int)0);
//         uint_v gammaIndex2( (unsigned int)0);
//         for(int iv=0; iv<nElements; iv++)
//         {
//           gammaIndex1[iv] = Particles[ particles2[iP2+iv].DaughterIds()[0] ].DaughterIds()[0] - indexOffset;
//           gammaIndex2[iv] = Particles[ particles2[iP2+iv].DaughterIds()[1] ].DaughterIds()[0] - indexOffset;
//         }
//         
//         KFParticleSIMD gamma1(*fEmcClusters, gammaIndex1, vDaughters[0]);
//         KFParticleSIMD gamma2(*fEmcClusters, gammaIndex2, vDaughters[0]);
//         const KFParticleSIMD* pi0Daughters[2] = {&gamma1, &gamma2};
//         
//         int_v gammaId = vDaughters[1].Id();
//         vDaughters[1].SetVtxGuess(vDaughters[0].X(), vDaughters[0].Y(), vDaughters[0].Z());
//         vDaughters[1].Construct(pi0Daughters, 2);
//         vDaughters[1].SetId(gammaId);
//         
//         float_v mass, dm;
//         vDaughters[1].GetMass(mass,dm);
//         const float& mPi0 = KFParticleDatabase::Instance()->GetPi0Mass();
//         const float& mPi0Sigma = KFParticleDatabase::Instance()->GetPi0MassSigma();
//         active &= (abs(mass - mPi0)/mPi0Sigma) < 3.f;
//         vDaughters[1].SetNonlinearMassConstraint(mPi0);
//         if(active.isEmpty()) continue;
//       }
      
      if(isPrimary)
      {
        float_v errGuess[3] = {100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[0]),
                               100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[2]),
                               100.f*sqrt(PrimVtx[iPV].CovarianceMatrix()[5])};
        mother.SetVtxGuess(PrimVtx[iPV].X(), PrimVtx[iPV].Y(), PrimVtx[iPV].Z());
        mother.SetVtxErrGuess(errGuess[0], errGuess[1], errGuess[2]);
        const KFParticleSIMD* vDaughtersPointer[2] = {&vDaughters[0], &vDaughters[1]};
        mother.Construct(vDaughtersPointer, 2, 0, -1, 0, 0);
      }
      else
      {
        const KFParticleSIMD* vDaughtersPointer[2] = {&vDaughters[0], &vDaughters[1]};
        mother.Construct(vDaughtersPointer, 2, 0);
      }
  
      float_m saveParticle(active);
      saveParticle &= (mother.Chi2()/static_cast<float_v>(mother.NDF()) < cuts[2] );
      saveParticle &= KFPMath::Finite(mother.GetChi2());
      saveParticle &= (mother.GetChi2() >= 0.0f);
      saveParticle &= (mother.GetChi2() == mother.GetChi2());
      
      if( saveParticle.isEmpty() ) { continue; }

      int_m isSameTrack(false);
      for(unsigned short iD=0; iD<vDaughters[0].DaughterIds().size(); iD++)
        for(unsigned short iD1=0; iD1<vDaughters[1].DaughterIds().size(); iD1++)
          isSameTrack |= ( int_v(vDaughters[0].DaughterIds()[iD]) == int_v(vDaughters[1].DaughterIds()[iD1]) );
      saveParticle &= ( !static_cast<float_m>(isSameTrack));
      if( saveParticle.isEmpty() ) { continue; }        
      
      float_v lMin(1.e8f);
      float_v ldlMin(1.e8f);
      float_m isParticleFromVertex(false);

      for(int iP=0; iP<fNPV; iP++)
      {
        if( (iPV > -1) && (iP !=iPV) ) continue;
        float_m isParticleFromVertexLocal;
        mother.GetDistanceToVertexLine(PrimVtx[iP], l[iP], dl[iP], &isParticleFromVertexLocal);
        isParticleFromVertex |= isParticleFromVertexLocal;
        float_v ldl = (l[iP]/dl[iP]);
        lMin( (l[iP] < lMin) && active) = l[iP];
        ldlMin( (ldl < ldlMin) && active) = ldl;
      }
      saveParticle &= ( (float_m(!isPrimary) && ldlMin > cuts[0]) || float_m(isPrimary) );
      saveParticle &= (lMin < 200.f);
    
//         if(isPrimary && (float(ldlMin > 3) )) continue;
      saveParticle &= ((float_m(!isPrimary) && isParticleFromVertex) || float_m(isPrimary) );
      if( saveParticle.isEmpty() ) { continue; }

      float_m isSameParticle(isPrimary);
      if(!((isSameParticle).isFull()))
      {
        float_m isParticleFromVertexLocal;
        float_v l1, dl1;
        vDaughters[0].GetDistanceToVertexLine(mother, l1, dl1, &isParticleFromVertexLocal);
        
        saveParticle &= ( isSameParticle || ((!isSameParticle) && isParticleFromVertexLocal));
        if( saveParticle.isEmpty() ) { continue; }
      }
  
      for(int iP=0; iP<fNPV; iP++)
      {
        if( (iPV > -1) && (iP !=iPV) ) continue;
        motherTopo[iP] = mother;
        motherTopo[iP].SetProductionVertex(PrimVtx[iP]);
      }
  
      vector<int> iPrimVert[float_vLen];
      float_m isPrimaryPart(false);

      for(int iP=0; iP<fNPV; iP++)
      {
        if( (iPV > -1) && (iP !=iPV) ) continue;
        const float_v& motherTopoChi2Ndf = motherTopo[iP].GetChi2()/motherTopo[iP].GetNDF();
        const float_m isPrimaryPartLocal = ( motherTopoChi2Ndf < cuts[1] );
        isPrimaryPart |= isPrimaryPartLocal;
        for(int iV=0; iV<float_vLen; iV++)
        {
          if(isPrimaryPartLocal[iV])
            iPrimVert[iV].push_back(iP);
        }
      }
      
      for(unsigned int iv=0; iv<nElements; iv++)
      {
        if(!saveParticle[iv]) continue; 
        
        mother.GetKFParticle(mother_temp, iv);

        // reset daughter ids for 3- and 4-particle decays
        if( (abs(mother.PDG()[iv]) == 428) )
        {
          mother_temp.CleanDaughtersId();
          for(int iD=0; iD < particles1[iP1].NDaughters(); iD++)
            mother_temp.AddDaughterId( particles1[iP1].DaughterIds()[iD] );
          mother_temp.AddDaughterId(tmpPart2[iv]->Id());
        }
    
        if(saveOnlyPrimary)
        {
          if(isPrimaryPart[iv])
          {
            mother_temp.SetId(Particles.size());
            Particles.push_back(mother_temp);
          }
        }
        else
        {
          mother_temp.SetId(Particles.size());
          Particles.push_back(mother_temp);
        }
        
        if(vMotherPrim || vMotherSec)
        {
          float mass, errMass;
          mother_temp.GetMass(mass, errMass);
          if( (fabs(mass - massMotherPDG)/massMotherPDGSigma) > 3.f ) continue;
          mother_temp.SetNonlinearMassConstraint(massMotherPDG);
          
          if(MotherPDG == 428)
          {
            mother_temp.CleanDaughtersId();
            for(int iD=0; iD < tmpPart2[iv]->NDaughters(); iD++)
              mother_temp.AddDaughterId( tmpPart2[iv]->DaughterIds()[iD] );
            for(int iD=0; iD < particles1[iP1].NDaughters(); iD++)
              mother_temp.AddDaughterId( particles1[iP1].DaughterIds()[iD] );            
          }
          
          if(vMotherSec && (!(isPrimaryPart[iv])) )
            vMotherSec->push_back(mother_temp);
          if(vMotherPrim)
            for(unsigned int iP=0; iP<iPrimVert[iv].size(); iP++)
              (*vMotherPrim)[iPrimVert[iv][iP]].push_back(mother_temp);
        }
      }
    }
  }
  
  if(motherTopo) delete [] motherTopo;
}
