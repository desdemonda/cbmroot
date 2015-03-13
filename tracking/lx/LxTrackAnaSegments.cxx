#include "LxTrackAnaSegments.h"
#include "LxTrackAna.h"
#include "TH1.h"
#include "TH2.h"
#include <cmath>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;

static TH1F* muchInStationXDispLeft[LXSTATIONS];
static TH1F* muchInStationXDispRight[LXSTATIONS];
static TH1F* muchInStationYDispLeft[LXSTATIONS];
static TH1F* muchInStationYDispRight[LXSTATIONS];

static TH1F* muchInStationXDispRL[LXSTATIONS];
static TH1F* muchInStationYDispRL[LXSTATIONS];

static TH1F* muchInStationTxBreak[LXSTATIONS];
static TH1F* muchInStationTyBreak[LXSTATIONS];

static TH1F* muchOutStationTxBreakLeft[LXSTATIONS - 1];
static TH1F* muchOutStationTxBreakRight[LXSTATIONS - 1];
static TH1F* muchOutStationTyBreakLeft[LXSTATIONS - 1];
static TH1F* muchOutStationTyBreakRight[LXSTATIONS - 1];

static TH1F* muchOutStationXDispByTriplet[LXSTATIONS - 1];
static TH1F* muchOutStationYDispByTriplet[LXSTATIONS - 1];

static TH1F* muchOutStationXDispByVertex[LXSTATIONS - 1];
static TH1F* muchOutStationYDispByVertex[LXSTATIONS - 1];

static TH1F* muchLongSegmentTxHisto[LXSTATIONS - 1];
static TH1F* muchLongSegmentTyHisto[LXSTATIONS - 1];

static TH1F* muchSegmentTxBreakHisto[LXSTATIONS - 2];
static TH1F* muchSegmentTyBreakHisto[LXSTATIONS - 2];

static TH1F* muchStationTxDispHisto[LXSTATIONS - 1];
static TH1F* muchStationTyDispHisto[LXSTATIONS - 1];

static TH2F* muchXTxCovHisto[LXSTATIONS - 1];
static TH2F* muchYTyCovHisto[LXSTATIONS - 1];

static TH1F* muchClusterXDispHisto[LXSTATIONS - 1];
static TH1F* muchClusterYDispHisto[LXSTATIONS - 1];
static TH1F* muchClusterTxDispHisto[LXSTATIONS - 1];
static TH1F* muchClusterTyDispHisto[LXSTATIONS - 1];

LxTrackAnaSegments::LxTrackAnaSegments(LxTrackAna& o) : owner(o)
{
}

