// -------------------------------------------------------------------------
// -----                  CbmMvdSensorFrameBuffer source file              -----
// -----                  Created 02.02.2012 by M. Deveaux            -----
// -------------------------------------------------------------------------
#include "CbmMvdSensorFrameBuffer.h"

#include "TClonesArray.h"
#include "TObjArray.h"
#include "CbmMvdPoint.h"




// -----   Default constructor   -------------------------------------------
CbmMvdSensorFrameBuffer::CbmMvdSensorFrameBuffer() 
 : CbmMvdSensorBuffer(),
   fCurrentEvent(NULL),
   fOutputPoints(NULL),
   lastFrame(-1), 
   thisFrame(-1),
   fSensorData(NULL),
   bOverflow(kFALSE),
   currentTime(0.)
{
  fBuffer     = NULL;
  bFlag       = false;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMvdSensorFrameBuffer::~CbmMvdSensorFrameBuffer() {
  fBuffer->Delete();
  delete fBuffer;
}
// -------------------------------------------------------------------------

void CbmMvdSensorFrameBuffer::Init (CbmMvdSensor* mySensor) {
  
  bOverflow		= false;
  bFlag 		= false;
  fSensor		= mySensor;
  fSensorData		= fSensor->GetDataSheet();
  fBuffer=new TClonesArray("CbmMvdPoint",1000);
  
  fCurrentEvent	= new TClonesArray("CbmMvdPoint",1000);
  
  fOutputPoints = new TClonesArray("CbmMvdPoint", 1000);
  
  
 cout << "-I- " << GetName() << ": Initialisation of sensor " << fSensor->GetName() << endl;

  lastFrame = 0;
 
};
// -------------------------------------------------------------------------

void CbmMvdSensorFrameBuffer::ExecChain(){ 
 
SetPluginReady(false);
currentTime = fSensor->GetCurrentEventTime(); 
thisFrame = fSensor->GetFrameNumber(fSensorData->GetNPixelsY(), currentTime);

 if (thisFrame > lastFrame)
      {
      //cout << endl << "Detected full Frame "<< lastFrame << " on sensor " << fSensor->GetName() << endl;
      fOutputPoints->AbsorbObjects(fCurrentEvent);
      //
      lastFrame = thisFrame;
 
      if (fOutputPoints->GetEntriesFast()>0)
	  {
	  SetPluginReady(true);
// 	 s cout << endl << "Buffer is ready for readout on sensor " << fSensor->GetName()<< endl << endl;
//  	  fOutputPoints->Print();
	  }
      }
BuildMimosaFrame(thisFrame); // compute all hits in this frame for this Event
ClearFrame(thisFrame);	      // delete all hits in this frame from input      
}




// -------------------------------------------------------------------------

void CbmMvdSensorFrameBuffer::SendInputArray(TClonesArray* inputStream)
{ 
  
  CbmMvdPoint* point;
  Int_t nEntries = inputStream->GetEntriesFast();
  Int_t i=0;
   while ( i<nEntries )   {
    point= (CbmMvdPoint*) inputStream->At(i);
    //cout << endl <<"check if point belongs to this sensor" << endl;
    if (point->GetDetectorID()==fSensor->GetDetectorID()) {
     
      new((*fBuffer)[fBuffer->GetEntriesFast()]) CbmMvdPoint(*((CbmMvdPoint*)inputStream->At(i)));
      nEntries = inputStream->GetEntriesFast();
     // cout << endl << "New Input registered at " << fSensor->GetName() << endl;
      
    };
    i++;
  };
 
}  
  
// -------------------------------------------------------------------------

void CbmMvdSensorFrameBuffer::SetInput(CbmMvdPoint* point)
{ 
  
  new((*fBuffer)[fBuffer->GetEntriesFast()]) CbmMvdPoint(*((CbmMvdPoint*)point));

}  
// -------------------------------------------------------------------------

void CbmMvdSensorFrameBuffer::BuildMimosaFrame(Int_t frameNumber) {

  /**Builds a new event in TClonesArray.
   * Important notes:
   * 
   * - The previously built event is cleared. As only a pointer on related TClonesArray
   * is provided by the buffer (not a copy!), this clearing also affects the outside 
   * "non-copies" the previous event.
   * Make sure you have processed the previous event or (if needed) perform a manual copy
   * externally.
   * 
   * - By building a new event, the objects in the local input buffer are not cleared.
   * To avoid memory leaks, clear them manually with the "ClearFrame" methode.
   *  
  **/
  
  
  CbmMvdPoint* point; 
  CbmMvdPoint* myEvent;
  Int_t pointFrameNumber;
  Double_t position[3];
  Int_t pixelX,pixelY;
  Int_t nPoints;
  
				  
  
  for(Int_t i=0; i<fBuffer->GetEntriesFast(); i++){
    
    point=(CbmMvdPoint*)fBuffer->At(i);
    //compute pixel number related to the frameNumber
    position[0]=point->GetX();
    position[1]=point->GetY();
    position[2]=point->GetZ();
    
  
    fSensor->TopToPixel(position, pixelX, pixelY);
    
    //Get frame number from the CbmMvdSensor according to assumed position of the 
    //rolling shutter.
    pointFrameNumber = fSensor->GetFrameNumber(pixelY, point->GetAbsTime());
   // cout << endl << "current pointFrameNumber " << pointFrameNumber << endl;
 
    if(pointFrameNumber < frameNumber)
    {
     cout << endl << "super error" << endl; 
    }
   
     // if it belongs to the frame, copy it to the event buffer
    if(pointFrameNumber==frameNumber){
     nPoints=fCurrentEvent->GetEntriesFast();
      new((*fCurrentEvent)[nPoints]) CbmMvdPoint(*point);
      myEvent=(CbmMvdPoint*)fCurrentEvent->At(nPoints);
      myEvent->SetTime(((pixelY*(115e3/576)) + (115000*(frameNumber))));
      myEvent->SetFrameNr(frameNumber);
//       cout << endl << "new Point in frame " << frameNumber << " in row " << pixelY << endl;
//       cout << "at time " << point->GetAbsTime() << endl;
//       cout << "readouttime is " << myEvent->GetTime() << " ns" << endl;
//       cout << "at station " << point->GetStationNr() << endl << endl;
   }  
    
  };
 
};  
 
void CbmMvdSensorFrameBuffer::ClearFrame(Int_t frameNumber) {

  /**Builds a new event in TClonesArray.
   * 
   * Removes objects related to a frame from the internal buffer
   *  
   **/
  
  CbmMvdPoint* point;
  Int_t pointFrameNumber;
  Double_t position[3];
  Int_t pixelX,pixelY;
  Int_t nPoints;
  
  for(Int_t i=0; i<fBuffer->GetEntriesFast(); i++){
    
    //compute pixel number related to the frameNumber
    
    point=(CbmMvdPoint*)fBuffer->At(i);
    position[0]=point->GetX();
    position[1]=point->GetY();
    position[2]=point->GetZ();
        
    fSensor->TopToPixel(position, pixelX, pixelY);
    
    //Get frame number from the CbmMvdSensor according to assumed position of the 
    //rolling shutter.
    pointFrameNumber = fSensor->GetFrameNumber(pixelY, point->GetAbsTime());
    
    // if it belongs to the frame, delete it from the internal buffer
    
    if(pointFrameNumber==frameNumber){
      fBuffer->RemoveAt(i);
      //cout << endl << "new Point in frame " << frameNumber << " removed from Buffer" << endl;
      
    };
    
  };
 
  fBuffer->Compress(); //defrag buffer
  
};   
 
void CbmMvdSensorFrameBuffer::ClearTimeSlice  (Double_t tStart, Double_t tStop) {
  
  CbmMvdPoint* point;
  
  for (Int_t i=0; i<fBuffer->GetEntriesFast(); i++){
    
    point=(CbmMvdPoint*)fBuffer->At(i);
    if((point->GetTime()>tStart) && (point->GetTime()<tStop)){fBuffer->RemoveAt(i);}
  };
  
  fBuffer->Compress(); // defrag
      
};


  


ClassImp(CbmMvdSensorFrameBuffer)
