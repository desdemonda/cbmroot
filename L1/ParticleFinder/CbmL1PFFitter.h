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
 *  Finds Particles: Lambdas, K0
 *
 */

#ifndef _CbmL1PFFitter_h_
#define _CbmL1PFFitter_h_

#include "CbmL1Def.h"
#include <vector>

class CbmL1Track;
class CbmStsTrack;
class L1TrackPar;
class L1UMeasurementInfo;
class L1MaterialInfo;
class L1Station;
class L1FieldRegion;
class CbmKFVertex;

class CbmL1PFFitter
{
 public:
  CbmL1PFFitter();
  ~CbmL1PFFitter();

  void FilterFirst( L1TrackPar &track,fvec &x, fvec &y, L1Station &st );

  //functions for fitting CbmStsTrack
  void Fit(std::vector<CbmStsTrack> &Tracks, std::vector<int>& pidHypo);
  void CalculateFieldRegion(std::vector<CbmStsTrack> &Tracks,std::vector<L1FieldRegion> &Field);
  void GetChiToVertex(std::vector<CbmStsTrack> &Tracks, std::vector<L1FieldRegion> &field, std::vector<float> &chiToVtx, CbmKFVertex &primVtx,
                      float chiPrim = -1);
  
  void EnergyLossCorrection(L1TrackPar& T, const fvec& mass2, const fvec& radThick, fvec& qp0, float direction);
  fvec ApproximateBetheBloch( const fvec &beta2 );
};
#endif
