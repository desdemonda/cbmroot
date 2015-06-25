/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction
 *
 *  Authors: M.Zyzak
 *
 *  e-mail :
 *
 *=====================================================
 *
 *  SIMD Fitter for CbmL1Track class
 *
 */

#include "CbmL1PFFitter.h"

#include "CbmL1.h"
#include "TClonesArray.h"
#include "CbmStsTrack.h"
#include "CbmStsAddress.h"

//L1Algo tools
#include "L1Algo.h"
#include "CbmL1Track.h"
#include "L1TrackPar.h"
#include "L1Station.h"
#include "L1Extrapolation.h"
#include "L1AddMaterial.h"
#include "L1Filtration.h"
#include "L1MaterialInfo.h"

#include "FairRootManager.h"
#include "TDatabasePDG.h"

#include "CbmKFVertex.h"
#include "KFParticleDatabase.h"

using std::vector;


namespace NS_L1TrackFitter{
  const fvec c_light = 0.000299792458, c_light_i = 1./c_light;
  const fvec ZERO = 0.;
  const fvec ONE = 1.;
  const fvec vINF = 0.1;
}
using namespace NS_L1TrackFitter;

CbmL1PFFitter::CbmL1PFFitter()
{
}

CbmL1PFFitter::~CbmL1PFFitter()
{
}

void CbmL1PFFitter::FilterFirst( L1TrackPar &track,fvec &x, fvec &y, L1Station &st )
{
  track.C00= st.XYInfo.C00;
  track.C10= st.XYInfo.C10;      track.C11= st.XYInfo.C11;
  track.C20= ZERO;         track.C21= ZERO;      track.C22= vINF;
  track.C30= ZERO;         track.C31= ZERO;      track.C32= ZERO;    track.C33= vINF;
  track.C40= ZERO;         track.C41= ZERO;      track.C42= ZERO;    track.C43= ZERO;     track.C44= ONE;

  track.x = x;
  track.y = y;
  track.NDF = -3.0;
  track.chi2 = ZERO;
}

fvec CbmL1PFFitter::ApproximateBetheBloch( const fvec &bg2 )
{
  //
  // This is the parameterization of the Bethe-Bloch formula inspired by Geant.
  //
  // bg2  - (beta*gamma)^2
  // kp0 - density [g/cm^3]
  // kp1 - density effect first junction point
  // kp2 - density effect second junction point
  // kp3 - mean excitation energy [GeV]
  // kp4 - mean Z/A
  //
  // The default values for the kp* parameters are for silicon.
  // The returned value is in [GeV/(g/cm^2)].
  //

  const fvec &kp0 = 2.33f;
  const fvec &kp1 = 0.20f;
  const fvec &kp2 = 3.00f;
  const fvec &kp3 = 173e-9f;
  const fvec &kp4 = 0.49848f;
                                           
  const float mK  = 0.307075e-3f; // [GeV*cm^2/g]
  const float _2me  = 1.022e-3f;    // [GeV/c^2]
  const fvec &rho = kp0;
  const fvec &x0  = kp1 * 2.303f;
  const fvec &x1  = kp2 * 2.303f;
  const fvec &mI  = kp3;
  const fvec &mZA = kp4;
  const fvec &maxT = _2me * bg2;    // neglecting the electron mass

  //*** Density effect
  fvec d2( 0.f );
  const fvec x = 0.5f * log( bg2 );
  const fvec lhwI = log( 28.816f * 1e-9f * sqrt( rho * mZA ) / mI );
  
  fvec init = x > x1;
  
  d2 = fvec(init & (lhwI + x - 0.5f));
  const fvec &r = ( x1 - x ) / ( x1 - x0 );
  init = (x > x0) & (x1 > x);
  d2 = fvec(init & (lhwI + x - 0.5f + ( 0.5f - lhwI - x0 ) * r * r * r)) + fvec( (!init) & d2);

  return mK*mZA*( fvec( 1.f ) + bg2 ) / bg2*( 0.5f*log( maxT * maxT / ( mI*mI ) ) - bg2 / ( fvec( 1.f ) + bg2 ) - d2 );
}

