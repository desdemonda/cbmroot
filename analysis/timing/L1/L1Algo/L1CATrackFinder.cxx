/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction
 *
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de
 *
 *=====================================================
 *
 *  Finds tracks using the Cellular Automaton algorithm
 *
 */

#include "TStopwatch.h"

#include "L1Algo.h"
#include "L1TrackPar.h"
#include "L1Branch.h"
#include "L1Track.h"
#include "L1Extrapolation.h"
#include "L1Filtration.h"
#include "L1AddMaterial.h"
#include "L1HitPoint.h"

#include "L1Portion.h"

#ifdef DRAW
#include "utils/L1AlgoDraw.h"
#endif
#ifdef PULLS
#include "utils/L1AlgoPulls.h"
#endif
#ifdef TRIP_PERFORMANCE
#include "utils/L1AlgoEfficiencyPerformance.h"
#endif
#ifdef DOUB_PERFORMANCE
#include "utils/L1AlgoEfficiencyPerformance.h"
#endif

#ifdef TBB
#include "L1AlgoTBB.h"
#endif

#include <iostream>
#include <map>
#include <list>
#include <stdio.h>
#include <algorithm>

// using namespace std;
using std::cout;
using std::endl;
using std::vector;


          /// Prepare the portion of left hits data
inline void L1Algo::f10(  // input
                int start_lh, int n1,  L1HitPoint *vStsHits_l,
                  // output
                fvec *u_front, fvec *u_back, fvec *zPos,
                vector<THitI> &hitsl_1
                )
{
  int end_lh = start_lh+n1;
  for (int ilh = start_lh, i1 = 0; ilh < end_lh; ilh++, i1++){
    int i1_V= i1/fvecLen;
    int i1_4= i1%fvecLen;
    L1HitPoint &hitl = vStsHits_l[ilh];

    hitsl_1.push_back(ilh);
    u_front[i1_V][i1_4] = hitl.u;
    u_back [i1_V][i1_4] = hitl.v;
    zPos   [i1_V][i1_4] = hitl.z;
  }
}


          /// Get the field approximation. Add the target to parameters estimation. Propagaete to middle station.
inline void L1Algo::f11(  // input
                int istal, int istam,
                int n1_V,

                fvec *u_front, fvec *u_back,  fvec *zPos,
                  // output
//                 L1TrackPar *T_1,
                nsL1::vector<L1TrackPar>::TSimd &T_1,
                nsL1::vector<L1FieldRegion>::TSimd &fld_1,
                fvec* x_minusV, fvec* x_plusV, fvec* y_minusV, fvec* y_plusV
               )
{
  L1Station &stal = vStations[istal];
  L1Station &stam = vStations[istam];
  fvec zstal = stal.z;
  fvec zstam = stam.z;

  for( int i1_V=0; i1_V<n1_V; i1_V++ ){
        // get the magnetic field along the track.
        // (suppose track is straight line with origin in the target)
    fvec &u = u_front[i1_V];
    fvec &v = u_back [i1_V];
    fvec xl, yl; // left(1-st) hit coor
    fvec zl = zPos   [i1_V];

    fvec dzli = 1./(zl-targZ);
    StripsToCoor(u, v, xl, yl, stal);

    fvec tx = (xl - targX)*dzli;
    fvec ty = (yl - targY)*dzli;

    L1FieldRegion fld0;      // by  left   hit, target and "center" (station in-between). Used here for extrapolation to target and to middle hit
    L1FieldRegion fld1; // by  middle hit, left hit and "center" . Will be used for extrapolation to right hit

            // estimate field for singlet creation
    L1FieldValue centB, l_B _fvecalignment;
    int istac = istal/2; // "center" station
//     if (istal >= NMvdStations) // to make it in the same way for both with and w\o mvd
//       istac = (istal-NMvdStations)/2+NMvdStations;
    L1Station &stac = vStations[istac];
    fvec zstac = stac.z;
    stac.fieldSlice.GetFieldValue( targX + tx*(zstac - targZ), targY + ty*(zstac - targZ), centB );
    stal.fieldSlice.GetFieldValue( targX + tx*(zstal - targZ), targY + ty*(zstal - targZ), l_B );
    if( istac != istal  ){
      fld0.Set( l_B, stal.z, centB, stac.z, targB, targZ );
    }
    else{
      fld0.Set( l_B, zstal, targB, targZ );
    }
          // estimate field for the next extrapolations
    L1FieldValue m_B _fvecalignment;
    stam.fieldSlice.GetFieldValue( targX + tx*(zstam - targZ), targY + ty*(zstam - targZ), m_B );
#define USE_3HITS
#ifndef USE_3HITS
    if( istac != istal ){
      fld1.Set( m_B, zstam, l_B, zstal, centB, zstac  );
    }
    else{
      fld1.Set( m_B, zstam, l_B, zstal, targB, targZ  );
    }
#else // if USE_3HITS  // the best now
    L1FieldValue r_B _fvecalignment;
    L1Station &star = vStations[istam+1];
    fvec zstar = star.z;
    star.fieldSlice.GetFieldValue( targX + tx*(zstar - targZ), targY + ty*(zstar - targZ), r_B );
    fld1.Set( r_B, zstar, m_B, zstam, l_B, zstal);
#endif // USE_3HITS
    fld_1.push_back(fld1);

// #define BEGIN_FROM_TARGET
#ifndef BEGIN_FROM_TARGET // the best now
    L1TrackPar T;

    T.chi2 = 0.;
    T.NDF = 2.;
    if ( (isec == kAllSecIter) || (isec == kAllSecJumpIter) ) T.NDF = 0;
    T.x  = xl;
    T.y  = yl;
    T.tx = tx;
    T.ty = ty;
    T.qp = 0.;
    T.z  = zl;
    T.C00 = stal.XYInfo.C00;
    T.C10 = stal.XYInfo.C10;
    T.C11 = stal.XYInfo.C11;
    T.C20 = T.C21 = 0;
    T.C30 = T.C31 = T.C32 = 0;
    T.C40 = T.C41 = T.C42 = T.C43 = 0;
    T.C22 = T.C33 = 10.; T.C44 = MaxInvMom/3.*MaxInvMom/3.;

    //add the target
    {
      fvec eX, eY, J04, J14;
      fvec dz;
      dz = targZ - zl;
      L1ExtrapolateJXY0(T.tx, T.ty, dz, fld0, eX, eY, J04, J14 );
      fvec J[6];
      J[0]= dz; J[1] = 0; J[2]= J04;
      J[3] = 0; J[4]= dz; J[5]= J14;
      L1FilterVtx( T, targX, targY, TargetXYInfo, eX, eY, J );
    }
#else  // TODO: doesn't work. Why?
    L1TrackPar T;

    T.chi2 = 0.;
    T.NDF = 2.;
    if ( (isec == kAllSecIter) || (isec == kAllSecJumpIter) ) T.NDF = 0;
    T.x  = targX;
    T.y  = targY;
    T.tx = tx;
    T.ty = ty;
    T.qp = 0.;
    T.z  = targZ;
    T.C00 = TargetXYInfo.C00;
    T.C10 = TargetXYInfo.C10;
    T.C11 = TargetXYInfo.C11;
    T.C20 = T.C21 = 0;
    T.C30 = T.C31 = T.C32 = 0;
    T.C40 = T.C41 = T.C42 = T.C43 = 0;
    T.C22 = T.C33 = 10.; T.C44 = MaxInvMom/3.*MaxInvMom/3.;
    
      // extrapolate to left hit
    L1Extrapolate0( T, zl, fld0 );
    for (int ista = 0; ista <= istal-1; ista++){
      L1AddMaterial( T, vStations[ista].materialInfo, MaxInvMom );
      if (ista == NMvdStations - 1) L1AddPipeMaterial( T, MaxInvMom );
    }
      // add left hit
    L1Filter( T, stal.frontInfo, u );
    L1Filter( T, stal.backInfo,  v );
#endif

    L1AddMaterial( T, stal.materialInfo, MaxInvMom );
    if ( (istam >= NMvdStations) && (istal <= NMvdStations - 1) )  L1AddPipeMaterial( T, MaxInvMom );

     L1Extrapolate0( T, zstam, fld0 ); // TODO: fld1 doesn't work!
//     L1Extrapolate( T, zstam, T.qp, fld1 );

      // find region on next station to find middle hit
//     fvec dxm_est = Pick_m*sqrt(fabs(T.C00+stam.XYInfo.C00));
//     fvec dym_est = Pick_m*sqrt(fabs(T.C11+stam.XYInfo.C11));
// 
//     x_minusV[i1_V] = T.x - dxm_est;
//     x_plusV [i1_V] = T.x + dxm_est;
//     y_minusV[i1_V] = T.y - dym_est;
//     y_plusV [i1_V] = T.y + dym_est;

    T_1.push_back(T);
  }// i1_V
}


          /// Find the doublets. Reformat data in the portion of doublets.
inline void L1Algo::f20(  // input
                int n1, L1Station &stal, L1Station &stam,
                L1HitPoint *vStsHits_l, L1HitPoint *vStsHits_m, int NHits_m,
                fscal *y_minus, fscal *x_minus, fscal *y_plus, fscal *x_plus,
                nsL1::vector<L1TrackPar>::TSimd &T_1, nsL1::vector<L1FieldRegion>::TSimd &fld_1,
                vector<THitI> &hitsl_1,
                map<unsigned /*short*/ int, THitI> &mrDuplets_start,
                  // output
                int &n2,
                vector<THitI> &i1_2,
                int &start_mhit,
#ifdef DOUB_PERFORMANCE
                vector<THitI> &hitsl_2,
#endif // DOUB_PERFORMANCE
                vector<THitI> &hitsm_2,
                map<unsigned /*short*/ int, THitI> &lmDuplets_start, vector<THitI> &lmDuplets_hits, unsigned int &nDuplets_lm
                )
{
  n2 = 0; // number of doublet
  for (int i1 = 0; i1 < n1; i1++){ // for each singlet
    int i1_V = i1/fvecLen;
    int i1_4 = i1%fvecLen;
    L1TrackPar& T1 = T_1[i1_V];

    lmDuplets_start[hitsl_1[i1]] = nDuplets_lm; // mark begin

    THitI nl = vStsHits_l[hitsl_1[i1]].n;
    Double_t tl = vStsHits_l[hitsl_1[i1]].t;
      // -- find first possible hit for next singlet --
    for (; start_mhit < NHits_m; start_mhit++){      // 1.0/1000 sec
      L1HitPoint &hitm = vStsHits_m[start_mhit];

        // find track position in the hit plane
      fscal &zm = hitm.z;
      L1TrackPar T1_tmp = T1;
      L1ExtrapolateShort( T1_tmp, zm, T1.qp, fld_1[i1_V]);
      fvec dym_est = Pick_m * sqrt(fabs(T1_tmp.C11 + stam.XYInfo.C11));
      fvec y_minus_new_break = T1_tmp.y - dym_est;
      y_minus_new_break -= 2*0.4*fabs(T1_tmp.ty); // take into account overlapping on left&middle station. dz ~ 0.4 cm.

        // check position
      fscal ym = hitm.y;
      if ( ym >= y_minus_new_break[i1_4]) break;
    }
    
      // -- collect possible doublets --
    for (int imh = start_mhit; imh < NHits_m; imh++){   // 6.3/100 sec
//       if( n2 >= MaxPortionDoublets ) break;
      L1HitPoint &hitm = vStsHits_m[imh];
         // find track position in the hit plane
      fscal &zm = hitm.z;
      double tm = hitm.t;
      L1TrackPar T1_tmp = T1;
      L1ExtrapolateShort( T1_tmp, zm, T1.qp, fld_1[i1_V]);
      fvec dym_est = Pick_m * sqrt(fabs(T1_tmp.C11 + stam.XYInfo.C11));
      fvec y_minus_new = T1_tmp.y - dym_est;
      fvec y_plus_new = T1_tmp.y + dym_est;
      fvec y_plus_new_break = y_plus_new + 0.4*fabs(T1_tmp.ty); // take into account overlapping on middle station. dz_max ~ 0.4 cm. dy/dz ~ 1
      
        // check position
      unsigned short int nm = hitm.n;
      fscal ym = hitm.y;
      if ( ym > y_plus_new_break[i1_4] ) break;
      if ( ( ym < y_minus_new[i1_4]) || ( ym > y_plus_new[i1_4] ) || ( nl != nm)) continue;
      
      fvec dxm_est = Pick_m * sqrt(fabs(T1_tmp.C00 + stam.XYInfo.C00));
      fvec x_minus_new = T1_tmp.x - dxm_est;
      fvec x_plus_new = T1_tmp.x + dxm_est;

      fscal xm = hitm.x;
      if ( (xm < x_minus_new[i1_4] ) || (xm > x_plus_new[i1_4]) ) continue;
      if (tm-tl>10 || tm-tl<-10) continue;
//       lmDuplets_hits[nDuplets_lm++]=imh;
      lmDuplets_hits.push_back(imh);
      nDuplets_lm++;

//       i1_2[n2] = i1;
//      hitsm_2[n2] = imh;
      i1_2.push_back(i1);
#ifdef DOUB_PERFORMANCE
      hitsl_2.push_back(hitsl_1[i1]);
#endif // DOUB_PERFORMANCE
      hitsm_2.push_back(imh);

      n2++;
    }

    lmDuplets_start[hitsl_1[i1]+1] = nDuplets_lm; // mark the end
  }  // for i1
}


          /// Add the middle hits to parameters estimation. Propagate to right station.
