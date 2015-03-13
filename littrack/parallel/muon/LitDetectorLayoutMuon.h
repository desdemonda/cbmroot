/**
 * \file LitDetectorLayoutMuon.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Classes for muon geometry description of CBM.
 *
 * Classes for muon geometry description of CBM.
 * Detector layout consists of station groups. Each station
 * group consists of stations and absorber. Each station consists
 * of substations. Detector layout also provides access to
 * material of the detector and approximated magnetic field.
 **/

#ifndef LITDETECTORLAYOUTMUON_H_
#define LITDETECTORLAYOUTMUON_H_

#include "LitStationGroupMuon.h"

#include <vector>

namespace lit {
namespace parallel{

/**
 * \class LitDetectorLayoutMuon
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Muon detector layout.
 **/
template<class T>
class LitDetectorLayoutMuon
{
public:
   /**
    * \brief Constructor.
    */
   LitDetectorLayoutMuon():fStationGroups() {};

   /**
    * \brief Destructor.
    */
   virtual ~LitDetectorLayoutMuon() {}

   /**
    * \brief Add station group to the layout.
    * \param stationGroup Station group to be added.
    */
   void AddStationGroup(const LitStationGroupMuon<T>& stationGroup) { fStationGroups.push_back(stationGroup); }

   /**
    * \brief Return number of station groups.
    * \return Number of station groups.
    */
   unsigned char GetNofStationGroups() const { return fStationGroups.size(); }

   /**
    * \brief Return number of stations for specified station group.
    * \param[in] stationGroup Station group index.
    * \return Number of stations for specified station group.
    */
   unsigned char GetNofStations(unsigned char stationGroup) const {
	   return fStationGroups[stationGroup].GetNofStations();
   }

   /**
    * \brief Return number of substations for specified station group and station.
    * \param[in] stationGroup Station group index.
    * \param[in] station Station index.
    * \return Number of substations for specified station group and station.
    */
   unsigned char GetNofSubstations(unsigned char stationGroup, unsigned char station) const {
      return fStationGroups[stationGroup].GetStation(station).GetNofSubstations();
   }

   /**
    * \brief Return station group for specified station group index.
    * \param[in] stationGroup Station group index.
    * \return Station group for specified station group index.
    */
   const LitStationGroupMuon<T>& GetStationGroup(unsigned char stationGroup) const {
      return fStationGroups[stationGroup];
   }

   /**
    * \brief Return station for specified station group and station indices.
    * \param[in] stationGroup Station group index.
    * \param[in] station Station index.
    * \return Station for specified station group and station indices.
    */
   const LitStationMuon<T>& GetStation(unsigned char stationGroup, unsigned char station) const {
      return fStationGroups[stationGroup].GetStation(station);
   }

   /**
    * \brief Return substation for specified station group, station and substation indices.
    * \param[in] stationGroup Station group index.
    * \param[in] station Station index.
    * \param[in] substation Substation index.
    * \return[in] Substation for specified station group, station and substation indices.
    */
   const LitSubstationMuon<T>& GetSubstation(unsigned char stationGroup, unsigned char station, unsigned char substation) const {
      return fStationGroups[stationGroup].GetStation(station).GetSubstation(substation);
   }

   /**
    * \brief Return std::string representation of the class.
    * \return std::string representation of the class.
    */
   std::string ToString() const {
      std::string str = "LitDetectorLayoutMuon: nofStationGroups="
            + lit::parallel::ToString<int>((int)GetNofStationGroups()) + "\n";
      for (unsigned char i = 0; i < GetNofStationGroups(); i++) {
         str += lit::parallel::ToString<int>((int)i) + " " + GetStationGroup(i).ToString();
      }
      return str;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return std::ostream for continuous output.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitDetectorLayoutMuon& layout) {
      strm << layout.ToString();
      return strm;
   }

private:
   std::vector<LitStationGroupMuon<T> > fStationGroups; // Array of station groups
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitDetectorLayoutMuon<fvec> LitDetectorLayoutMuonVec;
typedef LitDetectorLayoutMuon<fscal> LitDetectorLayoutMuonScal;

} // namespace parallel
} // namespace lit
#endif /*LITDETECTORLAYOUTMUON_H_*/
