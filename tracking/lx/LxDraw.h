#ifndef LXDRAW_INCLUDED
#define LXDRAW_INCLUDED

#pragma GCC diagnostic ignored "-Weffc++"

#include "TCanvas.h"
#include "TGeoNode.h"

class LxDraw
{
public:
  LxDraw();
  void ClearView();
  void Ask();
  void DrawMCTracks();
  void DrawInputHits();
  void DrawMuch(TGeoNode* node);
  void DrawMuch();
  void DrawRays();
  void DrawRecoTracks();
  void DrawMCPoints();
  void DrawExtTracks();
  void SaveCanvas(TString name);

private:
  TCanvas YZ;
  TCanvas YX;
  TCanvas XZ;
  bool ask;
};

#endif//LXDRAW_INCLUDED
