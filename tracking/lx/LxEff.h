#ifndef LXEFF_INCLUDED
#define LXEFF_INCLUDED

#pragma GCC diagnostic ignored "-Weffc++"

#include "Rtypes.h"
#include "LxSettings.h"
#include <map>
#include <list>
#include <iostream>

class LxFinder;
class LxPoint;

struct LxEff
{
  explicit LxEff(LxFinder& owner);
  void CalcRecoEff(bool joinExt);
#ifdef CLUSTER_MODE
  void AnalyzeNotMatched(std::ofstream& out, std::list<LxPoint*> hits[LXSTATIONS][LXLAYERS]);
  void AnalyzeNotMatched2(std::ofstream& out, std::list<LxPoint*> hits[LXSTATIONS][LXLAYERS]);
  void DumpNotMatchedMC(std::list<LxPoint*> hits[LXSTATIONS][LXLAYERS]);
#endif//CLUSTER_MODE
  LxFinder& finder;
};

struct LxMCTrack;

struct LxStsMCPoint
{
  Double_t p, q, x, y, z, px, py, pz;
  Int_t stationNumber;
  LxMCTrack* mcTrack;
};

#endif//LXEFF_INCLUDED
