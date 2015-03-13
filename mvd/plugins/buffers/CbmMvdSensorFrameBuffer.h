// ------------------------------------------------------------------------
// -----                  CbmMvdSensorFrameBuffer header file              -----
// -----                   Created 02/02/12  by M. Deveaux            -----
// ------------------------------------------------------------------------

/**  CbmMvdSensorFrameBuffer.h
 ** @author M.Deveaux <deveaux@physik.uni-frankfurt>
 **
 ** Class used to resort GEANT-Events to MAPS-frames
 ** Operates on CbmMvdPoints
 **
 ** Because of the new FairWriteoutBuffer concept most of this code has to be changed or can be simplified 
 **
 **/

#ifndef CBMMVDSENSORFRAMEBUFFER_H
#define CBMMVDSENSORFRAMEBUFFER_H 1

#include <iostream>
#include "TObject.h"
#include "CbmMvdSensor.h"
#include "CbmMvdSensorBuffer.h"

#include "CbmMvdHit.h"

using std::cout;
using std::endl;

class TClonesArray;
class CbmMvdSensorDataSheet;

class CbmMvdSensorFrameBuffer : public CbmMvdSensorBuffer
{

 public:

  /** Default constructor **/
  CbmMvdSensorFrameBuffer();

  /** Destructor **/
  virtual ~CbmMvdSensorFrameBuffer();
  
  /** Input/Output **/
  
  // Send a new event to the buffer. The event will be absorbed but not processed.
  // The input - array will be emptied
  void SendInputArray(TClonesArray* inputStream);
  void SetInput(CbmMvdPoint* point);
  
  // Receives the current event from the buffer. The Event is defined by the functions
  // BuildTimeSlice or BuildMimosaFrame. The memory is not emtied, use Clear*-methods
  // to clear it explitly
  TClonesArray* GetOutputArray() {SetPluginReady(false); return fOutputPoints; };
 
  
  /** Data Processing **/
 
  virtual void 		ExecChain();
  virtual void 		Init		(CbmMvdSensor* mySensor);
  virtual void 		BuildTimeSlice	(Double_t tStart, Double_t tStop){
			cout<<"Do not use " << GetName() << "::BuildTimeSlice()"<< endl;};
  
  /**BuildMimosaFrame: Provides a TClonesArray containing all points related to a frame.
   * Important notes:
   * 
   * - Obviously: Works only if the sensor time is correctly set and if all points were
   * previously loaded to the buffer by means of SendInputEvent().
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
  virtual void		BuildMimosaFrame(Int_t frameNumber);
  
  virtual void 		Finish		()				 {;};
  
  
  /** Memory Management **/
  //==============================================================  
   /** ClearFrame()
    * Clears the objects related to a dedicated MAPS-frame from the buffer
   **/
  virtual void          ClearFrame      (Int_t frameNumber);              
  
  /** ClearTimeSlice() 
   * Clears the objects related to a time periode from the buffer.
   * Use ClearTimeSlice(0,t) to clear all objects earlier than t
   **/
  
  virtual void          ClearTimeSlice  (Double_t tStart, Double_t tStop);

  private:
   
  
  TClonesArray* fCurrentEvent;
  TClonesArray* fOutputPoints;

  Int_t lastFrame, thisFrame; 
  CbmMvdSensorDataSheet* fSensorData;
  Bool_t bOverflow ;
  Double_t currentTime;

  CbmMvdSensorFrameBuffer(const CbmMvdSensorFrameBuffer&);
  CbmMvdSensorFrameBuffer operator=(const CbmMvdSensorFrameBuffer&);

 ClassDef(CbmMvdSensorFrameBuffer,1);

};


#endif