void CbmL1PFFitter::EnergyLossCorrection(L1TrackPar& T, const fvec& mass2, const fvec& radThick, fvec& qp0, float direction)
{
  const fvec& p2 = 1.f/(T.qp*T.qp);
  const fvec& E2 = mass2 + p2;
  
  const fvec& bethe = ApproximateBetheBloch( p2/mass2 );
    
  const fvec& dE = bethe * radThick * 2.33f * 9.34961f;
  
  const fvec& E2Corrected = (sqrt(E2) + direction*dE) * (sqrt(E2) + direction*dE);
  const fvec& corr = sqrt( p2/( E2Corrected - mass2 ) );
  
  for(int iV=0; iV<fvecLen; iV++)
    if( !(corr[iV] == corr[iV]) )
      return;
    
  qp0   *= corr;
  T.qp  *= corr;
  T.C40 *= corr;
  T.C41 *= corr;
  T.C42 *= corr;
  T.C43 *= corr;
  T.C44 *= corr * corr;
}

void CbmL1PFFitter::Fit(vector<CbmStsTrack> &Tracks, vector<int>& pidHypo)
{      
  L1FieldValue fB0, fB1, fB2 _fvecalignment;
  L1FieldRegion fld _fvecalignment;

  FairRootManager *fManger = FairRootManager::Instance();
  TClonesArray *listStsHits = (TClonesArray *)  fManger->GetObject("StsHit");
  int NMvdStations = CbmL1::Instance()->algo->NMvdStations;
  TClonesArray *listMvdHits=0;
  if(NMvdStations>0.)
    listMvdHits = (TClonesArray *)  fManger->GetObject("MvdHit");

  static int nHits = CbmL1::Instance()->algo->NStations;
  int iVec=0, i=0;
  int nTracks_SIMD = fvecLen;
  L1TrackPar T; // fitting parametr coresponding to current track

  CbmStsTrack *t[fvecLen];

  int ista;
  L1Station *sta = CbmL1::Instance()->algo->vStations;
  fvec* x = new fvec[nHits];
  fvec* u = new fvec[nHits];
  fvec* v = new fvec[nHits];
  fvec* y = new fvec[nHits];
  fvec* z = new fvec[nHits];
  fvec* w = new fvec[nHits];
  fvec y_temp;
  fvec x_first, y_first, x_last, y_last; 
  fvec fz0, fz1, fz2, dz, z_start, z_end;
  L1FieldValue* fB = new L1FieldValue[nHits];
  L1FieldValue fB_temp _fvecalignment;

  unsigned short N_vTracks = Tracks.size();

  for(unsigned short itrack = 0; itrack < N_vTracks; itrack++)
  {
    Tracks[itrack].SetPidHypo(pidHypo[itrack]);
  }
        
  fvec mass, mass2;
    
  for(unsigned short itrack = 0; itrack < N_vTracks; itrack+=fvecLen)
  {
    if(N_vTracks - itrack < static_cast<unsigned short>(fvecLen))
      nTracks_SIMD = N_vTracks - itrack;
    for(i=0; i<nTracks_SIMD; i++)
    {
      t[i] = & Tracks[itrack+i]; // current track
      T.x[i]  = t[i]->GetParamFirst()->GetX();
      T.y[i]  = t[i]->GetParamFirst()->GetY();
      T.tx[i] = t[i]->GetParamFirst()->GetTx();
      T.ty[i] = t[i]->GetParamFirst()->GetTy();
      T.qp[i] = t[i]->GetParamFirst()->GetQp();
      T.z[i]  = t[i]->GetParamFirst()->GetZ();
      T.C00[i] = t[i]->GetParamFirst()->GetCovariance(0,0);
      T.C10[i] = t[i]->GetParamFirst()->GetCovariance(1,0);
      T.C11[i] = t[i]->GetParamFirst()->GetCovariance(1,1);
      T.C20[i] = t[i]->GetParamFirst()->GetCovariance(2,0);
      T.C21[i] = t[i]->GetParamFirst()->GetCovariance(2,1);
      T.C22[i] = t[i]->GetParamFirst()->GetCovariance(2,2);
      T.C30[i] = t[i]->GetParamFirst()->GetCovariance(3,0);
      T.C31[i] = t[i]->GetParamFirst()->GetCovariance(3,1);
      T.C32[i] = t[i]->GetParamFirst()->GetCovariance(3,2);
      T.C33[i] = t[i]->GetParamFirst()->GetCovariance(3,3);
      T.C40[i] = t[i]->GetParamFirst()->GetCovariance(4,0);
      T.C41[i] = t[i]->GetParamFirst()->GetCovariance(4,1);
      T.C42[i] = t[i]->GetParamFirst()->GetCovariance(4,1);
      T.C43[i] = t[i]->GetParamFirst()->GetCovariance(4,3);
      T.C44[i] = t[i]->GetParamFirst()->GetCovariance(4,4);
      
      int pid = pidHypo[itrack+i];
      if(pid == -1) pid = 211;
//       mass[i] = TDatabasePDG::Instance()->GetParticle(pid)->Mass();
      mass[i] = KFParticleDatabase::Instance()->GetMass(pid);
    }
    mass2 = mass*mass;
    // get hits of current track
    for(i=0; i<nHits; i++)
    {
      w[i] = ZERO;
      z[i] = sta[i].z;
    }
    
    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      int nHitsTrackMvd = t[iVec]->GetNofMvdHits();
      int nHitsTrackSts = t[iVec]->GetNofHits();
      int nHitsTrack = nHitsTrackMvd + nHitsTrackSts;
      for(i = 0; i < nHitsTrack; i++ )
      {
        float posx = 0.f, posy = 0.f, posz = 0.f;

        if(i<nHitsTrackMvd)
        {
          if(!listMvdHits) continue;
          int hitIndex = t[iVec]->GetMvdHitIndex(i);
          CbmMvdHit *hit = L1_DYNAMIC_CAST<CbmMvdHit*>(listMvdHits->At(hitIndex));

          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = hit->GetStationNr();
        }
        else
        {
          if(!listStsHits) continue;
          int hitIndex = t[iVec]->GetHitIndex(i - nHitsTrackMvd);
          CbmStsHit *hit = L1_DYNAMIC_CAST<CbmStsHit*>(listStsHits->At(hitIndex));

          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = CbmStsAddress::GetElementId(hit->GetAddress(), kStsStation) + NMvdStations;//hit->GetStationNr() - 1 + NMvdStations;
        }
        w[ista][iVec] = 1.f;

        x[ista][iVec] = posx;
        y[ista][iVec] = posy;
        u[ista][iVec] = posx*sta[ista].frontInfo.cos_phi[0] + posy*sta[ista].frontInfo.sin_phi[0];
        v[ista][iVec] = posx* sta[ista].backInfo.cos_phi[0] + posy*sta[ista].backInfo.sin_phi[0];
        z[ista][iVec] = posz;
        sta[ista].fieldSlice.GetFieldValue( x[ista], y[ista], fB_temp );
        fB[ista].x[iVec] = fB_temp.x[iVec];
        fB[ista].y[iVec] = fB_temp.y[iVec];
        fB[ista].z[iVec] = fB_temp.z[iVec];
        if(i == 0)
        {
          z_start[iVec] = posz;
          x_first[iVec] = x[ista][iVec];
          y_first[iVec] = y[ista][iVec];
        }
        if(i == nHitsTrack-1)
        {
          z_end[iVec] = posz;
          x_last[iVec] = x[ista][iVec];
          y_last[iVec] = y[ista][iVec];
        }
      }
    }
    // fit forward
    i = 0;
    FilterFirst( T, x_first, y_first, sta[i] );
    fvec qp0 = T.qp;
    fz1 = z[i];
    sta[i].fieldSlice.GetFieldValue( T.x, T.y, fB1 );
    fB1.Combine( fB[i], w[i] );
    fz2 = z[i+2];
    dz = fz2-fz1;
    sta[i].fieldSlice.GetFieldValue( T.x + T.tx*dz, T.y + T.ty*dz, fB2 );
    fB2.Combine( fB[i+2], w[i+2] );
    fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
    for( ++i; i<nHits; i++ )
    {
      fz0 = z[i];
      dz = (fz1-fz0);
      sta[i].fieldSlice.GetFieldValue( T.x - T.tx*dz, T.y - T.ty*dz, fB0 );
      fB0.Combine( fB[i], w[i] );
      fld.Set( fB0, fz0, fB1, fz1, fB2, fz2 );

      fvec initialised = fvec(z[i] <= z_end) & fvec(z_start < z[i]);
      fvec w1  = (w[i] & (initialised));
      fvec wIn = (ONE  & (initialised));
          
      L1Extrapolate( T, z[i], qp0, fld,&w1 );
      if(i == NMvdStations) L1AddPipeMaterial( T, qp0, wIn );
  #ifdef USE_RL_TABLE
      L1AddMaterial( T, CbmL1::Instance()->algo->fRadThick[i].GetRadThick(T.x, T.y), qp0, wIn, mass2 );
      EnergyLossCorrection( T, mass2, CbmL1::Instance()->algo->fRadThick[i].GetRadThick(T.x, T.y), qp0, -1 );
  #else
      L1AddMaterial( T, sta[i].materialInfo, qp0, wIn, mass2 );
  #endif
      L1Filter( T, sta[i].frontInfo, u[i], w1 );
      L1Filter( T, sta[i].backInfo,  v[i], w1 );

      fB2 = fB1; 
      fz2 = fz1;
      fB1 = fB0; 
      fz1 = fz0;
    }

    L1TrackPar Tout = T;
    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      FairTrackParam par;
      par.SetX(T.x[iVec]);
      par.SetY(T.y[iVec]);
      par.SetTx(T.tx[iVec]);
      par.SetTy(T.ty[iVec]);
      par.SetQp(T.qp[iVec]);
      par.SetZ(T.z[iVec]);

      par.SetCovariance(0,0,Tout.C00[iVec]);
      par.SetCovariance(1,0,Tout.C10[iVec]);
      par.SetCovariance(1,1,Tout.C11[iVec]);
      par.SetCovariance(2,0,Tout.C20[iVec]);
      par.SetCovariance(2,1,Tout.C21[iVec]);
      par.SetCovariance(2,2,Tout.C22[iVec]);
      par.SetCovariance(3,0,Tout.C30[iVec]);
      par.SetCovariance(3,1,Tout.C31[iVec]);
      par.SetCovariance(3,2,Tout.C32[iVec]);
      par.SetCovariance(3,3,Tout.C33[iVec]);
      par.SetCovariance(4,0,Tout.C40[iVec]);
      par.SetCovariance(4,1,Tout.C41[iVec]);
      par.SetCovariance(4,2,Tout.C42[iVec]);
      par.SetCovariance(4,3,Tout.C43[iVec]);
      par.SetCovariance(4,4,Tout.C44[iVec]);
      t[iVec]->SetParamLast(&par);
    }
    
  //fit backward
    qp0 = T.qp;

    i= nHits-1;

    FilterFirst( T, x_last, y_last, sta[i] );

    fz1 = z[i];
    sta[i].fieldSlice.GetFieldValue( T.x, T.y, fB1 );
    fB1.Combine( fB[i], w[i] );

    fz2 = z[i-2];
    dz = fz2-fz1;
    sta[i].fieldSlice.GetFieldValue( T.x + T.tx*dz, T.y + T.ty*dz, fB2 );
    fB2.Combine( fB[i-2], w[i-2] );
    fld.Set( fB2, fz2, fB1, fz1, fB0, fz0 );
    for( --i; i>=0; i-- )
    {
      fz0 = z[i];
      dz = (fz1-fz0);
      sta[i].fieldSlice.GetFieldValue( T.x - T.tx*dz, T.y - T.ty*dz, fB0 );
      fB0.Combine( fB[i], w[i] );
      fld.Set( fB0, fz0, fB1, fz1, fB2, fz2 );

      fvec initialised = fvec(z[i] < z_end) & fvec(z_start <= z[i]);
      fvec w1 = (w[i] & (initialised));
      fvec wIn = (ONE & (initialised));

      L1Extrapolate( T, z[i], qp0, fld, &w1 );
      if(i == NMvdStations - 1) L1AddPipeMaterial( T, qp0, wIn );
  #ifdef USE_RL_TABLE
      L1AddMaterial( T, CbmL1::Instance()->algo->fRadThick[i].GetRadThick(T.x, T.y), qp0, wIn, mass2 );
      EnergyLossCorrection( T, mass2, CbmL1::Instance()->algo->fRadThick[i].GetRadThick(T.x, T.y), qp0, 1 );
  #else
      L1AddMaterial( T, sta[i].materialInfo, qp0, wIn, mass2 );
  #endif
      L1Filter( T, sta[i].frontInfo, u[i], w1 );
      L1Filter( T, sta[i].backInfo,  v[i], w1 );

      fB2 = fB1;
      fz2 = fz1;
      fB1 = fB0;
      fz1 = fz0;
    }

    for(iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      FairTrackParam par;
      par.SetX(T.x[iVec]);
      par.SetY(T.y[iVec]);
      par.SetTx(T.tx[iVec]);
      par.SetTy(T.ty[iVec]);
      par.SetQp(T.qp[iVec]);
      par.SetZ(T.z[iVec]);

      par.SetCovariance(0,0,T.C00[iVec]);
      par.SetCovariance(1,0,T.C10[iVec]);
      par.SetCovariance(1,1,T.C11[iVec]);
      par.SetCovariance(2,0,T.C20[iVec]);
      par.SetCovariance(2,1,T.C21[iVec]);
      par.SetCovariance(2,2,T.C22[iVec]);
      par.SetCovariance(3,0,T.C30[iVec]);
      par.SetCovariance(3,1,T.C31[iVec]);
      par.SetCovariance(3,2,T.C32[iVec]);
      par.SetCovariance(3,3,T.C33[iVec]);
      par.SetCovariance(4,0,T.C40[iVec]);
      par.SetCovariance(4,1,T.C41[iVec]);
      par.SetCovariance(4,2,T.C42[iVec]);
      par.SetCovariance(4,3,T.C43[iVec]);
      par.SetCovariance(4,4,T.C44[iVec]);
      t[iVec]->SetParamFirst(&par);

      t[iVec]->SetChiSq(T.chi2[iVec]);
      t[iVec]->SetNDF(static_cast<int>(T.NDF[iVec]));
    }
  }

  delete[] x;
  delete[] u;
  delete[] v;
  delete[] y;
  delete[] z;
  delete[] w;
  delete[] fB;
}

