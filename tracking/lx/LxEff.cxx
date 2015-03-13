#include "LxEff.h"
#include "Lx.h"
#include <cmath>

//#define LXDRAW

#ifdef LXDRAW
#include "LxDraw.h"
//#define LXDRAWSAVE
//#define LXDRAWWAIT
#endif//LXDRAW

using namespace std;

LxEff::LxEff(LxFinder& owner) : finder(owner)
{
}

#ifdef CLUSTER_MODE
void LxEff::AnalyzeNotMatched(ofstream& out, list<LxPoint*> hits[LXSTATIONS][LXLAYERS])
{
  out << "AnalyzeNotMatched:" << endl;

  for (Int_t stNum = LXFIRSTSTATION; stNum < LXSTATIONS - 1; ++stNum)
  {
    out << "Segments #" << stNum << endl;
    list<LxPoint*> lHits[LXLAYERS] = hits[stNum];
    list<LxPoint*> rHits[LXLAYERS] = hits[stNum + 1];

    Double_t maxDx = 0;
    Double_t maxDy = 0;
    Double_t maxDtx = 0;
    Double_t maxDty = 0;
    Double_t minDx = 1000;
    Double_t minDy = 1000;
    Double_t minDtx = 1000;
    Double_t minDty = 1000;

    for (list<LxPoint*>::iterator l0 = lHits[0].begin(); l0 != lHits[0].end(); ++l0)
    {
      for (list<LxPoint*>::iterator l1 = lHits[1].begin(); l1 != lHits[1].end(); ++l1)
      {
        for (list<LxPoint*>::iterator l2 = lHits[2].begin(); l2 != lHits[2].end(); ++l2)
        {
          for (list<LxPoint*>::iterator r0 = rHits[0].begin(); r0 != rHits[0].end(); ++r0)
          {
            for (list<LxPoint*>::iterator r1 = rHits[1].begin(); r1 != rHits[1].end(); ++r1)
            {
              for (list<LxPoint*>::iterator r2 = rHits[2].begin(); r2 != rHits[2].end(); ++r2)
              {
                LxPoint* lPoints[LXLAYERS] = { *l0, *l1, *l2 };
                LxPoint* rPoints[LXLAYERS] = { *r0, *r1, *r2 };
                LxRay* rays[LXLAYERS * LXLAYERS] = {};

                for (Int_t i = 0; i < LXLAYERS; ++i)
                {
                  for (Int_t j = 0; j < LXLAYERS; ++j)
                    rays[j * LXLAYERS + i] = new LxRay(rPoints[j], lPoints[i], j * LXLAYERS + i);
                }

                for (Int_t i = 0; i < LXLAYERS * LXLAYERS; ++i)
                {
                  LxRay* ray1 = rays[i];
                  Double_t tx1 = ray1->tx;
                  Double_t ty1 = ray1->ty;
                  Double_t z1 = ray1->source->z;
                  Double_t x1 = ray1->source->x;
                  Double_t y1 = ray1->source->y;

                  if (minDx > ray1->source->dx)
                    minDx = ray1->source->dx;

                  if (minDy > ray1->source->dy)
                    minDy = ray1->source->dy;

                  if (minDtx > ray1->dtx)
                    minDtx = ray1->dtx;

                  if (minDty > ray1->dty)
                    minDty = ray1->dty;

                  for (Int_t j = i; j < LXLAYERS * LXLAYERS; ++j)
                  {
                    LxRay* ray2 = rays[j];
                    Double_t tx2 = ray2->tx;
                    Double_t ty2 = ray2->ty;
                    Double_t z2 = ray2->source->z;
                    Double_t diffZ = z1 - z2;
                    Double_t x2 = ray2->source->x + tx2 * diffZ;
                    Double_t y2 = ray2->source->y + ty2 * diffZ;

                    Double_t dtx = abs(tx2 - tx1);
                    Double_t dty = abs(ty2 - ty1);
                    Double_t dx = abs(x2 - x1);
                    Double_t dy = abs(y2 - y1);

                    if (maxDx < dx)
                      maxDx = dx;

                    if (maxDy < dy)
                      maxDy = dy;

                    if (maxDtx < dtx)
                      maxDtx = dtx;

                    if (maxDty < dty)
                      maxDty = dty;
                  }
                }

                for (Int_t i = 0; i < LXLAYERS * LXLAYERS; ++i)
                  delete rays[i];
              }
            }
          }
        }
      }
    }

    out << 4.0 * sqrt(minDx * minDx + finder.caSpace.stations[stNum + 1]->clusterXLimit2) - maxDx << " " <<
        4.0 * sqrt(minDy * minDy + finder.caSpace.stations[stNum + 1]->clusterYLimit2) - maxDy << " " <<
        4.0 * sqrt(minDtx * minDtx + finder.caSpace.stations[stNum + 1]->clusterTxLimit2) - maxDtx << " " <<
        4.0 * sqrt(minDty * minDty + finder.caSpace.stations[stNum + 1]->clusterTyLimit2) - maxDty << endl << endl;
  }
}

