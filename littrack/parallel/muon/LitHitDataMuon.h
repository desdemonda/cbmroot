/**
 * \file LitHitDataMuon.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Class for accessing hits in the track reconstruction.
 **/

#ifndef LITHITDATAMUON_H_
#define LITHITDATAMUON_H_

#include "LitDetectorLayoutMuon.h"
#include "../LitHit.h"
#include "../LitComparators.h"

#include <vector>
#include <algorithm>

namespace lit {
namespace parallel {

/**
 * \class LitHitDataMuon.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Class for accessing hits in the track reconstruction.
 **/
template<class T>
class LitHitDataMuon
{
public:
   /**
    * \brief Constructor.
    */
   LitHitDataMuon():
      fMaxErr(),
      fHits(),
      fLayout() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitHitDataMuon() {}

   /**
    * \brief Set detector layout for which hits are arranged.
    * \param[in] layout Detector layout.
    */
   void SetDetectorLayout(
      const LitDetectorLayoutMuon<T>& layout) {
      fLayout = layout;
      int nofGroups = layout.GetNofStationGroups();
      fHits.resize(nofGroups);
      fMaxErr.resize(nofGroups);
      for(int i = 0; i < nofGroups; i++) {
         int nofStations = layout.GetNofStations(i);
         fHits[i].resize(nofStations);
         fMaxErr[i].resize(nofStations);
         for(int j = 0; j < nofStations; j++) {
            int nofSubstations = layout.GetNofSubstations(i, j);
            fHits[i][j].resize(nofSubstations);
            fMaxErr[i][j].resize(nofSubstations);
            for(int k = 0; k < nofSubstations; k++) {
               fHits[i][j][k].reserve(1500);
            }
         }
      }
   }

   /**
    * \brief Add hit using station group, station and substation indices.
    * \param[in] stationGroup Index of the station group in the detector.
    * \param[in] station Index of the station in the station group.
    * \param[in] substation Index of the substation in the station.
    * \param[in] hit Pointer to the hit to be added.
    */
   void AddHit(
      int stationGroup,
      int station,
      int substation,
      LitScalPixelHit* hit) {
      fHits[stationGroup][station][substation].push_back(hit);
      if (fMaxErr[stationGroup][station][substation] < hit->Dx) {
         fMaxErr[stationGroup][station][substation] = hit->Dx;
      }
   }

   /**
    * \brief Add hit using absolute detector plane (substation) index in the detector.
    * \param[in] planeId Index of the detector plane (substation) in the detector.
    * \param[in] hit Pointer to the hit to be added.
    */
   void AddHit(
      unsigned char planeId,
      LitScalPixelHit* hit) {
      unsigned char stationGroup;
      unsigned char station;
      unsigned char substation;
      StationByPlaneId(planeId, stationGroup, station, substation);
      AddHit(stationGroup, station, substation, hit);
   }

   /**
    * \brief Return hit using station group, station and substation indices.
    * \param[in] stationGroup Index of the station group in the detector.
    * \param[in] station Index of the station in the station group.
    * \param[in] substation Index of the substation in the station.
    * \param[in] hitId Hit index in the array of hits for the specified substation.
    * \return Hit pointer.
    */
   const LitScalPixelHit* GetHit(
      int stationGroup,
      int station,
      int substation,
      int hitId) const {
      return fHits[stationGroup][station][substation][hitId];
   }

   /**
    * \brief Return hit vector using station group, station and substation indices.
    * \param[in] stationGroup Index of the station group in the detector.
    * \param[in] station Index of the station in the station group.
    * \param[in] substation Index of the substation in the station.
    * \return Hit vector.
    */
   const std::vector<LitScalPixelHit*>& GetHits(
      int stationGroup,
      int station,
      int substation) {
      return fHits[stationGroup][station][substation];
   }

   /**
    * \brief Return number of hits for the specified substation.
    * \param[in] stationGroup Index of the station group in the detector.
    * \param[in] station Index of the station in the station group.
    * \param[in] substation Index of the substation in the station.
    * \return Number of hits.
    */
   unsigned int GetNofHits(
      int stationGroup,
      int station,
      int substation) const {
      return fHits[stationGroup][station][substation].size();
   }

