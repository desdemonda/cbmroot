#include "CbmArrayI.h"

////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//       This class extends the possibilities of the usual TArrayI                //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////

ClassImp(CbmArrayI)

CbmArrayI::CbmArrayI()
{
  // Constructer.
  fN = 0;
  Set(0);
}


void CbmArrayI::ShiftByOneAndAddAtFirst(Int_t value)
{
  // Shifts every array element in the next slot and adds the value in
  // the first slot.

  Set(GetSize() + 1);

  {for (Int_t i = GetSize() - 2; i >= 0; i--) {
    AddAt(At(i), i+1);
  }}

  AddAt(value, 0);

}

CbmArrayI::~CbmArrayI()
{
  // Destructor.
  // Has nothing to do.
}