void LxEff::AnalyzeNotMatched2(ofstream& out, list<LxPoint*> hits[LXSTATIONS][LXLAYERS])
{
  out << "AnalyzeNotMatched2:" << endl;

  for (Int_t stNum = LXFIRSTSTATION; stNum < LXSTATIONS - 1; ++stNum)
  {
    out << "Segments #" << stNum << " has ";
    list<LxPoint*> lHits[LXLAYERS] = hits[stNum];
    list<LxPoint*> rHits[LXLAYERS] = hits[stNum + 1];

    Int_t maxOccupiedLevels = 0;
    Int_t maxNumPointsWithRays = 0;

    for (list<LxPoint*>::iterator l0 = lHits[0].begin(); l0 != lHits[0].end(); ++l0)
    {
      for (list<LxPoint*>::iterator l1 = lHits[1].begin(); l1 != lHits[1].end(); ++l1)
      {
        for (list<LxPoint*>::iterator l2 = lHits[2].begin(); l2 != lHits[2].end(); ++l2)
        {
          for (list<LxPoint*>::iterator r0 = rHits[0].begin(); r0 != rHits[0].end(); ++r0)
          {
            for (list<LxPoint*>::iterator r1 = rHits[1].begin(); r1 != rHits[1].end(); ++r1)
            {
              for (list<LxPoint*>::iterator r2 = rHits[2].begin(); r2 != rHits[2].end(); ++r2)
              {
                LxPoint* lPoints[LXLAYERS] = { *l0, *l1, *l2 };
                LxPoint* rPoints[LXLAYERS] = { *r0, *r1, *r2 };
                Int_t occupiedLevels = 0;
                Int_t numPointsWithRays = 0;

                for (Int_t i = 0; i < LXLAYERS; ++i)
                {
                  LxPoint* rPoint = rPoints[i];

                  if (!rPoint->rays.empty())
                    ++numPointsWithRays;

                  for (Int_t j = 0; j < LXLAYERS; ++j)
                  {
                    LxPoint* lPoint = lPoints[j];

                    for (list<LxRay*>::iterator k = rPoint->rays.begin(); k != rPoint->rays.end(); ++k)
                    {
                      LxRay* ray = *k;

                      if (ray->end == lPoint)
                      {
                        ++occupiedLevels;
                        break;
                      }
                    }
                  }
                }

                if (occupiedLevels > maxOccupiedLevels)
                  maxOccupiedLevels = occupiedLevels;

                if (numPointsWithRays > maxNumPointsWithRays)
                  maxNumPointsWithRays = numPointsWithRays;
              }
            }
          }
        }
      }
    }// for (list<LxPoint*>::iterator l0 = lHits[0].begin(); l0 != lHits[0].end(); ++l0)

    out << maxOccupiedLevels << " of occupied levels and " << maxNumPointsWithRays << " points with rays" << endl << endl;
  }
}

void LxEff::DumpNotMatchedMC(list<LxPoint*> hits[LXSTATIONS][LXLAYERS])
{
  AnalyzeNotMatched(finder.incomplete_events, hits);
  AnalyzeNotMatched2(finder.incomplete_events, hits);
  finder.incomplete_events << "Dumping not matched MC track" << endl;

  for (Int_t i = 0; i < LXSTATIONS; ++i)
  {
    finder.incomplete_events << "S" << i << " ";

    for (Int_t j = 0; j < LXLAYERS; ++j)
    {
      finder.incomplete_events << "L" << j << " ";

      for (list<LxPoint*>::iterator k = hits[i][j].begin(); k != hits[i][j].end(); ++k)
      {
        LxPoint* point = *k;
        finder.incomplete_events << "[";

        if (point->track)
          finder.incomplete_events << "t";
        else
        {
          if (point->leftClusteredRay)
            finder.incomplete_events << "r";

          finder.incomplete_events << "/";

          if (point->rightClusteredRay)
            finder.incomplete_events << "r";

        }

        finder.incomplete_events << "] ";
      }

      finder.incomplete_events << " ";
    }

    finder.incomplete_events << " ";
  }

  finder.incomplete_events << endl << endl;
}
#endif//CLUSTER_MODE

