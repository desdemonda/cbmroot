/**
 * \file CbmLitFitNode.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Data class for storage of fitted track parameters, transport matrix and chi-square on each detector station.
 */

#ifndef CBMLITFITNODE_H_
#define CBMLITFITNODE_H_

#include "data/CbmLitTrackParam.h"
#include <vector>
using std::vector;

/**
 * \class CbmLitFitNode
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 * \brief Data class for storage of fitted track parameters, transport matrix and chi-square on each detector station.
 */
class CbmLitFitNode
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitFitNode():
      fF(25, 0.),
      fPredictedParam(),
      fUpdatedParam(),
      fSmoothedParam(),
      fChiSqFiltered(0.),
      fChiSqSmoothed(0.) {
   }

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFitNode() {};

   /* Getters */
   const vector<litfloat>& GetF() const { return fF; }
   const CbmLitTrackParam* GetPredictedParam() const { return &fPredictedParam; }
   const CbmLitTrackParam* GetUpdatedParam() const { return &fUpdatedParam; }
   const CbmLitTrackParam* GetSmoothedParam() const { return &fSmoothedParam; }
   litfloat GetChiSqFiltered() const { return fChiSqFiltered; }
   litfloat GetChiSqSmoothed() const { return fChiSqSmoothed; }

   /* Setters */
   void SetF(const vector<litfloat>& F) { fF.assign(F.begin(), F.end()); }
   void SetPredictedParam(const CbmLitTrackParam* par) { fPredictedParam = *par;}
   void SetUpdatedParam(const CbmLitTrackParam* par) { fUpdatedParam = *par;}
   void SetSmoothedParam(const CbmLitTrackParam* par) { fSmoothedParam = *par;}
   void SetChiSqFiltered(litfloat chiSq) { fChiSqFiltered = chiSq; }
   void SetChiSqSmoothed(litfloat chiSq) { fChiSqSmoothed = chiSq; }

private:
   vector<litfloat> fF; // Transport matrix.

   CbmLitTrackParam fPredictedParam; // Predicted track parameters.
   CbmLitTrackParam fUpdatedParam; // Updated with KF track parameters.
   CbmLitTrackParam fSmoothedParam; // Smoothed track parameters.

   litfloat fChiSqFiltered; // Contribution to chi-square of updated track parameters and hit.
   litfloat fChiSqSmoothed; // Contribution to chi-square of smoothed track parameters and hit.
};

#endif /*CBMLITFITNODE_H_*/
