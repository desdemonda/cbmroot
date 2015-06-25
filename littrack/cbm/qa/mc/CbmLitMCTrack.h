/**
 * \file CbmLitMCTrack.h
 * \brief Monte-Carlo track.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
#ifndef CBMLITMCTRACK_H_
#define CBMLITMCTRACK_H_

#include "CbmLitMCPoint.h"
#include "CbmDetectorList.h"
#include <cassert>
#include <map>
#include <vector>
#include <set>

using std::vector;
using std::map;
using std::set;
using std::stringstream;
using std::string;
using std::endl;

/**
 * \class CbmLitMCTrack
 * \brief Monte-Carlo track.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 **/
class CbmLitMCTrack
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitMCTrack():
  fPoints(),
    fStationPoints(),
    fStationIds(),
    fMaxConsecutivePoints(),
    fNofRichHits(0),
    fRingAaxis(0.),
    fRingBaxis(0.),
    fRingCenterX(0.),
    fRingCenterY(0.) {
      // Initialize all maps
      fPoints[kMVD];
      fPoints[kSTS];
      fPoints[kTRD];
      fPoints[kMUCH];
      fPoints[kTOF];
      fPoints[kRICH];
      fStationPoints[kMVD];
      fStationPoints[kSTS];
      fStationPoints[kTRD];
      fStationPoints[kMUCH];
      fStationPoints[kTOF];
      fStationIds[kMVD];
      fStationIds[kSTS];
      fStationIds[kTRD];
      fStationIds[kMUCH];
      fStationIds[kTOF];
      fMaxConsecutivePoints[kSTS] = -1;
   }

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitMCTrack() {};

   /**
    * \brief Add point to track.
    * \param[in] detId Detector identificator.
    * \param[in] point MC point to be added.
    */
   void AddPoint(DetectorId detId, const CbmLitMCPoint& point) {
      fPoints[detId].push_back(point);
      if (detId != kRICH) {
    	  fStationPoints[detId][point.GetStationId()].push_back(point);
    	  fStationIds[detId].insert(point.GetStationId());
      }
   }

   /**
    * \brief Return vector of MC point for specified detector id.
    * \param[in] detId Detector identificator.
    * \return Array of MC points.
    */
   const vector<CbmLitMCPoint>& GetPoints(DetectorId detId) const {
      return fPoints.find(detId)->second;
   }

   /**
    * \brief Return MC point for specified detector id and point index.
    * \param[in] detId Detector identificator.
    * \param[in] index Index of MC point.
    * \return MC point.
    */
   const CbmLitMCPoint& GetPoint(DetectorId detId, Int_t index) const {
      assert(GetNofPoints(detId) != 0);
      return fPoints.find(detId)->second[index];
   }

   /**
    * \brief Return number of MC points for specified detector id.
    * \param[in] detId Detector identificator.
    * \return Number of MC points.
    */
   UInt_t GetNofPoints(DetectorId detId) const {
      return fPoints.find(detId)->second.size();
   }

   /**
    * \brief Return number of MC points in different stations for specified detector id.
    * \param[in] detId Detector identificator.
    * \return Number of MC points.
    */
   UInt_t GetNofPointsInDifferentStations(DetectorId detId) const {
	  assert(detId != kRICH);
      return fStationPoints.find(detId)->second.size();
   }

   /**
    * \brief Return number of consecutive MC points for specified detector id. Currently works only for STS.
    * \param[in] detId Detector identificator.
    * \return Number of MC points.
    */
   Int_t GetNofConsecutivePoints(DetectorId detId) const {
	  //assert(detId == kSTS);
//      return fMaxConsecutivePoints.find(detId)->second;
	   return (detId == kSTS) ? fMaxConsecutivePoints.find(detId)->second : -1;
   }

   /**
    * \brief Calculates number of consecutive MC points for specified detector id. Currently works only for STS.
    * \param[in] detId Detector identificator.
    * \return Number of MC points.
    */
   void CalculateNofConsecutivePoints() {
	  fMaxConsecutivePoints[kSTS] = MaxConsecutiveNumbers(fStationIds.find(kSTS)->second);
   }

   /**
    * \brief Return number of RICH hits in ring.
    * \return Number of RICH hits in ring.
    */
   Int_t GetNofRichHits() const {
	   return fNofRichHits;
   }

   /**
    * \brief SetNumber of RICH hits in ring.
    * \param[in] nofRIchHits Number of hits in RICH.
    */
   void SetNofRichHits(Int_t nofRichHits) {
	   fNofRichHits = nofRichHits;
   }

   /**
    * \brief Return major semi-axis of the ellipse.
    * \return Major semi-axis of the ellipse.
    */
   Double_t GetRingAaxis() const {
      return fRingAaxis;
   }

   /**
    * \brief Set major semi-axis of the ellipse.
    * \param[in] a Major semi-axis of the ellipse.
    */
   void SetRingAaxis(Double_t a) {
      fRingAaxis = a;
   }

   /**
    * \brief Return minor semi-axis of the ellipse.
    * \return Minor semi-axis of the ellipse.
    */
   Double_t GetRingBaxis() const {
      return fRingBaxis;
   }

   /**
    * \brief Set minor semi-axis of the ellipse.
    * \param[in] b Minor semi-axis of the ellipse.
    */
   void SetRingBaxis(Double_t b) {
      fRingBaxis = b;
   }

   /**
    * \brief Return X coordinate of the ellipse center.
    * \return X coordinate of the ellipse center.
    */
   Double_t GetRingCenterX() const {
      return fRingCenterX;
   }

   /**
    * \brief Set X coordinate of the ellipse center.
    * \param[in] xc X coordinate of the ellipse center.
    */
   void SetRingCenterX(Double_t xc) {
      fRingCenterX = xc;
   }

   /**
    * \brief Return Y coordinate of the ellipse center.
    * \return Y coordinate of the ellipse center.
    */
   Double_t GetRingCenterY() const {
      return fRingCenterY;
   }

   /**
    * \brief Set Y coordinate of the ellipse center.
    * \param[in] yc Y coordinate of the ellipse center.
    */
   void SetRingCenterY(Double_t yc) {
      fRingCenterY = yc;
   }

   /**
    * \brief Return MC point for specified detector id and point index.
    *
    * Before accessing points with this function one has to check
    * the number of points on this station!
    *
    * \param[in] detId Detector identificator.
    * \param[in] stationId Station id.
    * \param[in] index Index of MC point.
    * \return MC point.
    */
   const CbmLitMCPoint& GetPointAtStation(
         DetectorId detId,
         Int_t stationId,
         Int_t index) const {
      assert((detId != kRICH) && (GetNofPointsAtStation(detId, stationId) != 0));
      return fStationPoints.find(detId)->second.find(stationId)->second[index];
   }

   /**
    * \brief Return number of MC points for specified detector ID and station ID.
    * \param[in] detId Detector identificator.
    *  \param[in] stationId Station id.
    * \return Number of MC points.
    */
   UInt_t GetNofPointsAtStation(
         DetectorId detId,
         Int_t stationId) const {
      assert(detId != kRICH);
      if (fStationPoints.find(detId)->second.count(stationId) > 0) {
         return fStationPoints.find(detId)->second.find(stationId)->second.size();
      } else return 0;
   }

