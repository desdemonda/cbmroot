#ifndef LXCA_INCLUDED
#define LXCA_INCLUDED

#pragma GCC diagnostic ignored "-Weffc++"

#include "Rtypes.h"
#include <list>
#include <math.h>
#include "LxSettings.h"
#include "LxMC.h"

struct LxRay;
struct LxLayer;
struct LxTrack;
struct LxStation;
struct LxSpace;
struct LxTrackCandidate;

struct LxPoint
{
  Double_t x, y, z;
  Double_t dx, dy, dz;
  bool used;
  bool valid;
  bool artificial;
  LxTrack* track;
  std::list<LxRay*> rays;
  LxLayer* layer;
  Int_t hitId;
#ifdef MAKE_EFF_CALC
  std::list<LxMCPoint*> mcPoints;
#endif//MAKE_EFF_CALC

#ifdef REMEMBER_CLUSTERED_RAYS_IN_POINTS
  LxRay* leftClusteredRay;
  LxRay* rightClusteredRay;
#endif//REMEMBER_CLUSTERED_RAYS_IN_POINTS

  LxPoint(Double_t X, Double_t Y, Double_t Z, Double_t Dx, Double_t Dy, Double_t Dz, LxLayer* lay, int hId, bool isArtificial = false) :
      x(X), y(Y), z(Z), dx(Dx), dy(Dy), dz(Dz), layer(lay), used(false), valid(true), track(0), hitId(hId), artificial(isArtificial)
#ifdef REMEMBER_CLUSTERED_RAYS_IN_POINTS
      , leftClusteredRay(0), rightClusteredRay(0)
#endif//REMEMBER_CLUSTERED_RAYS_IN_POINTS
        {}
  ~LxPoint();
  void CreateRay(LxPoint* lPoint, Double_t tx, Double_t ty, Double_t dtx, Double_t dty);
};

#ifdef USE_KALMAN
struct LxKalmanParams
{
  Double_t tx, ty;
  Double_t C11, C22;
  Double_t chi2;
};
#endif//USE_KALMAN

struct LxRay
{
  Double_t tx, ty;
  Double_t dtx, dty;
  LxPoint* source;
  LxPoint* end;
  LxStation* station;
  std::list<LxRay*> neighbours;
#ifdef CLUSTER_MODE
  Int_t level;
  bool used;
  std::list<LxRay*> neighbourhood;
  std::list<LxPoint*> clusterPoints;
#endif//CLUSTER_MODE

#ifdef USE_KALMAN
  LxKalmanParams kalman;
#endif//USE_KALMAN
  LxRay(LxPoint* s, LxPoint* e
#ifdef CLUSTER_MODE
      , Int_t
#endif//CLUSTER_MODE
      );
  LxRay(LxPoint* s, LxPoint* e, Double_t Tx, Double_t Ty, Double_t Dtx, Double_t Dty
#ifdef CLUSTER_MODE
      , Int_t
#endif//CLUSTER_MODE
      );
};

struct LxLayer
{
  std::list<LxPoint*> points;
  LxStation* station;
  int layerNumber;
  Double_t zCoord;
  LxLayer(LxStation* st, int lNum);
  ~LxLayer();
  void Clear();

  LxPoint* AddPoint(int hitId, Double_t x, Double_t y, Double_t z, Double_t dx, Double_t dy, Double_t dz, bool isArtificial = false)
  {
    LxPoint* result = new LxPoint(x, y, z, dx, dy, dz, this, hitId, isArtificial);
    points.push_back(result);
    return result;
  }

  LxPoint* PickNearestPoint(Double_t x, Double_t y);// Used in track building.
  LxPoint* PickNearestPoint(LxRay* ray);// Used in track building.
  LxPoint* PickNearestPoint(Double_t x, Double_t y, Double_t deltaX, Double_t deltaY);// Used in middle point building.
  bool HasPointInRange(Double_t x, Double_t y, Double_t deltaX, Double_t deltaY);
};

#ifdef CLUSTER_MODE
typedef void* kdt_rays_handle;
#endif//CLUSTER_MODE

struct LxStation
{
  std::vector<LxLayer*> layers;
#ifdef CLUSTER_MODE
  kdt_rays_handle raysHandle;
  std::vector<std::list<LxRay*>*> clusters[2 * LXLAYERS];
  kdt_rays_handle clusteredRaysHandle;
  std::list<LxPoint*> clusteredPoints;
  Double_t clusterXLimit;
  Double_t clusterXLimit2;
  Double_t clusterYLimit;
  Double_t clusterYLimit2;
  Double_t clusterTxLimit;
  Double_t clusterTxLimit2;
  Double_t clusterTyLimit;
  Double_t clusterTyLimit2;
#endif//CLUSTER_MODE
  LxSpace* space;
  int stationNumber;
  Double_t zCoord;
  Double_t txLimit;
  Double_t tyLimit;
  Double_t txBreakLimit;
  Double_t tyBreakLimit;
  Double_t txBreakSigma;
  Double_t tyBreakSigma;
  Double_t disp01XSmall;// 'disp' -- means: dispersion.
  Double_t disp01XBig;
  Double_t disp01YSmall;
  Double_t disp01YBig;
  Double_t disp02XSmall;
  Double_t disp02XBig;
  Double_t disp02YSmall;
  Double_t disp02YBig;
#ifdef USE_KALMAN_FIT
  Double_t MSNoise[2][2][2];
#endif//USE_KALMAN_FIT
  LxStation(LxSpace* sp, int stNum);
  ~LxStation();
  void Clear();

