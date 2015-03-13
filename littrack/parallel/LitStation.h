/**
 * \class LitStation
 * \brief Detector station.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */

#ifndef LITSTATION_H_
#define LITSTATION_H_

#include "LitVirtualStation.h"
#include <sstream>
#include <vector>
using std::stringstream;
using std::ostream;
using std::vector;

namespace lit {
namespace parallel {

/**
 * \class LitStation
 * \brief Detector station.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * Station stores list of virtual stations which are used in track propagation.
 *
 */
template<class T>
class LitStation
{
public:
   /**
    * \brief Constructor.
    */
   LitStation():
      fVirtualStations() {}

   /**
    * \brief Destructor
    */
   virtual ~LitStation() {}

   /**
    * \brief Add virtual station to detector layout.
    * \param[in] virtualStation Virtual station to be added.
    */
   void AddVirtualStation(const LitVirtualStation<T>& virtualStation) {
      fVirtualStations.push_back(virtualStation);
   }

   /**
    * \brief Return number of virtual stations.
    * \return Number of virtual stations.
    */
   unsigned char GetNofVirtualStations() const {
      return fVirtualStations.size();
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
      ss << "LitVirtualStation:\n";
      ss << "virtual stations: nofVirtualStations=" << (int)GetNofVirtualStations() << "\n";
      for (unsigned char i = 0; i < GetNofVirtualStations(); i++) {
         ss << (int)i << " " << GetVirtualStation(i).ToString() << "\n";
      }
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitStation& station ) {
      strm << station.ToString();
      return strm;
   }

private:
   vector<LitVirtualStation<T> > fVirtualStations; // List of virtual stations
} _fvecalignment;

/**
 * \typedef LitStation<fvec> LitStationVec
 * \brief Vector version of LitStation.
 */
typedef LitStation<fvec> LitStationVec;

/**
 * \typedef LitStation<fscal> LitStationScal
 * \brief Scalar version of LitStation.
 */
typedef LitStation<fscal> LitStationScal;

} // namespace parallel
} // namespace lit
#endif /* LITSTATION_H_ */
