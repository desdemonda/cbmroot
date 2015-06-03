/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  Read Event information to L1
 *
 *====================================================================
 */

#include "CbmL1.h"
#include "L1Algo/L1Algo.h"
#include "CbmKF.h"
#include "CbmMatch.h"
#include "CbmStsAddress.h"

#include "CbmMvdHitMatch.h"


#include "TDatabasePDG.h"
#include "TRandom.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
using std::find;

//#define MVDIDEALHITS
//#define STSIDEALHITS

struct TmpMCPoint{ // used for sort MCPoints for creation of MCTracks
  int ID;         // MCPoint ID
  double z;       // MCPoint z
  int StsHit;     // index of hit in algo->vStsHits
  int MCPoint;    // index of mcPoint in vMCPoints
  bool eff;
  static bool compareIDz( const TmpMCPoint &a, const TmpMCPoint &b )
  {
    return ( a.ID < b.ID ) || ( ( a.ID == b.ID ) && (a.z < b.z) );
  }
};

struct TmpHit{ // used for sort Hits before writing in the normal arrays
  int iStripF, iStripB;   // indices of real-strips, sts-strips (one got from detector. They consist from parts with differen positions, so will be divided before using)
  int indStripF, indStripB; // indices of L1-strips, indices in TmpStrip arrays
  int iSector;
  int iStation;
  int ExtIndex;           // index of hit in the TClonesArray array ( negative for MVD )
  bool isStrip;
  double u_front, u_back; // positions of strips
  double x, y;            // position of hit
  int iMC;                // index of MCPoint in the vMCPoints array
  double time;
  static bool Compare( const TmpHit &a, const TmpHit &b ){
    return ( a.iStation <  b.iStation ) ||
         ( ( a.iStation == b.iStation ) && ( a.y < b.y ) );
  }
};

struct TmpStrip{
  fscal u;
  int iStation;
  int iSector, iStrip;
  bool isStrip;
  int effIndex; // used for unefficiency
};

  /// Repack data from Clones Arrays to L1 arrays
