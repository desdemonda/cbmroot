// ------------------------------------------------------------------------
// -----                  CbmMvdSensorBuffer header file              -----
// -----                   Created 02/02/12  by M. Deveaux            -----
// ------------------------------------------------------------------------

/**  CbmMvdSensorBuffer.h
 ** @author M.Deveaux <deveaux@physik.uni-frankfurt>
 **
 ** Base class for the Buffers of the MVD sensor 
 ** Buffers aim to resort CBMRoot-Events / Point-streams into
 ** frames of the MAPS. Moreover, they are to resort frames into
 ** hit information used for a time slice.
 **
 **/

#ifndef CBMMVDSENSORBUFFER_H
#define CBMMVDSENSORBUFFER_H 1


#include "TObject.h"
#include "plugins/CbmMvdSensorPlugin.h"
#include <iostream>


class TClonesArray;
class CbmMvdSensor;

class CbmMvdSensorBuffer : public CbmMvdSensorPlugin
{

 public:

  /** Default constructor **/
  CbmMvdSensorBuffer();

  /** Destructor **/
  virtual ~CbmMvdSensorBuffer();
  
  /** Input/Output **/
  //=======================================================
  
  /** Send a new event to the buffer. The event will be absorbed but not processed.
   *The input - array will be emptied
   **/
  virtual void 		SendInputEvent	(TClonesArray* inputStream)	{;};
  
  /** Receives the current event from the buffer. The Event is defined by the functions
  *BuildTimeSlice or BuildMimosaFrame. The memory is not emtied, use Clear*-methods
  * to clear it explitly**/
  
  virtual TClonesArray* GetOutputArray()			{return fCurrentEvent;};
   
  /** Data Processing **/
  //=======================================================
  virtual void ExecChain(){;}
  virtual void 		Init		(CbmMvdSensor* mySensor)	 {fSensor=mySensor;};
  virtual void 		BuildTimeSlice	(Double_t tStart, Double_t tStop){;};
  virtual void 		Finish		()				 {;};
  
  
  /** Memory Management **/
  
  /** Clear()
   * 
   * Clears the buffer completely. 
   * Important: Clears also the output buffer delivered by GetCurrentEvent()
   * Make sure you don't need it.
   **/ 
  virtual void          Clear           ();                                
  
  /**Clears the objects related to a time periode from the buffer.
  * Use ClearTimeSlice(0,t) to clear all objects earlier than t
  * Not implemented in base class as data type of objects in buffer is unknown
  **/
  
  virtual void            ClearTimeSlice  (Double_t tStart, Double_t tStop) {;};
  
  
  
  /** Returns task type to a upper control unit **/
  const MvdSensorPluginType GetPluginType(){return buffer;};
  
  protected:

  TClonesArray* fBuffer;     // full data buffer
  TClonesArray* fCurrentEvent;
  CbmMvdSensor* fSensor;
 
  
  
  virtual void 		  BufferReady(){;};

private:
  CbmMvdSensorBuffer(const CbmMvdSensorBuffer&); 
  CbmMvdSensorBuffer operator=(const CbmMvdSensorBuffer&); 
  
  ClassDef(CbmMvdSensorBuffer,1);

};


#endif
