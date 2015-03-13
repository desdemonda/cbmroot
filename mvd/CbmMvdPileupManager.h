// -------------------------------------------------------------------------
// -----                  CbmMvdPileupManager header file              -----
// -----                   Created 08/11/06  by V. Friese              -----
// -------------------------------------------------------------------------

/**  CbmStsPileupManager.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Auxiliary class for the MvdHitProducer. It provides MvdPoints from
 ** pileup events taken from a proper file. These background events
 ** are stored into a buffer for fast access.
 **
 ** Previously CbmStsPileupManager (V. Friese, 24/05/05)
 **
 **/

#ifndef CBMMVDPILEUPMANAGER_H
#define CBMMVDPILEUPMANAGER_H 1


#include "TString.h"
#include "TObject.h"
#include "TObjArray.h"

class TClonesArray;


class CbmMvdPileupManager : public TObject
{

 public:

  /** Default constructor **/
  CbmMvdPileupManager();


  /** Standard constructor 
   *@param fileName   Name of background file
   *@param branchName name of branch to be buffered
   *@param nEvents    Number of pile-up events
   **/
  CbmMvdPileupManager(TString fileName, TString branchName = "MVDPoint",
		      Int_t nEvents = 500);


  /** Destructor **/
  virtual ~CbmMvdPileupManager();


  /** Returns the number of events in the buffer **/
  Int_t GetNEvents() { return fBuffer->GetEntriesFast(); };


  /** Get an event from the background buffer
   *@param  iEvent  Event number
   *@return Pointer to TClonesArray
   **/
  TClonesArray* GetEvent(Int_t iEvent);

 private:

  TObjArray* fBuffer;     // Buffer of background events


  /** Fill the background buffer from the background file
   *@param fileName   Name of background file
   *@param branchName name of branch to be buffered
   *@param nEvents    Number of events to be read from file
   *@return  Number of buffered events
   **/
  Int_t FillBuffer(TString fileName, TString branchName, 
		    Int_t nEvents);

  CbmMvdPileupManager(const CbmMvdPileupManager&);
  CbmMvdPileupManager& operator=(const CbmMvdPileupManager&);

  ClassDef(CbmMvdPileupManager,1);

};


#endif