void LxEff::CalcRecoEff(bool joinExt)
{
  Double_t result = 0;
  static Int_t signalRecoTracks = 0;
  static Int_t signalMCTracks = 0;
  static Int_t bgrRecoTracks = 0;
  static Int_t bgrMCTracks = 0;
  static Int_t stsNotFound = 0;
  static Int_t stsPresentButWronglyLinked = 0;
  static Int_t stsAbsentButLinked = 0;

  bool hasJPsiMuPlus = false;
  bool hasJPsiMuMinus = false;

  for (vector<LxMCTrack>::iterator i = finder.MCTracks.begin(); i != finder.MCTracks.end(); ++i)
  {
    LxMCTrack& mcTrack = *i;
    Int_t pdgCode = mcTrack.pdg;
    map<LxTrack*, Int_t> recoTracks;// Mapped value is the number of common points in MC and reconstructed tracks.
    memset(mcTrack.hitsOnStations, 0, sizeof(mcTrack.hitsOnStations));
    bool isSignal = true;
    list<LxPoint*> mcTrackHits[LXSTATIONS][LXLAYERS];

    for (vector<LxMCPoint*>::iterator j = mcTrack.Points.begin(); j != mcTrack.Points.end(); ++j)
    {
      LxMCPoint* pMCPoint = *j;

      for (list<LxPoint*>::iterator k = pMCPoint->lxPoints.begin(); k != pMCPoint->lxPoints.end(); ++k)
      {
        LxPoint* point = *k;
        mcTrack.hitsOnStations[point->layer->station->stationNumber][point->layer->layerNumber] = true;
        mcTrackHits[point->layer->station->stationNumber][point->layer->layerNumber].push_back(point);
        LxTrack* track = point->track;

        if (0 == track)
          continue;
        
        if (track->matched)
          continue;

        if (track->clone)
          continue;

        map<LxTrack*, int>::iterator l = recoTracks.find(track);
        
        if (l != recoTracks.end())
          ++(l->second);
        else
          recoTracks[track] = 1;
      }
    }
    
    bool enoughHits = true;
    mcTrack.stationsWithHits = 0;
    mcTrack.layersWithHits = 0;

    for (Int_t j = 0; j < LXSTATIONS; ++j)
    {
      Int_t hitsOnSt = 0;

      for (Int_t k = 0; k < LXLAYERS; ++k)
      {
        if (mcTrack.hitsOnStations[j][k])
        {
          ++hitsOnSt;
          ++mcTrack.layersWithHits;
        }
      }

      if (hitsOnSt < 3)
        enoughHits = false;
      else
        ++mcTrack.stationsWithHits;
    }

    if (enoughHits && mcTrack.mother_ID < 0)
    {
      if (-13 == pdgCode)
        hasJPsiMuPlus = true;
      else if (13 == pdgCode)
        hasJPsiMuMinus = true;
    }

    Double_t pt2 = mcTrack.px * mcTrack.px + mcTrack.py * mcTrack.py;

    if (!enoughHits || mcTrack.mother_ID >= 0 || (pdgCode != 13 && pdgCode != -13) || mcTrack.p < 3.0 || pt2 < 1.0)
      isSignal = false;
    else
      ++signalMCTracks;

    LxTrack* matchTrack = 0;
    Int_t matchedPoints = 0;

    for (map<LxTrack*, Int_t>::iterator j = recoTracks.begin(); j != recoTracks.end(); ++j)
    {
      if (0 == matchTrack || matchedPoints < j->second + j->first->restoredPoints)
      {
        matchTrack = j->first;
        matchedPoints = j->second + j->first->restoredPoints;
      }
    }

    if (0 == matchTrack)
    {
#ifdef MAKE_EFF_CALC
      //finder.incomplete_events << finder.eventNumber << " matched points: " << matchedPoints << " ; MC points: " << mcTrack.Points.size() << endl;
      //finder.caSpace.CheckArray(finder.incomplete_events, mcTrack);
#endif//MAKE_EFF_CALC
      //cout << "MC track does not have common point with a reconstructed" << endl;

#ifdef CLUSTER_MODE
      if (isSignal)
        DumpNotMatchedMC(mcTrackHits);
#endif//CLUSTER_MODE

      if (isSignal)
        LxFinder::effByMomentumProfile->Fill(mcTrack.p, 0.0);

      continue;
    }

    Int_t numberOfMuchMCPoints = LXLAYERS * (LXSTATIONS - LXFIRSTSTATION);

    if (matchedPoints < 0.7 * numberOfMuchMCPoints)
    {
#ifdef MAKE_EFF_CALC
      //finder.incomplete_events << finder.eventNumber << " matched points: " << matchedPoints << " ; MC points: " << mcTrack.Points.size() << endl;
      //finder.caSpace.CheckArray(finder.incomplete_events, mcTrack);
#endif//MAKE_EFF_CALC
      //cout << "MC track have only " << matchedPoints << " common points with reconstructed" << endl;

#ifdef CLUSTER_MODE
      if (isSignal)
        DumpNotMatchedMC(mcTrackHits);
#endif//CLUSTER_MODE

      if (isSignal)
        LxFinder::effByMomentumProfile->Fill(mcTrack.p, 0.0);

      continue;
    }

    matchTrack->mcTrack = &mcTrack;
    matchTrack->matched = true;

    {
      LxRay* ray = matchTrack->rays[0];
      LxPoint* point = ray->end;
      Double_t diffZMag = 40.0 - point->z;
      Double_t magX = point->x + ray->tx * diffZMag;
      Double_t magTx = magX / 40.0;
      Double_t muchMom = abs(0.8029 / (ray->tx - magTx));
      Double_t momErr = 100 * (muchMom - mcTrack.p) / mcTrack.p;
      cout << "**********************************************************************" << endl;
      cout << "**********************************************************************" << endl;
      cout << "*** Momentum determination relative error: " << momErr << "%" << endl;
      cout << "**********************************************************************" << endl;
      cout << "**********************************************************************" << endl;

      if (isSignal)
        LxFinder::muchMomErrSig->Fill(momErr);
      else
        LxFinder::muchMomErrBgr->Fill(momErr);
    }

    if (!isSignal)
      continue;

    {
      LxRay* ray = matchTrack->rays[0];
      LxPoint* point = ray->end;

      if (-13 == pdgCode)
        LxFinder::muPlusDtxHisto->Fill(ray->tx - point->x / point->z);
      else
        LxFinder::muMinusDtxHisto->Fill(ray->tx - point->x / point->z);
    }

#ifdef CALC_LINK_WITH_STS_EFF
    if (0 != matchTrack && matchedPoints >= 0.7 * numberOfMuchMCPoints)
      matchTrack->mcTracks.push_back(&mcTrack);
#endif//CALC_LINK_WITH_STS_EFF

    if (!joinExt)
    {
      ++signalRecoTracks;
      LxFinder::effByMomentumProfile->Fill(mcTrack.p, 100.0);
    }
    else
    {
      if (0 == matchTrack->externalTrack)
      {
        if (0 == mcTrack.externalTrack)
        {
          ++signalRecoTracks;
          LxFinder::effByMomentumProfile->Fill(mcTrack.p, 100.0);
        }
        else
          ++stsNotFound;
      }
      else if (matchTrack->externalTrack->track == mcTrack.externalTrack)
      {
        ++signalRecoTracks;
        LxFinder::effByMomentumProfile->Fill(mcTrack.p, 100.0);
      }
      else
      {
        if (0 == mcTrack.externalTrack)
          ++stsAbsentButLinked;
        else
          ++stsPresentButWronglyLinked;
      }
    }
  }
  
  result = 100 * signalRecoTracks;
  result /= signalMCTracks;
  
#ifdef MAKE_EFF_CALC
  //if (foundNow != mcNow)
    //finder.incomplete_events << finder.eventNumber << endl;
#endif//MAKE_EFF_CALC

  cout << "LxEff::CalcRecoEff signal efficiency: " << result << "( " << signalRecoTracks << " / " << signalMCTracks << " )" << endl;
  cout << "LxEff::CalcRecoEff STS link must be but not found: " << stsNotFound << endl;
  cout << "LxEff::CalcRecoEff STS link is present but wrongly linked: " << stsPresentButWronglyLinked << endl;
  cout << "LxEff::CalcRecoEff STS link is absent but linked: " << stsAbsentButLinked << endl;

  // The code below is mostly dedicated to counting ghosts.
  static Int_t recoTracks = 0;
  static Int_t matchedMCTracks = 0;
  static Int_t completeleyUnmatched = 0;
  static Int_t notEnoughPoints = 0;
  static Int_t notEnoughStations = 0;
  static Int_t incompleteStations[LXSTATIONS] = {};
  static Int_t doesntHaveStsPart = 0;
  static Int_t doesntHaveStsPart1 = 0;
  static Int_t doesntHaveStsPart2 = 0;
  static Int_t unmatchedButHaveStsPart = 0;
  static Int_t jPsiHypotheses = 0;
  static Int_t jPsiTrueHypotheses = 0;
  static Int_t jPsiInAcceptance = 0;

  static Int_t jPsiTrueHypotheses2 = 0;
  static Int_t jPsiFalseHypotheses1 = 0;
  static Int_t jPsiFalseHypotheses2 = 0;

  static Int_t jPsiDominatedIn3 = 0;

  bool muPlusFound = false;
  bool muMinusFound = false;
  list<LxTrack*> muPlusTrackCandidates;
  list<LxTrack*> muMinusTrackCandidates;
  Int_t numberOfTracks = 0;
  LxTrack* jPsiMuPlus = 0;
  LxTrack* jPsiMuMinus = 0;

  for (list<LxTrack*>::iterator i = finder.caSpace.tracks.begin(); i != finder.caSpace.tracks.end(); ++i)
  {
    //++recoTracks;
    LxTrack* recoTrack = *i;

    if (recoTrack->clone)
      continue;

    ++numberOfTracks;

    {
      LxRay* ray = recoTrack->rays[0];
      LxPoint* point = ray->end;
      Double_t particleCharge = ray->tx - point->x / point->z;

      if (particleCharge > 0)
      {
        muPlusFound = true;
        muPlusTrackCandidates.push_back(recoTrack);
      }
      else if (particleCharge < 0)
      {
        muMinusFound = true;
        muMinusTrackCandidates.push_back(recoTrack);
      }

      if (0 != recoTrack->mcTrack && 0 > recoTrack->mcTrack->mother_ID)
      {
        if (-13 == recoTrack->mcTrack->pdg)
          jPsiMuPlus = recoTrack;
        else if (13 == recoTrack->mcTrack->pdg)
          jPsiMuMinus = recoTrack;
      }
    }

    map<LxMCTrack*, Int_t> mcTracks;// Mapped value is the number of common points in MC and reconstructed tracks.

#ifdef CLUSTER_MODE
    for (Int_t ii = 0; ii < recoTrack->length; ++ii)
    {
      LxRay* ray = recoTrack->rays[ii];

      for (list<LxPoint*>::iterator j = ray->clusterPoints.begin(); j != ray->clusterPoints.end(); ++j)
      {
        LxPoint* recoPoint = *j;
#else//CLUSTER_MODE
    for (Int_t j = 0; j < (LXSTATIONS - LXFIRSTSTATION) * LXLAYERS; ++j)
    {
      LxPoint* recoPoint = recoTrack->points[j];
#endif//CLUSTER_MODE

      if (0 == recoPoint)
        continue;

      for (list<LxMCPoint*>::iterator k = recoPoint->mcPoints.begin(); k != recoPoint->mcPoints.end(); ++k)
      {
        LxMCPoint* mcPoint = *k;
        LxMCTrack* mcTrack = mcPoint->track;

        if (0 == mcTrack)
          continue;

        map<LxMCTrack*, Int_t>::iterator mcIter = mcTracks.find(mcTrack);

        if (mcIter == mcTracks.end())
          mcTracks[mcTrack] = 1;
        else
          ++(mcIter->second);
      }
#ifdef CLUSTER_MODE
    }
#endif//CLUSTER_MODE
    }// for (Int_t j = 0; j < (LXSTATIONS - LXFIRSTSTATION) * LXLAYERS; ++j)

    LxMCTrack* bestMatch = 0;
    Int_t matchedPoints = 0;

    for (map<LxMCTrack*, Int_t>::iterator j = mcTracks.begin(); j != mcTracks.end(); ++j)
    {
      if (0 == bestMatch || j->second > matchedPoints)
      {
        bestMatch = j->first;
        matchedPoints = j->second;
      }
    }

    if (0 == bestMatch)
    {
      ++completeleyUnmatched;
      cout << "This reconstructed track is not intersected with any MC track in any point" << endl;

      if (!joinExt || 0 != recoTrack->externalTrack)
        ++recoTracks;

      continue;
    }

    ++recoTracks;

    bool unmatched = false;

    if (matchedPoints < 0.7 * LXSTATIONS * LXLAYERS)
    {
      ++notEnoughPoints;
      cout << "This reconstructed track match with an MC track in only " << matchedPoints << "points" << endl;
      unmatched = true;
    }

    if (bestMatch->stationsWithHits < LXSTATIONS)
    {
      ++notEnoughStations;

#ifdef LXDRAW
      LxDraw drawer;
      drawer.ClearView();
      drawer.DrawInputHits();
      drawer.DrawMCTracks();
      drawer.DrawRays();

#ifdef LXDRAWSAVE
      char drawSaveName[64];
      sprintf(drawSaveName, "MC_Rays_%d_", finder.eventNumber);
      drawer.SaveCanvas(drawSaveName);
#endif//LXDRAWSAVE

#ifdef LXDRAWWAIT
      gPad->WaitPrimitive();
#endif//LXDRAWWAIT
#endif//LXDRAW

      for (Int_t j = 0; j < LXSTATIONS; ++j)
      {
        Int_t hitsOnStation = 0;

        for (Int_t k = 0; k < LXLAYERS; ++k)
        {
          if (bestMatch->hitsOnStations[j][k])
            ++hitsOnStation;
        }

        if (hitsOnStation < 3)
          ++incompleteStations[j];
      }

      cout << "The matched MC track has hits not on all stations. Only on: " << bestMatch->stationsWithHits << endl;
      unmatched = true;
    }

    {
      LxRay* ray0 = recoTrack->rays[0];
      LxPoint* point01 = ray0->end;

      if (0 != bestMatch && bestMatch->mother_ID < 0 && (-13 == bestMatch->pdg || 13 == bestMatch->pdg))
      {
        LxFinder::signalXAtZ0->Fill(point01->x - point01->z * ray0->tx);
        LxFinder::signalYAtZ0->Fill(point01->y - point01->z * ray0->ty);
      }
      else
      {
        LxFinder::bgrXAtZ0->Fill(point01->x - point01->z * ray0->tx);
        LxFinder::bgrYAtZ0->Fill(point01->y - point01->z * ray0->ty);
      }
    }

    if (0 == bestMatch->externalTrack)
    {
      ++doesntHaveStsPart;

      if (unmatched)
        ++doesntHaveStsPart1;
      else
        ++doesntHaveStsPart2;
    }

    if (!unmatched)
    {
      if (!joinExt)
        ++matchedMCTracks;
      else
      {
        if (0 == recoTrack->externalTrack)
        {
          if (0 == bestMatch->externalTrack)
            ++matchedMCTracks;
          else
            cout << "LxEff::CalcRecoEff: STS track not found" << endl;
        }
        else
        {
          if (bestMatch->externalTrack == recoTrack->externalTrack->track)
            ++matchedMCTracks;
          else
            cout << "LxEff::CalcRecoEff: STS track determined wrongly" << endl;
        }
      }
    }
    else if (0 != bestMatch->externalTrack)
      ++unmatchedButHaveStsPart;
  }// for (list<LxTrack*>::iterator i = finder.caSpace.tracks.begin(); i != finder.caSpace.tracks.end(); ++i)

  LxFinder::numberOfTracks->Fill(numberOfTracks);

  if (0 != jPsiMuPlus && 0 != jPsiMuMinus)
  {
    Double_t deltaX = jPsiMuPlus->rays[0]->end->x - jPsiMuMinus->rays[0]->end->x;
    Double_t deltaY = jPsiMuPlus->rays[0]->end->y - jPsiMuMinus->rays[0]->end->y;
    LxFinder::signalInterTracksDistanceOn1st->Fill(sqrt(deltaX * deltaX + deltaY * deltaY));
  }

  for (list<LxTrack*>::iterator i = finder.caSpace.tracks.begin(); i != finder.caSpace.tracks.end(); ++i)
  {
    LxTrack* track1 = *i;

    if (track1->clone)
      continue;

    list<LxTrack*>::iterator j = i;
    ++j;

    for (; j != finder.caSpace.tracks.end(); ++j)
    {
      LxTrack* track2 = *j;

      if (track2->clone)
        continue;

      if (0 != track1->mcTrack && 0 != track2->mcTrack && 0 > track1->mcTrack->mother_ID && 0 > track2->mcTrack->mother_ID &&
          ((-13 == track1->mcTrack->pdg && 13 == track2->mcTrack->pdg) || (13 == track1->mcTrack->pdg && -13 == track2->mcTrack->pdg)))
      {
        continue;
      }

      Double_t deltaX = track1->rays[0]->end->x - track2->rays[0]->end->x;
      Double_t deltaY = track1->rays[0]->end->y - track2->rays[0]->end->y;
      LxFinder::bgrInterTracksDistanceOn1st->Fill(sqrt(deltaX * deltaX + deltaY * deltaY));
    }
  }

  for (list<LxTrack*>::iterator i = muPlusTrackCandidates.begin(); i != muPlusTrackCandidates.end(); ++i)
  {
    LxTrack* track1 = *i;

    for (list<LxTrack*>::iterator j = muMinusTrackCandidates.begin(); j != muMinusTrackCandidates.end(); ++j)
    {
      LxTrack* track2 = *j;

      if (0 != track1->mcTrack && 0 != track2->mcTrack && 0 > track1->mcTrack->mother_ID && 0 > track2->mcTrack->mother_ID &&
          ((-13 == track1->mcTrack->pdg && 13 == track2->mcTrack->pdg) || (13 == track1->mcTrack->pdg && -13 == track2->mcTrack->pdg)))
      {
        continue;
      }

      Double_t deltaX = track1->rays[0]->end->x - track2->rays[0]->end->x;
      Double_t deltaY = track1->rays[0]->end->y - track2->rays[0]->end->y;
      LxFinder::bgrInterTracksDistanceOn1stSigns->Fill(sqrt(deltaX * deltaX + deltaY * deltaY));
    }
  }

  bool hasPairWithDifferentSigns = false;

#define CHECK_PAIR_WITH_DIFFERENT_SIGNS

#ifdef CHECK_PAIR_WITH_DIFFERENT_SIGNS
  if (muPlusFound && muMinusFound)
  {
    for (list<LxTrack*>::iterator i = muPlusTrackCandidates.begin(); i != muPlusTrackCandidates.end(); ++i)
    {
      LxTrack* track1 = *i;
      LxRay* ray1 = track1->rays[0];
      LxPoint* point1 = ray1->end;
      Double_t x1 = point1->x;
      Double_t y1 = point1->y;
      //Double_t z1 = point1->z;
      //Double_t ax = ray1->tx;
      //Double_t ay = ray1->ty;
      //Double_t az = 1.0;

      for (list<LxTrack*>::iterator j = muMinusTrackCandidates.begin(); j != muMinusTrackCandidates.end(); ++j)
      {
        LxTrack* track2 = *j;
        LxRay* ray2 = track2->rays[0];
        LxPoint* point2 = ray2->end;
        Double_t x2 = point2->x;
        Double_t y2 = point2->y;
        //Double_t z2 = point2->z;
        //Double_t bx = ray2->tx;
        //Double_t by = ray2->ty;
        //Double_t bz = 1.0;

        //Double_t A = ay * bz - az * by;
        //Double_t B = az * bx - ax * bz;
        //Double_t C = ax * by - ay * bx;
        //Double_t P = A * x2 + B * y2 + C * z2;

        //Double_t ABCLen = sqrt(A * A + B * B + C * C);

        //Double_t cosA = A / ABCLen;
        //Double_t cosB = B / ABCLen;
        //Double_t cosC = C / ABCLen;
        //Double_t p = P / ABCLen;
        //Double_t interTrackDistance = abs(cosA * x1 + cosB * y1 + cosC * z1 - p);
        Double_t interTrackDistanceOn1stSq = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);

        if (interTrackDistanceOn1stSq > 50.0 * 50.0)
        {
          track1->triggering = true;
          track2->triggering = true;
          hasPairWithDifferentSigns = true;
        }

        /*if (0 != track1->mcTrack && 0 != track2->mcTrack && track1->mcTrack->mother_ID < 0 && track2->mcTrack->mother_ID < 0 &&
            ((-13 == track1->mcTrack->pdg && 13 == track2->mcTrack->pdg) || (13 == track1->mcTrack->pdg && -13 == track2->mcTrack->pdg)))
        {
          LxFinder::signalInterTracksDistance->Fill(interTrackDistance);
          LxFinder::signalTanSigns->Fill(tansProduct < 0 ? -1.0 : 1.0);
          LxFinder::signalCoordSigns->Fill(coordsProduct < 0 ? -1.0 : 1.0);
        }
        else
        {
          LxFinder::bgrInterTracksDistance->Fill(interTrackDistance);
          LxFinder::bgrTanSigns->Fill(tansProduct < 0 ? -1.0 : 1.0);
          LxFinder::bgrCoordSigns->Fill(coordsProduct < 0 ? -1.0 : 1.0);
        }*/
      }
    }

    if (hasPairWithDifferentSigns)
      ++jPsiHypotheses;
  }

  if (hasJPsiMuPlus && hasJPsiMuMinus)
  {
    ++jPsiInAcceptance;

    if (muPlusFound && muMinusFound && hasPairWithDifferentSigns)
    {
      ++jPsiTrueHypotheses;

      if (0 != jPsiMuPlus && 0 != jPsiMuMinus)
      {
        ++jPsiTrueHypotheses2;

        if (finder.caSpace.tracks.size() == 3)
        {
          Double_t jPsiDistanceOn1StSq = (jPsiMuPlus->rays[0]->end->x - jPsiMuMinus->rays[0]->end->x) *
              (jPsiMuPlus->rays[0]->end->x - jPsiMuMinus->rays[0]->end->x) +
              (jPsiMuPlus->rays[0]->end->y - jPsiMuMinus->rays[0]->end->y) *
              (jPsiMuPlus->rays[0]->end->y - jPsiMuMinus->rays[0]->end->y);
          bool hasBetter = false;

          for (list<LxTrack*>::iterator i = muPlusTrackCandidates.begin(); i != muPlusTrackCandidates.end(); ++i)
          {
            LxTrack* track1 = *i;
            LxRay* ray1 = track1->rays[0];
            LxPoint* point1 = ray1->end;
            Double_t x1 = point1->x;
            Double_t y1 = point1->y;

            for (list<LxTrack*>::iterator j = muMinusTrackCandidates.begin(); j != muMinusTrackCandidates.end(); ++j)
            {
              LxTrack* track2 = *j;
              LxRay* ray2 = track2->rays[0];
              LxPoint* point2 = ray2->end;
              Double_t x2 = point2->x;
              Double_t y2 = point2->y;

              Double_t interTrackDistanceOn1stSq = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);

              if (interTrackDistanceOn1stSq > jPsiDistanceOn1StSq)
                hasBetter = true;
            }
          }

          if (hasBetter)
            ++jPsiDominatedIn3;
        }
      }
    }
  }
  else if (hasJPsiMuPlus || hasJPsiMuMinus)
  {
    if (muPlusFound && muMinusFound && hasPairWithDifferentSigns)
      ++jPsiFalseHypotheses1;
  }
  else
  {
    if (muPlusFound && muMinusFound && hasPairWithDifferentSigns)
      ++jPsiFalseHypotheses2;
  }
