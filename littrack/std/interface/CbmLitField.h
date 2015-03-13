/**
 * \file CbmLitField.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Interface for accessing the magnetic field.
 */
#ifndef CBMLITFIELD_H_
#define CBMLITFIELD_H_

#include "base/CbmLitFloat.h"
#include <string>
using std::string;

/**
 * \class CbmLitField
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 * \brief Interface for accessing the magnetic field.
 */
class CbmLitField
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitField() {};

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitField() {};

   /**
    * \brief Return field value at (x,y,z) position.
    * \param[in] x X coordinate [cm].
    * \param[in] y Y coordinate [cm].
    * \param[in] z Z coordinate [cm].
    * \param[out] Bx Output Bx field value [kGauss].
    * \param[out] By Output By field value [kGauss].
    * \param[out] Bz Output Bz field value [kGauss].
    */
   virtual void GetFieldValue(
         litfloat x,
         litfloat y,
         litfloat z,
         litfloat& Bx,
         litfloat& By,
         litfloat& Bz) const = 0;

   /*
    * \brief Return string representation of class.
    * \return String representation of this class.
    */
   virtual string ToString() const {
      return "CbmLitField::ToString";
   }
};

#endif /* CBMLITFIELD_H_ */
