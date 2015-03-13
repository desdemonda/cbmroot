/**
 * \file LitSubstationMuon
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Substation in muon detector layout.
 **/

#ifndef LITSUBSTATIONMUON_H_
#define LITSUBSTATIONMUON_H_

#include "../LitMaterialInfo.h"

namespace lit {
namespace parallel {

/**
 * \class LitSubstationMuon
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Substation in muon detector layout.
 **/
template<class T>
class LitSubstationMuon
{
public:
   /**
    * \brief Constructor.
    */
   LitSubstationMuon():
      fZ(0.),
      fMaterial() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitSubstationMuon() {}

   /**
    * \brief Return Z position of substation.
    * \return Z position of substation.
    */
   const T& GetZ() const { return fZ; }

   /**
    * \brief Set Z pposition of substation.
    * \param[in] z Z position value.
    */
   void SetZ(const T& z) { fZ = z; }

   /**
    * \brief Return material of substation.
    * \return Material of substation.
    */
   const LitMaterialInfo<T>& GetMaterial() const { return fMaterial; }

   /**
    * \brief Set material of substation.
    * \param[in] materiel Material value.
    */
   void SetMaterial(const LitMaterialInfo<T>& material) { fMaterial = material; }

   /**
    * \brief Return string representation for the class.
    * \return String representation of class.
    */
   std::string ToString() const {
      return "LitSubstationMuon: Z=" + lit::parallel::ToString<T>(GetZ())
            + ", material=" + GetMaterial().ToString();
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return std::ostream for continuous output.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitSubstationMuon& substation ) {
      strm << substation.ToString();
      return strm;
   }

private:
   T fZ; // Z position of substation in [cm]
   LitMaterialInfo<T> fMaterial; // Material of substation
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitSubstationMuon<fvec> LitSubstationMuonVec;
typedef LitSubstationMuon<fscal> LitSubstationMuonScal;

} // namespace parallel
} // namespace lit
#endif /* LITSUBSTATIONMUON_H_ */