void CbmL1PFFitter::GetChiToVertex(vector<CbmStsTrack> &Tracks, vector<L1FieldRegion> &field, vector<float> &chiToVtx, CbmKFVertex &primVtx, float chiPrim)
{
  chiToVtx.reserve(Tracks.size());

  int nTracks_SIMD = fvecLen;
  L1TrackPar T; // fitting parametr coresponding to current track

  CbmStsTrack *t[fvecLen];

  int nStations = CbmL1::Instance()->algo->NStations;
  int NMvdStations = CbmL1::Instance()->algo->NMvdStations;
  L1Station *sta = CbmL1::Instance()->algo->vStations;
  fvec* zSta = new fvec[nStations];
  for(int iSta=0; iSta<nStations; iSta++)
    zSta[iSta] = sta[iSta].z;

  field.reserve(Tracks.size());

  L1FieldRegion fld _fvecalignment;
  L1FieldValue fB[3], fB_temp _fvecalignment;
  fvec zField[3];

  FairRootManager *fManger = FairRootManager::Instance();
  TClonesArray *listStsHits = (TClonesArray *)  fManger->GetObject("StsHit");
  TClonesArray *listMvdHits=0;
  if(NMvdStations>0.)
    listMvdHits = (TClonesArray *)  fManger->GetObject("MvdHit");

  unsigned short N_vTracks = Tracks.size();
  int ista;
  for(unsigned short itrack = 0; itrack < N_vTracks; itrack+=fvecLen)
  {
    if(N_vTracks - itrack < static_cast<unsigned short>(fvecLen))
      nTracks_SIMD = N_vTracks - itrack;

    fvec mass2;
    for(int iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      t[iVec] = & Tracks[itrack+iVec]; // current track
      T.x[iVec]  = t[iVec]->GetParamFirst()->GetX();
      T.y[iVec]  = t[iVec]->GetParamFirst()->GetY();
      T.tx[iVec] = t[iVec]->GetParamFirst()->GetTx();
      T.ty[iVec] = t[iVec]->GetParamFirst()->GetTy();
      T.qp[iVec] = t[iVec]->GetParamFirst()->GetQp();
      T.z[iVec]  = t[iVec]->GetParamFirst()->GetZ();
      T.C00[iVec] = t[iVec]->GetParamFirst()->GetCovariance(0,0);
      T.C10[iVec] = t[iVec]->GetParamFirst()->GetCovariance(1,0);
      T.C11[iVec] = t[iVec]->GetParamFirst()->GetCovariance(1,1);
      T.C20[iVec] = t[iVec]->GetParamFirst()->GetCovariance(2,0);
      T.C21[iVec] = t[iVec]->GetParamFirst()->GetCovariance(2,1);
      T.C22[iVec] = t[iVec]->GetParamFirst()->GetCovariance(2,2);
      T.C30[iVec] = t[iVec]->GetParamFirst()->GetCovariance(3,0);
      T.C31[iVec] = t[iVec]->GetParamFirst()->GetCovariance(3,1);
      T.C32[iVec] = t[iVec]->GetParamFirst()->GetCovariance(3,2);
      T.C33[iVec] = t[iVec]->GetParamFirst()->GetCovariance(3,3);
      T.C40[iVec] = t[iVec]->GetParamFirst()->GetCovariance(4,0);
      T.C41[iVec] = t[iVec]->GetParamFirst()->GetCovariance(4,1);
      T.C42[iVec] = t[iVec]->GetParamFirst()->GetCovariance(4,1);
      T.C43[iVec] = t[iVec]->GetParamFirst()->GetCovariance(4,3);
      T.C44[iVec] = t[iVec]->GetParamFirst()->GetCovariance(4,4);
//      float mass = TDatabasePDG::Instance()->GetParticle(t[iVec]->GetPidHypo())->Mass();
      const float mass = KFParticleDatabase::Instance()->GetMass(t[iVec]->GetPidHypo());
      mass2[iVec] = mass*mass;

      int nHitsTrackMvd = t[iVec]->GetNofMvdHits();
      for(int iH = 0; iH < 2; iH++ )
      {
        float posx = 0.f, posy = 0.f, posz = 0.f;

        if(iH<nHitsTrackMvd)
        {
          if(!listMvdHits) continue;
          int hitIndex = t[iVec]->GetMvdHitIndex(iH);
          CbmMvdHit *hit = L1_DYNAMIC_CAST<CbmMvdHit*>(listMvdHits->At(hitIndex));

          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = posz < 7.f ? 0 : 1;
        }
        else
        {
          if(!listStsHits) continue;
          int hitIndex = t[iVec]->GetHitIndex(iH - nHitsTrackMvd);
          CbmStsHit *hit = L1_DYNAMIC_CAST<CbmStsHit*>(listStsHits->At(hitIndex));

          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = CbmStsAddress::GetElementId(hit->GetAddress(), kStsStation) + NMvdStations;//hit->GetStationNr()-1+NMvdStations;
        }

        sta[ista].fieldSlice.GetFieldValue( posx, posy, fB_temp );
        fB[iH+1].x[iVec] = fB_temp.x[iVec];
        fB[iH+1].y[iVec] = fB_temp.y[iVec];
        fB[iH+1].z[iVec] = fB_temp.z[iVec];
        zField[iH+1][iVec] = posz;
      }
    }
    
    fB[0] = CbmL1::Instance()->algo->GetvtxFieldValue();
    zField[0] = 0;
    fld.Set( fB[2], zField[2], fB[1], zField[1], fB[0], zField[0] );
    field.push_back(fld);

    for(int iSt= nStations-4; iSt>=0; iSt--)
    {
      fvec w=ONE;
      fvec initialized = fvec(T.z > (zSta[iSt]+2.5));
      w = fvec(w & initialized);

      L1Extrapolate( T, zSta[iSt], T.qp, fld, &w );
      if(iSt == NMvdStations - 1) L1AddPipeMaterial( T, T.qp, w, mass2);
      L1AddMaterial( T, CbmL1::Instance()->algo->fRadThick[iSt].GetRadThick(T.x, T.y), T.qp, w, mass2);
    }
    fvec ONE=1;
    if( NMvdStations <= 0 ) L1AddPipeMaterial( T, T.qp, ONE, mass2);
    L1Extrapolate( T, primVtx.GetRefZ(), T.qp, fld);

    Double_t Cv[3] = { primVtx.GetCovMatrix()[0], primVtx.GetCovMatrix()[1], primVtx.GetCovMatrix()[2] };

    fvec dx = T.x - primVtx.GetRefX();
    fvec dy = T.y - primVtx.GetRefY();
    fvec c[3] = { T.C00, T.C10, T.C11 };
    c[0]+= Cv[0];  c[1]+= Cv[1];  c[2]+= Cv[2];
    fvec d = c[0]*c[2] - c[1]*c[1] ;
    fvec chi = sqrt( fabs( 0.5*(dx*dx*c[0]-2*dx*dy*c[1]+dy*dy*c[2])/d ) );
    fvec isNull = fvec(fabs(d)<1.e-20);
    chi = fvec(fvec(!isNull) & chi) + fvec(isNull & fvec(0));

    for(int iVec=0; iVec<nTracks_SIMD; iVec++)
      chiToVtx.push_back(chi[iVec]);
    
    if(chiPrim>0)
    {
      for(int iVec=0; iVec<nTracks_SIMD; iVec++)
      {
        if( chi[iVec] < chiPrim )
        {
           FairTrackParam par;
          par.SetX(T.x[iVec]);
          par.SetY(T.y[iVec]);
          par.SetTx(T.tx[iVec]);
          par.SetTy(T.ty[iVec]);
          par.SetQp(T.qp[iVec]);
          par.SetZ(T.z[iVec]);

          par.SetCovariance(0,0,T.C00[iVec]);
          par.SetCovariance(1,0,T.C10[iVec]);
          par.SetCovariance(1,1,T.C11[iVec]);
          par.SetCovariance(2,0,T.C20[iVec]);
          par.SetCovariance(2,1,T.C21[iVec]);
          par.SetCovariance(2,2,T.C22[iVec]);
          par.SetCovariance(3,0,T.C30[iVec]);
          par.SetCovariance(3,1,T.C31[iVec]);
          par.SetCovariance(3,2,T.C32[iVec]);
          par.SetCovariance(3,3,T.C33[iVec]);
          par.SetCovariance(4,0,T.C40[iVec]);
          par.SetCovariance(4,1,T.C41[iVec]);
          par.SetCovariance(4,2,T.C42[iVec]);
          par.SetCovariance(4,3,T.C43[iVec]);
          par.SetCovariance(4,4,T.C44[iVec]);
          t[iVec]->SetParamFirst(&par);
        }
      }
    }
  }
  delete[] zSta;
}

