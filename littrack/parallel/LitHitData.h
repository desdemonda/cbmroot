/**
 * \file LitHitData.h
 * \brief Class for accessing hits in track reconstruction.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 **/

#ifndef LITHITDATA_H_
#define LITHITDATA_H_

#include "LitScalPixelHit.h"
#include "LitScalStripHit.h"

#include <vector>
#include <set>
#include <sstream>
#include <cmath>
#include <cassert>
using std::vector;
using std::string;
using std::stringstream;
using std::ostream;
using std::max;
using std::endl;
using std::set;

namespace lit {
namespace parallel {

/**
 * \class LitHitData
 * \brief Class for accessing hits in track reconstruction.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class LitHitData
{
public:
   /**
    * \brief Constructor.
    */
   LitHitData():
      fHits(),
      fMaxErrX(),
      fMaxErrY(),
      fNofStations(0),
      fZPosSet(),
      fZPosBins() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitHitData() {}

   /**
    * \brief Set number of stations.
    * \param[in] layout Detector layout.
    */
   void SetNofStations(
      unsigned char nofStations) {
      fNofStations = nofStations;
      fHits.resize(nofStations);
      fMaxErrX.resize(nofStations);
      fMaxErrY.resize(nofStations);
      fZPosSet.resize(nofStations);
      fZPosBins.resize(nofStations);
      for(unsigned char i = 0; i < nofStations; i++) {
         fHits[i].reserve(1500);
      }
   }

   /**
    * \brief Add hit.
    * \param[in] station Station index.
    * \param[in] hit Pointer to hit to be added.
    */
   void AddHit(
      LitScalPixelHit* hit) {
      unsigned char station = hit->stationId;
      assert(station > -1 && station < fNofStations);
      fHits[station].push_back(hit);
      fZPosSet[station].insert(hit->Z); // Find different Z positions of hits
      fMaxErrX[station] = max(hit->Dx, fMaxErrX[station]);
      fMaxErrY[station] = max(hit->Dy, fMaxErrY[station]);
   }

   /**
    * \brief Return hit pointer.
    * \param[in] station Station index.
    * \param[in] hitId Hit index in array of hits for specified station.
    * \return Hit pointer.
    */
   const LitScalPixelHit* GetHit(
      unsigned char  station,
      unsigned int hitId) const {
      return fHits[station][hitId];
   }

   /**
    * \brief Return vector of hits.
    * \param[in] station Station index.
    * \return Vector of hits.
    */
   const vector<LitScalPixelHit*>& GetHits(
      unsigned char  station) {
      return fHits[station];
   }

   /**
    * \brief Return number of hits for specified station.
    * \param[in] station Station index.
    * \return Number of hits in station.
    */
   unsigned int GetNofHits(
      unsigned char  station) const {
      return fHits[station].size();
   }

   /**
    * \brief Return maximum X error in [cm] for specified station.
    * \param[in] station Station index in station group.
    * \return Hit error.
    */
   fscal GetMaxErrX(
      unsigned char station) const {
      return fMaxErrX[station];
   }

   /**
    * \brief Return maximum Y error in [cm] for specified station.
    * \param[in] station Station index in station group.
    * \return Hit error.
    */
   fscal GetMaxErrY(
      unsigned char station) const {
      return fMaxErrY[station];
   }

   const vector<int>& GetZPosBins(
      unsigned char station) const {
      return fZPosBins[station];
   }

   fscal GetZPosByBin(
      unsigned char station,
      int bin) const {
      return GetMinZPos(station) + bin * EPSILON;
   }

   int GetBinByZPos(
      unsigned char station,
      fscal zPos) const {
      return (zPos - GetMinZPos(station)) / EPSILON;
   }

   fscal GetMinZPos(
      unsigned char station) const {
      return (fZPosSet[station].empty()) ? 0. : *fZPosSet[station].begin();
   }

   /**
    * \brief Clear hit arrays.
    */
   void Clear() {
      for(unsigned int i = 0; i < fNofStations; i++) {
         fHits[i].clear();
         fHits[i].reserve(1500);
         fMaxErrX[i] = 0.;
         fMaxErrY[i] = 0.;
         fZPosSet[i].clear();
         fZPosBins[i].clear();
      }
   }

   /**
    * \brief Must be called after all hits are added.
    */
   void Arrange() {
      for (int iStation = 0; iStation < fNofStations; iStation++) {
         if (fZPosSet[iStation].empty()) continue;
         fscal minZ = *fZPosSet[iStation].begin();
         fscal maxZ = *fZPosSet[iStation].rbegin();
         set<int> binSet;
         set<fscal>::const_iterator it;
         for (it = fZPosSet[iStation].begin(); it != fZPosSet[iStation].end(); it++) {
            fscal z = *it;
            int bin = (z - minZ) / EPSILON;
            binSet.insert(bin);
         }

         set<int>::const_iterator it2;
         for (it2 = binSet.begin(); it2 != binSet.end(); it2++) {
            int bin = *it2;
            fscal z = minZ + bin * EPSILON;
          //  fZPos[iStation].push_back(z);
            fZPosBins[iStation].push_back(bin);
         }
      }
   }

   /**
    * \brief Returns string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitHitData:" << endl;
      for(unsigned int i = 0; i < fHits.size(); i++) {
         ss << " station " << i << ": " << GetNofHits(i) << " hits, "
               << "maxerrx=" << GetMaxErrX(i) << ", maxerry=" << GetMaxErrY(i) << ", ";
         ss << "zposset=(";
         for (set<fscal>::const_iterator it = fZPosSet[i].begin(); it != fZPosSet[i].end(); it++) {
            ss << *it << ", ";
         }
         ss << ")" << endl;
         ss << "zposbins=(";
         for (vector<int>::const_iterator it = fZPosBins[i].begin(); it != fZPosBins[i].end(); it++) {
            ss << "|" << *it << "," << GetZPosByBin(i, *it) << "| ";
         }
         ss << ")" << endl;
      }
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitHitData& hitData) {
      strm << hitData.ToString();
      return strm;
   }

private:
   static const fscal EPSILON;
   vector<vector<LitScalPixelHit*> > fHits; // Array of hits
   vector<fscal> fMaxErrX; // Array of maximum X error for each station
   vector<fscal> fMaxErrY; // Array of maximum Y error for each station
   unsigned char fNofStations; // Number of stations
   vector<set<fscal> > fZPosSet; // Set of Z positions of hits in each station
                                    // Temporarily used for Z different Z positions calculation
   vector<vector<int> > fZPosBins; // Array of Z positions bin number of hits in each station
} _fvecalignment;

} // namespace parallel
} // namespace lit

#endif /* LITHITDATA_H_ */