private:
   // map<detector id, vector of MC points>
   map<Int_t, vector<CbmLitMCPoint> > fPoints;

   // map<detector id, map<station id, vector of MC points> >
   map<Int_t, map<Int_t, vector<CbmLitMCPoint> > > fStationPoints;

   // Temporary set to store unique station indices for fast access
   // map<detector id, set<station index>>
   map<Int_t, set<Int_t> > fStationIds;

   // temporary storage for maximum number of consecutive MC points
   // map<detector id, number of MC points>
   map<Int_t, Int_t> fMaxConsecutivePoints;

   Int_t fNofRichHits; // Number of hits in RICH ring
   Double_t fRingAaxis; // major semi-axis of the ellipse
   Double_t fRingBaxis; // minor semi-axis of the ellipse
   Double_t fRingCenterX; // X coordinate of the ring center
   Double_t fRingCenterY; // Y coordinate of the ring center

private:

   Int_t MaxConsecutiveNumbers(
         const set<Int_t>& numbers) const {
      if (numbers.size() == 0) return 0;
      if (numbers.size() == 1) return 1;

      vector<Int_t> a(numbers.begin(), numbers.end());

      int maxCnt = 0;
      int cnt = 1;
      for (Int_t i = 0; i < a.size() - 1; i++) {
         if (a[i] == (a[i + 1] - 1)) {
            cnt++;
         } else {
            maxCnt = std::max(cnt, maxCnt);
            cnt = 1;
         }
      }
      maxCnt = std::max(cnt, maxCnt);
      return maxCnt;
   }

   string PointsToString(
         DetectorId detId,
         const string& detName) const {
      stringstream ss;
      ss << detName << " np=" << GetNofPoints(detId) << " npds=" << GetNofPointsInDifferentStations(detId)
    		  << " ncp=" << GetNofConsecutivePoints(detId) << " points=(";
      for (Int_t i = 0; i < GetNofPoints(detId); i++) {
         ss << ":" << GetPoint(detId, i).GetRefId() << ":" << GetPoint(detId, i).GetStationId() << ",";
         //ss << GetPoint(detId, i);
      }
      ss << ") ";
      return ss.str();
   }

public:

   /**
    * \brief Returns string representation of the class.
    * \return Class representation as string.
    */
   virtual string ToString() const {
      stringstream ss;
      ss << "MCTrack: ";
      ss << PointsToString(kMVD, "MVD") << "|";
      ss << PointsToString(kSTS, "STS") << "|";
      ss << PointsToString(kTRD, "TRD") << "|";
      ss << PointsToString(kMUCH, "MUCH") << "|";
      ss << PointsToString(kTOF, "TOF") << "|";
      ss << endl;
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend std::ostream& operator<<(std::ostream& strm, const CbmLitMCTrack& track) {
      strm << track.ToString();
      return strm;
   }
};

#endif /* CBMLITMCTRACK_H_ */
