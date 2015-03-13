/**@file CbmDaqMapCosy2014Much.h
 **@date 16.12.2014
 **author V. Friese <v.friese@gsi.de>
 **
 ** Provides the mapping of DAQ channel to detector channel
 ** 
 ** Valid for the MUCH setup of COSY beam test DEC 2014
 **/



#ifndef CBMDAQMAPCOSY2014MUCH_H
#define CBMDAQMAPCOSY2014MUCH_H 1

#include "CbmDaqMap.h"

class CbmDaqMapCosy2014Much : public CbmDaqMap
{

 public:

  /**   Default constructor   **/
  CbmDaqMapCosy2014Much();

  /**   Standard constructor 
   **@param iRun   run number
   **/
  CbmDaqMapCosy2014Much(Int_t iRun);

  /**   Destructor   **/
  virtual ~CbmDaqMapCosy2014Much();

  /**  System Id from ROC Id  **/
  Int_t GetSystemId(Int_t rocId);

  /**  STS station from ROC Id  **/
  Int_t GetStsStation(Int_t rocId);

  /**  MUCH station from ROC Id  **/
  Int_t GetMuchStation(Int_t rocId);

  /**  Fiber Hodoscope station from ROC Id  **/
  Int_t GetFiberHodoStation(Int_t rocId);

  /**  STS sensor side from ROC and NXYTER Id **/
  Int_t GetStsSensorSide(Int_t rocId);

  /** STS channel from ROC, NXYTER and NXYTER channel **/
  Int_t GetStsChannel(Int_t rocId, Int_t nxId, Int_t nxChannel);

  /** Fiber Hodoscope channel from ROC, NXYTER and NXYTER channel **/
  Int_t GetFiberHodoChannel(Int_t rocId, Int_t nxId, Int_t nxChannel);


  /**   Mapping
   **@param iRoc   ROC number (from hit message)
   **@param iNx    NXYter number (from hit message) 
   **@param iId    Channel ID (from hit message)
   **@param iStation  Station number (return)
   **@param iSector   Sector number ( return)
   **@param iSide     Front side (0) / Back side (1) ( return)
   **@param iChannel  Channel number (return)  
  **/
  Bool_t Map(Int_t iRoc, Int_t iNx, Int_t iId,
	     Int_t& iStation, Int_t& iSector, Int_t& iSide, Int_t& iChannel);
    


 private:

  void InitializeFiberHodoMapping();
  
  // --- Get FEB ID (used in the channel map) from ROC and NXYTER ID
  Int_t GetFebId(Int_t rocId, Int_t nxyterId);

  Int_t fFiberHodoFiber[128];  /** Mapping from fiber hodoscope feb channel to fiber number **/
  Int_t fFiberHodoPlane[128];  /** Mapping from fiber hodoscope feb channel to plane number 1=X, 2=Y **/
  Int_t fFiberHodoPixel[128];  /** Mapping from fiber hodoscope feb channel to pixel number **/



  ClassDef(CbmDaqMapCosy2014Much,1);

};

#endif



  