/*void inline f21(  // input
                int n2_V,
                L1Station &stam, L1Station &star,
                fvec *u_front, fvec *u_back,
                L1FieldRegion *fld_2,
                double Pick_r,
                  // output
                L1TrackPar *T_2,
                fvec* x_minusV, fvec* x_plusV, fvec* y_minusV, fvec* y_plusV
               )
{
  for( int i2_V=0; i2_V<n2_V; i2_V++){

    L1TrackPar &T2 = T_2[i2_V];
    L1FieldRegion &f2 = fld_2[i2_V];
    L1Filter( T2, stam.frontInfo, u_front[i2_V] );
    L1Filter( T2, stam.backInfo,  u_back[i2_V] );

    L1AddMaterial( T2, stam.materialInfo, T2.qp );

    L1Extrapolate( T2, star.z, T2.qp, f2 );
    fvec dxm_est = Pick_r*sqrt(fabs(T2.C00+star.XYInfo.C00));
    fvec dym_est = Pick_r*sqrt(fabs(T2.C11+star.XYInfo.C11));
    x_minusV[i2_V] = T2.x - dxm_est;
    x_plusV [i2_V] = T2.x + dxm_est;
    y_minusV[i2_V] = T2.y - dym_est;
    y_plusV [i2_V] = T2.y + dym_est;

  }
}
*/

          /// Add the middle hits to parameters estimation. Propagate to right station.
          /// Find the triplets(right hit). Reformat data in the portion of triplets.
inline void L1Algo::f30(  // input
                L1HitPoint *vStsHits_r, int NHits_r, L1Station &stam, L1Station &star,

                int istam, int istar, int n1,
                L1HitPoint *vStsHits_m,
                nsL1::vector<L1TrackPar>::TSimd &T_1, nsL1::vector<L1FieldRegion>::TSimd &fld_1,
                vector<THitI> &hitsl_1,
                map<unsigned /*short*/ int, THitI> &lmDuplets_start, vector<THitI> &lmDuplets_hits,

                int n2,
                vector<THitI> &hitsm_2,
                vector<THitI> &i1_2,

                map<unsigned /*short*/ int, THitI> &mrDuplets_start, vector<THitI> &mrDuplets_hits,
//                 int MaxPortionTriplets,
                  // output
                int &n3,
                nsL1::vector<L1TrackPar>::TSimd &T_3,
                vector<THitI> &hitsl_3,  vector<THitI> &hitsm_3,  vector<THitI> &hitsr_3,
                nsL1::vector<fvec>::TSimd &u_front_3, nsL1::vector<fvec>::TSimd &u_back_3
                )
{
  nsL1::vector<L1TrackPar>::TSimd T_2;
  nsL1::vector<L1FieldRegion>::TSimd fld_2;
  vector<THitI> hitsl_2;
  fvec fvec_0;
  L1TrackPar L1TrackPar_0;
  nsL1::vector<fvec>::TSimd u_front_2;
  nsL1::vector<fvec>::TSimd u_back_2;
  nsL1::vector<fvec>::TSimd zPos_2;
  nsL1::vector<fvec>::TSimd x_minusV_2;
  nsL1::vector<fvec>::TSimd x_plusV_2;
  nsL1::vector<fvec>::TSimd y_minusV_2;
  nsL1::vector<fvec>::TSimd y_plusV_2;

  T_2.reserve(MaxPortionDoublets/fvecLen);
  fld_2.reserve(MaxPortionDoublets/fvecLen);
  hitsl_2.reserve(MaxPortionDoublets);
  u_front_2.reserve(MaxPortionDoublets/fvecLen);
  u_back_2.reserve(MaxPortionDoublets/fvecLen);
  zPos_2.reserve(MaxPortionDoublets/fvecLen);
  x_minusV_2.reserve(MaxPortionDoublets/fvecLen);
  x_plusV_2.reserve(MaxPortionDoublets/fvecLen);
  y_minusV_2.reserve(MaxPortionDoublets/fvecLen);
  y_plusV_2.reserve(MaxPortionDoublets/fvecLen);



    // ---- Add the middle hits to parameters estimation. Propagate to right station. ----
  int n2_V = (n2+fvecLen-1)/fvecLen;
  if (istar < NStations){
    for (int i2_V = 0; i2_V < n2_V; i2_V++){
      const int max_i2_4 = ( (n2-i2_V*fvecLen) >= fvecLen ) ? fvecLen : (n2-i2_V*fvecLen);
//       L1TrackPar &T2 = T_2[i2_V];
//       L1FieldRegion &f2 = fld_2[i2_V];
      L1TrackPar T2;
      L1FieldRegion f2;

        // pack the data
      u_front_2.push_back(fvec_0);
      u_back_2.push_back(fvec_0);
      zPos_2.push_back(fvec_0);
      for (int i2_4 = 0; i2_4 < max_i2_4; i2_4++){
        int i2 = i2_V*fvecLen+i2_4;
        int i1 = i1_2[i2];
        int i1_V = i1/fvecLen;
        int i1_4 = i1%fvecLen;

        L1TrackPar &T1 = T_1[i1_V];
        L1FieldRegion &f1 = fld_1[i1_V];
        T2.SetOneEntry(i2_4, T1, i1_4);
        f2.SetOneEntry(i2_4, f1, i1_4);
        
        int imh = hitsm_2[i2];
        L1HitPoint &hitm = vStsHits_m[imh];
        fscal &um = hitm.u;
        fscal &vm = hitm.v;
        fscal &zm = hitm.z;
        u_front_2[i2_V][i2_4] = um;
        u_back_2 [i2_V][i2_4] = vm;
        zPos_2   [i2_V][i2_4] = zm;

//         hitsl_2[i2] = hitsl_1[i1];
        hitsl_2.push_back(hitsl_1[i1]);
      }  // i2_4

        // add middle hit
      L1ExtrapolateShort( T2, zPos_2[i2_V], T2.qp, f2);
      L1Filter( T2, stam.frontInfo, u_front_2[i2_V] );
      L1Filter( T2, stam.backInfo,  u_back_2[i2_V] );

      L1AddMaterial( T2, stam.materialInfo, T2.qp );
      if ( (istar >= NMvdStations) && (istam <= NMvdStations - 1) ) L1AddPipeMaterial( T2, T2.qp );

//         // update field     // don't help
//       L1TrackPar T2_tmp = T2;
//       L1Extrapolate( T2_tmp, star.z, T2.qp, f2 );
//       fvec lz = vStations[istar-2].z;
//       L1FieldValue l_B,m_B,r_B _fvecalignment;
//       l_B = f2.Get(lz);
//       m_B = f2.Get(stam.z);
//       star.fieldSlice.GetFieldValue( T2_tmp.x + T2_tmp.tx*(star.z - T2_tmp.z), T2_tmp.y + T2_tmp.ty*(star.z - T2_tmp.z), r_B );
//       f2.Set     ( l_B, lz, m_B, stam.z, r_B, star.z );

//       L1FieldValue r_B1, r_B2 _fvecalignment; // Debug
//       star.fieldSlice.GetFieldValue( T2.x + T2.tx*(star.z - T2.z), T2.y + T2.ty*(star.z - T2.z), r_B1 );
//       r_B2 = f2.Get(T2.z);
//       if ( fabs(((r_B2.x-r_B1.x)+(r_B2.y-r_B1.y)+(r_B2.z-r_B1.z))[0]) > 10. )
//         cout << T2_tmp.x[0] << " " << T2_tmp.y[0] << " " << T2_tmp.z[0] << endl << r_B2 << endl << r_B1 << endl;

        // extrapolate to the right hit station
      L1Extrapolate( T2, star.z, T2.qp, f2 );
//       T2.L1Extrapolate( star.z, T2.qp, f2 );
//       fvec dxm_est = Pick_r*sqrt(fabs(T2.C00+star.XYInfo.C00));
//       fvec dym_est = Pick_r*sqrt(fabs(T2.C11+star.XYInfo.C11));
// //       x_minusV_2[i2_V] = T2.x - dxm_est;
// //       x_plusV_2 [i2_V] = T2.x + dxm_est;
// //       y_minusV_2[i2_V] = T2.y - dym_est;
// //       y_plusV_2 [i2_V] = T2.y + dym_est;
//       x_minusV_2.push_back(T2.x - dxm_est);
//       x_plusV_2 .push_back(T2.x + dxm_est);
//       y_minusV_2.push_back(T2.y - dym_est);
//       y_plusV_2 .push_back(T2.y + dym_est);

      T_2.push_back(T2);
      fld_2.push_back(f2);
    }   // i2_V
  }  // if

//   fscal *x_minus  = (fscal*) (&(x_minusV_2[0]));
//   fscal *x_plus  =  (fscal*) (&(x_plusV_2 [0]));
//   fscal *y_minus =  (fscal*) (&(y_minusV_2[0]));
//   fscal *y_plus  =  (fscal*) (&(y_plusV_2 [0]));

    // ---- Find the triplets(right hit). Reformat data in the portion of triplets. ----
  int n3_V = 0, n3_4 = 0;
  
  for( int i2 = 0; i2 < n2; i2++){
    int i2_V = i2/fvecLen;
    int i2_4 = i2%fvecLen;

    THitI duplet_b = mrDuplets_start[hitsm_2[i2]];      // < 0.001/100000 sec (cash!)
    THitI duplet_e = mrDuplets_start[hitsm_2[i2]+1];

      //     THitI nm = vStsHits_m[hitsm_2[i2]].n;
    L1TrackPar &T2 = T_2[i2_V];

    T_3.push_back(L1TrackPar_0);
    u_front_3.push_back(fvec_0);
    u_back_3.push_back(fvec_0);
    for (unsigned int irh_index = duplet_b; irh_index < duplet_e; irh_index++){ //  2.1/10 sec

        //       if( n3 >= MaxPortionTriplets ) break;
      int irh = mrDuplets_hits[irh_index];

      L1HitPoint &hitr = vStsHits_r[irh];

      fscal ur = hitr.u;
      fscal vr = hitr.v;
      fscal zr = hitr.z;

        // find track position in the hit plane
      L1TrackPar T2_new = T2;
      L1ExtrapolateShort( T2_new, zr, T2.qp, fld_2[i2_V]);
        //       L1Extrapolate( T2_new, zr, T2.qp, fld_2[i2_V] );
      fvec dym_est = Pick_r*sqrt(fabs(T2_new.C11 + star.XYInfo.C11));
      fvec y_minus_new = T2_new.y - dym_est;

      fscal yr = hitr.y;

        // check the position
      if (yr < y_minus_new[i2_4]) continue;

      fvec y_plus_new  = T2_new.y + dym_est;
      fvec y_plus_new_break = y_plus_new + 0.4*fabs(T2_new.ty); // take into account overlapping on right station. dz_max ~ 0.4 cm. dy/dz ~ 1 TODO read parameter

      if (yr > y_plus_new_break [i2_4] ) break;
      if (yr > y_plus_new [i2_4] ) continue;

      fvec dxm_est = Pick_r*sqrt(fabs(T2_new.C00 + star.XYInfo.C00));
      fvec x_minus_new = T2_new.x - dxm_est;
      fvec x_plus_new  = T2_new.x + dxm_est;

      fscal xr = hitr.x;
      
      if ((xr < x_minus_new[i2_4]) || (xr > x_plus_new[i2_4]) ) continue;

        // pack triplet
      L1TrackPar &T3 = T_3[n3_V];

        //       hitsl_3[n3] = hitsl_2[i2];   // 1/3  2.3/1000 sec
        //       hitsm_3[n3] = hitsm_2[i2];   // 1/3  2.3/1000 sec
        //       hitsr_3[n3] = irh;           // 1/3  2.3/1000 sec
      hitsl_3.push_back(hitsl_2[i2]);   // 1/3  2.3/1000 sec
      hitsm_3.push_back(hitsm_2[i2]);   // 1/3  2.3/1000 sec
      hitsr_3.push_back(irh);           // 1/3  2.3/1000 sec

      T3.SetOneEntry(n3_4, T2_new, i2_4);
      u_front_3[n3_V][n3_4] = ur;
      u_back_3 [n3_V][n3_4] = vr;


      n3++;                    // < 0.001/1000000
      n3_V = n3/fvecLen;      // < 0.001/1000000
      n3_4 = n3 % fvecLen;
      if (!n3_4){ T_3.push_back(L1TrackPar_0); u_front_3.push_back(fvec_0); u_back_3.push_back(fvec_0); }
    }
  }// i2
  

}


          /// Add the right hits to parameters estimation.
inline void L1Algo::f31(  // input
                int n3_V,
                L1Station &star,
                nsL1::vector<fvec>::TSimd &u_front, nsL1::vector<fvec>::TSimd &u_back,
                  // output
                //                L1TrackPar *T_3
                nsL1::vector<L1TrackPar>::TSimd &T_3
               )
{
  for( int i3_V=0; i3_V<n3_V; i3_V++){
    L1Filter( T_3[i3_V], star.frontInfo, u_front[i3_V] );    // 2.1/100 sec
    L1Filter( T_3[i3_V], star.backInfo,  u_back [i3_V] );   // 2.0/100 sec
  };
}


          /// Refit Triplets.