#else//CHECK_PAIR_WITH_DIFFERENT_SIGNS
  if (finder.caSpace.tracks.size() == 2)
  {
    list<LxTrack*>::iterator i = finder.caSpace.tracks.begin();
    LxTrack* track1 = *i;
    LxRay* ray1 = track1->rays[0];
    LxPoint* point1 = ray1->end;
    Double_t x1 = point1->x;
    Double_t y1 = point1->y;
    ++i;
    LxTrack* track2 = *i;
    LxRay* ray2 = track2->rays[0];
    LxPoint* point2 = ray2->end;
    Double_t x2 = point2->x;
    Double_t y2 = point2->y;

    if (x1 * x2 + y1 * y2 < 0)
      hasPairWithDifferentSigns = true;
  }

  if (hasPairWithDifferentSigns)
    ++jPsiHypotheses;

  if (hasJPsiMuPlus && hasJPsiMuMinus)
  {
    ++jPsiInAcceptance;

    if (hasPairWithDifferentSigns)
      ++jPsiTrueHypotheses;
  }
#endif//CHECK_PAIR_WITH_DIFFERENT_SIGNS

  result = 100 * matchedMCTracks;
  result /= recoTracks;
  cout << "LxEff::CalcRecoEff matching reconstucted efficiency: " << result << "( " << matchedMCTracks << " / " << recoTracks << " )" << endl;
  cout << "LxEff::CalcRecoEff 0-matched: " << completeleyUnmatched << ", not enough points: " << notEnoughPoints <<
      ", not enough stations: " << notEnoughStations << " : ";

  for (Int_t i = 0; i < LXSTATIONS; ++i)
    cout << "[" << incompleteStations[i] << "]";

  cout << endl;
  cout << "LxEff::CalcRecoEff doesn't have an STS part: " << doesntHaveStsPart <<
      ", unmatched: " << doesntHaveStsPart1 << ", matched: " << doesntHaveStsPart2 << endl;
  cout << "LxEff::CalcRecoEff unmatched but have an STS part: " << unmatchedButHaveStsPart << endl;
  cout << "LxEff::CalcRecoEff J/Psi hypotheses: " << jPsiHypotheses << endl;
  result = 100 * jPsiTrueHypotheses;
  result /= jPsiInAcceptance;
  cout << "LxEff::CalcRecoEff J/Psi true hypotheses: " << result << "( " << jPsiTrueHypotheses << " / " << jPsiInAcceptance << " )" << endl;
  cout << "LxEff::CalcRecoEff J/Psi true hypotheses 2: " << jPsiTrueHypotheses2 << endl;
  cout << "LxEff::CalcRecoEff J/Psi false hypotheses 1: " << jPsiFalseHypotheses1 << endl;
  cout << "LxEff::CalcRecoEff J/Psi false hypotheses 2: " << jPsiFalseHypotheses2 << endl;
  cout << "LxEff::CalcRecoEff J/Psi false hypotheses all: " << jPsiFalseHypotheses1 + jPsiFalseHypotheses2 << endl;
  cout << "LxEff::CalcRecoEff J/Psi pair dominated by third track: " << jPsiDominatedIn3 << endl;
  finder.falseJPsiTriggerings = jPsiFalseHypotheses1 + jPsiFalseHypotheses2;
}
