////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
// CbmArrayI class -      class extends the possibilities of the usual TArrayI    //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////

#ifndef CBM_ARRAY_INT
#define CBM_ARRAY_INT

#include "TArrayI.h"
#include "TObject.h"


class CbmArrayI : public TArrayI, public TObject {

public:

           CbmArrayI();
          ~CbmArrayI();
    void   AddLast(Int_t value);
   Int_t   AtFirst();
   Int_t   AtLast();
    void   Fill(Int_t value);
    void   SetFill(Int_t size, Int_t value);
    void   ShiftByOneAndAddAtFirst(Int_t value);
   Int_t   CountAppearance(Int_t value);

  ClassDef(CbmArrayI, 1)    // Class of an array of integers
};


inline void CbmArrayI::AddLast(Int_t value) 
{
  // Adds the value after the last entry of the array.
  // Therefore the array size has to be increased by one.

  Set(GetSize() + 1);
  AddAt(value, GetSize() - 1);
}


inline Int_t CbmArrayI::AtLast()
{
  // Returns the value of the last array element.

  return At(GetSize()-1);
}


inline Int_t CbmArrayI::AtFirst()
{
  // Returns the value of the last array element.

  return At(0);
}


inline void CbmArrayI::Fill(Int_t value)
{
  // Fills the whole array with the value 'value'.

  for (Int_t i = 0; i < GetSize(); AddAt(value, i), i++);
}


inline void CbmArrayI::SetFill(Int_t size, Int_t value)
{
  // Set the array size to 'size' and fill the whole array with 'value'.

  if (GetSize() > 0) {
    Set(0);
  }
  
  for (Int_t i = 0; i < size; AddLast(value), i++);
}


#endif