inline void L1Algo::f32( // input // TODO not updated after gaps introduction
    int n3, int istal,
                nsL1::vector<L1TrackPar>::TSimd &T_3,
                vector<THitI> &hitsl_3,  vector<THitI> &hitsm_3,  vector<THitI> &hitsr_3,
                int nIterations
                       )
{
  const int NHits = 3; // triplets

    // prepare data
  int ista[NHits] = {
    istal,
    istal + 1,
    istal + 2
  };
  
  L1Station sta[3];
  for (int is = 0; is < NHits; is++){
    sta[is] = vStations[ista[is]];
  };

  for( int i3=0; i3<n3; i3++){
    int i3_V = i3/fvecLen;
    int i3_4 = i3%fvecLen;

    L1TrackPar &T3 = T_3[i3_V];
    fscal qp0 = T3.qp[i3_4];

      // prepare data
    THitI ihit[NHits] = {
      RealIHit[hitsl_3[i3] + StsHitsUnusedStartIndex[ista[0]]],
      RealIHit[hitsm_3[i3] + StsHitsUnusedStartIndex[ista[1]]],
      RealIHit[hitsr_3[i3] + StsHitsUnusedStartIndex[ista[2]]]
    };

    fscal u[NHits], v[NHits], x[NHits], y[NHits], z[NHits];
    for (int ih = 0; ih < NHits; ih++){
      L1StsHit &hit = vStsHits[ihit[ih]];
      u[ih] = vStsStrips[hit.f];
      v[ih] = vStsStripsB[hit.b];
      StripsToCoor(u[ih], v[ih], x[ih], y[ih], sta[ih]);
      z[ih] = vStsZPos[hit.iz];
    };

      // initialize parameters
    L1TrackPar T;
    
    const fvec vINF = .1;
    T.x  = x[0];
    T.y  = y[0];

    fvec dz01 = 1./(z[1]-z[0]);
    T.tx = (x[1]-x[0])*dz01;
    T.ty = (y[1]-y[0])*dz01;

    T.qp = qp0;
    T.z  = z[0];
    T.chi2 = 0.;
    T.NDF = 2.;
    T.C00 = sta[0].XYInfo.C00;
    T.C10 = sta[0].XYInfo.C10;
    T.C11 = sta[0].XYInfo.C11;

    T.C20 = T.C21 = 0;
    T.C30 = T.C31 = T.C32 = 0;
    T.C40 = T.C41 = T.C42 = T.C43 = 0;
    T.C22 = T.C33 = vINF;
    T.C44 = 1.;

      // find field along the track
    L1FieldValue B[3] _fvecalignment;
    L1FieldRegion fld _fvecalignment;

    fvec tx[3] = {
      (x[1]-x[0])/(z[1]-z[0]),
      (x[2]-x[0])/(z[2]-z[0]),
      (x[2]-x[1])/(z[2]-z[1])
    };
    fvec ty[3] = {
      (y[1]-y[0])/(z[1]-z[0]),
      (y[2]-y[0])/(z[2]-z[0]),
      (y[2]-y[1])/(z[2]-z[1])
    };
    for (int ih = 0; ih < NHits; ih++){
      fvec dz = (sta[ih].z - z[ih]);
      sta[ih].fieldSlice.GetFieldValue( x[ih] + tx[ih]*dz, y[ih] + ty[ih]*dz, B[ih] );
    };
    fld.Set( B[0], sta[0].z, B[1], sta[1].z, B[2], sta[2].z );

      // fit
    for( int ih = 1; ih < NHits; ih++){
      L1Extrapolate( T, z[ih], T.qp, fld );
      L1AddMaterial( T, sta[ih].materialInfo, T.qp );
      if (ista[ih] == NMvdStations - 1) L1AddPipeMaterial( T, T.qp );
      
      L1Filter( T, sta[ih].frontInfo, u[ih] );
      L1Filter( T, sta[ih].backInfo,  v[ih] );
    }

      // repeat several times in order to improve precision
    for (int iiter = 0; iiter < nIterations; iiter++){
        // fit backward
      // keep tx,ty,q/p
      int ih = NHits-1;
      T.x  = x[ih];
      T.y  = y[ih];
      T.z  = z[ih];
      T.chi2 = 0.;
      T.NDF = 2.;
      T.C00 = sta[ih].XYInfo.C00;
      T.C10 = sta[ih].XYInfo.C10;
      T.C11 = sta[ih].XYInfo.C11;

      T.C20 = T.C21 = 0;
      T.C30 = T.C31 = T.C32 = 0;
      T.C40 = T.C41 = T.C42 = T.C43 = 0;
      T.C22 = T.C33 = vINF;
      T.C44 = 1.;

//       L1Filter( T, sta[ih].frontInfo, u[ih] );
//       L1Filter( T, sta[ih].backInfo,  v[ih] );
      for( ih = NHits-2; ih >= 0; ih--){
        L1Extrapolate( T, z[ih], T.qp, fld );
        L1AddMaterial( T, sta[ih].materialInfo, T.qp );
        if (ista[ih] == NMvdStations) L1AddPipeMaterial( T, T.qp );
        
        L1Filter( T, sta[ih].frontInfo, u[ih] );
        L1Filter( T, sta[ih].backInfo,  v[ih] );
      }
        // fit forward
      ih = 0;
      T.x  = x[ih];
      T.y  = y[ih];
      T.z  = z[ih];
      T.chi2 = 0.;
      T.NDF = 2.;
      T.C00 = sta[ih].XYInfo.C00;
      T.C10 = sta[ih].XYInfo.C10;
      T.C11 = sta[ih].XYInfo.C11;

      T.C20 = T.C21 = 0;
      T.C30 = T.C31 = T.C32 = 0;
      T.C40 = T.C41 = T.C42 = T.C43 = 0;
      T.C22 = T.C33 = vINF;
      T.C44 = 1.;

//       L1Filter( T, sta[ih].frontInfo, u[ih] );
//       L1Filter( T, sta[ih].backInfo,  v[ih] );
      for( ih = 1; ih < NHits; ih++){
        L1Extrapolate( T, z[ih], T.qp, fld );
        L1AddMaterial( T, sta[ih].materialInfo, T.qp );
        if (ista[ih] == NMvdStations + 1) L1AddPipeMaterial( T, T.qp );
        
        L1Filter( T, sta[ih].frontInfo, u[ih] );
        L1Filter( T, sta[ih].backInfo,  v[ih] );
      }
    } // for iiter

    T3.SetOneEntry(i3_4, T,i3_4);
  }//i3
} // f32


          /// Select triplets. Save them into vTriplets.
inline void L1Algo::f4(  // input
                int n3, int istal, int istam, int istar,
                nsL1::vector<L1TrackPar>::TSimd &T_3,

                vector<THitI> &hitsl_3,  vector<THitI> &hitsm_3,  vector<THitI> &hitsr_3,
                // output
                unsigned int &nstaltriplets,
                std::vector<L1Triplet> &vTriplets_part,
                unsigned int *TripStartIndexH, unsigned int *TripStopIndexH
// #ifdef XXX
//                 ,unsigned int &stat_n_trip
// #endif
               )
{

  for( int i3=0; i3<n3; i3++){
    int i3_V = i3/fvecLen;
    int i3_4 = i3%fvecLen;
    L1TrackPar &T3 = T_3[i3_V];
    
      // select
    fscal chi2 = T3.chi2[i3_4];
    if ( !finite(chi2) || chi2 < 0 || chi2 > TRIPLET_CHI2_CUT * (T3.NDF[i3_4] - 5) )  continue; 

      // prepare data
    fscal qp = MaxInvMom + T3.qp[i3_4];
    if( qp < 0 )            qp = 0;          
    if( qp > MaxInvMom*2 )  qp = MaxInvMom*2;
    fscal Cqp = 5.*sqrt(fabs(T3.C44[i3_4]));  

    fscal scale = 255/(MaxInvMom*2);

    qp  = (static_cast<unsigned int>(qp*scale) )%256;
    Cqp = (static_cast<unsigned int>(Cqp*scale))%256;
    Cqp += 1;

    if( Cqp < 0  ) Cqp = 0;
    if( Cqp > 20 ) Cqp = 20;
    qp = static_cast<unsigned char>( qp );

    THitI ihitl = hitsl_3[i3] + StsHitsUnusedStartIndex[istal];
    THitI ihitm = hitsm_3[i3] + StsHitsUnusedStartIndex[istam];
    THitI ihitr = hitsr_3[i3] + StsHitsUnusedStartIndex[istar];
    L1_ASSERT( ihitl < StsHitsUnusedStopIndex[istal], ihitl << " < " << StsHitsUnusedStopIndex[istal] );
    L1_ASSERT( ihitm < StsHitsUnusedStopIndex[istam], ihitm << " < " << StsHitsUnusedStopIndex[istam] );
    L1_ASSERT( ihitr < StsHitsUnusedStopIndex[istar], ihitr << " < " << StsHitsUnusedStopIndex[istar] );
    
      // save
    L1Triplet trip;
    trip.Set( ihitl, ihitm, ihitr,
              istal, istam, istar,
              0, 0,
              0, static_cast<unsigned char>( qp ), chi2);   // 1.2/1000 sec
    
    trip.Cqp   = static_cast<unsigned char>( Cqp );

          //count statistics

    vTriplets_part.push_back(trip);  // 0.67/1000 sec

    if ( TripStopIndexH[ihitl] == 0 )
      TripStartIndexH[ihitl] = nstaltriplets;
    TripStopIndexH[ihitl] = ++nstaltriplets;  // < 0.001/100000

// #ifdef XXX
//          stat_n_trip++;
// #endif
  }//i3
} // f4()


          /// Find neighbours of triplets. Calculate level of triplets.
inline void L1Algo::f5(  // input
                 // output
               unsigned int *TripStartIndexH, unsigned int *TripStopIndexH,
               int *nlevel
               )
{
//     cout << "Start  Finding levels of triplets "  << endl;

  for  (int istal = NStations - 4; istal >= 0; istal--){
    for (int tripType = 0; tripType < 3; tripType++) { // tT = 0 - 123triplet, tT = 1 - 124triplet, tT = 2 - 134triplet
      if ( ( ((isec != kFastPrimJumpIter) && (isec != kAllPrimJumpIter) && (isec != kAllSecJumpIter)) && (tripType != 0)          ) ||
           ( ((isec == kFastPrimJumpIter) || (isec == kAllPrimJumpIter) || (isec == kAllSecJumpIter)) && (tripType != 0) && (istal == NStations - 4) )
        ) continue;

      int istam = istal + 1;
      int istar = istal + 2;
      switch (tripType){
        case 1:
          istar++;
          break;
        case 2:
          istam++;
          istar++;
          break;  
      }

      unsigned int offset_m = TripStartIndex[istam];

      for (int it = TripStartIndex[istal]; it < TripStopIndex[istal]; it++){
        L1Triplet *trip = &(vTriplets[it]);
        if ( istam != trip->GetMSta() ) continue;
        if ( istar != trip->GetRSta() ) continue;

        unsigned char level = 0;
        float  chi2 = trip->GetChi2();
        unsigned char  qp = trip->GetQp();

        THitI ihitl = trip->GetLHit();
        THitI ihitm = trip->GetMHit();
        THitI ihitr = trip->GetRHit();
        L1_ASSERT( ihitl < StsHitsUnusedStopIndex[istal], ihitl << " < " << StsHitsUnusedStopIndex[istal] );
        L1_ASSERT( ihitm < StsHitsUnusedStopIndex[istam], ihitm << " < " << StsHitsUnusedStopIndex[istam] );
        L1_ASSERT( ihitr < StsHitsUnusedStopIndex[istar], ihitr << " < " << StsHitsUnusedStopIndex[istar] );

        THitI first_neighbour = 0;
        THitI end_neighbour   = 0;

          // neighbours should have 2 common hits
        unsigned int first_triplet;
        unsigned int last_triplet;
        unsigned int iN = TripStartIndexH[ihitm];   // first posible neighbour
        unsigned int lastN = TripStopIndexH[ihitm]; // last posible neighbour

          //   // find first triplet with 2 common hits
          // for (; (iN < lastN); ++iN){
          //   unsigned int jhitm;
          //   jhitm = vTriplets[iN].GetMHit();
          //   if (jhitm == ihitr) break;
          // };
        
        if (iN < lastN) { // if exist at least one trip
          L1_ASSERT(iN >= offset_m, iN << " >= " << offset_m);
          
          first_triplet = iN;
        
            // // find last triplets with 2 common hits
            // for (; iN < lastN; ++iN){
            //   if (vTriplets[iN].GetLHit() != ihitm) break;
            //   unsigned int jhitm;
            //   jhitm = vTriplets[iN].GetMHit();
            //   if (jhitm != ihitr) break;
            // };
            // last_triplet = iN-1;
          last_triplet = lastN;

          vector<unsigned int> neighCands; // save neighbour candidates
          for (iN = first_triplet; iN <= last_triplet; ++iN){

            if (vTriplets[iN].GetMSta() != istar) continue; // neighbours should have 2 common hits
            if (vTriplets[iN].GetMHit() != ihitr) continue;
          
            L1Triplet *tripn = &vTriplets[iN];

            fscal qp2 = tripn->GetQp();
            fscal Cqp1 = trip->Cqp;
            fscal Cqp2 = tripn->Cqp;
            if ( fabs(qp - qp2) > PickNeighbour * (Cqp1 + Cqp2) )  continue; // neighbours should have same qp
          
              // calculate level
            unsigned char jlevel = tripn->GetLevel();
            if ( level <= jlevel ) level = jlevel + 1;
            if (level == jlevel + 1) neighCands.push_back(iN);
          }
          for (unsigned int in = 0; in < neighCands.size(); in++) {
            const int nLevel = vTriplets[neighCands[in]].GetLevel();
            if (level == nLevel + 1) trip->neighbours.push_back(neighCands[in] - offset_m);
          }

        }; // if exist at least one trip

          // save information
        trip->Set( ihitl, ihitm, ihitr,
        istal, istam, istar,
        first_neighbour, end_neighbour - first_neighbour,
        level, qp, chi2 );

        nlevel[level]++;

      }// vTriplets
    } // tripType
  } // istal
}

                                /// ------------------- doublets on station ----------------------

