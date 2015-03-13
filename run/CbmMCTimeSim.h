/** @file CbmMCTimeSim.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 8 February 2012
 **/


#ifndef CBMMCTIMESIM_H
#define CBMMCTIMESIM_H 1

#define NDET 20


#include <vector>

#include "TStopwatch.h"

#include "FairMCPoint.h"
#include "FairTask.h"

#include "CbmDetectorList.h"
#include "CbmMCEventHeader.h"

class CbmMCBuffer;


class TClonesArray;



using namespace std;


/** @class CbmMCTimeSim
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 8 February 2012
 ** @brief CBM task class for generating a time-based stream of MCPoints
 **
 ** This class reads transport data (MCPoints) event-by-event into buffers.
 ** For each event, a start time is generated according to the specified
 ** interaction rate and beam profile, and the absolute time (since run start)
 ** is calculated for each MCPoint. The MCPoints are sorted w.r.t. absolute
 ** time within the buffers. 
 **
 ** The buffered points can be retrieved from CbmMCBuffer by the method
 ** GetNextPoint(DetectorId det). This will deliver a time-ordered
 ** sequence of MCPoints (to be casted to the concrete type).
 ** The tasks using the buffered points should call this method until it
 ** returns a NULL pointer.
 **/
class CbmMCTimeSim : public FairTask
{


 public:

  /**   Default constructor  **/
  CbmMCTimeSim();


  /**   Standard constructor
   ** @param rate     Interaction rate
   ** @param profile  Beam profile type. 0 = constant, 1 = Poissonian
   ** @param name     Task name (default is MCBuffer)
   **/
  CbmMCTimeSim(Double_t rate, 
	      Int_t profile = 2, 
	      const char* name = "MCTimeSim");


  /**   Destructor  **/
  virtual ~CbmMCTimeSim();


  /**   Event rate
   ** @value  event rate [1/s]
   **/
  Double_t GetEventRate() const { return fEventRate; }


  /**   Beam profile  
   ** @value  beam profile (1 = constant, 2 = exponential)
   **/
  Int_t GetBeamProfile() const { return fBeamProfile; }


  /**   Maximal buffer size
   ** @value  Maximal buffer size [MB]
   **/
  Double_t GetMaxBufferSize() const { return fMaxBufferSize; }


  /**   Set maximal buffer size
   ** @param size  Maximal buffer size< Default is 1 GB
   **/
  void SetMaxBufferSize(Double_t size) { fMaxBufferSize = size; }





 private:

  /** Average event rate [1/s] **/
  Double_t fEventRate;


  /** Time profile of beam.
   ** 0 = constant
   ** 1 = exponential (Poissonian)  
   **/
  Int_t    fBeamProfile;


  /** Maximal size [MB] **/
  Double_t   fMaxBufferSize;  ///< Maximal buffer size [MB]
  Int_t      fEventId;        ///< ID of current event
  Double_t   fEventTime;      ///< Start time of current event
  Int_t      fNofEvents;      ///< Event counter
  Double_t   fTimeEventFirst; ///< Time of first event
  Double_t   fTimeEventLast ; ///< Time of last event
  TStopwatch fTimer;          ///< ROOT time


  // --- Input
  CbmMCEventHeader*     fEvent;        ///< Input of CbmMCEventHeader
  vector<TClonesArray*> fPointArrays;  ///< Input MCPoint arrays


  /** Create event time
   ** @value Start time of new event [ns]
   **
   ** Samples the start time of the next event from the given
   ** interaction rate and beam profile.
   **/
  Double_t CreateEventTime();


  /**   Task execution  **/
  virtual void Exec(Option_t* opt = "");


  /**   End-of-event action
   **
   **   Deletes all retrieved MCPoints from the buffers
   **/
  virtual void Finish();


  /**   Initialisation   **/
  virtual InitStatus Init();


  CbmMCTimeSim(const CbmMCTimeSim&);
  CbmMCTimeSim& operator=(const CbmMCTimeSim&);

  ClassDef(CbmMCTimeSim,1);

};

#endif
