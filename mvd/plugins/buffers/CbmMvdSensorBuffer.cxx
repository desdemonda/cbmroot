// -------------------------------------------------------------------------
// -----                  CbmMvdSensorBuffer source file              -----
// -----                  Created 02.02.2012 by M. Deveaux            -----
// -------------------------------------------------------------------------
#include "CbmMvdSensorBuffer.h"

#include "TClonesArray.h"
#include "TObjArray.h"



// -----   Default constructor   -------------------------------------------
CbmMvdSensorBuffer::CbmMvdSensorBuffer() 
  : CbmMvdSensorPlugin(),
    fBuffer(NULL),
    fCurrentEvent(NULL),
    fSensor(NULL)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMvdSensorBuffer::~CbmMvdSensorBuffer() {
  fBuffer->Delete();
  delete fBuffer;
}
// -------------------------------------------------------------------------

void CbmMvdSensorBuffer::Clear () {
  if (fBuffer)		{fBuffer->Clear();}
  if (fCurrentEvent)	{fCurrentEvent->Clear();}
}

ClassImp(CbmMvdSensorBuffer)