inline void L1Algo::DupletsStaPort(  // input
                      int istal, int istam,

                      vector<int> &n_g1, unsigned int *portionStopIndex,
                      L1Portion<L1TrackPar> &T_g1,
                      L1Portion<L1FieldRegion> &fld_g1,
                      L1Portion<THitI> &hitsl_g1,

                        // output
                      map<THitI,THitI> *Duplets_start, vector<THitI> *Duplets_hits,

                      vector<int> &n_g2,
                      L1Portion<THitI> &i1_g2,
                      L1Portion<THitI> &hitsm_g2

                      )
{
      L1Station &stal = vStations[istal];
      L1Station &stam = vStations[istam];

            // prepare data
      map<THitI, THitI>
      &mrDuplets_start = Duplets_start[istam],// mrDuplets_start - first right hit in mrDuplets_hits, array paralel to middle hits
      &lmDuplets_start = Duplets_start[istal];// lmDuplets_start -
      vector<THitI>
//       &mrDuplets_hits = Duplets_hits[istam], // mrDuplets_hits - right hits of middle-right doublets
      &lmDuplets_hits = Duplets_hits[istal]; // lmDuplets_hits - same for left-middle doublets

      L1HitPoint *vStsHits_l = &((*vStsHitPointsUnused)[0]) + StsHitsUnusedStartIndex[istal];
      L1HitPoint *vStsHits_m = &((*vStsHitPointsUnused)[0]) + StsHitsUnusedStartIndex[istam];

      int NHits_m = StsHitsUnusedStopIndex[istam] - StsHitsUnusedStartIndex[istam];



      unsigned int nDuplets_lm = 0;  // number of created doublets on this stantion
      int start_mhit = 0;   // hit on the middle stantion to start find new doublets

      for(unsigned int ip = portionStopIndex[istal+1]; ip < portionStopIndex[istal]; ip++ ){

        fvec u_front[Portion/fvecLen], u_back[Portion/fvecLen];
        fvec zPos[Portion/fvecLen];

          /// prepare the portion of left hits data

//         THitI *hitsl_1;
//         hitsl_1 = &(hitsl_g1[ip*Portion+0]);
        vector<THitI> &hitsl_1 = hitsl_g1[ip];
        
        int n1 = n_g1[ip];
        
        f10(  // input
            (ip - portionStopIndex[istal+1]) * Portion, n1, vStsHits_l,
              // output
            u_front, u_back, zPos,
            hitsl_1
          );

        for (unsigned int i = 0; i < hitsl_1.size(); i++)
          L1_ASSERT(hitsl_1[i] < StsHitsUnusedStopIndex[istal] - StsHitsUnusedStartIndex[istal],
            hitsl_1[i] << " < " << StsHitsUnusedStopIndex[istal] - StsHitsUnusedStartIndex[istal]);
        
        int n1_V = (n1+fvecLen-1)/fvecLen;

          /// Get the field approximation. Add the target to parameters estimation. Propagaete to middle station.

//         L1TrackPar *T_1;      // parameters estimation

//         L1FieldRegion *fld_1;// field approximation on track

//         T_1 = &(T_g1[ip*Portion/fvecLen+0]);
        nsL1::vector<L1TrackPar>::TSimd &T_1 = T_g1[ip];
//         fld_1 = &(fld_g1[ip*Portion/fvecLen+0]);
        nsL1::vector<L1FieldRegion>::TSimd &fld_1 = fld_g1[ip];

        fvec x_minusV_1[Portion/fvecLen];    // region on next station to add hit
        fvec x_plusV_1 [Portion/fvecLen];
        fvec y_minusV_1[Portion/fvecLen];
        fvec y_plusV_1 [Portion/fvecLen];

//         cout << "Run f11" << endl;

        f11(istal, istam,
            n1_V,

            u_front, u_back, zPos,
              // output
            T_1, fld_1,
            x_minusV_1, x_plusV_1, y_minusV_1, y_plusV_1
          );

          /// Find the doublets. Reformat data in the portion of doublets.

        fscal *x_minus;
        fscal *x_plus;
        fscal *y_minus;
        fscal *y_plus;
        x_minus  = reinterpret_cast<fscal*>(&(x_minusV_1[0]));
        x_plus   = reinterpret_cast<fscal*>(&(x_plusV_1 [0]));
        y_minus  = reinterpret_cast<fscal*>(&(y_minusV_1[0]));
        y_plus   = reinterpret_cast<fscal*>(&(y_plusV_1 [0]));

//         THitI *hitsm_2;
//         hitsm_2 = &(hitsm_g2[ip*MaxPortionDoublets+0]);
        vector<THitI> &hitsm_2 = hitsm_g2[ip];
#ifdef DOUB_PERFORMANCE
        vector<THitI> hitsl_2;
#endif // DOUB_PERFORMANCE

//         THitI *i1_2;
//         i1_2 = &(i1_g2[ip*MaxPortionDoublets+0]);
        vector<THitI> &i1_2 = i1_g2[ip];

         int n2;


        f20(  // input
            n1, stal, stam,
            vStsHits_l, vStsHits_m, NHits_m,
            y_minus, x_minus, y_plus, x_plus,
            T_1, fld_1,
            hitsl_1,
            mrDuplets_start,
              // output
            n2,
            i1_2,
            start_mhit,
#ifdef DOUB_PERFORMANCE
            hitsl_2,
#endif // DOUB_PERFORMANCE
            hitsm_2,
            lmDuplets_start, lmDuplets_hits, nDuplets_lm
           );

        for (unsigned int i = 0; i < hitsm_2.size(); i++)
          L1_assert(hitsm_2[i] < StsHitsUnusedStopIndex[istam] - StsHitsUnusedStartIndex[istam]);

#ifdef DOUB_PERFORMANCE
        THitI* RealIHitL = &(RealIHit[StsHitsUnusedStartIndex[istal]]);
        THitI* RealIHitM = &(RealIHit[StsHitsUnusedStartIndex[istam]]);
        for (int i = 0; i < n2; i++){
          // int i_4 = i%4;
          // int i_V = i/4;
          THitI iHits[2] = {
            RealIHitL[hitsl_2[i]],
            RealIHitM[hitsm_2[i]]
          };
          fL1Eff_doublets->AddOne(iHits);
        }
#endif // DOUB_PERFORMANCE

          // Add the middle hits to parameters estimation. Propagate to right station.

//          int n2_V = (n2+fvecLen-1)/fvecLen;

//         fvec *x_minusV_2;
//         fvec *x_plusV_2;
//         fvec *y_minusV_2;
//         fvec *y_plusV_2;
//         x_minusV_2 = (&(x_minusV_g2[ip*MaxPortionDoublets/fvecLen+0]));
//         x_plusV_2  = (&(x_plusV_g2 [ip*MaxPortionDoublets/fvecLen+0]));
//         y_minusV_2 = (&(y_minusV_g2[ip*MaxPortionDoublets/fvecLen+0]));
//         y_plusV_2  = (&(y_plusV_g2 [ip*MaxPortionDoublets/fvecLen+0]));

//         f21(  // input
//             n2_V,
//             stam, star,
//             u_front, u_back,
//             fld_2,
//             Pick_r,
//               // output
//             T_2,
//             x_minusV_2, x_plusV_2, y_minusV_2, y_plusV_2
//            );


//         if (n2 >= MaxPortionDoublets) cout << "isec: " << isec << " stantion: " << istal << " portion number: " << ip << " CATrackFinder: Warning: Too many doublets created in portion" << endl;

        n_g2[ip] = n2;

      } // portion of left hits


//     if (nDuplets_lm > MaxArrSize) cout << "isec: " << isec << " stantion: " << istal << " CATrackFinder: Error: Too many doublets on stantion was created" << endl;


}


                            /// ------------------- Triplets on station ----------------------

inline void L1Algo::TripletsStaPort(  // input
                            int istal, int istam, int istar,

                            vector<int> &n_g1,
                            L1Portion<L1TrackPar> &T_g1,
                            L1Portion<L1FieldRegion> &fld_g1,
                            L1Portion<THitI> &hitsl_g1,

                            vector<int> &n_g2, unsigned int *portionStopIndex,
                            L1Portion<THitI> &i1_g2,
                            L1Portion<THitI> &hitsm_g2,
                            
                            map<unsigned /*short*/ int,THitI> *Duplets1_start, vector<THitI>  *Duplets1_hits,
                            map<unsigned /*short*/ int,THitI> *Duplets2_start, vector<THitI>  *Duplets2_hits,
                            
                              // output
                            std::vector<L1Triplet> *vTriplets_part,
                            unsigned int *TripStartIndexH, unsigned int *TripStopIndexH
                            )
{

        // prepare data
      map<unsigned /*short*/ int, THitI>
      &mrDuplets_start = Duplets2_start[istam],// mrDuplets_start - first right hit in mrDuplets_hits, array paralel to middle hits
      &lmDuplets_start = Duplets1_start[istal];// lmDuplets_start -
      vector<THitI>
      &mrDuplets_hits = Duplets2_hits[istam], // mrDuplets_hits - right hits of middle-right doublets
      &lmDuplets_hits = Duplets1_hits[istal]; // lmDuplets_hits - same for left-middle doublets
      
      L1Station &stam = vStations[istam];
      L1Station &star = vStations[istar];

      L1HitPoint *vStsHits_m = &((*vStsHitPointsUnused)[0]) + StsHitsUnusedStartIndex[istam];

      L1HitPoint *vStsHits_r = 0;
      int NHits_r = 0;
      if( istar < NStations ){
        vStsHits_r = &((*vStsHitPointsUnused)[0]) + StsHitsUnusedStartIndex[istar];
        NHits_r = StsHitsUnusedStopIndex[istar] - StsHitsUnusedStartIndex[istar];
      }

      unsigned int nstaltriplets = 0; // find triplets begin from this stantion

      for(unsigned int ip = portionStopIndex[istal+1]; ip < portionStopIndex[istal]; ip++ ){


        int n3=0, n3_V;

//        L1TrackPar *T_1;
//        T_1 = &(T_g1[ip*Portion/fvecLen+0]);
        nsL1::vector<L1TrackPar>::TSimd &T_1 = T_g1[ip];
//        L1FieldRegion *fld_1;
//         fld_1 = &(fld_g1[ip*Portion/fvecLen+0]);
        nsL1::vector<L1FieldRegion>::TSimd &fld_1 = fld_g1[ip];
//        THitI *hitsl_1;
//        hitsl_1 = &(hitsl_g1[ip*Portion+0]);
        vector<THitI> &hitsl_1 = hitsl_g1[ip];

        int n1 = n_g1[ip];

//        THitI *hitsm_2;
//        hitsm_2 = &(hitsm_g2[ip*MaxPortionDoublets+0]);
        vector<THitI> &hitsm_2 = hitsm_g2[ip];

//        THitI *i1_2;
//        i1_2 = &(i1_g2[ip*MaxPortionDoublets+0]);
        vector<THitI> &i1_2 = i1_g2[ip];
        int n2 = n_g2[ip];


//        L1TrackPar T_3[MaxPortionTriplets/fvecLen];
        nsL1::vector<L1TrackPar>::TSimd T_3;

//        THitI hitsl_3[MaxPortionTriplets];
//        THitI hitsm_3[MaxPortionTriplets];
//        THitI hitsr_3[MaxPortionTriplets];
        vector<THitI> hitsl_3, hitsm_3, hitsr_3;


          /// Add the middle hits to parameters estimation. Propagate to right station.


//        fvec u_front3[MaxPortionTriplets/fvecLen], u_back3[MaxPortionTriplets/fvecLen];
        nsL1::vector<fvec>::TSimd u_front3, u_back3;

        T_3.reserve(MaxPortionTriplets/fvecLen);
        hitsl_3.reserve(MaxPortionTriplets);
        hitsm_3.reserve(MaxPortionTriplets);
        hitsr_3.reserve(MaxPortionTriplets);
        u_front3.reserve(MaxPortionTriplets/fvecLen);
        u_back3.reserve(MaxPortionTriplets/fvecLen);

          /// Find the triplets(right hit). Reformat data in the portion of triplets.

        f30(  // input
            vStsHits_r, NHits_r, stam, star,

            istam, istar, n1,
            vStsHits_m,
            T_1, fld_1,
            hitsl_1,
            lmDuplets_start, lmDuplets_hits,

            n2,
            hitsm_2,
            i1_2,

            mrDuplets_start, mrDuplets_hits,
//            MaxPortionTriplets,
              // output
            n3,
            T_3,
            hitsl_3, hitsm_3, hitsr_3,
            u_front3, u_back3
           );
        n3_V = (n3+fvecLen-1)/fvecLen;

        for (unsigned int i = 0; i < hitsl_3.size(); i++)
          L1_assert(hitsl_3[i] < StsHitsUnusedStopIndex[istal] - StsHitsUnusedStartIndex[istal]);
        for (unsigned int i = 0; i < hitsm_3.size(); i++)
          L1_assert(hitsm_3[i] < StsHitsUnusedStopIndex[istam] - StsHitsUnusedStartIndex[istam]);
        for (unsigned int i = 0; i < hitsr_3.size(); i++)
          L1_assert(hitsr_3[i] < StsHitsUnusedStopIndex[istar] - StsHitsUnusedStartIndex[istar]);

//        if (n3 >= MaxPortionTriplets) cout << "isec: " << isec << " stantion: " << istal << " portion number: " << ip << " CATrackFinder: Warning: Too many Triplets created in portion" << endl;




          /// Add the right hits to parameters estimation.
        f31(  // input
            n3_V,
            star,
            u_front3, u_back3,
              // output
            T_3
           );

                  /// refit
//         f32(  n3, istal, _RealIHit,          T_3,         hitsl_3, hitsm_3, hitsr_3, 0 );


#ifdef TRIP_PERFORMANCE
        THitI* RealIHitL = &(RealIHit[StsHitsUnusedStartIndex[istal]]);
        THitI* RealIHitM = &(RealIHit[StsHitsUnusedStartIndex[istam]]);
        THitI* RealIHitR = &(RealIHit[StsHitsUnusedStartIndex[istar]]);
        for (int i = 0; i < n3; i++){
          int i_4 = i%4;
          int i_V = i/4;
          THitI iHits[3] = {
            RealIHitL[hitsl_3[i]],
            RealIHitM[hitsm_3[i]],
            RealIHitR[hitsr_3[i]]
          };
#ifdef PULLS
          if ( fL1Eff_triplets->AddOne(iHits) )
            fL1Pulls->AddOne(T_3[i_V], i_4, iHits[2]);
#else
          fL1Eff_triplets->AddOne(iHits);
#endif
          if (T_3[i_V].chi2[i_4] < TRIPLET_CHI2_CUT*T_3[i_V].NDF[i_4])
            fL1Eff_triplets2->AddOne(iHits);
        }
#endif // TRIP_PERFORMANCE

          /// Fill Triplets.

        f4( // input
          n3, istal, istam, istar,
          T_3,
          hitsl_3, hitsm_3, hitsr_3,
            // output
          nstaltriplets,
          vTriplets_part[istal],
          TripStartIndexH, TripStopIndexH
          );

      } // start_lh - portion of left hits

}

    ///**********************************************************************************************************************
    ///**********************************************************************************************************************
    ///**********************************************************************************************************************
    ///*                                                                                                                    *
    ///*                                                                                                                    *
    ///*                                                                                                                    *
    ///*                                          CATrackFinder procedure                                                   *
    ///*                                                                                                                    *
    ///*                                                                                                                    *
    ///*                                                                                                                    *
    ///**********************************************************************************************************************
    ///**********************************************************************************************************************
    ///**********************************************************************************************************************




