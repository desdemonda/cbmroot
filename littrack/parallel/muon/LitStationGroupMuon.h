/**
 * \file LitAbsorber.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Station group in muon detector layout.
 **/

#ifndef LITSTATIONGROUP_H_
#define LITSTATIONGROUP_H_

#include "LitStationMuon.h"
#include "LitAbsorber.h"
#include "../LitFieldGrid.h"
#include "../LitFieldRegion.h"

namespace lit {
namespace parallel{

/**
 * \class LitAbsorber
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Station group in muon detector layout.
 **/
template<class T>
class LitStationGroupMuon
{
public:
   /**
    * \brief Constructor.
    */
   LitStationGroupMuon():
		fFieldGridFront(),
		fFieldGridMiddle(),
		fFieldGridBack(),
        fStations(),
        fAbsorber() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitStationGroupMuon() {}

   /**
    * \brief Return magnetic field grid in front of the absorber.
    * \return Magnetic field grid in front of the absorber.
    */
   const LitFieldGrid& GetFieldGridFront() const { return fFieldGridFront; }

   /**
    * \brief Set field grid in front of the absorber.
    * \param[in] fieldGrid Field grid value.
    */
   void SetFieldGridFront(const LitFieldGrid& fieldGrid) { fFieldGridFront = fieldGrid; }

   /**
    * \brief Return magnetic field grid in the middle of the absorber.
    * \return Magnetic field grid in the middle of the absorber.
    */
   const LitFieldGrid& GetFieldGridMiddle() const { return fFieldGridMiddle; }

   /**
    * \brief Set field grid in the middle of the absorber.
    * \param[in] fieldGrid Field grid value.
    */
   void SetFieldGridMiddle(const LitFieldGrid& fieldGrid) { fFieldGridMiddle = fieldGrid; }

   /**
    * \brief Return magnetic field grid in the back of the absorber.
    * \return Magnetic field grid in the back of the absorber.
    */
   const LitFieldGrid& GetFieldGridBack() const { return fFieldGridBack; }

   /**
    * \brief Set magnetic field grid in the back of the absorber.
    * \param[in] fieldGrid Field grid value.
    */
   void SetFieldGridBack(const LitFieldGrid& fieldGrid) { fFieldGridBack = fieldGrid; }

   /**
    * \brief Add station to the station group.
    * \param[in] station Station to be added.
    */
   void AddStation(const LitStationMuon<T>& station) { fStations.push_back(station); }

   /**
    * \brief Return station by index.
    * \param[in] index Index of the station.
    * \return Station by index.
    */
   const LitStationMuon<T>& GetStation(unsigned short index) const { return fStations[index]; }

   /**
    * \brief Return number of stations in this station group.
    * \return Number of stations in this station group.
    */
   unsigned char GetNofStations() const { return fStations.size(); }

   /**
    * \brief Return absorber.
    * \return Absorber.
    */
   const LitAbsorber<T>& GetAbsorber() const { return fAbsorber; }

   /**
    * \brief Set absorber.
    * \param[in] absorber Absorber value.
    */
   void SetAbsorber(const LitAbsorber<T>& absorber) { fAbsorber = absorber; }

   /**
    * \brief Calculate field region for the group of stations.
    * \param[in] x X position.
    * \param[in] y Y position.
    * \param[out] field Output field region.
    */
   void GetFieldRegion(T x, T y, LitFieldRegion<T>& field) const {
      LitFieldValue<T> v1, v2, v3;
      fFieldGridFront.GetFieldValue(x, y, v1);
      fFieldGridMiddle.GetFieldValue(x, y, v2);
      fFieldGridBack.GetFieldValue(x, y, v3);
      field.Set(v1, fFieldGridFront.GetZ(), v2, fFieldGridMiddle.GetZ(), v3, fFieldGridBack.GetZ());
   }

   /**
    * \brief Return std::string representation for the class.
    * \return std::string representation for the class.
    */
   std::string ToString() const {
      std::string str = "LitStationGroupMuon: nofStations="
            + lit::parallel::ToString<int>((int)GetNofStations()) + "\n";
      for (unsigned char i = 0; i < GetNofStations(); i++) {
         str += "  " + lit::parallel::ToString<int>((int)i) + " " + GetStation(i).ToString();
      }
      str += "  " + GetAbsorber().ToString();
      return str;
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return std::ostream for continuous output.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitStationGroupMuon& stationGroup) {
      strm << stationGroup.ToString();
      return strm;
   }

private:
   // This field slices are used to construct LitFieldRegion
   // in the group of stations. We need 3 values in order to use
   // parabolic approximation. So the values are in front, middle
   // and back of the gap.
   LitFieldGrid fFieldGridFront; // Magnetic field approximation in front
   LitFieldGrid fFieldGridMiddle; // Magnetic field approximation in the middle
   LitFieldGrid fFieldGridBack; // Magnetic field approximation in the back

   std::vector<LitStationMuon<T> > fStations; // Array with stations
   LitAbsorber<T> fAbsorber; // Absorber
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitStationGroupMuon<fvec> LitStationGroupMuonVec;
typedef LitStationGroupMuon<fscal> LitStationGroupMuonScal;

} // namespace parallel
} // namespace lit

#endif /* LITSTATIONGROUP_H_ */
