// -------------------------------------------------------------------------
// -----                  CbmMvdSensorTrackingBuffer source file       -----
// -----                  Created 14/02/12 by P.Sitzmann               -----
// -------------------------------------------------------------------------
#include "CbmMvdSensorTrackingBuffer.h"


// -----   Default constructor   -------------------------------------------
CbmMvdSensorTrackingBuffer::CbmMvdSensorTrackingBuffer() 
  : CbmMvdSensorBuffer(),
    ftimeStart(-1),
    ftimeStop(-1),
    ftimestep(-1)
{
};
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMvdSensorTrackingBuffer::~CbmMvdSensorTrackingBuffer() {
  
  fBuffer->Delete();
  delete fBuffer;
  
};
// -------------------------------------------------------------------------

// -----   Init         ----------------------------------------------------
void CbmMvdSensorTrackingBuffer::Init(CbmMvdSensor* mySensor) {
  
  fBuffer = new TClonesArray("CbmMvdHit", 1000);
  fCurrentEvent = new TClonesArray("CbmMvdHit", 1000);
  fSensor = mySensor;
  ftimeStart = 0;
  ftimeStop = 5000;
  ftimestep = 5000;
  bFlag = false;
};
// -------------------------------------------------------------------------

// -----   Exec         ----------------------------------------------------
void CbmMvdSensorTrackingBuffer::ExecChain() {

    SetInputArray(fPreviousPlugin->GetOutputArray());  // auto datatransport from Plugin to Plugin
    SetPluginReady(false);			       // Flag Plugin as working. as long as now timeslice is available Plugin is still on working state
    Int_t lastEntrie = fBuffer->GetLast();
    CbmMvdHit* myHit;
    Int_t time;
   // cout << endl << "exec output buffer" << endl;
    if(lastEntrie>-1)
   {
    myHit = (CbmMvdHit*)fBuffer->At(lastEntrie);	
    time = myHit->GetTimeStamp();
   
    if(time > (ftimeStop + ftimestep))			// computing full timeslice
	{
	  BuildTimeSlice(ftimeStart, ftimeStop);
	  ClearTimeSlice(0,ftimeStart);
	  ftimeStart = ftimeStop;
	  ftimeStop += ftimestep;
	  if (fCurrentEvent->GetEntriesFast() > 0)
	  {
	    SetPluginReady(true);
	    
	    cout << endl << "OutputBuffer is ready for readout on sensor " << fSensor->GetName() << endl;
	    //fCurrentEvent->Print();
          }
	}
  }    
};
// -------------------------------------------------------------------------

//--------------------------------------------------------------------------  
void CbmMvdSensorTrackingBuffer::BuildTimeSlice(Double_t tStart, Double_t tStop)
{
Int_t nEntries = fBuffer->GetEntriesFast();
CbmMvdHit* myHit;
Int_t time;
Int_t nHits;

for (Int_t i = 0; i < nEntries; i++)
    {
    myHit = (CbmMvdHit*)fBuffer->At(i);
    time = myHit->GetTimeStamp();
    if(tStart <= time <= ( tStop + fSensor->GetIntegrationtime()))  // building full timeslice
	{
         nHits = fCurrentEvent->GetEntriesFast();
	 new((*fCurrentEvent)[nHits]) CbmMvdHit(*myHit);
	 //cout << endl << "new Hits in Output" << endl;
	}
    }
ClearTimeSlice(tStart, tStop);
};  
//--------------------------------------------------------------------------  

//--------------------------------------------------------------------------  
void CbmMvdSensorTrackingBuffer::ClearTimeSlice(Double_t tStart, Double_t tStop) {
  
  Int_t nEntries = fBuffer->GetEntriesFast();
CbmMvdHit* myHit;
Int_t time;
Int_t nHits;



for (Int_t i = 0; i < nEntries; i++)
    {
    myHit = (CbmMvdHit*)fBuffer->At(i);
    time = myHit->GetTimeStamp();
    if(tStart<=time<tStop)
	{
	 fBuffer->Remove(myHit);
	}
    }
  ;
  
};
//--------------------------------------------------------------------------  


//--------------------------------------------------------------------------  
 void CbmMvdSensorTrackingBuffer::SetInputArray(TClonesArray* inputStream){
 
	fBuffer->AbsorbObjects(inputStream);
	fBuffer->Compress();
	//cout << endl << "new Objects in Output buffer" << endl;
   
};
//--------------------------------------------------------------------------  




ClassImp(CbmMvdSensorTrackingBuffer)
