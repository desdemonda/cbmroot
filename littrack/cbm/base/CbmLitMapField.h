/** CbmLitMapField.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 *
 * Interface to CBM field map.
 **/

#ifndef CBMLITMAPFIELD_H_
#define CBMLITMAPFIELD_H_

#include "interface/CbmLitField.h"

#include "FairField.h"

class CbmLitMapField :public CbmLitField
{
public:
   /* Constructor */
   CbmLitMapField(FairField* field):fField(field) {}

   /* Destructor */
   virtual ~CbmLitMapField() {};

   /* Inherited from CbmLitField */
   void GetFieldValue(
      litfloat x, litfloat y, litfloat z,
      litfloat& Bx, litfloat& By, litfloat& Bz) const {
      double pos[3] = {x, y, z};
      double B[3];
      fField->GetFieldValue(pos, B);
      Bx = B[0];
      By = B[1];
      Bz = B[2];
   }

   /* Inherited from CbmLitField */
   virtual std::string ToString() const {
      return "CbmLitMapField::ToString";
   }

private:
   FairField* fField; // Pointer to field map
};

#endif /* CBMLITMAPFIELD_H_ */
