// ------------------------------------------------------------------------
// -----                  CbmMvdSensorTrackingBuffer header file              -----
// -----                   Created 14/02/12  by P. Stizmann            -----
// ------------------------------------------------------------------------

/**  CbmMvdSensorTrackingBuffer.h
 ** @author P. Sitzmann <philipp.sitzmann@stud.uni-frankfurt.de>
 ** Class used to resort MAPS-frames to Time-Slices
 ** Operates on CbmMvdHits
 **/

#ifndef CBMMVDSENSORTRACKINGBUFFER_H
#define CBMMVDSENSORTRACKINGBUFFER_H 1


#include "TObject.h"
#include "CbmMvdSensor.h"
#include "CbmMvdSensorBuffer.h"

#include "CbmMvdHit.h"

class TClonesArray;
class CbmMvdSensor;
using namespace std;

class CbmMvdSensorTrackingBuffer : public CbmMvdSensorBuffer
{

 public:

  /** Default constructor **/
  CbmMvdSensorTrackingBuffer();

  /** Destructor **/
  virtual ~CbmMvdSensorTrackingBuffer();
  
  /** Input/Output **/
  //=======================================================
  
  /** Send a new event to the buffer. The event will be absorbed but not processed.
   *The input - array will be emptied
   **/
  virtual void 		SetInputArray	(TClonesArray* inputStream);
  
  virtual TClonesArray* GetOutputArray() {SetPluginReady(false); return fCurrentEvent; };

  

  /** Data Processing **/
  //=======================================================
  
  virtual void 		Init		(CbmMvdSensor* mySensor);
  virtual void 		ExecChain		();
  virtual void 		BuildTimeSlice	(Double_t tStart, Double_t tStop);
  virtual void 		Finish		()				 {;};
  
 
  
  virtual void          ClearTimeSlice  (Double_t tStart, Double_t tStop);



  private: 

  Int_t ftimeStart;
  Int_t ftimeStop;
  Int_t ftimestep;
  ClassDef(CbmMvdSensorTrackingBuffer,1);

};


#endif
