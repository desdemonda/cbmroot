/**
 * \file LitDetectorLayout.h
 * \brief Detector layout class.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * Classes for geometry description. Detector consists of stations.
 * Station contains information about magnetic field approximation and
 * approximation of the material budget. In addition virtual stations are
 * stored which are used in the track extrapolation to the first detector station.
 *
 **/

#ifndef LITDETECTORLAYOUT_H_
#define LITDETECTORLAYOUT_H_

#include "LitStation.h"
#include "LitVirtualStation.h"

namespace lit {
namespace parallel {

/**
 * \class LitDetectorLayout
 * \brief Represents detector layout.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * Detector layout provides access to the detector information.
 * It stores a list of virtual planes and a list of stations.
 */
template<class T>
class LitDetectorLayout
{
public:
   /**
    * \brief Constructor.
    */
   LitDetectorLayout():
      fStations(),
      fVirtualStations() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitDetectorLayout() {}

   /**
    * \brief Add station to detector layout.
    * \param[in] station Station to be added.
    */
   void AddStation(const LitStation<T>& station) {
      fStations.push_back(station);
   }

   /**
    * \brief Add virtual station to detector layout.
    * \param[in] virtualStation Virtual station to be added.
    */
   void AddVirtualStation(const LitVirtualStation<T>& virtualStation) {
      fVirtualStations.push_back(virtualStation);
   }

   /**
    * \brief Return number of stations.
    * \return Number of stations.
    */
   unsigned char GetNofStations() const {
      return fStations.size();
   }

   /**
    * \brief Return number of virtual stations.
    * \return Number of virtual stations.
    */
   unsigned char GetNofVirtualStations() const {
      return fVirtualStations.size();
   }

   /**
    * \brief Return station by index.
    * \param[in] station Index of station.
    * \return Station by index.
    */
   const LitStation<T>& GetStation(unsigned char station) const {
      return fStations[station];
   }

   /**
    * \brief Return virtual station by index.
    * \param[in] station Virtual station index.
    * \return Virtual station by index.
    */
   const LitVirtualStation<T>& GetVirtualStation(unsigned char virtualStation) const {
      return fVirtualStations[virtualStation];
   }

   /**
    * \brief Returns string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitDetectorLayout: \n";
      ss << "virtual stations: nofVirtualStations=" << (int)GetNofVirtualStations() << "\n";
      for (unsigned char i = 0; i < GetNofVirtualStations(); i++) {
         ss << (int)i << " " << GetVirtualStation(i).ToString() << "\n";
      }
      ss << "stations: nofStations=" << (int)GetNofStations() << "\n";
      for (unsigned char i = 0; i < GetNofStations(); i++) {
         ss << (int)i << " " << GetStation(i).ToString() << "\n";
      }
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitDetectorLayout& layout) {
      strm << layout.ToString();
      return strm;
   }

private:
   vector<LitStation<T> > fStations; // Array with stations
   vector<LitVirtualStation<T> > fVirtualStations; // Array with virtual stations
} _fvecalignment;

/**
 * \typedef LitDetectorLayout<fvec> LitDetectorLayoutVec
 * \brief Vector version of LitDetectorLayout.
 */
typedef LitDetectorLayout<fvec> LitDetectorLayoutVec;

/**
 * \typedef LitDetectorLayout<fscal> LitDetectorLayoutScal
 * \brief Scalar version of LitDetectorLayout.
 */
typedef LitDetectorLayout<fscal> LitDetectorLayoutScal;

} // namespace parallel
} // namespace lit
#endif /* LITDETECTORGEOMETRY_H_ */