void LxTrackAnaSegments::Init()
{
  char name[64];
  char title[256];

  for (Int_t i = 0; i < LXSTATIONS; ++i)
  {
    sprintf(name, "muchInStationXDispLeft_%d", i);
    sprintf(title, "X dispersion from central to left layer inside station: %d", i);
    muchInStationXDispLeft[i] = new TH1F(name, title, 100, -3.0, 3.0);
    muchInStationXDispLeft[i]->StatOverflows();

    sprintf(name, "muchInStationXDispRight_%d", i);
    sprintf(title, "X dispersion from central to right layer inside station: %d", i);
    muchInStationXDispRight[i] = new TH1F(name, title, 100, -3.0, 3.0);
    muchInStationXDispRight[i]->StatOverflows();

    sprintf(name, "muchInStationYDispLeft_%d", i);
    sprintf(title, "Y dispersion from central to left layer inside station: %d", i);
    muchInStationYDispLeft[i] = new TH1F(name, title, 100, -3.0, 3.0);
    muchInStationYDispLeft[i]->StatOverflows();

    sprintf(name, "muchInStationYDispRight_%d", i);
    sprintf(title, "Y dispersion from central to right layer inside station: %d", i);
    muchInStationYDispRight[i] = new TH1F(name, title, 100, -3.0, 3.0);
    muchInStationYDispRight[i]->StatOverflows();

    sprintf(name, "muchInStationXDispRL_%d", i);
    sprintf(title, "X dispersion on left layer predicted by right station: %d", i);
    muchInStationXDispRL[i] = new TH1F(name, title, 100, -0.1, 0.1);
    muchInStationXDispRL[i]->StatOverflows();

    sprintf(name, "muchInStationYDispRL_%d", i);
    sprintf(title, "Y dispersion on left layer predicted by right station: %d", i);
    muchInStationYDispRL[i] = new TH1F(name, title, 100, -0.1, 0.1);
    muchInStationYDispRL[i]->StatOverflows();

    sprintf(name, "muchInStationTxBreak_%d", i);
    sprintf(title, "Tx break inside station: %d", i);
    muchInStationTxBreak[i] = new TH1F(name, title, 100, -0.02, 0.02);
    muchInStationTxBreak[i]->StatOverflows();

    sprintf(name, "muchInStationTyBreak_%d", i);
    sprintf(title, "Ty break inside station: %d", i);
    muchInStationTyBreak[i] = new TH1F(name, title, 100, -0.02, 0.02);
    muchInStationTyBreak[i]->StatOverflows();

    if (i > 0)
    {
      sprintf(name, "muchLongSegmentTxHisto_%d", i);
      sprintf(title, "Tx tangents distribution for segments between stations: %d and %d", i - 1, i);
      muchLongSegmentTxHisto[i - 1] = new TH1F(name, title, 100, -.15, .15);
      muchLongSegmentTxHisto[i - 1]->StatOverflows();

      sprintf(name, "muchLongSegmentTyHisto_%d", i);
      sprintf(title, "Ty tangents distribution for segments between stations: %d and %d", i - 1, i);
      muchLongSegmentTyHisto[i - 1] = new TH1F(name, title, 100, -.15, .15);
      muchLongSegmentTyHisto[i - 1]->StatOverflows();

      sprintf(name, "muchClusterXDispHisto_%d", i);
      sprintf(title, "X coordinate dispersion for cluster segments between stations: %d and %d", i - 1, i);
      muchClusterXDispHisto[i - 1] = new TH1F(name, title, 100, .0, 3.0);
      muchClusterXDispHisto[i - 1]->StatOverflows();

      sprintf(name, "muchClusterYDispHisto_%d", i);
      sprintf(title, "Y coordinate dispersion for cluster segments between stations: %d and %d", i - 1, i);
      muchClusterYDispHisto[i - 1] = new TH1F(name, title, 100, .0, 3.0);
      muchClusterYDispHisto[i - 1]->StatOverflows();

      sprintf(name, "muchClusterTxDispHisto_%d", i);
      sprintf(title, "Tx tangent dispersion for cluster segments between stations: %d and %d", i - 1, i);
      muchClusterTxDispHisto[i - 1] = new TH1F(name, title, 100, .0, .05);
      muchClusterTxDispHisto[i - 1]->StatOverflows();

      sprintf(name, "muchClusterTyDispHisto_%d", i);
      sprintf(title, "Ty tangent dispersion for cluster segments between stations: %d and %d", i - 1, i);
      muchClusterTyDispHisto[i - 1] = new TH1F(name, title, 100, .0, .05);
      muchClusterTyDispHisto[i - 1]->StatOverflows();

      sprintf(name, "muchOutStationTxBreakLeft_%d", i);
      sprintf(title, "Tx break between right segment of station and left tip of the interstation segment: %d", i);
      muchOutStationTxBreakLeft[i - 1] = new TH1F(name, title, 100, -0.15, 0.15);
      muchOutStationTxBreakLeft[i - 1]->StatOverflows();

      sprintf(name, "muchOutStationTxBreakRight_%d", i);
      sprintf(title, "Tx break between left segment of station and right tip of the interstation segment: %d", i);
      muchOutStationTxBreakRight[i - 1] = new TH1F(name, title, 100, -0.15, 0.15);
      muchOutStationTxBreakRight[i - 1]->StatOverflows();

      sprintf(name, "muchOutStationTyBreakLeft_%d", i);
      sprintf(title, "Ty break between right segment of station and left tip of the interstation segment: %d", i);
      muchOutStationTyBreakLeft[i - 1] = new TH1F(name, title, 100, -0.15, 0.15);
      muchOutStationTyBreakLeft[i - 1]->StatOverflows();

      sprintf(name, "muchOutStationTyBreakRight_%d", i);
      sprintf(title, "Ty break between left segment of station and right tip of the interstation segment: %d", i);
      muchOutStationTyBreakRight[i - 1] = new TH1F(name, title, 100, -0.15, 0.15);
      muchOutStationTyBreakRight[i - 1]->StatOverflows();

      sprintf(name, "muchOutStationXDispByTriplet_%d", i);
      sprintf(title, "X dispersion of prediction by triplet angle for station: %d", i);
      muchOutStationXDispByTriplet[i - 1] = new TH1F(name, title, 100, -10.0, 10.0);
      muchOutStationXDispByTriplet[i - 1]->StatOverflows();

      sprintf(name, "muchOutStationYDispByTriplet_%d", i);
      sprintf(title, "Y dispersion of prediction by triplet angle for station: %d", i);
      muchOutStationYDispByTriplet[i - 1] = new TH1F(name, title, 100, -10.0, 10.0);
      muchOutStationYDispByTriplet[i - 1]->StatOverflows();

      sprintf(name, "muchOutStationXDispByVertex_%d", i);
      sprintf(title, "X dispersion of prediction by an angle to vertex for station: %d", i);
      muchOutStationXDispByVertex[i - 1] = new TH1F(name, title, 100, -10.0, 10.0);
      muchOutStationXDispByVertex[i - 1]->StatOverflows();

      sprintf(name, "muchOutStationYDispByVertex_%d", i);
      sprintf(title, "Y dispersion of prediction by an angle to vertex for station: %d", i);
      muchOutStationYDispByVertex[i - 1] = new TH1F(name, title, 100, -10.0, 10.0);
      muchOutStationYDispByVertex[i - 1]->StatOverflows();

      if (i < LXSTATIONS - 1)
      {
        sprintf(name, "muchSegmentTxBreakHisto_%d", i);
        sprintf(title, "Tx tangents breaks distribution for adjacent segments on station: %d", i);
        muchSegmentTxBreakHisto[i - 1] = new TH1F(name, title, 100, -.15, .15);
        muchSegmentTxBreakHisto[i - 1]->StatOverflows();

        sprintf(name, "muchSegmentTyBreakHisto_%d", i);
        sprintf(title, "Ty tangents breaks distribution for adjacent segments on station: %d", i);
        muchSegmentTyBreakHisto[i - 1] = new TH1F(name, title, 100, -.15, .15);
        muchSegmentTyBreakHisto[i - 1]->StatOverflows();
      }

      sprintf(name, "muchStationTxDispHisto_%d", i);
      sprintf(title, "Tx tangents dispersion for segments between stations: %d and %d", i - 1, i);
      muchStationTxDispHisto[i - 1] = new TH1F(name, title, 100, -.05, .05);
      muchStationTxDispHisto[i - 1]->StatOverflows();

      sprintf(name, "muchStationTyDispHisto_%d", i);
      sprintf(title, "Ty tangents dispersion for segments between stations: %d and %d", i - 1, i);
      muchStationTyDispHisto[i - 1] = new TH1F(name, title, 100, -.05, .05);
      muchStationTyDispHisto[i - 1]->StatOverflows();
    }

    if (i < LXSTATIONS - 1)
    {
      sprintf(name, "muchXTxCovHisto_%d", i);
      sprintf(title, "muchXTxCovHisto on %d", i);
      muchXTxCovHisto[i] = new TH2F(name, title, 100, -5.0, 5.0, 100, -0.15, 0.15);
      muchXTxCovHisto[i]->StatOverflows();

      sprintf(name, "muchYTyCovHisto_%d", i);
      sprintf(title, "muchYTyCovHisto on %d", i);
      muchYTyCovHisto[i] = new TH2F(name, title, 100, -5.0, 5.0, 100, -0.15, 0.15);
      muchYTyCovHisto[i]->StatOverflows();
    }
  }
}