void CbmL1::ReadEvent()
{
  if (fVerbose >= 10) cout << "ReadEvent: start." << endl;
    // clear arrays for next event
  vMCPoints.clear();
  vMCTracks.clear();
  vStsHits.clear();
  vRTracks.clear();
  vHitMCRef.clear();
  vHitStore.clear();
  algo->vStsHits.clear();
  algo->vStsStrips.clear();
  algo->vStsStripsB.clear();
  algo->vStsZPos.clear();
  algo->vSFlag.clear();
  algo->vSFlagB.clear();
  if (fVerbose >= 10) cout << "ReadEvent: clear is done." << endl;
  
  vector<TmpHit> tmpHits;
  vector<TmpStrip> tmpStrips;
  vector<TmpStrip> tmpStripsB;

    // -- produce Sts hits from space points --

  for(int i = 0; i < NStation; i++){
    algo->StsHitsStartIndex[i] = static_cast<THitI>(-1);
    algo->StsHitsStopIndex[i]  = 0;
  }
  
  //for a new definition of the reconstructable track (4 consequtive MC Points):
  vector<bool> isUsedMvdPoint; //marks, whether Mvd MC Point already in the vMCPoints
  vector<bool> isUsedStsPoint; //marks, whether Mvd MC Point already in the vMCPoints

    // get MVD hits
  int nMvdHits=0;
#ifdef MVDIDEALHITS
  if(listMvdPts){
    Int_t nMC  = listMvdPts->GetEntries();
    isUsedMvdPoint.resize(nMC);
    for(int iMc=0; iMc<nMC; iMc++){
      isUsedMvdPoint[iMc]=0;
      TmpHit th;
      CbmMvdPoint *point = L1_DYNAMIC_CAST<CbmMvdPoint*>( listMvdPts->At(iMc) );
      th.ExtIndex = -(1+iMc);
      if((point->GetZOut() < 4) || (point->GetZOut() > 21)){
        std::cout<<"[CbmL1ReadEvent]: MVDIdealHits - wrong Z-coord of MCPoint\n";
        if(point->GetZOut() < 4) th.iStation = 0;
        else th.iStation = 3;
      }
      else if(point->GetZOut() < 6) th.iStation = 0;
      else if(point->GetZOut() < 11) th.iStation = 1;
      else if(point->GetZOut() < 16) th.iStation = 2;
      else th.iStation = 3;
      th.iSector  = 0;
      th.isStrip  = 0;
      th.iStripF = iMc;
      th.iStripB = -1;
      if( th.iStripF<0 ) continue;
      if( th.iStripF>=0 && th.iStripB>=0 ) th.isStrip  = 1;
      if( th.iStripB <0 ) th.iStripB = th.iStripF;
      th.x = 0.5 * ( point->GetX() + point->GetXOut() );
      th.y = 0.5 * ( point->GetY() + point->GetYOut() );
      L1Station &st = algo->vStations[th.iStation];
      th.u_front = th.x*st.frontInfo.cos_phi[0] + th.y*st.frontInfo.sin_phi[0] + gRandom->Gaus(0,0.00048);
      th.u_back  = th.x*st.backInfo.cos_phi[0] + th.y*st.backInfo.sin_phi[0] + gRandom->Gaus(0,0.00048);
      CbmL1MCPoint MC;
      ReadMCPoint( &MC, iMc, 1 );
      MC.iStation = th.iStation;
      vMCPoints.push_back( MC );
      isUsedMvdPoint[iMc] = 1;
      th.iMC = vMCPoints.size() - 1;
      tmpHits.push_back(th);
      nMvdHits++;
    }
  }
#else
  if( listMvdHits ){
    Int_t nEnt  = listMvdHits->GetEntries();
    Int_t nMC = (listMvdPts) ? listMvdPts->GetEntries() : 0;

    if(listMvdPts)
    {
      isUsedMvdPoint.resize(nMC);
      for(int iMc=0; iMc<nMC; iMc++) isUsedMvdPoint[iMc]=0;
    }

    for (int j=0; j <nEnt; j++ ){
      TmpHit th;
      {
        CbmMvdHit *mh = L1_DYNAMIC_CAST<CbmMvdHit*>( listMvdHits->At(j) );
        th.ExtIndex = -(1+j);
        th.iStation = mh->GetStationNr();// - 1;
        th.iSector  = 0;
        th.isStrip  = 0;
        th.iStripF = j;
        th.iStripB = -1;
        if( th.iStripF<0 ) continue;
        if( th.iStripF>=0 && th.iStripB>=0 ) th.isStrip  = 1;
        if( th.iStripB <0 ) th.iStripB = th.iStripF;

        TVector3 pos, err;
        mh->Position(pos);
        mh->PositionError(err);

        th.x = pos.X();
        th.y = pos.Y();

        L1Station &st = algo->vStations[th.iStation];
        th.u_front = th.x*st.frontInfo.cos_phi[0] + th.y*st.frontInfo.sin_phi[0];
        th.u_back  = th.x*st.backInfo.cos_phi[0] + th.y*st.backInfo.sin_phi[0];
      }
      th.iMC=-1;
      int iMC = -1;
//   int iMCTr = -1;
//   if( listMvdHitMatches ){
//     CbmMvdHitMatch *match = (CbmMvdHitMatch*) listMvdHitMatches->At(j);
//     if( match){
//       iMC = match->GetPointId();
//       iMCTr = match->GetTrackId();
//     }
//   }
//       if( listMvdHitMatches ){
//         CbmMvdHitMatch *match = L1_DYNAMIC_CAST<CbmMvdHitMatch*>( listMvdHitMatches->At(j) );
//         if( match){
//           int iDigi = match->GetIndexNumber();
//           CbmMvdDigiMatch *digimatch =  L1_DYNAMIC_CAST<CbmMvdDigiMatch*>( listMvdDigiMatches->At(iDigi));
//           if(digimatch)
//           {
//             
//             iMC = digimatch->GetIndexNumber();
//             std::cout << "iMC " << iMC << std::endl;
//           }
//         }
//       }  
  
      if( listMvdHitMatches ){
        CbmMatch *hm = L1_DYNAMIC_CAST<CbmMatch*>( listMvdHitMatches->At(j) );
       
        if( hm->GetNofLinks()>0 )
        iMC = hm->GetLink(0).GetIndex();
      }
      if( listMvdPts && iMC>=0 ){ // TODO1: don't need this with FairLinks
        CbmL1MCPoint MC;
        if( ! ReadMCPoint( &MC, iMC, 1 ) ){
          MC.iStation = th.iStation;
          isUsedMvdPoint[iMC] = 1;

//       MC.ID = iMCTr; // because atch->GetPointId() == 0 !!! and ReadMCPoint don't work
//       MC.z = th.iStation; // for sort in right order
      
          vMCPoints.push_back( MC );
          th.iMC = vMCPoints.size()-1;
        }
      } // if listStsPts
  //if(  h.MC_Point >=0 ) // DEBUG !!!!
      {
        tmpHits.push_back(th);
        nMvdHits++;
      }
    } // for j
  } // if listMvdHits
  if (fVerbose >= 10) cout << "ReadEvent: mvd hits are gotten." << endl;
#endif

    // get STS hits
  int nStsHits = 0;
#ifdef STSIDEALHITS
  if(listStsPts){
    Int_t nMC = listStsPts->GetEntries();
    isUsedStsPoint.resize(nMC);
    for(Int_t iMc = 0; iMc < nMC; iMc++){
      isUsedStsPoint[iMc] = 0;
      CbmStsPoint* point = L1_DYNAMIC_CAST<CbmStsPoint*>( listStsPts->At(iMc) );
      TmpHit th;
      th.ExtIndex = iMc;
      if((point->GetZIn() < 29) || (point->GetZIn() > 101)){
        std::cout<<"[CbmL1ReadEvent]: STSIdealHits - wrong Z-coord of MCPoint\n";
        if(point->GetZIn() < 29) th.iStation = NMvdStations;
        else th.iStation = NMvdStations + 7; //!!!
      }
      else if(point->GetZOut() < 31) th.iStation = NMvdStations;
      else if(point->GetZOut() < 41) th.iStation = NMvdStations + 1;
      else if(point->GetZOut() < 51) th.iStation = NMvdStations + 2;
      else if(point->GetZOut() < 61) th.iStation = NMvdStations + 3;
      else if(point->GetZOut() < 71) th.iStation = NMvdStations + 4;
      else if(point->GetZOut() < 81) th.iStation = NMvdStations + 5;
      else if(point->GetZOut() < 91) th.iStation = NMvdStations + 6;
      else if(point->GetZOut() < 101) th.iStation = NMvdStations + 7;
      else continue;
      th.iSector = 0;
      th.isStrip = 0;
      th.iStripF = 0;
      th.iStripB = 0;
      th.time = point->GetTime();
      th.iStripF += nMvdHits;
      th.iStripB += nMvdHits;
      Double_t z_coord = 0.5 * (point->GetZOut() + point->GetZIn());
      th.x = point->GetX(z_coord);
      th.y = point->GetY(z_coord);
      L1Station &st = algo->vStations[th.iStation];
      th.u_front = th.x*st.frontInfo.cos_phi[0] + th.y*st.frontInfo.sin_phi[0] + gRandom->Gaus(0,0.00167432);
      th.u_back  = th.x*st.backInfo.cos_phi[0] + th.y*st.backInfo.sin_phi[0] + gRandom->Gaus(0,0.00167432);
      CbmL1MCPoint MC;
      if( ! ReadMCPoint( &MC, iMc, 0 ) ){
        MC.iStation = th.iStation;
        vMCPoints.push_back( MC );
        isUsedStsPoint[iMc] = 1;
        th.iMC = vMCPoints.size()-1;
      }
      MC.iStation = th.iStation;
      vMCPoints.push_back( MC );
      isUsedStsPoint[iMc] = 1;
      th.iMC = vMCPoints.size() - 1;
      tmpHits.push_back(th);
      nStsHits++;
    }
  }
#else
  if( listStsHits ){
    Int_t nEnt = listStsHits->GetEntries();
    Int_t nMC = (listStsPts) ? listStsPts->GetEntries() : 0;

    if(listStsPts)
    {
      isUsedStsPoint.resize(nMC);
      for(int iMc=0; iMc<nMC; iMc++) isUsedStsPoint[iMc]=0;
    }

    int negF=0;
    for (int j=0; j < nEnt; j++ ){
      CbmStsHit *sh = L1_DYNAMIC_CAST<CbmStsHit*>( listStsHits->At(j) );
      TmpHit th;
      {
        CbmStsHit *mh = L1_DYNAMIC_CAST<CbmStsHit*>( listStsHits->At(j) );
        th.ExtIndex = j;
        th.iStation = NMvdStations + CbmStsAddress::GetElementId(mh->GetAddress(), kStsStation);//mh->GetStationNr() - 1;
        th.iSector  = mh->GetSectorNr();
        th.isStrip  = 0;
        th.iStripF = mh->GetFrontDigiId();
        th.iStripB = mh->GetBackDigiId();
        if( th.iStripF<0 ){ negF++; continue;}
        if( th.iStripF>=0 && th.iStripB>=0 ) th.isStrip  = 1;
        if( th.iStripB <0 ) th.iStripB = th.iStripF;
        
        //Get time
        if(listStsClusters)
        {
          int iFrontCluster = mh->GetFrontClusterId();
          int iBackCluster = mh->GetBackClusterId();
          CbmStsCluster *frontCluster = L1_DYNAMIC_CAST<CbmStsCluster*>( listStsClusters->At( iFrontCluster ) );
          Double_t hitTime = 0;
          Int_t nMcPointsInHit = 0;
          for(unsigned int iDigi=0; iDigi<frontCluster->GetDigis().size(); iDigi++)
          {
            CbmMatch *stsDigiMatch = L1_DYNAMIC_CAST<CbmMatch*>( listStsDigiMatch->At( frontCluster->GetDigis()[iDigi] ) );

            int nPoints = stsDigiMatch->GetNofLinks();

            for(int iPoint=0; iPoint<nPoints; iPoint++)
            {
              int pointIndex = stsDigiMatch->GetLink(iPoint).GetIndex();
              CbmStsPoint *stsPoint = L1_DYNAMIC_CAST<CbmStsPoint*>( listStsPts->At( pointIndex ) );
              Double_t digiTime = stsPoint->GetTime() + gRandom->Gaus(0,5);
              hitTime += digiTime;
              nMcPointsInHit++;
            }
          }
          CbmStsCluster *backCluster = L1_DYNAMIC_CAST<CbmStsCluster*>( listStsClusters->At( iBackCluster ) );
          for(unsigned int iDigi=0; iDigi<backCluster->GetDigis().size(); iDigi++)
          {
            CbmMatch *stsDigiMatch = L1_DYNAMIC_CAST<CbmMatch*>( listStsDigiMatch->At( backCluster->GetDigis()[iDigi] ) );

            int nPoints = stsDigiMatch->GetNofLinks();

            for(int iPoint=0; iPoint<nPoints; iPoint++)
            {
              int pointIndex = stsDigiMatch->GetLink(iPoint).GetIndex();
              CbmStsPoint *stsPoint = L1_DYNAMIC_CAST<CbmStsPoint*>( listStsPts->At( pointIndex ) );
              Double_t digiTime = stsPoint->GetTime() + gRandom->Gaus(0,5);
              hitTime += digiTime;
              nMcPointsInHit++;
            }
          }
          hitTime /= nMcPointsInHit;

          th.time = hitTime;
        }
        th.iStripF += nMvdHits;
        th.iStripB += nMvdHits;

        TVector3 pos, err;
        mh->Position(pos);
        mh->PositionError(err);

        th.x = pos.X();
        th.y = pos.Y();

        L1Station &st = algo->vStations[th.iStation];
        th.u_front = th.x*st.frontInfo.cos_phi[0] + th.y*st.frontInfo.sin_phi[0];
        th.u_back  = th.x* st.backInfo.cos_phi[0] + th.y* st.backInfo.sin_phi[0];
      }
      th.iMC=-1;

      int iMC = -1;
      if(listStsClusterMatch)
      {
        
        const CbmMatch* frontClusterMatch = static_cast<const CbmMatch*>(listStsClusterMatch->At(sh->GetFrontClusterId()));
        const CbmMatch* backClusterMatch  = static_cast<const CbmMatch*>(listStsClusterMatch->At(sh->GetBackClusterId()));
        
        CbmMatch stsHitMatch;
        
        for(int iFrontLink = 0; iFrontLink<frontClusterMatch->GetNofLinks(); iFrontLink++)
        {
          const CbmLink& frontLink = frontClusterMatch->GetLink(iFrontLink);
          for(int iBackLink = 0; iBackLink<backClusterMatch->GetNofLinks(); iBackLink++)
          {
            const CbmLink& backLink = backClusterMatch->GetLink(iBackLink);
            
            if(frontLink.GetIndex() == backLink.GetIndex())
            {
              stsHitMatch.AddLink(frontLink);
              stsHitMatch.AddLink(backLink);
            }
          }
        }

        if( stsHitMatch.GetNofLinks()>0 )
        {
          Float_t bestWeight = 0.f;
          
          for(int iLink=0; iLink < stsHitMatch.GetNofLinks(); iLink++)
          {
            if( stsHitMatch.GetLink(iLink).GetWeight() > bestWeight)
            {
              bestWeight = stsHitMatch.GetLink(iLink).GetWeight();
              iMC = stsHitMatch.GetLink(iLink).GetIndex();
            }
          }
        }
      }
      else
        iMC = sh->GetRefId(); // TODO1: don't need this with FairLinks
        
      if( listStsPts && iMC>=0 && iMC<nMC){
        CbmL1MCPoint MC;
        if( ! ReadMCPoint( &MC, iMC, 0 ) ){
          MC.iStation = th.iStation;
          vMCPoints.push_back( MC );
          isUsedStsPoint[iMC] = 1;
          th.iMC = vMCPoints.size()-1;
        }
      }  // if listStsPts
      
  //if(  th.iMC >=0 ) // DEBUG !!!!
      {
        tmpHits.push_back(th);
        nStsHits++;
      }
    } // for j
  } // if listStsHits
  if (fVerbose >= 10) cout << "ReadEvent: sts hits are gotten." << endl;
#endif

  //add MC points, which has not been added yet
  if(listMvdHits && listMvdPts)
  {
    int nMC = listMvdPts->GetEntriesFast();
    for(int iMC=0; iMC<nMC; iMC++){
      if(!(isUsedMvdPoint[iMC])) {
        CbmL1MCPoint MC;
        if( ! ReadMCPoint( &MC, iMC, 1 ) ){
          MC.iStation = (L1_DYNAMIC_CAST<CbmMvdPoint*>(listMvdPts->At(iMC)))->GetStationNr() - 1;
          isUsedMvdPoint[iMC] = 1;
          vMCPoints.push_back( MC );
        }
      }
    }
  }
  if(listStsHits && listStsPts)
  {
    int nMC = listStsPts->GetEntriesFast();
    for(int iMC=0; iMC<nMC; iMC++){
      if(!(isUsedStsPoint[iMC])) {
        CbmL1MCPoint MC;
        if( ! ReadMCPoint( &MC, iMC, 0 ) ){
          MC.iStation = -1;
          L1Station *sta = algo->vStations + NMvdStations;
          for(int iSt=0; iSt < NStsStations; iSt++)
            MC.iStation = ( MC.z > sta[iSt].z[0] - 2.5 ) ? (NMvdStations+iSt) : MC.iStation;
          isUsedStsPoint[iMC] = 1;
          vMCPoints.push_back( MC );
        }
      }
    }
  }
    // sort hits
  int nHits = nMvdHits + nStsHits;
  sort( tmpHits.begin(), tmpHits.end(), TmpHit::Compare );

    // -- create strips --
  int NStrips = 0, NStripsB = 0;
  for ( int ih = 0; ih<nHits; ih++ ){
    TmpHit &th = tmpHits[ih];
    
      // try to find the respective front and back strip from the already created
    th.indStripF = -1;
    th.indStripB = -1;
    for( int is = 0; is<NStrips; is++ ){
      TmpStrip &s = tmpStrips[is];
      if( s.iStation!=th.iStation ||  s.iSector!=th.iSector ) continue;
      if( s.iStrip!=th.iStripF ) continue;
      if( fabs(s.u - th.u_front)>1.e-4 ) continue;
      th.indStripF = is;
    }
    for( int is = 0; is<NStripsB; is++ ){
      TmpStrip &s = tmpStripsB[is];
      if( s.iStation!=th.iStation ||  s.iSector!=th.iSector ) continue;
      if( s.iStrip!=th.iStripB ) continue;
      if( fabs(s.u - th.u_back)>1.e-4 ) continue;
      th.indStripB = is;
    }
      // create new strips
    if( th.indStripF<0 ){
      TmpStrip tmp;
      tmp.iStation = th.iStation;
      tmp.iSector = th.iSector;
      tmp.iStrip = th.iStripF;
      tmp.u = th.u_front;
      tmp.isStrip = th.isStrip;
      tmpStrips.push_back(tmp);
      th.indStripF = NStrips++;
    }
    if( th.indStripB<0 ){
      TmpStrip tmp;
      tmp.iStation = th.iStation;
      tmp.iSector = th.iSector;
      tmp.iStrip = th.iStripB;
      tmp.isStrip = th.isStrip;
      tmp.u = th.u_back;
      tmpStripsB.push_back(tmp);
      th.indStripB = NStripsB++;
    }
  } // ih
  
      // -- save strips --
//   for( int i=0; i<NStrips; i++ ) tmpStrips[i].effIndex = -1;
//   for( int i=0; i<NStripsB; i++ ) tmpStripsB[i].effIndex = -1;

//     // make unefficiency
//   if(1){ // space point unefficiency
//     for( int i=0; i<nHits; i++ ){
//       TmpHit &th = tmpHits[i];
//       if( th.indStripF <0 || th.indStripF >= NStrips ) continue;
//       if( th.indStripB <0 || th.indStripB >= NStripsB ) continue;
//       if( th.iMC<0 ) continue;
//       if( gRandom->Uniform(1)>fDetectorEfficiency ){
//         tmpStrips [th.indStripF].effIndex = -2;
//         tmpStripsB[th.indStripB].effIndex = -2;
//       }
//     }
//   } else { // strip unefficiency
// 
//     for( int i=0; i<NStrips; i++ ){
//       if( gRandom->Uniform(1)>fDetectorEfficiency ) tmpStrips[i].effIndex = -2;
//     }
//     
//     for( int i=0; i<NStripsB; i++ ){
//       TmpStrip &ts = tmpStripsB[i];
//       if( ts.isStrip && gRandom->Uniform(1)>fDetectorEfficiency ) ts.effIndex = -2;
//     }
//   }

    // take into account unefficiency and save strips in L1Algo
  Int_t NEffStrips = 0, NEffStripsB = 0;
  for( int i=0; i<NStrips; i++ ){
    TmpStrip &ts = tmpStrips[i];
      //     if( ts.effIndex == -1 ){
    ts.effIndex = NEffStrips++;
    char flag = ts.iStation*4;
    algo->vStsStrips.push_back(ts.u);
    algo->vSFlag.push_back(flag);
      //     }
  }
  for( int i=0; i<NStripsB; i++ ){
    TmpStrip &ts = tmpStripsB[i];
      //     if( ts.effIndex == -1 ){
    ts.effIndex = NEffStripsB++;
    char flag = ts.iStation*4;
    algo->vStsStripsB.push_back(ts.u);
    algo->vSFlagB.push_back(flag);
      //     }
  }

  if (fVerbose >= 10) cout << "ReadEvent: strips are read." << endl;
  
    // -- save hits --
//   vector<int> vUnEffHitMCRef;
//   vector<CbmL1HitStore> vUnEffHitStore;
  int nEffHits = 0;
  vector<float> vStsZPos_temp; // temp array for unsorted z positions of detectors segments
  for( int i=0; i<nHits; i++ ){
    TmpHit &th = tmpHits[i];
 
    CbmL1HitStore s;
    s.ExtIndex = th.ExtIndex;
    s.iStation = th.iStation;
    s.x        = th.x;
    s.y        = th.y;

    if( th.indStripF <0 || th.indStripF >= NStrips ) continue;
    if( th.indStripB <0 || th.indStripB >= NStripsB ) continue;

    TmpStrip &tsF = tmpStrips [th.indStripF];
    TmpStrip &tsB = tmpStripsB[th.indStripB];
//     if( tsF.effIndex <0 || tsB.effIndex <0 ){
//       vUnEffHitStore.push_back(s);
//       vUnEffHitMCRef.push_back(th.iMC);
//       continue;
//     }

    L1StsHit h;
    h.f = tsF.effIndex;
    h.b = tsB.effIndex;
    h.time = th.time; 
    
      // find and save z positions
    float z_tmp;
    int ist = th.iStation;
    if (ist < NMvdStations){
#ifdef MVDIDEALHITS
      CbmMvdPoint* point = L1_DYNAMIC_CAST<CbmMvdPoint*>(listMvdPts->At(- s.ExtIndex - 1));
      z_tmp = 0.5 * ( point->GetZOut() + point->GetZIn() );
#else
      CbmMvdHit *mh_m = L1_DYNAMIC_CAST<CbmMvdHit*>( listMvdHits->At(- s.ExtIndex - 1));
      z_tmp = mh_m->GetZ();
#endif
    }
    else {
#ifdef STSIDEALHITS
      CbmStsPoint* point = L1_DYNAMIC_CAST<CbmStsPoint*>(listStsPts->At(s.ExtIndex));
      z_tmp = 0.5 * ( point->GetZOut() + point->GetZIn() );
#else
      CbmStsHit *mh_m = L1_DYNAMIC_CAST<CbmStsHit*>( listStsHits->At(s.ExtIndex));
      z_tmp = mh_m->GetZ();
#endif
    }
//     h.z = z_tmp;
    
    unsigned int k;
    for (k = 0; k < vStsZPos_temp.size(); k++){
      if (vStsZPos_temp[k] == z_tmp){
        h.iz = k;
        break;
      }
    }
    if (k == vStsZPos_temp.size()){
      h.iz = vStsZPos_temp.size();
      vStsZPos_temp.push_back(z_tmp);
    }
    
      // save hit
    vStsHits.push_back( CbmL1StsHit(algo->vStsHits.size(), th.ExtIndex ) );
    algo->vStsHits.push_back(h);
    
    int sta = th.iStation;
    if (algo->StsHitsStartIndex[sta] == static_cast<THitI>(-1)) algo->StsHitsStartIndex[sta] = nEffHits;
    nEffHits++;
    algo->StsHitsStopIndex[sta] = nEffHits;
    
    vHitStore.push_back(s);
    vHitMCRef.push_back(th.iMC);
  }
  for(int i = 0; i < NStation; i++){
    if (algo->StsHitsStartIndex[i] == static_cast<THitI>(-1)) algo->StsHitsStartIndex[i] = algo->StsHitsStopIndex[i];
  }

//   if(0){ // check index z-pos befor sort
//     for (int k = 0; k < algo->vStsHits.size(); k++){
//       cout << algo->vStsHits[k].z << "  " << vStsZPos_temp[algo->vStsHits[k].iz] << endl;
//     }
//     int kk; std::cin >> kk;
//   }

  if (fVerbose >= 10) cout << "ReadEvent: mvd and sts are saved." << endl;
  
    // sort z-pos
  if (vStsZPos_temp.size() != 0){
    vector<float> vStsZPos_temp2;
    vStsZPos_temp2.clear();
    vStsZPos_temp2.push_back(vStsZPos_temp[0]);
    vector<int> newToOldIndex;
    newToOldIndex.clear();
    newToOldIndex.push_back(0);

    for (unsigned int k = 1; k < vStsZPos_temp.size(); k++){
      vector<float>::iterator itpos = vStsZPos_temp2.begin()+1;
      vector<int>::iterator iti = newToOldIndex.begin()+1;
      for (; itpos < vStsZPos_temp2.end(); itpos++, iti++){
        if (vStsZPos_temp[k] < *itpos){
          vStsZPos_temp2.insert(itpos,vStsZPos_temp[k]);
          newToOldIndex.insert(iti,k);
          break;
        }
      }
      if (itpos == vStsZPos_temp2.end()){
        vStsZPos_temp2.push_back(vStsZPos_temp[k]);
        newToOldIndex.push_back(k);
      }
    } // k


    if (fVerbose >= 10) cout << "ReadEvent: z-pos are sorted." << endl;
    
      // save z-pos
    for (unsigned int k = 0; k < vStsZPos_temp2.size(); k++)
      algo->vStsZPos.push_back(vStsZPos_temp2[k]);
      // correct index of z-pos in hits array
    int size_nto_tmp = newToOldIndex.size();
    vector<int> oldToNewIndex;
    oldToNewIndex.clear();
    oldToNewIndex.resize(size_nto_tmp);
    for (int k = 0; k < size_nto_tmp; k++)
      oldToNewIndex[newToOldIndex[k]] = k;
    
    int size_hs_tmp = vHitStore.size();
    for (int k = 0; k < size_hs_tmp; k++)
      algo->vStsHits[k].iz = oldToNewIndex[algo->vStsHits[k].iz];
    
//    // check index z-pos
//     for (int k = 0; k < algo->vStsHits.size(); k++){
//       fvec z1 = algo->vStsZPos[algo->vStsHits[k].iz];
//       fvec z2 = algo->vStations[algo->vSFlag[algo->vStsHits[k].f]/4].z;
//       if (fabs(z1[0] - z2[0]) > 1.0){
//         cout << z1[0] << "  " << z2[0] << endl;
//         int kk;
//         std::cin >> k;
//       }
//     }
  }

  if (fVerbose >= 10) cout << "ReadEvent: z-pos are saved." << endl;

  HitMatch();
  
  if (fVerbose >= 10) cout << "HitMatch is done." << endl;
  
    // -- sort MC points --
  vector<TmpMCPoint> vtmpMCPoints;
  for ( unsigned int iMCPoint = 0; iMCPoint < vMCPoints.size(); iMCPoint++) {
    CbmL1MCPoint &MC = vMCPoints[iMCPoint];
    TmpMCPoint tmp;
    tmp.ID = MC.ID;
    tmp.z = MC.z;
//    tmp.StsHit = i;
    tmp.MCPoint = iMCPoint;
    tmp.eff = 1;
    vtmpMCPoints.push_back(tmp);
  }

//   for ( unsigned int i=0; i<vUnEffHitStore.size(); i++ ){
//     int iMC = vUnEffHitMCRef[i];
//     if( iMC < 0 ) continue;
//     CbmL1MCPoint &MC = vMCPoints[iMC];
//     TmpMCPoint tmp;
//     tmp.ID = MC.ID;
//     tmp.z = MC.z;
//     tmp.StsHit = i;
//     tmp.MCPoint = &MC;
//     tmp.eff = 0;
//     vtmpMCPoints.push_back(tmp);
//   }
  
  sort( vtmpMCPoints.begin(), vtmpMCPoints.end(), TmpMCPoint::compareIDz );
  if (fVerbose >= 10) cout << "MCPoints are sorted." << endl;

//     // -- fill vMCTracks array --
//   PrimVtx.MC_ID = 999;
//   {
//     CbmL1Vtx Vtxcurr;
//     int nvtracks=0,
//         nvtrackscurr=0;
//     int ID = -10000;
//     for ( vector<TmpMCPoint>::iterator i= vtmpMCPoints.begin(); i!=vtmpMCPoints.end(); ++i){
//       if( vMCTracks.empty() || i->ID!= ID ){ // new track
//         ID = i->ID;
//         CbmL1MCTrack T;
//         T.ID = ID;
//         if( listMCTracks )
//         {
//           CbmMCTrack *MCTrack = L1_DYNAMIC_CAST<CbmMCTrack*>( listMCTracks->At( T.ID ) );
//           int mother_ID = MCTrack->GetMotherId();
//           TVector3 vr;
//           TLorentzVector vp;
//           MCTrack->GetStartVertex(vr);
//           MCTrack->Get4Momentum(vp);
// 
//           T = CbmL1MCTrack(vMCPoints[i->MCPoint].mass, vMCPoints[i->MCPoint].q, vr, vp, i->ID, mother_ID, vMCPoints[i->MCPoint].pdg);
//           T.time = MCTrack->GetStartT();
//         }
//         vMCTracks.push_back( T );
//         
//         if ( T.mother_ID ==-1 ){ // vertex track
// 
//           if (  PrimVtx.MC_ID == 999 || fabs(T.z-Vtxcurr.MC_z)>1.e-7 ){// new vertex
//             if( nvtrackscurr > nvtracks ){
//               PrimVtx = Vtxcurr;
//               nvtracks = nvtrackscurr;
//             }
//             Vtxcurr.MC_x  = T.x;
//             Vtxcurr.MC_y  = T.y;
//             Vtxcurr.MC_z  = T.z;
//             Vtxcurr.MC_ID = T.mother_ID;
//             nvtrackscurr = 1;
//           }
//           else nvtrackscurr++;
//           
//         }
//       } // new track
//       vMCTracks.back().Points.push_back(i->MCPoint);
// //       if( i->eff ) vMCTracks.back().StsHits.push_back(i->StsHit);
//     } // for i of tmpMCPoints
//     if( nvtrackscurr > nvtracks ) PrimVtx = Vtxcurr;
//   } // fill MC tracks
  
  PrimVtx.MC_ID = 999;
  {
    CbmL1Vtx Vtxcurr;
    int nvtracks=0, nvtrackscurr=0;
 
    if( listMCTracks )
    {
      unsigned int iMCPoint = 0;
      
      for(int iMCTrack=0; iMCTrack<listMCTracks->GetEntriesFast(); iMCTrack++)
      {
        CbmMCTrack *MCTrack = L1_DYNAMIC_CAST<CbmMCTrack*>( listMCTracks->At( iMCTrack ) );
        
        int mother_ID = MCTrack->GetMotherId();
        TVector3 vr;
        TLorentzVector vp;
        MCTrack->GetStartVertex(vr);
        MCTrack->Get4Momentum(vp);

        Int_t pdg = MCTrack->GetPdgCode();
        Double_t q=1, mass = 0.;
        if ( pdg < 9999999 && ( (TParticlePDG *)TDatabasePDG::Instance()->GetParticle(pdg) ))
        {
          q = TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.0;
          mass = TDatabasePDG::Instance()->GetParticle(pdg)->Mass(); 
        }
        else    q = 0;
        
        CbmL1MCTrack T(mass, q, vr, vp, iMCTrack, mother_ID, pdg);
        T.time = MCTrack->GetStartT();
        
        if(vtmpMCPoints.size()>0)
          while( vtmpMCPoints[iMCPoint].ID == iMCTrack && iMCPoint < vtmpMCPoints.size() ) 
          {
            T.Points.push_back(vtmpMCPoints[iMCPoint].MCPoint);
            iMCPoint++;
          }
        
        vMCTracks.push_back( T );
        
        if ( T.mother_ID ==-1 ){ // vertex track
          if (  PrimVtx.MC_ID == 999 || fabs(T.z-Vtxcurr.MC_z)>1.e-7 ){// new vertex
            if( nvtrackscurr > nvtracks ){
              PrimVtx = Vtxcurr;
              nvtracks = nvtrackscurr;
            }
            Vtxcurr.MC_x  = T.x;
            Vtxcurr.MC_y  = T.y;
            Vtxcurr.MC_z  = T.z;
            Vtxcurr.MC_ID = T.mother_ID;
            nvtrackscurr = 1;
          }
          else nvtrackscurr++;
        }
      }
    }
    if( nvtrackscurr > nvtracks ) PrimVtx = Vtxcurr;
  }
  
  if (fVerbose >= 10) cout << "MCPoints and MCTracks are saved." << endl;

  
    // calculate the max number of Hits\mcPoints on continuous(consecutive) stations
  for( vector<CbmL1MCTrack>::iterator it = vMCTracks.begin(); it != vMCTracks.end(); ++it){
    it->Init();
  } // for i vMCTracks
  
  if ( fVerbose && PrimVtx.MC_ID == 999 ) cout<<"No primary vertex !!!"<<endl;



  if (fVerbose >= 10) cout << "ReadEvent is done." << endl;
} // void CbmL1::ReadEvent()


