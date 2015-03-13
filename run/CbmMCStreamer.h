/** CbmMCEpochiser.h
 *@author Volker Friese v.friese@gsi.de
 *@since 13.11.2009
 *@version 1.0
 **
 ** CBM task for converting event-based MC data into a data stream.
 ** The data stream is grouped in epoch blocks.
 ** The MC event time is generated according to an assumed beam profile.
 **/


#ifndef CBMMCSTREAMER_H
#define CBMMCSTREAMER_H 1

#include "sts/CbmStsPoint.h"
#include "much/CbmMuchPoint.h"

#include "FairTask.h"

#include <map>


class TClonesArray;
class CbmMCEpoch;


using namespace std;


class CbmMCStreamer : public FairTask
{

 public:

  /**   Default constructor   **/
  CbmMCStreamer();


  /**   Standard constructor
   **@param interactionRate   Interaction rate [1/s]
   **@param beamProfile       0 = constant, 1 = exponential
   **@param epochLength       Duration of epoch [ns]
   **@param persistence       Persistence flag for output arrays
   **@param verbosity         Verbosity level (default = 0) 
   **@param name              Task name (default = MCStreamer)
   **/
  CbmMCStreamer(Double_t interactionRate,
		 Int_t beamProfile,
		 Double_t epochLenth,
		 Bool_t persistency = kTRUE,
		 Int_t verbosity = 0,
		 const char* name = "MCStreamer");


  /**   Destructor   **/
  virtual ~CbmMCStreamer();


  /**   Execution   **/
  virtual void Exec(Option_t* opt="");


  /**   End-of-run action   **/
  virtual void Finish();


  /**   Initialisation   **/
  virtual InitStatus Init();



 private:

  Double_t fEventRate;       /**  Interaction rate [1/s]      **/
  Double_t fEventTau;        /**  Average event spacing [ns]  **/
  Int_t    fBeamProfile;     /**  Beam profile: 0=const., 1 = exponential **/
  Bool_t   fPersistence;     /**  Persistence flag for output array **/
  Double_t fEpochLength;     /**  Duration of epoch [ns] **/

  Int_t       fEventId;      /**  ID of current event  **/
  Double_t    fEventTime;    /**  Time of current event [ns] **/
  CbmMCEpoch* fEpoch;        /**  Current epoch   **/

  Bool_t fEpochIsChanged;    /**  Flag whether current epoch was changed  **/
  Bool_t fMarkNewEpoch;      /**  Flag to create a new epoch at the nxt call to Ecec **/


  /**   Input arrays   **/   
  TClonesArray* fInMvd;
  TClonesArray* fStsPoints;
  TClonesArray* fMuchPoints;


  /**   Buffers   **/
  multimap<Double_t, CbmStsPoint>  stsBuffer;
  multimap<Double_t, CbmMuchPoint> muchBuffer;


  /**   Activate / dactivate subtasks   **/
  void ActivateTasks(Bool_t activate);


  /**  Start new epoch   **/
  void NextEpoch();


  /**   Show number of points in buffer   **/
  void PrintBuffer();


  /**   Process the buffer   **/
  void ProcessBuffer();  


  /**   Read one event into the buffer  
   *@value  Number of points filled into the buffer
   **/
  Int_t ReadEvent();

  CbmMCStreamer(const CbmMCStreamer&);
  CbmMCStreamer& operator=(const CbmMCStreamer&);

  ClassDef(CbmMCStreamer,1);

};

#endif
