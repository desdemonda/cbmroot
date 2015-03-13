#ifndef LXMC_INCLUDED
#define LXMC_INCLUDED

#pragma GCC diagnostic ignored "-Weffc++"

#include <vector>
#include <list>
#include "Rtypes.h"
#include "LxSettings.h"
#include "CbmStsTrack.h"

struct LxPoint;
struct LxStsMCPoint;
struct LxMCTrack;

struct LxMCPoint
{
  Double_t p, q, x, y, z, px, py, pz;
  Int_t stationNumber, layerNumber;
  std::list<LxPoint*> lxPoints;
  Int_t trackId;// Original track id -- an index (offset) in the array of MC tracks.
  LxMCTrack* track;

  //LxMCPoint() : p(0), q(0), x(0), y(0), z(0), px(0), py(0), pz(0), stationNumber(0), layerNumber(0), trackId(0), track(0) {}
};

struct LxMCTrack
{
  Double_t p, q, x, y, z, px, py, pz;
  Int_t mother_ID;
  Int_t pdg;
  std::vector<LxMCPoint*> Points;
#ifdef MAKE_STS_MC_RESEARCHES
  std::list<LxStsMCPoint*> stsPoints[8];
#endif//MAKE_STS_MC_RESEARCHES
  UInt_t fUniqueID;
  CbmStsTrack* externalTrack;
  Int_t stationsWithHits;
  Int_t layersWithHits;
  bool hitsOnStations[LXSTATIONS][LXLAYERS];

  //LxMCTrack() : p(0), q(0), x(0), y(0), z(0), px(0), py(0), pz(0), mother_ID(0), pdg(0), fUniqueID(0), externalTrack(0),
      //stationsWithHits(0), layersWithHits(0) {}
};

#endif//LXMC_INCLUDED