void L1Algo::CATrackFinder()
{

//   cout << "Start TrackFinder" << endl;



//   cout<<" total STS hits "<<vStsHits.size()<<endl;
    //cout<<" total STS hits, strips, back strips "
    //<<vStsHits.size()<<" "<<vStsStrips.size()<<" "<<vStsStripsB.size()<<endl;

#ifdef PULLS
  static L1AlgoPulls *l1Pulls_ = new L1AlgoPulls();
  fL1Pulls = l1Pulls_;
  fL1Pulls->Init();
#endif
#ifdef TRIP_PERFORMANCE
  static L1AlgoEfficiencyPerformance<3> *l1Eff_triplets_ = new L1AlgoEfficiencyPerformance<3>();
  fL1Eff_triplets = l1Eff_triplets_;
  fL1Eff_triplets->Init();
  static L1AlgoEfficiencyPerformance<3> *l1Eff_triplets2_ = new L1AlgoEfficiencyPerformance<3>();
  fL1Eff_triplets2 = l1Eff_triplets2_;
  fL1Eff_triplets2->Init();
#endif
#ifdef DOUB_PERFORMANCE
  static L1AlgoEfficiencyPerformance<2> *l1Eff_doublets_ = new L1AlgoEfficiencyPerformance<2>();
  fL1Eff_doublets = l1Eff_doublets_;
  fL1Eff_doublets->Init();
#endif
  
#ifdef DRAW
  static L1AlgoDraw draw;
  draw.InitL1Draw(this);
#endif

  cout.precision(6);
        
#ifdef XXX
  static unsigned int stat_N = 0; // number of events

  const int Nisec = fNFindIterations; //number of main loops

    // timers for different part of CATrackFinder
  const int ntimers = 6; //number of timers + 1

  static double
    stat_time = 0.0, stat_time_real = 0.0, stat_time_trip[Nisec], stat_time_fit[Nisec],  stat_time_fit_bra[Nisec], stat_time_fit_fin[Nisec];

  if (stat_N == 0)
    for(int i1 = 0; i1 < Nisec; i1++){
      stat_time_trip[i1] = .0;
      stat_time_fit[i1] = .0;
      stat_time_fit_bra[i1] = .0;
      stat_time_fit_fin[i1] = .0;
    }

  double
  time_trip[Nisec], time_fit[Nisec], time_fit_bra[Nisec], time_fit_fin[Nisec];

  for(int i1 = 0; i1 < Nisec; i1++){
    time_trip[i1] = .0;
    time_fit[i1] = .0;
    time_fit_bra[i1] = .0;
    time_fit_fin[i1] = .0;
  }

  static double stat_time_find[ntimers][2][Nisec];
  if (stat_N == 0)
    for(int i1 = 0; i1 < Nisec; i1++)
      for(int i2 = 0; i2 < ntimers; i2++)
        for(int i3 = 0; i3 < 2; i3++)
          stat_time_find[i2][i3][i1] = 0.;

  double stat_time_find_all[ntimers][2];
  for(int i2 = 0; i2 < ntimers; i2++)
    for(int i3 = 0; i3 < 2; i3++)
      stat_time_find_all[i2][i3] = 0.;


  double time_find[ntimers][2][Nisec];
  for(int i1 = 0; i1 < Nisec; i1++)
    for(int i2 = 0; i2 < ntimers; i2++)
      for(int i3 = 0; i3 < 2; i3++)
        time_find[i2][i3][i1] = 0.;

  TStopwatch c_time_find[ntimers][2];


#endif


  TStopwatch c_time;  // all CATrackFinder work time
  c_time.Start();

#ifdef XXX
    c_time_find[0][0].Start();
#endif

  vTracks.clear();
  vRecoHits.clear();

    // set all hits as usable
  for( vector<unsigned char>::iterator is= vSFlag.begin(); is!=vSFlag.end(); ++is)
    SetFUnUsed(*is);
  for( vector<unsigned char>::iterator is= vSFlagB.begin(); is!=vSFlagB.end(); ++is)
    SetFUnUsed(*is);


#ifdef XXX
  unsigned int
    stat_max_n_trip = 0,
    stat_max_trip_size = 0,
    stat_max_n_dup = 0,
    stat_max_BranchSize = 0,
    stat_max_n_branches = 0;
#endif

// #ifdef DRAW
// //   draw.DrawInfo();
//     draw.ClearVeiw();
//     draw.DrawInputHits();
//     draw.DrawMCTracks();
//     draw.SaveCanvas("MC_");
//     draw.DrawAsk();
// #endif

  vector< L1StsHit > vStsDontUsedHits_A;  // arrays for copy from vStsHits only hits, which aren't used in created tracks
  vector< L1StsHit > vStsDontUsedHits_B;

  vector< L1HitPoint > vStsDontUsedHitsxy_A;  // arrays for contain prepared information
  vector< L1HitPoint > vStsDontUsedHitsxy_B;

  vStsHitsUnused = &vStsDontUsedHits_B;      // array of hits uses on current iteration
  std::vector< L1StsHit > *vStsHitsUnused_buf = &vStsDontUsedHits_A; // buffer for copy

  vStsHitPointsUnused = &vStsDontUsedHitsxy_B;// array of info for hits uses on current iteration
  std::vector< L1HitPoint > *vStsHitPointsUnused_buf = &vStsDontUsedHitsxy_A;

  vStsHitsUnused_buf->clear();
  vStsHitPointsUnused_buf->clear();
  vector<THitI> RealIHit_v; // index of hit in vStsHits indexed by index of hit in vStsHitsUnused;
  RealIHit_v.reserve(vStsHits.size());
  RealIHit = &(RealIHit_v[0]);


    // fill the arrays for the first iteration
  for(int ista = 0; ista < NStations; ista++){
    for(THitI ih = StsHitsStartIndex[ista]; ih < StsHitsStopIndex[ista]; ih++){
      L1StsHit &hit = vStsHits[ih];

      RealIHit[ih] = ih;
      vStsDontUsedHits_B  .push_back( hit );
      vStsDontUsedHitsxy_B.push_back( CreateHitPoint(hit,ista) );
    }
    StsHitsUnusedStartIndex[ista] = StsHitsStartIndex[ista];
    StsHitsUnusedStopIndex[ista]  = StsHitsStopIndex[ista];
  }
  
#ifndef L1_NO_ASSERT
    for (int istal = NStations - 1; istal >= 0; istal--){
      L1_ASSERT( StsHitsStopIndex[istal] <= (vStsHits).size()             , StsHitsStopIndex[istal] << " <= " << (vStsHits).size());
      L1_ASSERT( StsHitsUnusedStopIndex[istal] <= (*vStsHitsUnused).size(), StsHitsUnusedStopIndex[istal] << " <= " << (*vStsHitsUnused).size());
    }
#endif // L1_NO_ASSERT

  
#ifdef XXX
  c_time_find[0][0].Stop();
  time_find[0][0][0] += double(c_time_find[0][0].CpuTime()); //ms
  time_find[0][1][0] += double(c_time_find[0][0].RealTime()); //ms
#endif

  
    // iterations of finding:
    // kFastPrimIter = 0, // primary fast track
    // kFastPrimJumpIter, // primary fast tracks with gaps. can be dissabled by macro
    // kAllPrimIter,      // primary all track
    // kAllPrimJumpIter,  // primary tracks with gaps. can be dissabled by macro
    // kAllSecIter        // secondary all track 
  for (isec = 0; isec < fNFindIterations; isec++){ // all finder
#ifdef XXX
//     cout << " Begin of iteration " << isec << endl;
    TStopwatch c_time_trip;  // time for find all triplets
#endif
    
    Pick_m = 2.0; // coefficient for size of region on middle station for add middle hits in triplets: Dx = Pick*sigma_x Dy = Pick*sigma_y
    Pick_r = 4.0; // coefficient for size of region on right  station for add right  hits in triplets
    Pick_gather = 3.0; // coefficient for size of region for attach new hits to the created track
    if ( (isec == kAllSecIter) || (isec == kAllSecJumpIter) ){ // it's hard to estimate errors correctly for slow tracks & w\o target!
      Pick_m =  3.0;
    }
    if ( (isec == kAllPrimIter) || (isec == kAllPrimJumpIter) || (isec == kAllSecIter) || (isec == kAllSecJumpIter) )
      Pick_gather = 4.0;

    PickNeighbour = 1.0; // (PickNeighbour < dp/dp_error)  =>  triplets are neighbours

    MaxInvMom = 1.0/0.5;                     // max considered q/p
    if ( (isec == kAllPrimIter) || (isec == kAllPrimJumpIter) || (isec == kAllSecIter) || (isec == kAllSecJumpIter) ) MaxInvMom =  1.0/0.1;
      // define the target
    targX = 0; targY = 0; targZ = 0;      //  suppose, what target will be at (0,0,0)
    
    float SigmaTargetX = 0, SigmaTargetY = 0; // target constraint [cm]
    if ( (isec == kFastPrimIter) || (isec == kFastPrimJumpIter) || (isec == kAllPrimIter) || (isec == kAllPrimJumpIter) ){ // target
      targB = vtxFieldValue;
      if (isec ==-1)
        SigmaTargetX = SigmaTargetY = 0.01; // target
      else
        SigmaTargetX = SigmaTargetY = 0.1;
    }
    if ( (isec == kAllSecIter) || (isec == kAllSecJumpIter) ) { //use outer radius of the 1st station as a constraint
      L1Station &st = vStations[0];
      SigmaTargetX = SigmaTargetY = 3;//st.Rmax[0];
      targZ = 0.;//-1.;
      st.fieldSlice.GetFieldValue( 0, 0, targB );
    }

    TargetXYInfo.C00 = SigmaTargetX * SigmaTargetX;
    TargetXYInfo.C10 = 0;
    TargetXYInfo.C11 = SigmaTargetY * SigmaTargetY;


    if (NStations > MaxNStations) cout << " CATrackFinder: Error: Too many Stantions" << endl;

    vector<THitI> Duplets_hits[MaxNStations];       // right hits of doublets(left-right)
    map<THitI, THitI> Duplets_start[MaxNStations];  // index of first right hits of doublets in Duplets_hits array   indexed by left hit
    vector<THitI> DupletsG_hits[MaxNStations];      // gaped duplets. istam = istal + 2;
    map<THitI, THitI> DupletsG_start[MaxNStations];
    
      // set 0 for all doublets begin from last station - they don't exist.
    for (int i = 0; i < MaxNStations; i++){
      Duplets_hits[i].reserve(MaxArrSize);
//       Duplets_start[i].reserve(MaxArrSize);
      DupletsG_hits[i].reserve(MaxArrSize);
    }

      // global arrays for keep data of doublets on all stations
//     static int n_g1[MaxNPortion];  // number of singlets in same portion
    vector<int> n_g1;
//     static L1TrackPar T_g1[MaxNPortion*Portion/fvecLen];  // estimation of parameters
    L1Portion<L1TrackPar> T_g1(MaxNPortion, Portion/fvecLen);
    L1Portion<L1TrackPar> TG_g1(MaxNPortion, Portion/fvecLen); // for doublets with gaps

//     static L1FieldRegion fld_g1[MaxNPortion*Portion/fvecLen];  // magnetic field
    L1Portion<L1FieldRegion> fld_g1(MaxNPortion, Portion/fvecLen);
    L1Portion<L1FieldRegion> fldG_g1(MaxNPortion, Portion/fvecLen);
//     static THitI hitsl_g1[MaxNPortion*Portion];  // left and middle hits indexed by number of doublets in portion(i2)
    L1Portion<THitI> hitsl_g1(MaxNPortion, Portion);
    L1Portion<THitI> hitslG_g1(MaxNPortion, Portion);

       // global arrays for get data of triplets on all stations
//     static int n_g2[MaxNPortion];  // number of doublets in same portion
    vector<int> n_g2;
    vector<int> nG_g2;

//     static THitI i1_g2[MaxNPortion*MaxPortionDoublets];   // index in portion of singlets(i1) indexed by index in portion of doublets(i2)
    L1Portion<THitI> i1_g2(MaxNPortion, MaxPortionDoublets);
    L1Portion<THitI> i1G_g2(MaxNPortion, MaxPortionDoublets);

//     static THitI hitsm_g2[MaxNPortion*MaxPortionDoublets];// left and middle hits indexed by number of doublets in portion(i2)
    L1Portion<THitI> hitsm_g2(MaxNPortion, MaxPortionDoublets);
    L1Portion<THitI> hitsmG_g2(MaxNPortion, MaxPortionDoublets);

    n_g1.reserve(MaxNPortion);
//     T_g1.reserve();
//     fld_g1.reserve();
//     hitsl_g1.reserve(MaxNPortion*Portion);
    n_g2.reserve(MaxNPortion);
    nG_g2.reserve(MaxNPortion);
//     i1_g2.reserve(MaxNPortion*MaxPortionDoublets);
//     hitsm_g2.reserve(MaxNPortion*MaxPortionDoublets);





    vector <L1Triplet> vTriplets_part[MaxNStations];  // temporary arrays for parallelizing save triplets
    vector <L1Triplet> vTripletsG124_part[MaxNStations]; // gaped triplets. type 124
    vector <L1Triplet> vTripletsG134_part[MaxNStations]; // gaped triplets. type 134


//     static unsigned int TripStartIndexH[MaxArrSize], TripStopIndexH[MaxArrSize]; // index region for triplets, which begins from hit, indexed by index of lhit.
    vector<unsigned int> TripStartIndexH_v, TripStopIndexH_v;
    vector<unsigned int> TripStartIndexHG124_v, TripStopIndexHG124_v;
    vector<unsigned int> TripStartIndexHG134_v, TripStopIndexHG134_v;
            
    TripStartIndexH_v.resize(vStsHitsUnused->size());
    TripStopIndexH_v .resize(vStsHitsUnused->size());
    TripStartIndexHG124_v.resize(vStsHitsUnused->size());
    TripStopIndexHG124_v .resize(vStsHitsUnused->size());
    TripStartIndexHG134_v.resize(vStsHitsUnused->size());
    TripStopIndexHG134_v .resize(vStsHitsUnused->size());
    unsigned int *TripStartIndexH = &(TripStartIndexH_v[0]), *TripStopIndexH = &(TripStopIndexH_v[0]);
    unsigned int *TripStartIndexHG124 = &(TripStartIndexHG124_v[0]), *TripStopIndexHG124 = &(TripStopIndexHG124_v[0]);
    unsigned int *TripStartIndexHG134 = &(TripStartIndexHG134_v[0]), *TripStopIndexHG134 = &(TripStopIndexHG134_v[0]);
        
    for(unsigned int i = 0; i < vStsHitsUnused->size(); i++){
      TripStartIndexH[i] = 0; // actually don't need this one, will be initialized at f4(..)
      TripStopIndexH[i] = 0;
      TripStartIndexHG124[i] = 0;
      TripStopIndexHG124[i] = 0;
      TripStartIndexHG134[i] = 0;
      TripStopIndexHG134[i] = 0;
    }

    unsigned int portionStopIndex[MaxNStations];  //number of portion on each station

#ifndef L1_NO_ASSERT
    for (int istal = NStations - 1; istal >= 0; istal--){
      L1_ASSERT( StsHitsUnusedStopIndex[istal] >= StsHitsUnusedStartIndex[istal], StsHitsUnusedStopIndex[istal] << " >= " << StsHitsUnusedStartIndex[istal]);
      L1_ASSERT( StsHitsUnusedStopIndex[istal] <= (*vStsHitsUnused).size(), StsHitsUnusedStopIndex[istal] << " <= " << (*vStsHitsUnused).size());
    }
#endif // L1_NO_ASSERT
    unsigned int nPortions = 0; // number of portions
    {    // split left hits on portions
      portionStopIndex[NStations-1] = 0;
      unsigned int ip = 0;  //index of curent portion

      for (int istal = NStations-2; istal >= FIRSTCASTATION; istal--){  //start downstream chambers
        int NHits_l = StsHitsUnusedStopIndex[istal] - StsHitsUnusedStartIndex[istal];

        int NHits_l_P = NHits_l/Portion;

        for( int ipp = 0; ipp < NHits_l_P; ipp++ ){
//           n_g1[ip++] = Portion;
          n_g1.push_back(Portion);
          ip++;
        } // start_lh - portion of left hits

//         n_g1[ip++] = NHits_l - NHits_l_P*Portion;
        n_g1.push_back(NHits_l - NHits_l_P*Portion);
        ip++;
        portionStopIndex[istal] = ip;
      }// lstations
      nPortions = ip;
    }

      ///   CREATE doublets

#ifdef XXX
    cout << "CREATE doublets" << endl;
#endif


#ifdef XXX
    c_time_find[1][0].Start();
#endif

#ifdef TBB
    {
      task_scheduler_init init(nthreads);
      static affinity_partitioner ap;
      ParalleledDup af(  // input
                      vStations, NStations,
                      StsHitsUnusedStartIndex, StsHitsUnusedStopIndex,
                      vSFlag, vSFlagB,
                      Pick_r,
                      Pick_m, MaxInvMom,
                      targX, targY, targZ, targB, TargetXYInfo,

                      n_g1, portionStopIndex,
                      T_g1,
                      fld_g1,
                      hitsl_g1,

                        // output
                      Duplets_start, Duplets_hits,

                      n_g2,
                      i1_g2,
                      hitsm_g2
                      );
      parallel_for(blocked_range<size_t>(FIRSTCASTATION,NStations-2+1,
                                         nblocks
                                        ), af,ap);
    }
#else // TBB
    
    T_g1.resize(nPortions);
    fld_g1.resize(nPortions);
    hitsl_g1.resize(nPortions);
    n_g2.resize(nPortions);
    i1_g2.resize(nPortions);
    hitsm_g2.resize(nPortions);
    for (int istal = NStations-2; istal >= FIRSTCASTATION; istal--){//  //start downstream chambers
      DupletsStaPort(  // input
                      istal, istal + 1,

                      n_g1, portionStopIndex,
                      
                                              // output
                      T_g1,
                      fld_g1,
                      hitsl_g1,

                      Duplets_start, Duplets_hits,

                      n_g2,
                      i1_g2,
                      hitsm_g2
                      );
    }// lstations

    if ( (isec == kFastPrimJumpIter) || (isec == kAllPrimJumpIter) || (isec == kAllSecJumpIter) ) {
      TG_g1.resize(nPortions);
      fldG_g1.resize(nPortions);
      hitslG_g1.resize(nPortions);
      nG_g2.resize(nPortions);
      i1G_g2.resize(nPortions);
      hitsmG_g2.resize(nPortions);
      for (int istal = NStations-3; istal >= FIRSTCASTATION; istal--){//  //start downstream chambers
        DupletsStaPort(  // input
                      istal, istal + 2,

                      n_g1, portionStopIndex,
                        // output
                      TG_g1,
                      fldG_g1,
                      hitslG_g1,
                      
                      DupletsG_start, DupletsG_hits,

                      nG_g2,
                      i1G_g2,
                      hitsmG_g2
                      );
      }// lstations
    }
#endif // TBB

#ifdef XXX
        c_time_find[1][0].Stop();
        time_find[1][0][isec] += double(c_time_find[1][0].CpuTime()); //ms
        time_find[1][1][isec] += double(c_time_find[1][0].RealTime()); //ms
#endif

#ifdef XXX
    int istat_n_triplet_c=0;
    for(unsigned int ip = 0; ip < portionStopIndex[FIRSTCASTATION]; ip++)
      istat_n_triplet_c += n_g2[ip];
#endif

// #ifdef DRAW
//     draw.ClearVeiw();
// //     draw.DrawInputHits();
//     draw.DrawRestHits(StsHitsUnusedStartIndex, StsHitsUnusedStopIndex, RealIHit);
//     draw.DrawDoublets(Duplets_hits, Duplets_start, MaxArrSize, StsHitsUnusedStartIndex, RealIHit);
// //     cout << "isec " << isec << " ";
//     draw.DrawAsk();
// //     for (int ista = 0; ista < NStations; ista++){
// //       draw.DrawDoubletsOnSta(ista, Duplets_hits, Duplets_start, MaxArrSize, StsHitsUnusedStartIndex, RealIHit);
// //       cout << "isec=" << isec << " " << "ista=" << ista << " ";
// //       draw.DrawAsk();
// //     }
//     TString sD = "Doubl_";
//     sD += isec;
//     sD += "_";
//     draw.SaveCanvas(sD);
// #endif

      ///   CREATE triplets

#ifdef XXX
    cout << " CREATE TRIPLETS " << endl;
#endif



#ifdef XXX
    c_time_find[1][0].Start();
#endif

#ifdef TBB
    {
      task_scheduler_init init(nthreads);
      static affinity_partitioner ap;
      ParalleledTrip af(  // input
                      vStations, NStations,
                      StsHitsUnusedStartIndex, StsHitsUnusedStopIndex,

                      n_g1,
                      T_g1,
                      fld_g1,
                      hitsl_g1,
                      n_g2, portionStopIndex,
                      i1_g2,
                      hitsm_g2,
                        // output
                      Duplets_start, Duplets_hits,
                      vTriplets_part,
                      TripStartIndexH, TripStopIndexH
                      );
      parallel_for(blocked_range<size_t>(FIRSTCASTATION,NStations-2+1,
                                        nblocks
                                        ), af,ap);
    }
#else // TBB
    for (int istal = NStations-2; istal >= FIRSTCASTATION; istal--){//  //start downstream chambers
      TripletsStaPort(  // input
        istal, istal + 1,  istal + 2,

        n_g1,
        T_g1,
        fld_g1,
        hitsl_g1,

        n_g2, portionStopIndex,
        i1_g2,
        hitsm_g2,

        Duplets_start, Duplets_hits,
        Duplets_start, Duplets_hits,
          // output
        vTriplets_part,
        TripStartIndexH, TripStopIndexH
        );
    }// l-stations
    if ( (isec == kFastPrimJumpIter) || (isec == kAllPrimJumpIter) || (isec == kAllSecJumpIter) ) {
      for (int istal = NStations-3; istal >= FIRSTCASTATION; istal--){//  //start downstream chambers
        TripletsStaPort(  // input
          istal, istal + 1,  istal + 3,

          n_g1,
          T_g1,
          fld_g1,
          hitsl_g1,

          n_g2, portionStopIndex,
          i1_g2,
          hitsm_g2,

          Duplets_start, Duplets_hits,
          DupletsG_start, DupletsG_hits,
            // output
          vTripletsG124_part,
          TripStartIndexHG124, TripStopIndexHG124
          );
      }// l-stations
      for (int istal = NStations-3; istal >= FIRSTCASTATION; istal--){//  //start downstream chambers
        TripletsStaPort(  // input
          istal, istal + 2,  istal + 3,

          n_g1,
          TG_g1,
          fldG_g1,
          hitslG_g1,

          nG_g2, portionStopIndex,
          i1G_g2,
          hitsmG_g2,

          DupletsG_start, DupletsG_hits,
          Duplets_start, Duplets_hits,
            // output
          vTripletsG134_part,
          TripStartIndexHG134, TripStopIndexHG134
          );
      }// l-stations
    }
#endif // TBB
#ifdef XXX
        c_time_find[1][0].Stop();
        time_find[2][0][isec] += double(c_time_find[1][0].CpuTime()); //ms
        time_find[2][1][isec] += double(c_time_find[1][0].RealTime()); //ms
#endif

// #ifdef XXX
//     cout<<"isec: " << isec <<  "  n hits, dup, tr_c, trip: "
//         <<istat_nhits<<" "<<istat_nduplets<<" "<<istat_n_triplet_c<<" "<<istat_n_triplets<<endl;
// #endif

          /// save triplets in vTriplets
#ifdef XXX
    cout << " copy triplets in vTriplets " << endl;
#endif
#ifdef XXX
    c_time_find[2][0].Start();
#endif

    vTriplets.clear();

    for (int istal = NStations - 2; istal >= FIRSTCASTATION; istal--){
      TripStartIndex[istal] = vTriplets.size();

      for (unsigned int i = 0; i < vTriplets_part[istal].size(); i++){
        vTriplets.push_back(vTriplets_part[istal][i]);
      }
      for (unsigned int i = 0; i < vTripletsG124_part[istal].size(); i++){ // it's initialized, so for all iterations it's ok
        vTriplets.push_back(vTripletsG124_part[istal][i]);
      }
      for (unsigned int i = 0; i < vTripletsG134_part[istal].size(); i++){
        vTriplets.push_back(vTripletsG134_part[istal][i]);
      }

      for (THitI i = StsHitsUnusedStartIndex[istal]; i < StsHitsUnusedStopIndex[istal]; i++){
        TripStartIndexH[i] += TripStartIndex[istal];
        TripStopIndexH[i]  += TripStartIndex[istal]
          + TripStopIndexHG124[i] - TripStartIndexHG124[i]
          + TripStopIndexHG134[i] - TripStartIndexHG134[i];
      }

      TripStopIndex[istal] = vTriplets.size();
    }

// #ifdef DRAW
//     {
//       draw.ClearVeiw();
//   //   draw.DrawInfo();
//       draw.DrawRestHits(StsHitsUnusedStartIndex, StsHitsUnusedStopIndex, RealIHit);
//       draw.DrawTriplets(vTriplets,RealIHit);
//       TString name = "Trip_";
//       name += isec;
//       name += "_";
//       draw.SaveCanvas(name);
//       draw.DrawAsk();
//     }
// #endif

#ifdef XXX
    { // calculate occupied memory size

//       int sizeD, sizeN, sizeT, sizeF, sizeH, sizeTrip;
//       sizeD = sizeof(THitI)*MaxArrSize*MaxNStations*2; // Duplets_*,
//       sizeN = sizeof(int)*MaxNPortion*2; // n_g*
//       sizeT = sizeof(L1TrackPar)*MaxNPortion*Portion/fvecLen; // T_g1
//       sizeF = sizeof(L1FieldRegion)*MaxNPortion*Portion/fvecLen; // fld_g1
//       sizeH = sizeof(THitI)*MaxNPortion*(Portion + MaxPortionDoublets*2); // hitsl_g1, hitsm_g2,i1_g2
//       sizeTrip = sizeof(L1Triplet)*vTriplets.size();

      int sizeD2 = 0, sizeN2, sizeT2, sizeF2, sizeH2, sizeTrip2;
      int ndoublets = 0;
      for(int i = 0; i < NStations; i++){
        sizeD2 += Duplets_hits[i].size();
        sizeD2 += Duplets_start[i].size();
        ndoublets += Duplets_hits[i].size();
        sizeD2*=sizeof(THitI);
      }

      cout << "number of doublets: " << ndoublets << endl;
      cout << "number of triplets: " << vTriplets.size() << endl;
/*
      sizeN2 = ( n_g1.size() + n_g2.size() )*sizeof(int);
      sizeT2 = T_g1.CalcSize();
      sizeF2 = fld_g1.CalcSize();
      sizeH2 = ( hitsl_g1.CalcSize() + hitsm_g2.CalcSize() + i1_g2.CalcSize());

      cout << sizeD/1000 << " D " << sizeD2/1000 << " [1000elem]" << endl;
      cout << sizeN/1000 << " N " << sizeN2/1000 << " [1000elem]" << endl;
      cout << sizeT/1000 << " T " << sizeT2/1000 << " [1000elem]" << endl;
      cout << sizeF/1000 << " F " << sizeF2/1000 << " [1000elem]" << endl;
      cout << sizeH/1000 << " H " << sizeH2/1000 << " [1000elem]" << endl;

      long int size = sizeD2 + sizeN2 + sizeT2+ sizeF2 + sizeH2 + sizeTrip;
      cout << "Size: " << size/1000 << " KB" << endl;*/
    }
#endif

#ifdef XXX
    c_time_find[2][0].Stop();
    time_find[2][0][isec] += double(c_time_find[2][0].CpuTime()); //ms
#endif

          /// Find neighbours of triplets.
#ifdef XXX
    cout << " Find neighbours of triplets. " << endl;
#endif
#ifdef XXX
    c_time_find[4][0].Start();
#endif

    int nlevels[NStations];  // number of triplets with some number of neighbours.
    for (int il = 0; il < NStations; ++il) nlevels[il] = 0;

     f5(   // input
          // output
        TripStartIndexH, TripStopIndexH,
        nlevels
      );

#ifdef XXX
    c_time_find[4][0].Stop();
    time_find[4][0][isec] += double(c_time_find[4][0].CpuTime()); //ms
#endif

#ifdef XXX
    c_time_trip.Stop();
    time_trip[isec] += double(c_time_trip.CpuTime()); //ms
#endif

    ///====================================================================
    ///=                                                                  =
    ///=        Collect track candidates. CREATE TRACKS                   =
    ///=                                                                  =
    ///====================================================================

#ifdef XXX
    cout<<"---- Collect track candidates. ----"<<endl;
#endif
#ifdef XXX
    TStopwatch c_time_fit;
#endif
    int min_level = 0; // min level for start triplet. So min track length = min_level+3.
//    if (isec == kFastPrimJumpIter) min_level = 1;
    if ( (isec == kAllSecIter) || (isec == kAllSecJumpIter) ) min_level = 2; // only the long low momentum tracks
//    if (isec == -1) min_level = NStations-3 - 3; //only the longest tracks

      // collect consequtive: the longest tracks, shorter, more shorter and so on
    for (int ilev = NStations-3; ilev >= min_level; ilev--){ // choose length
#ifdef XXX
      TStopwatch c_time_fit_bra;
#endif
      vector<L1Branch> vtrackcandidate; vtrackcandidate.clear();

        //  how many levels to check
      int nlevel = (NStations-2)-ilev+1;
      int ntracks = 0;

      for( int istaF = 0; istaF <= NStations-3-ilev; istaF++ ){ // choose first track-station

        // if( ( (isec == kAllSecIter) || (isec == kAllSecJumpIter) ) &&
        //     ( istaF >= NStations-6) ) break; // ghost supression !!!  too strong for Lambda. According to I.Vassiliev NStations-4 is good

        int trip_first = TripStartIndex[istaF];
        int trip_end   = TripStopIndex[istaF];
        for( int itrip=trip_first; itrip<trip_end; itrip++ ){
          L1Triplet *first_trip = &vTriplets[itrip];

#ifndef FIND_GAPED_TRACKS
          if( /*(isec == kFastPrimIter) ||*/ (isec == kAllPrimIter) || (isec == kAllSecIter) || (isec == kAllSecJumpIter) ) {
#else
          if( (isec == kFastPrimIter) || (isec == kFastPrimJumpIter) || (isec == kAllPrimIter) || (isec == kAllPrimJumpIter) || (isec == kAllSecIter) || (isec == kAllSecJumpIter) ) {
#endif
            if ( first_trip->GetLevel() == 0 ) continue; // ghost suppression // find track with 3 hits only if it was created from a chain of triplets, but not from only one triplet
            if ( first_trip->GetLevel() < ilev ) continue; // try only triplets, which can start track with ilev+3 length. w\o it have more ghosts, but efficiency either
            if ( (ilev == 0) &&
                 (GetFStation(vSFlag[(*vStsHitsUnused)[first_trip->GetLHit()].f]) != 0) ) continue;  // ghost supression // collect only MAPS tracks-triplets
          }
          if( GetFUsed( vSFlag[(*vStsHitsUnused)[first_trip->GetLHit()].f] | vSFlagB[(*vStsHitsUnused)[first_trip->GetLHit()].b] ) ) continue;

           // OK, search the best track candidate in the branches starting from the first_trip
          
          L1Triplet *curr_trip = first_trip;
          L1Branch curr_tr;
          curr_tr.StsHits.push_back(RealIHit[first_trip->GetLHit()]);
          unsigned char curr_L = 1;
          fscal curr_chi2 = first_trip->GetChi2();

          L1Branch best_tr = curr_tr;
          fscal best_chi2 = curr_chi2;
          unsigned char best_L = curr_L;
          int NCalls = 1;
          CAFindTrack(istaF, best_tr, best_L, best_chi2, curr_trip, curr_tr, curr_L, curr_chi2, NCalls);

          // if( ( (isec == kAllSecIter) || (isec == kAllSecJumpIter) ) &&
          //     (GetFStation((*vStsHitsUnused)[best_tr.StsHits[0]].f ) >= 4) ) break; // ghost supression
          
          // {
          //   if ( best_L < 3 ) continue;
          //   BranchExtender(best_tr);
          //   best_L = best_tr.StsHits.size();
          // }

          if ( best_L < ilev + 2 ) continue;      // lose maximum one hit
          if ( best_L < min_level + 3 ) continue; // should find all hits for min_level

          int ndf = best_L*2-5;
          best_chi2 = best_chi2/ndf; //normalize
          if (best_chi2 > TRACK_CHI2_CUT) continue;
          
          // BranchExtender(best_tr);
          // best_L = best_tr.StsHits.size();

//          if( (isec == kAllPrimIter) || (isec == kAllPrimJumpIter) || (isec == kAllSecIter) || (isec == kAllSecJumpIter) )
            if( fGhostSuppression ){
              if( best_L == 3 ){
                  // if( isec == kAllSecIter ) continue; // too /*short*/ secondary track
                if( ( (isec == kAllSecIter) || (isec == kAllSecJumpIter) ) && (istaF != 0) ) continue; // too /*short*/ non-MAPS track
                if( (isec != kAllSecIter) && (isec != kAllSecJumpIter) && (best_chi2 > 5.0) ) continue;
              }
            }

          // BranchExtender(best_tr);
          // best_L = best_tr.StsHits.size();
          
             // store candidate
          best_tr.Set( istaF, best_L, best_chi2, first_trip->GetQpOrig(MaxInvMom));
          vtrackcandidate.push_back(best_tr);
          
          ntracks++;
        }  // itrip
        //cout<<" level "<<ilev<<", station "<<ista<<" ok"<<endl;
      } // istaF

      // cout <<"Level "<< ilev <<" track candidates "<< ntracks << endl;
      // cout << " total number of track candidates " << vtrackcandidate.size() <<endl;

      if (--nlevel == 0) break;
#ifdef XXX
      c_time_fit_bra.Stop();
      time_fit_bra[isec] += double(c_time_fit_bra.CpuTime());
#endif
        // -- make competition between tracks of the same length --
#ifdef XXX
      TStopwatch c_time_fit_fin;
#endif
        // sort track candidates
      vector<L1Branch*> vptrackcandidate;  // vptrackcandidate - array of pointers to vtrackcandidate
      vptrackcandidate.clear();

      for (vector<L1Branch>::iterator trIt = vtrackcandidate.begin();
                                       trIt != vtrackcandidate.end(); ++trIt){

        vptrackcandidate.push_back(&(*trIt));

      }
      //if (isec <= 1)
      sort(vptrackcandidate.begin(), vptrackcandidate.end(), L1Branch::comparePChi2);
      //else
      //sort(vptrackcandidate.begin(), vptrackcandidate.end(), L1Branch::comparePMomentum);



        // choose and save tracks
//       int nUsed[10] = {0,0,0,0,0, 0,0,0,0,0};
      ntracks = 0;
      for (vector<L1Branch*>::iterator   trIt = vptrackcandidate.begin();
                                        trIt != vptrackcandidate.end(); ++trIt){
        L1Branch *tr = *trIt;
        
          // check if some hits have been used already
        int nused = 0;
        for (vector<THitI>::iterator phIt = tr->StsHits.begin();
            phIt != tr->StsHits.end(); ++phIt){
          L1StsHit &h = vStsHits[*phIt];
          if ( GetFUsed( vSFlag[h.f] | vSFlagB[h.b] ) ) nused++;
        }
        if (nused > 0){/*nUsed[nused+1]++;*/ continue;} // don't allow tracks have shared hits // track will be shorter, leave it for the next iteration

        //=======================================================
        // gather MAPS hits using the Kalman filter
        // was here, skipped for a moment
        //=======================================================

        // if(isec <= kAllSecIter){ // check chi2. gives almost nothing
        //   L1TrackPar T;
        //   BranchFitterFast(*tr, T, 0);
        //   if (T.chi2[0]/T.NDF[0] > TRACK_CHI2_CUT) continue;
        // }
        
        // BranchExtender(*tr);
        
          // store track
        for (vector<THitI>::iterator phitIt = tr->StsHits.begin();
                                                  phitIt != tr->StsHits.end(); ++phitIt){
          L1StsHit &h = vStsHits[*phitIt];
          SetFUsed( vSFlag[h.f] );
          SetFUsed( vSFlagB[h.b] );
          vRecoHits.push_back(*phitIt);
        }

        L1Track t;
        t.NHits = tr->StsHits.size();
        t.Momentum = tr->Momentum;
        for( int i=0; i<6; i++) t.TFirst[i] = t.TLast[i]=0; // CHECKME don't need this
        for( int i=0; i<15; i++ ) t.CFirst[i] = t.CLast[i] = 10;
        t.chi2 = 100;

        vTracks.push_back(t);
        ntracks++;

      } // i_trackCandidate

//       for (int iu = 0; iu < 10; iu++) cout << iu+1 << " " << nUsed[iu] << endl;
#ifdef XXX
      c_time_fit_fin.Stop();
      time_fit_fin[isec] += double(c_time_fit_fin.CpuTime());

      unsigned int Bsize=0, nb=0;
      for (vector<L1Branch*>::iterator   trIt = vptrackcandidate.begin();
                                         trIt != vptrackcandidate.end(); ++trIt){
        L1Branch *tr = *trIt;
        Bsize+= sizeof(L1Branch) + sizeof(THitI) * tr->StsHits.size();
        nb++;
      }

      if( Bsize>stat_max_BranchSize ) stat_max_BranchSize = Bsize;
      if( nb>stat_max_n_branches ) stat_max_n_branches = nb;
#endif

      //cout<< " track candidates stored: "<< ntracks <<endl;
    } //ilev

      // renew hits array
#ifdef XXX
    cout << " renew hits arrays " << endl;
#endif

    int StsHitsUnusedStartIndex_temp;
    int ista = 0;
    int nDontUsedHits = 0;
    vStsHitsUnused_buf->clear();
    vStsHitPointsUnused_buf->clear();
    for(; ista < NStations; ista++){

      StsHitsUnusedStartIndex_temp = StsHitsUnusedStartIndex[ista];
      StsHitsUnusedStartIndex[ista] = nDontUsedHits;
      for(THitI ih = StsHitsUnusedStartIndex_temp; ih < StsHitsUnusedStopIndex[ista]; ih++){
        THitI rih = RealIHit[ih];

        L1StsHit hit = vStsHits[rih];
        if( GetFUsed( vSFlag[hit.f] | vSFlagB[hit.b] ) ){ continue;} // if used
        vStsHitsUnused_buf->push_back(hit);
        vStsHitPointsUnused_buf->push_back((*vStsHitPointsUnused)[ih]);

        RealIHit[nDontUsedHits] = rih;
        nDontUsedHits++;
      }
      StsHitsUnusedStopIndex[ista] = nDontUsedHits;
    }
    std::vector< L1StsHit > *vStsHitsUnused_temp = vStsHitsUnused;
    vStsHitsUnused = vStsHitsUnused_buf;
    vStsHitsUnused_buf = vStsHitsUnused_temp;
    std::vector< L1HitPoint > *vStsHitsUnused_temp2 = vStsHitPointsUnused;
    vStsHitPointsUnused = vStsHitPointsUnused_buf;
    vStsHitPointsUnused_buf = vStsHitsUnused_temp2;

    vStsHitsUnused_buf->clear();
    vStsHitPointsUnused_buf->clear();
// cout << " clear hits array end " << endl;

//     cout<<"End of collecting track candidates "<<endl;
#ifdef XXX
    c_time_fit.Stop();
    time_fit[isec] += double(c_time_fit.CpuTime());
#endif
#ifdef XXX
//     if( stat_max_n_trip<stat_n_trip ) stat_max_n_trip = vTriplets.size();
    unsigned int tsize = vTriplets.size()*sizeof(L1Triplet);
    if( stat_max_trip_size < tsize ) stat_max_trip_size = tsize;
#endif
// #ifdef DRAW
//     draw.ClearVeiw();
// //   draw.DrawInfo();
//     draw.DrawRestHits(StsHitsUnusedStartIndex, StsHitsUnusedStopIndex, RealIHit);
//     draw.DrawRecoTracks();
//     draw.SaveCanvas("Reco_"+isec+"_");
//     draw.DrawAsk();
// #endif

// #ifdef PULLS
//       fL1Pulls->Build(1);
// #endif
    
  } // for (int isec

  CAMergeClones();


  //cout<< "Total number of tracks found " << vTracks.size() <<endl;

  //==================================

  c_time.Stop();

//   cout << "End TrackFinder" << endl;
  CATime = (double(c_time.CpuTime()));


#ifdef XXX
  double CATimeReal = (double(c_time.RealTime()));
  /*
  cout<<"CA Track Finder: " << (double(c_time.CpuTime()))
      <<" (" << time_trip <<" ["
      << time_find[1][0] << "/"<< time_find[1][1]<< "+"
      << time_find[1][0] << "/"<< time_find[1][1]<< "+"
      << time_find[2][0] << "/"<< time_find[2][1]<< "+"
      << time_find[4][0] << "+"
      << time_find[4][0] << "+"
      << "]"
      << " + " << time_fit <<" [" << time_fit_bra << "/" << time_fit_fin << "]"
      << " ) s/ev"<<endl;
  */
  stat_N++;
  stat_time     += CATime;
  stat_time_real += CATimeReal;


  cout.precision(3);
  cout<<"CA stat time: " << (stat_time/stat_N) << "/" << stat_time_real/stat_N << "(k=" << stat_time/stat_time_real <<") s/ev"<<endl;

  for (int isec2 = 0; isec2 < Nisec; isec2++){
    stat_time_trip[isec2] += time_trip[isec2];

    for (int i1 = 0; i1 < ntimers; i1++)
      for (int i2 = 0; i2 < 2; i2++)
        stat_time_find[i1][i2][isec] += time_find[i1][i2][isec];

    stat_time_fit[isec] += time_fit[isec];
    stat_time_fit_bra[isec] += time_fit_bra[isec];
    stat_time_fit_fin[isec] += time_fit_fin[isec];



  cout  << isec << " isec :   "
        <<" (" << stat_time_trip[isec]/stat_N <<" ["
          << stat_time_find[1][0][isec]/stat_N  <<"/" << stat_time_find[1][1][isec]/stat_N
          <<  "(k=" << stat_time_find[1][0][isec]/stat_time_find[1][1][isec] <<")+"
          << stat_time_find[2][0][isec]/stat_N  <<"/" << stat_time_find[2][1][isec]/stat_N
          <<  "(k=" << stat_time_find[1][0][isec]/stat_time_find[1][1][isec] <<")+"
          << stat_time_find[2][0][isec]/stat_N  <<"/" << stat_time_find[2][1][isec]/stat_N <<" + "
          << stat_time_find[3][0][isec]/stat_N <<" + "
          << stat_time_find[4][0][isec]/stat_N
          << "]"
        << " + " << stat_time_fit[isec]/stat_N
          <<" [" << stat_time_fit_bra[isec]/stat_N << "\\" << stat_time_fit_fin[isec]/stat_N << "]"
        << ") s/ev"<<endl;

  stat_time_find_all[1][0] += stat_time_find[1][0][isec];
  stat_time_find_all[1][1] += stat_time_find[1][1][isec];
  stat_time_find_all[2][0] += stat_time_find[2][0][isec];
  stat_time_find_all[2][1] += stat_time_find[2][1][isec];
  stat_time_find_all[3][0] += stat_time_find[3][0][isec];
  stat_time_find_all[3][1] += stat_time_find[4][0][isec];
  stat_time_find_all[4][0] += stat_time_fit[isec];
  } // isec

  for(int i2 = 0; i2 < ntimers; i2++)
    for(int i3 = 0; i3 < 2; i3++)
      stat_time_find_all[i2][i3] /= stat_N;

  cout  << "all :   "
        << stat_time_find[0][0] << "+"
        <<" (" << "  ["
          << stat_time_find_all[1][0]  <<"/" <<   stat_time_find_all[1][1] << "(k=" << stat_time_find_all[1][0]/stat_time_find_all[1][1] <<")+"
          << stat_time_find_all[2][0]  <<"/" <<   stat_time_find_all[2][1] << "(k=" << stat_time_find_all[2][0]/stat_time_find_all[2][1] <<")+"
          << stat_time_find_all[3][0]  <<"+"
          << stat_time_find_all[3][1]
          << "]"
        << " + " << stat_time_find_all[4][0]
        << ") s/ev"<<endl;


  static long int NTimes =0, NHits=0, HitSize =0, NStrips=0, StripSize =0, NStripsB=0, StripSizeB =0,
    NDup=0, DupSize=0, NTrip=0, TripSize=0, NBranches=0, BranchSize=0, NTracks=0, TrackSize=0 ;

  NTimes++;
  NHits += vStsHitsUnused->size();
  HitSize += vStsHitsUnused->size()*sizeof(L1StsHit);
  NStrips+= vStsStrips.size();
  StripSize += vStsStrips.size()*sizeof(fscal) + vSFlag.size()*sizeof(unsigned char);
  NStripsB+= vSFlagB.size();
  StripSizeB += vStsStripsB.size()*sizeof(fscal) + vSFlagB.size()*sizeof(unsigned char);
  NDup += stat_max_n_dup;
  DupSize += stat_max_n_dup*sizeof(/*short*/ int);
  NTrip += stat_max_n_trip;
  TripSize += stat_max_trip_size;

  NBranches += stat_max_n_branches;
  BranchSize += stat_max_BranchSize;
  NTracks += vTracks.size();
  TrackSize += sizeof(L1Track)*vTracks.size() + sizeof(THitI)*vRecoHits.size();
  int k = 1024*NTimes;

  cout<<"L1 Event size: \n"
      <<HitSize/k<<"kB for "<<NHits/NTimes<<" hits, \n"
      <<StripSize/k<<"kB for "<<NStrips/NTimes<<" strips, \n"
      <<StripSizeB/k<<"kB for "<<NStripsB/NTimes<<" stripsB, \n"
      <<DupSize/k<<"kB for "<<NDup/NTimes<<" doublets, \n"
      <<TripSize/k<<"kB for "<<NTrip/NTimes<<" triplets\n"
      <<BranchSize/k<<"kB for "<<NBranches/NTimes<<" branches, \n"
      <<TrackSize/k<<"kB for "<<NTracks/NTimes<<" tracks. "<<endl;
  cout<<" L1 total event size = "
      <<( HitSize + StripSize +  StripSizeB + DupSize + TripSize + BranchSize + TrackSize )/k
      <<" Kb"<<endl;

  //cout << "===> ... CA Track Finder    " << endl << endl;
#endif

#ifdef DRAW
  draw.ClearVeiw();
  draw.DrawInputHits();
//   draw.DrawInfo();
  draw.DrawRecoTracks();
  draw.SaveCanvas("Reco_");
  draw.DrawAsk();
#endif 
#ifdef PULLS
  static int iEvee = 0;
  iEvee++;
  if (iEvee%1 == 0)
    fL1Pulls->Build(1);
#endif
#ifdef DOUB_PERFORMANCE
  fL1Eff_doublets->CalculateEff();
  fL1Eff_doublets->Print("Doublets performance.",1);
#endif
#ifdef TRIP_PERFORMANCE
  fL1Eff_triplets->CalculateEff();
  fL1Eff_triplets->Print("Triplet performance",1);
//   fL1Eff_triplets2->CalculateEff();
//   fL1Eff_triplets2->Print("Triplet performance. After chi2 cut");
#endif

}


  /** *************************************************************
   *                                                              *
   *     The routine performs recusrsive search for tracks        *
   *                                                              *
   *     I. Kisel                                    06.03.05     *
   *                                                              *
   ****************************************************************/
  
void L1Algo::CAFindTrack(int ista,
                         L1Branch &best_tr, unsigned char &best_L, fscal &best_chi2,
                         const L1Triplet* curr_trip,
                         L1Branch &curr_tr, unsigned char &curr_L, fscal &curr_chi2,
                         int &NCalls )
{
  if (curr_trip->GetLevel() == 0){ // the end of the track -> check and store
    
      // -- finish with current track
      // add rest of hits
    THitI ihitm = curr_trip->GetMHit();
    THitI ihitr = curr_trip->GetRHit();
    if( !GetFUsed( vSFlag[(*vStsHitsUnused)[ihitm].f] | vSFlagB[(*vStsHitsUnused)[ihitm].b] )  ){
      curr_tr.StsHits.push_back(RealIHit[ihitm]);
      curr_L++;
    }
    if( !GetFUsed( vSFlag[(*vStsHitsUnused)[ihitr].f] | vSFlagB[(*vStsHitsUnused)[ihitr].b] ) ){
      curr_tr.StsHits.push_back(RealIHit[ihitr]);
      curr_L++;
    }
    if( curr_chi2 > TRACK_CHI2_CUT * (curr_L*2-5) ) return;

      // try to find more hits
#define EXTEND_TRACKS
#ifdef EXTEND_TRACKS
    if (curr_L >= 3){
        // curr_chi2 =  // good for ghosts, refSec, but bad for time
      BranchExtender(curr_tr);
      curr_L = curr_tr.StsHits.size();
        // if( curr_chi2 > TRACK_CHI2_CUT * (curr_L*2-5) ) return;
    }
#endif // EXTEND_TRACKS
    
      // -- select the best
    if ( (curr_L > best_L ) || ( (curr_L == best_L) && (curr_chi2 < best_chi2) ) ){
      best_tr   = curr_tr;
      best_chi2 = curr_chi2;
      best_L    = curr_L;
    }
    
    return;
  }


    // try to extend. try all possible triplets
  int offset = TripStartIndex[curr_trip->GetMSta()];
  int NNeighs = curr_trip->neighbours.size();
  for (int in = 0; in < NNeighs; in++) {
    int index = curr_trip->neighbours[in] + offset;
    L1Triplet *new_trip = &vTriplets[index];

      // check new triplet
    const fscal qp1 = curr_trip->GetQp();
    const fscal qp2 = new_trip->GetQp();
    fscal dqp = fabs(qp1 - qp2);
    fscal Cqp = curr_trip->Cqp;
    Cqp      += new_trip->Cqp;
    if ( dqp > PickNeighbour * Cqp  ) continue; // bad neighbour // CHECKME why do we need recheck it?? (it really change result)

    if ( GetFUsed( vSFlag[(*vStsHitsUnused)[new_trip->GetLHit()].f] | vSFlagB[(*vStsHitsUnused)[new_trip->GetLHit()].b] ) ){ 
        //  no used hits allowed -> compare and store track
      if ( ( curr_L > best_L ) || ( (curr_L == best_L) && (curr_chi2 < best_chi2) ) ){
        best_tr   = curr_tr;
        best_chi2 = curr_chi2;
        best_L    = curr_L;
      }
    }
    else{ //  add new triplet to the current track

          // restore current track
        // save current tracklet
      L1Branch new_tr = curr_tr;
      unsigned char new_L = curr_L;
      fscal new_chi2 = curr_chi2;

        // add new hit
      new_tr.StsHits.push_back(RealIHit[new_trip->GetLHit()]);
      new_L += 1;
      dqp = dqp/Cqp*5.;  // CHECKME: understand 5, why no sqrt(5)?
      new_chi2 += dqp*dqp;
  //   dqp = dqp/Cqp; // IKu
  //   new_chi2 += dqp*dqp*5.;
      
      if( new_chi2 > TRACK_CHI2_CUT * new_L ) continue;

      const int new_ista = ista + new_trip->GetMSta() - new_trip->GetLSta();
      CAFindTrack(new_ista, best_tr, best_L, best_chi2, new_trip, new_tr, new_L, new_chi2, NCalls);
      NCalls++;
    } // add triplet to track
  } // for neighbours

  return;
}