  LxPoint* AddPoint(int layerNumber, int hitId, Double_t x, Double_t y, Double_t z, Double_t dx, Double_t dy, Double_t dz)
  {
    return layers[layerNumber]->AddPoint(hitId, x, y, z, dx, dy, dz);
  }

  void RestoreMiddlePoints();
  void BuildRays();
#ifdef CLUSTER_MODE
  void BuildRays2();
  void InsertClusterRay(Int_t levels, Int_t cardinality, LxRay* clusterRay);
#endif//CLUSTER_MODE
  void ConnectNeighbours();
};

struct LxExtTrack
{
  CbmStsTrack* track;
  Int_t extId;
  LxMCTrack* mcTrack;
#ifdef LX_EXT_LINK_SOPH
  std::pair<LxTrack*, Double_t> recoTrack;
#endif//LX_EXT_LINK_SOPH

  LxExtTrack() : track(0), extId(-1), mcTrack(0)
#ifdef LX_EXT_LINK_SOPH
      , recoTrack(0, 0)
#endif//LX_EXT_LINK_SOPH
  {}
};

struct LxTrack
{
  LxExtTrack* externalTrack;
#ifdef LX_EXT_LINK_SOPH
  std::list<std::pair<LxExtTrack*, Double_t> > extTrackCandidates;
#else//LX_EXT_LINK_SOPH
  Double_t extLinkChi2;
#endif//LX_EXT_LINK_SOPH
  bool matched;
  LxMCTrack* mcTrack;
#ifdef CALC_LINK_WITH_STS_EFF
  std::list<LxMCTrack*> mcTracks;
#endif//CALC_LINK_WITH_STS_EFF
  int length;
  LxRay* rays[LXSTATIONS - LXFIRSTSTATION - 1];// Rays are stored left to right.
  LxPoint* points[(LXSTATIONS - LXFIRSTSTATION) * LXLAYERS];
  Double_t chi2;
  Double_t aX;
  Double_t bX;
  Double_t aY;
  Double_t bY;
  int restoredPoints;
#ifdef USE_KALMAN_FIT
  Double_t x, y, z, dx, dy, tx, ty, dtx, dty;
#endif//USE_KALMAN_FIT
  bool clone;
  bool triggering;

  explicit LxTrack(LxTrackCandidate* tc);
  void Fit();
#ifdef LX_EXT_LINK_SOPH
  void Rebind();
#endif//LX_EXT_LINK_SOPH
};

struct LxSpace
{
  std::vector<LxStation*> stations;
  std::list<LxTrack*> tracks;
  std::list<LxExtTrack> extTracks;
  Double_t muchStsBreakX;
  Double_t muchStsBreakY;
  Double_t muchStsBreakTx;
  Double_t muchStsBreakTy;

  LxSpace();
  void Clear();

  LxPoint* AddPoint(int stationNumber, int layerNumber, int hitId, Double_t x, Double_t y, Double_t z, Double_t dx, Double_t dy, Double_t dz)
  {
    return stations[stationNumber]->AddPoint(layerNumber, hitId, x, y, z, dx, dy, dz);
  }

  void RestoreMiddlePoints();
  void BuildRays();
#ifdef CLUSTER_MODE
  void BuildRays2();
  void ConnectNeighbours2();
  void BuildCandidates2(LxRay* ray, LxRay** rays, std::list<LxTrackCandidate*>& candidates, Double_t chi2);
  void Reconstruct2();
#endif//CLUSTER_MODE
  void ConnectNeighbours();
  void BuildCandidates(LxRay* ray, LxRay** rays, std::list<LxTrackCandidate*>& candidates, Double_t chi2);
  void Reconstruct();
  void RemoveClones();
  void FitTracks();
  void JoinExtTracks();
  void CheckArray(Double_t xs[LXSTATIONS][LXLAYERS], Double_t ys[LXSTATIONS][LXLAYERS],
      Double_t zs[LXSTATIONS][LXLAYERS],
      Double_t xDisp2Limits[LXSTATIONS], Double_t yDisp2Limits[LXSTATIONS], Double_t tx2Limits[LXSTATIONS],
        Double_t ty2Limits[LXSTATIONS], Double_t txBreak2Limits[LXSTATIONS], Double_t tyBreak2Limits[LXSTATIONS]);
  void CheckArray(Double_t xs[LXSTATIONS][LXLAYERS], Double_t ys[LXSTATIONS][LXLAYERS],
      Double_t zs[LXSTATIONS][LXLAYERS], std::list<LxPoint*> pts[LXSTATIONS][LXLAYERS], int level,
      Double_t xDisp2Limits[LXSTATIONS], Double_t yDisp2Limits[LXSTATIONS], Double_t tx2Limits[LXSTATIONS],
        Double_t ty2Limits[LXSTATIONS], Double_t txBreak2Limits[LXSTATIONS], Double_t tyBreak2Limits[LXSTATIONS]);
  void CheckArray(std::ostream& out, LxMCTrack& track);
};

#endif//LXCA_INCLUDED