static void SaveHisto(TH1* histo)
{
  DIR* dir = opendir("configuration");

  if (dir)
    closedir(dir);
  else
    mkdir("configuration", 0700);

  char name[256];
  sprintf(name, "configuration/%s.root", histo->GetName());
  TFile fh(name, "RECREATE");
  histo->Write();
  fh.Close();
  delete histo;
}

void LxTrackAnaSegments::Finish()
{
  for (Int_t i = 0; i < LXSTATIONS; ++i)
  {
    SaveHisto(muchInStationXDispLeft[i]);
    SaveHisto(muchInStationXDispRight[i]);
    SaveHisto(muchInStationYDispLeft[i]);
    SaveHisto(muchInStationYDispRight[i]);
    SaveHisto(muchInStationXDispRL[i]);
    SaveHisto(muchInStationYDispRL[i]);
    SaveHisto(muchInStationTxBreak[i]);
    SaveHisto(muchInStationTyBreak[i]);

    if (i > 0)
    {
      SaveHisto(muchLongSegmentTxHisto[i - 1]);
      SaveHisto(muchLongSegmentTyHisto[i - 1]);

      SaveHisto(muchClusterXDispHisto[i - 1]);
      SaveHisto(muchClusterYDispHisto[i - 1]);
      SaveHisto(muchClusterTxDispHisto[i - 1]);
      SaveHisto(muchClusterTyDispHisto[i - 1]);

      SaveHisto(muchOutStationTxBreakLeft[i - 1]);
      SaveHisto(muchOutStationTxBreakRight[i - 1]);
      SaveHisto(muchOutStationTyBreakLeft[i - 1]);
      SaveHisto(muchOutStationTyBreakRight[i - 1]);

      SaveHisto(muchOutStationXDispByTriplet[i - 1]);
      SaveHisto(muchOutStationYDispByTriplet[i - 1]);
      SaveHisto(muchOutStationXDispByVertex[i - 1]);
      SaveHisto(muchOutStationYDispByVertex[i - 1]);

      if (i < LXSTATIONS - 1)
      {
        SaveHisto(muchSegmentTxBreakHisto[i - 1]);
        SaveHisto(muchSegmentTyBreakHisto[i - 1]);
      }

      SaveHisto(muchStationTxDispHisto[i - 1]);
      SaveHisto(muchStationTyDispHisto[i - 1]);
    }

    if (i < LXSTATIONS - 1)
    {
      SaveHisto(muchXTxCovHisto[i]);
      SaveHisto(muchYTyCovHisto[i]);
    }
  }
}