void CbmL1PFFitter::CalculateFieldRegion(vector<CbmStsTrack> &Tracks, vector<L1FieldRegion> &field)
{
  field.reserve(Tracks.size());

  L1FieldRegion fld _fvecalignment;

  FairRootManager *fManger = FairRootManager::Instance();
  TClonesArray *listStsHits = (TClonesArray *)  fManger->GetObject("StsHit");
  TClonesArray *listMvdHits=0;
  int NMvdStations = CbmL1::Instance()->algo->NMvdStations;
  if(NMvdStations>0.)
    listMvdHits = (TClonesArray *)  fManger->GetObject("MvdHit");

  int nTracks_SIMD = fvecLen;
  L1TrackPar T; // fitting parametr coresponding to current track

  CbmStsTrack *t[fvecLen];

  int ista;
  L1Station *sta = CbmL1::Instance()->algo->vStations;
  L1FieldValue fB[3], fB_temp _fvecalignment;
  fvec zField[3];

  unsigned short N_vTracks = Tracks.size();

  for(unsigned short itrack = 0; itrack < N_vTracks; itrack+=fvecLen)
  {
    if(N_vTracks - itrack < static_cast<unsigned short>(fvecLen))
      nTracks_SIMD = N_vTracks - itrack;

    for(int i=0; i<nTracks_SIMD; i++)
      t[i] = & Tracks[itrack+i]; // current track

    for(int iVec=0; iVec<nTracks_SIMD; iVec++)
    {
      int nHitsTrackMvd = t[iVec]->GetNofMvdHits();
      for(int iH = 0; iH < 2; iH++ )
      {
        float posx = 0.f, posy = 0.f, posz = 0.f;

        if(iH<nHitsTrackMvd)
        {
          if(!listMvdHits) continue;
          int hitIndex = t[iVec]->GetMvdHitIndex(iH);
          CbmMvdHit *hit = L1_DYNAMIC_CAST<CbmMvdHit*>(listMvdHits->At(hitIndex));

          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = posz < 7.f ? 0 : 1;
        }
        else
        {
          if(!listStsHits) continue;
          int hitIndex = t[iVec]->GetHitIndex(iH - nHitsTrackMvd);
          CbmStsHit *hit = L1_DYNAMIC_CAST<CbmStsHit*>(listStsHits->At(hitIndex));

          posx = hit->GetX();
          posy = hit->GetY();
          posz = hit->GetZ();
          ista = CbmStsAddress::GetElementId(hit->GetAddress(), kStsStation)  + NMvdStations;//hit->GetStationNr()-1+NMvdStations;
        }

        sta[ista].fieldSlice.GetFieldValue( posx, posy, fB_temp );
        fB[iH+1].x[iVec] = fB_temp.x[iVec];
        fB[iH+1].y[iVec] = fB_temp.y[iVec];
        fB[iH+1].z[iVec] = fB_temp.z[iVec];
        zField[iH+1][iVec] = posz;
      }
    }

    fB[0] = CbmL1::Instance()->algo->GetvtxFieldValue();
    zField[0] = 0;
    fld.Set( fB[2], zField[2], fB[1], zField[1], fB[0], zField[0] );
    field.push_back(fld);
  }
}