bool CbmL1::ReadMCPoint( CbmL1MCPoint *MC, int iPoint, bool MVD )
{
  TVector3 xyzI,PI, xyzO,PO;
  Int_t mcID=-1;
  Double_t time = 0.f;
  if( MVD && listMvdPts ){
    CbmMvdPoint *pt = L1_DYNAMIC_CAST<CbmMvdPoint*>( listMvdPts->At(iPoint) );
    if ( !pt ) return 1;
    pt->Position(xyzI);
    pt->Momentum(PI);
    pt->PositionOut(xyzO);
    pt->MomentumOut(PO);
    mcID = pt->GetTrackID();
    time = pt->GetTime();
  }else if( listStsPts ){
    CbmStsPoint *pt = L1_DYNAMIC_CAST<CbmStsPoint*>( listStsPts->At(iPoint) );
    if ( !pt ) return 1;
    pt->Position(xyzI);
    pt->Momentum(PI);
    pt->PositionOut(xyzO);
    pt->MomentumOut(PO);
    mcID = pt->GetTrackID();
    time = pt->GetTime();
  }
  TVector3 xyz = .5*(xyzI + xyzO );
  TVector3 P = .5*(PI + PO );
  MC->x  = xyz.X();
  MC->y  = xyz.Y();
  MC->z  = xyz.Z();
  MC->px = P.X();
  MC->py = P.Y();
  MC->pz = P.Z();
  MC->xIn  = xyzI.X();
  MC->yIn  = xyzI.Y();
  MC->zIn  = xyzI.Z();
  MC->pxIn = PI.X();
  MC->pyIn = PI.Y();
  MC->pzIn = PI.Z();
  MC->xOut  = xyzO.X();
  MC->yOut  = xyzO.Y();
  MC->zOut  = xyzO.Z();
  MC->pxOut = PO.X();
  MC->pyOut = PO.Y();
  MC->pzOut = PO.Z();
  MC->p = sqrt( fabs( MC->px*MC->px + MC->py*MC->py + MC->pz*MC->pz ) );
  MC->ID = mcID;
  MC->pointId = iPoint;
  
  MC->time = time;
  
  if(MC->ID < 0)
    return 1;
  CbmMCTrack *MCTrack = L1_DYNAMIC_CAST<CbmMCTrack*>( listMCTracks->At( MC->ID ) );
  if ( !MCTrack ) return 1;
  MC->pdg  = MCTrack->GetPdgCode();
  MC->mother_ID = MCTrack->GetMotherId();

  TParticlePDG* particlePDG = TDatabasePDG::Instance()->GetParticle(MC->pdg);
  if(particlePDG)
  {
    MC->q = particlePDG->Charge()/3.0;
    MC->mass = particlePDG->Mass();
  }

  return 0;
}


  /// Procedure for match hits and MCPoints.
  /// Read information about correspondence between hits and mcpoints and fill CbmL1MCPoint::hitIds and CbmL1StsHit::mcPointIds arrays
  /// should be called after fill of algo
void CbmL1::HitMatch()
{  
  const int NHits = vStsHits.size();
  for (int iH = 0; iH < NHits; iH++){
    CbmL1StsHit& hit = vStsHits[iH];

    int iP = vHitMCRef[iH];
    if (iP >= 0){
      hit.mcPointIds.push_back( iP );
      vMCPoints[iP].hitIds.push_back(iH);
    }
    
//     if (hit.mcPointIds.size() > 1){ // CHECKME  never works!
//       cout << "Hit number " << iH << " has been matched with " << hit.mcPointIds.size() << " mcPoints." << endl;
//       cout << "Hit number " << iH << " " <<  hit.mcPointIds[0] << " " << vHitMCRef[iH] << endl;
//     }
  } // for hits
}
