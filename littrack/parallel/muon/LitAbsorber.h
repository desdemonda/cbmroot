/**
 * \file LitAbsorber.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Absorber in muon detector layout.
 **/

#ifndef LITABSORBER_H_
#define LITABSORBER_H_

#include "../LitFieldGrid.h"

namespace lit {
namespace parallel{

/**
 * \class LitAbsorber
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Absorber in muon detector layout.
 **/
template<class T>
class LitAbsorber
{
public:
   /**
    * \brief Constructor.
    */
   LitAbsorber():
      fZ(0.),
      fMaterial(),
      fFieldGridFront(),
      fFieldGridMiddle(),
      fFieldGridBack() {}

   /**
    * \brief Destructor.
    */
   virtual ~LitAbsorber() {}

   /**
    * \brief Return Z position of absorber.
    * \return Z position of absorber.
    */
   const T& GetZ() const { return fZ; }

   /**
    * \brief Set Z position of absorber.
    * \param[in] z Z position value.
    */
   void SetZ(const T& z) { fZ = z; }

   /**
    * \brief Return absorber material.
    * \return Absorber material.
    */
   const LitMaterialInfo<T>& GetMaterial() const { return fMaterial; }

   /**
    * \brief Set absorber material.
    * \param[in] material Absorber material value.
    */
   void SetMaterial(const LitMaterialInfo<T>& material) { fMaterial = material; }

   /**
    * \brief Return magnetic field grid in front of the absorber.
    * \return magnetic field grid in front of the absorber.
    */
   const LitFieldGrid& GetFieldGridFront() const { return fFieldGridFront; }

   /**
    * \brief Set magnetic field grid in front of the absorber.
    * \param[in] fieldGrid Magnetic field value.
    */
   void SetFieldGridFront(const LitFieldGrid& fieldGrid) { fFieldGridFront = fieldGrid; }

   /**
    * \brief Return magnetic field grid in the middle of the absorber.
    * \return magnetic field grid in the middle of the absorber.
    */
   const LitFieldGrid& GetFieldGridMiddle() const { return fFieldGridMiddle; }

   /**
    * \brief Set magnetic field grid in the middle of the absorber.
    * \param[in] fieldGrid Magnetic field value.
    */
   void SetFieldGridMiddle(const LitFieldGrid& fieldGrid) { fFieldGridMiddle = fieldGrid; }

   /**
    * \brief Return magnetic field grid in the back of the absorber.
    * \return Magnetic field grid in the back of the absorber.
    */
   const LitFieldGrid& GetFieldGridBack() const { return fFieldGridBack; }

   /**
    * \brief Set magnetic field grid in the back of the absorber.
    * \param[in] fieldGrid magnetic field value.
    */
   void SetFieldGridBack(const LitFieldGrid& fieldGrid) { fFieldGridBack = fieldGrid; }

   /**
    * \brief Return std::string representation of the class.
    * \return std::string representation of the class.
    */
   std::string ToString() const {
      return "LitAbsorber: Z=" + lit::parallel::ToString<T>(GetZ())
            + ", material=" + GetMaterial().ToString();
   }

   /**
    * \brief Operator << for convenient output to std::ostream.
    * \return std::ostream for continuous output.
    */
   friend std::ostream& operator<<(std::ostream& strm, const LitAbsorber& absorber) {
      strm << absorber.ToString();
      return strm;
   }

private:
   T fZ; // Z position of the absorber in [cm]
   LitMaterialInfo<T> fMaterial; // Absorber material
   LitFieldGrid fFieldGridFront; // Magnetic field approximation in front of absorber
   LitFieldGrid fFieldGridMiddle; // Magnetic field approximation in the middle of absorber
   LitFieldGrid fFieldGridBack; // Magnetic field approximation in the back of absorber
} _fvecalignment;

/* Some typedefs for convenience */
typedef LitAbsorber<fvec> LitAbsorberVec;
typedef LitAbsorber<fscal> LitAbsorberScal;

} // namespace parallel
} // namespace lit

#endif /* LITABSORBER_H_ */
