// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                          CbmTbEventBuilder                        -----
// -----                    Created 14.12.2014 by F. Uhlig                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
#include "CbmTbEventBuilder.h"

#include "CbmTbEvent.h"

CbmTbEventBuilder::CbmTbEventBuilder()
  : TObject(),
    fCurrentEvent(NULL)
{
}

CbmTbEventBuilder::~CbmTbEventBuilder()
{
}

ClassImp(CbmTbEventBuilder)