void LxTrackAnaSegments::BuildStatistics()
{
  vector<LxSimpleTrack*>& tracks = owner.allTracks;

  for (vector<LxSimpleTrack*>::iterator i = tracks.begin(); i != tracks.end(); ++i)
  {
    LxSimpleTrack* track = *i;

    if (0 > track->motherId && (13 == track->pdgCode || -13 == track->pdgCode))
      StatForTrack(track);
  }
}

struct LxSimpleSegment
{
  LxSimplePoint source;
  LxSimplePoint end;
  Double_t tx;
  Double_t ty;

  LxSimpleSegment() : tx(0), ty(0) {}
  LxSimpleSegment(LxSimplePoint s, LxSimplePoint e) : source(s), end(e), tx((e.x - s.x) / (e.z - s.z)), ty((e.y - s.y) / (e.z - s.z)) {}
};

void LxTrackAnaSegments::StatForTrack(LxSimpleTrack* track)
{
  for (Int_t i = 0; i < LXSTATIONS; ++i)
  {
    for (Int_t j = 0; j < LXLAYERS; ++j)
    {
      if (track->muchPoints[i][j].size() < 1)
        return;
    }
  }

  LxSimplePoint p1;
  LxSimplePoint p2;
  LxSimplePoint p3;
  Double_t deltaZ;
  Double_t deltaZ2;
  Double_t tx;
  Double_t tx2;
  Double_t ty;
  Double_t ty2;
  Double_t stTx;
  Double_t stTy;
  Double_t stTxP;
  Double_t stTyP;

  for (Int_t i = 0; i < LXSTATIONS; ++i)
  {
    p1 = track->muchPoints[i][1].front();
    Double_t txEst = p1.x / p1.z;
    Double_t tyEst = p1.y / p1.z;

    p2 = track->muchPoints[i][0].front();
    deltaZ = p2.z - p1.z;
    Double_t xEst = p1.x + txEst * deltaZ;
    Double_t yEst = p1.y + tyEst * deltaZ;
    muchInStationXDispLeft[i]->Fill(p2.x - xEst);
    muchInStationYDispLeft[i]->Fill(p2.y - yEst);
    tx = (p2.x - p1.x) / deltaZ;
    ty = (p2.y - p1.y) / deltaZ;

    p2 = track->muchPoints[i][2].front();
    deltaZ = p2.z - p1.z;
    xEst = p1.x + txEst * deltaZ;
    yEst = p1.y + tyEst * deltaZ;
    muchInStationXDispRight[i]->Fill(p2.x - xEst);
    muchInStationYDispRight[i]->Fill(p2.y - yEst);

    tx2 = (p2.x - p1.x) / deltaZ;
    ty2 = (p2.y - p1.y) / deltaZ;

    muchInStationTxBreak[i]->Fill(tx2 - tx);
    muchInStationTyBreak[i]->Fill(ty2 - ty);

    stTxP = stTx;
    stTyP = stTy;
    p1 = track->muchPoints[i][0].front();
    deltaZ = p2.z - p1.z;
    stTx = (p2.x - p1.x) / deltaZ;
    stTy = (p2.y - p1.y) / deltaZ;
    muchInStationXDispRL[i]->Fill(p1.x - p2.x + tx2 * deltaZ);
    muchInStationYDispRL[i]->Fill(p1.y - p2.y + ty2 * deltaZ);

    if (i > 0)
    {
      p1 = track->muchPoints[i - 1][LXMIDDLE].front();
      p2 = track->muchPoints[i][LXMIDDLE].front();
      deltaZ = p2.z - p1.z;
      tx = (p2.x - p1.x) / deltaZ;
      muchLongSegmentTxHisto[i - 1]->Fill(tx - p2.x / p2.z);
      ty = (p2.y - p1.y) / deltaZ;
      muchLongSegmentTyHisto[i - 1]->Fill(ty - p2.y / p2.z);

      muchOutStationTxBreakRight[i - 1]->Fill(tx - stTx);
      muchOutStationTyBreakRight[i - 1]->Fill(ty - stTy);

      muchOutStationTxBreakLeft[i - 1]->Fill(tx - stTxP);
      muchOutStationTyBreakLeft[i - 1]->Fill(ty - stTyP);

      muchOutStationXDispByTriplet[i - 1]->Fill(p1.x - p2.x + stTx * deltaZ);
      muchOutStationYDispByTriplet[i - 1]->Fill(p1.y - p2.y + stTy * deltaZ);

      muchOutStationXDispByVertex[i - 1]->Fill(p1.x - p2.x + (p2.x / p2.z) * deltaZ);
      muchOutStationYDispByVertex[i - 1]->Fill(p1.y - p2.y + (p2.y / p2.z) * deltaZ);

      // Rather complex part for implementation: calculate the dispersion characteristics for segment clusters.
      Double_t maxXdisp = 0;
      Double_t maxYdisp = 0;
      Double_t maxTxdisp = 0;
      Double_t maxTydisp = 0;

      for (list<LxSimplePoint>::iterator l0 = track->muchPoints[i - 1][0].begin(); l0 != track->muchPoints[i - 1][0].end(); ++l0)
      {
        for (list<LxSimplePoint>::iterator l1 = track->muchPoints[i - 1][1].begin(); l1 != track->muchPoints[i - 1][1].end(); ++l1)
        {
          for (list<LxSimplePoint>::iterator l2 = track->muchPoints[i - 1][2].begin(); l2 != track->muchPoints[i - 1][2].end(); ++l2)
          {
            for (list<LxSimplePoint>::iterator r0 = track->muchPoints[i][0].begin(); r0 != track->muchPoints[i][0].end(); ++r0)
            {
              for (list<LxSimplePoint>::iterator r1 = track->muchPoints[i][1].begin(); r1 != track->muchPoints[i][1].end(); ++r1)
              {
                for (list<LxSimplePoint>::iterator r2 = track->muchPoints[i][2].begin(); r2 != track->muchPoints[i][2].end(); ++r2)
                {
                  LxSimplePoint lPoints[LXLAYERS] = { *l0, *l1, *l2 };
                  LxSimplePoint rPoints[LXLAYERS] = { *r0, *r1, *r2 };
                  LxSimpleSegment segments[LXLAYERS * LXLAYERS] = {};

                  for (Int_t j = 0; j < LXLAYERS; ++j)
                  {
                    for (Int_t k = 0; k < LXLAYERS; ++k)
                      segments[j * LXLAYERS + k] = LxSimpleSegment(rPoints[j], lPoints[k]);
                  }

                  for (Int_t j = 0; j < LXLAYERS * LXLAYERS - 1; ++j)
                  {
                    LxSimpleSegment s1 = segments[j];

                    for (Int_t k = j + 1; k < LXLAYERS * LXLAYERS; ++k)
                    {
                      LxSimpleSegment s2 = segments[k];
                      Double_t diffZ = s1.source.z - s2.source.z;
                      Double_t dtx = abs(s2.tx - s1.tx);
                      Double_t dty = abs(s2.ty - s1.ty);
                      Double_t dx = abs(s2.source.x + s2.tx * diffZ - s1.source.x);
                      Double_t dy = abs(s2.source.y + s2.ty * diffZ - s1.source.y);

                      if (maxXdisp < dx)
                        maxXdisp = dx;

                      if (maxYdisp < dy)
                        maxYdisp = dy;

                      if (maxTxdisp < dtx)
                        maxTxdisp = dtx;

                      if (maxTydisp < dty)
                        maxTydisp = dty;
                    }
                  }
                }
              }
            }
          }
        }
      }// for (list<LxSimplePoint>::iterator l0 = track->muchPoints[i - 1][0].begin(); l0 != track->muchPoints[i - 1][0].end(); ++l0)

      muchClusterXDispHisto[i - 1]->Fill(maxXdisp);
      muchClusterYDispHisto[i - 1]->Fill(maxYdisp);
      muchClusterTxDispHisto[i - 1]->Fill(maxTxdisp);
      muchClusterTyDispHisto[i - 1]->Fill(maxTydisp);

      if (i < LXSTATIONS - 1)
      {
        p1 = track->muchPoints[i - 1][LXMIDDLE].front();
        p2 = track->muchPoints[i][LXMIDDLE].front();
        deltaZ = p2.z - p1.z;
        tx = (p2.x - p1.x) / deltaZ;
        ty = (p2.y - p1.y) / deltaZ;
        p3 = track->muchPoints[i + 1][LXMIDDLE].front();
        deltaZ2 = p3.z - p2.z;
        tx2 = (p3.x - p2.x) / deltaZ2;
        muchSegmentTxBreakHisto[i - 1]->Fill(tx2 - tx);
        ty2 = (p3.y - p2.y) / deltaZ2;
        muchSegmentTyBreakHisto[i - 1]->Fill(ty2 - ty);
      }

      for (Int_t j = 0; j < LXLAYERS * LXLAYERS; ++j)
      {
        p1 = track->muchPoints[i - 1][j % LXLAYERS].front();
        p2 = track->muchPoints[i][j / LXLAYERS].front();
        deltaZ = p2.z - p1.z;
        tx = (p2.x - p1.x) / deltaZ;
        ty = (p2.y - p1.y) / deltaZ;

        for (Int_t k = j + 1; k < LXLAYERS * LXLAYERS; ++k)
        {
          p1 = track->muchPoints[i - 1][k % LXLAYERS].front();
          p2 = track->muchPoints[i][k / LXLAYERS].front();
          deltaZ = p2.z - p1.z;
          tx2 = (p2.x - p1.x) / deltaZ;
          muchStationTxDispHisto[i - 1]->Fill(tx2 - tx);
          ty2 = (p2.y - p1.y) / deltaZ;
          muchStationTyDispHisto[i - 1]->Fill(ty2 - ty);
        }
      }
    }

    if (i < LXSTATIONS - 1)
    {
      p1 = track->muchPoints[i + 1][LXMIDDLE].front();
      p2 = track->muchPoints[i][LXMIDDLE].front();
      deltaZ = p2.z - p1.z;
      Double_t deltaX = p2.x - p1.x - p1.tx * deltaZ;
      Double_t deltaY = p2.y - p1.y - p1.ty * deltaZ;
      Double_t deltaTx = p2.tx - p1.tx;
      Double_t deltaTy = p2.ty - p1.ty;
      muchXTxCovHisto[i]->Fill(deltaX, deltaTx);
      muchYTyCovHisto[i]->Fill(deltaY, deltaTy);
    }
  }
}
