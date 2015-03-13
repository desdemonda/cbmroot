/**
 * \class LitVirtualStation
 * \brief Virtual detector station which stores information needed for track propagation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2014
 */

#ifndef LITVIRTUALSTATION_H_
#define LITVIRTUALSTATION_H_

#include "LitMaterialGrid.h"
#include "LitFieldGrid.h"
#include <sstream>
using std::stringstream;
using std::ostream;

namespace lit {
namespace parallel {

/**
 * \class LitVirtualStation
 * \brief Virtual detector station which stores information needed for track propagation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2014
 *
 * Virtual station stores Z position of its center material approximation
 * in silicon equivalent and field approximation.
 *
 */
template<class T>
class LitVirtualStation
{
public:
   /**
    * \brief Constructor.
    */
   LitVirtualStation():
      fMaterial(),
      fField(),
      fZ(0.) {}

   /**
    * \brief Destructor
    */
   virtual ~LitVirtualStation() {}

   /* Setters */
   void SetMaterial(const LitMaterialGrid& material) { fMaterial = material; }
   void SetField(const LitFieldGrid& field) { fField = field; }
   void SetZ(T z) { fZ = z; }

   /* Getters */
   const LitMaterialGrid& GetMaterial() const { return fMaterial; }
   const LitFieldGrid& GetField() const { return fField; }
   T GetZ() const { return fZ; }

   /**
    * \brief Returns string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitVirtualStation: Z=" << GetZ() << "\n";
      ss << "   material: " << GetMaterial().ToString() << "\n";
      ss << "   field: " << GetField().ToString();
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitVirtualStation& station ) {
      strm << station.ToString();
      return strm;
   }

private:
   LitMaterialGrid fMaterial; // Material approximation
   LitFieldGrid fField; // Field approximation
   T fZ; // Z center of the station [cm]
} _fvecalignment;

/**
 * \typedef LitVirtualStation<fvec> LitVirtualStationVec
 * \brief Vector version of LitVirtualStation.
 */
typedef LitVirtualStation<fvec> LitVirtualStationVec;

/**
 * \typedef LitVirtualStation<fscal> LitVirtualStationScal
 * \brief Scalar version of LitVirtualStation.
 */
typedef LitVirtualStation<fscal> LitVirtualStationScal;

} // namespace parallel
} // namespace lit
#endif /* LITVIRTUALSTATION_H_ */