   /**
    * \brief Return maximum hit error in [cm] for the specified substation.
    * \param[in] stationGroup Index of the station group in the detector.
    * \param[in] station Index of the station in the station group.
    * \param[in] substation Index of the substation in the station.
    * \return Maximum hit error.
    */
   fscal GetMaxErr(
      int stationGroup,
      int station,
      int substation) const {
      return fMaxErr[stationGroup][station][substation];
   }

   /**
    * \brief Clear hit arrays.
    */
   void Clear() {
      for(unsigned int i = 0; i < fHits.size(); i++) {
         for(unsigned int j = 0; j < fHits[i].size(); j++) {
            for(unsigned int k = 0; k < fHits[i][j].size(); k++) {
               fHits[i][j][k].clear();
               fHits[i][j][k].reserve(1500);
               fMaxErr[i][j][k] = 0.;
            }
         }
      }
   }

   /**
    * \brief Sort hits in each substation by X coordinate.
    */
   void SortHits() {
      // TODO : add threads here
      for (int i = 0; i < fLayout.GetNofStationGroups(); i++) {
         for (int j = 0; j < fLayout.GetNofStations(i); j++) {
            for (int k = 0; k < fLayout.GetNofSubstations(i, j); k++) {
               std::vector<LitScalPixelHit*>& shits = fHits[i][j][k];
               std::sort(shits.begin(), shits.end(), ComparePixelHitXLess());
   //                std::cout << "station group " << i << " station " << j
   //                   << " substation " << k << std::endl;
   //                for(unsigned int i = 0; i < nh; i++)
   //                   std::cout << *shits[i];
            }
         }
      }
   }

   /**
    * \brief Return std::string representation for the class.
    * \return std::string representation for the class.
    */
   std::string ToString() const {
      std::string str = "HitDataMuon:\n";
      for(int i = 0; i < fLayout.GetNofStationGroups(); i++) {
         str += " station group " + lit::parallel::ToString<int>(i) + "\n";
         for(int j = 0; j < fLayout.GetNofStations(i); j++) {
            str += "  station " + lit::parallel::ToString<int>(j) + "\n";
            for(int k = 0; k < fLayout.GetNofSubstations(i, j); k++) {
               str += "   substation " + lit::parallel::ToString<int>(k)
                     + ": " +  lit::parallel::ToString<int>(GetNofHits(i, j, k)) + " hits, "
                     + "max err=" + lit::parallel::ToString<T>(GetMaxErr(i, j, k)) + "\n";
            }
         }
      }
      return str;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return std::ostream for continuous output.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitHitDataMuon& hitData) {
      strm << hitData.ToString();
      return strm;
   }

public:
   /**
    * \brief Calculate station group, station and substation indices using the detector plane number.
    * \param[in] planeId  Detector plane index.
    * \param[out] stationGroup Index of the station group in the detector.
    * \param[out] station Index of the station in the station group.
    * \param[out] subsattion Index of the substation in the station.
    */
   void StationByPlaneId(
      unsigned char planeId,
      unsigned char& stationGroup,
      unsigned char& station,
      unsigned char& substation) const {
      unsigned char counter = 0;
      for(unsigned char i = 0; i < fLayout.GetNofStationGroups(); i++) {
         for(unsigned char j = 0; j < fLayout.GetNofStations(i); j++) {
            counter += fLayout.GetNofSubstations(i, j);
            if (counter > planeId) {
               stationGroup = i;
               station = j;
               substation = fLayout.GetNofSubstations(i, j) - (counter - planeId);
               return;
            }
         }
      }
   }

   // Arrays of hits
   std::vector<std::vector<std::vector<PixelHitArray> > > fHits;
   // Arrays of maximum hit position errors for each substation
   std::vector<std::vector<std::vector<fscal> > > fMaxErr;
   // Detector layout
   LitDetectorLayoutMuon<T> fLayout;
} _fvecalignment;

} // namespace parallel
} // namespace lit
#endif /* LITHITDATAMUON_H_ */
