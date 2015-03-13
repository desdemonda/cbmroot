/**@file CbmDaqMap.h
 **@date 03.012.2009
 **author V. Friese <v.friese@gsi.de>
 **
 ** Provides the mapping of DAQ channel to detector channel
 ** 
 ** At the moment: implemented only for STS stations
 **/



#ifndef CBMDAQMAPCOSY2014_H
#define CBMDAQMAPCOSY2014_H 1

#include "CbmDaqMap.h"

class CbmDaqMapCosy2014 : public CbmDaqMap
{

 public:

  /**   Default constructor   **/
  CbmDaqMapCosy2014();

  /**   Standard constructor 
   **@param iRun   run number
   **/
  CbmDaqMapCosy2014(Int_t iRun);

  /**   Destructor   **/
  virtual ~CbmDaqMapCosy2014();

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
  void InitializeStsMapping();
  void InitializeRocToSystemArray();
  void InitializeRocToStsStation();
  void InitializeRocToHodoStation();
  
  static const Int_t fMaxNyterChannels=128;
  static const Int_t fMaxNumRoc = 8;
  static const Int_t fMaxStsRoc = 6;

  /** Mapping from fiber hodoscope feb channel to fiber number **/
  Int_t fFiberHodoFiber[fMaxNyterChannels];  
  /** Mapping from fiber hodoscope feb channel to plane number 1=X, 2=Y **/
  Int_t fFiberHodoPlane[fMaxNyterChannels];  
  /** Mapping from fiber hodoscope feb channel to pixel number **/
  Int_t fFiberHodoPixel[fMaxNyterChannels];  

  Int_t fRocToSystem[fMaxNumRoc];    //!
  Int_t fRocToStsStation[fMaxNumRoc]; //!
  Int_t fRocToHodoStation[fMaxNumRoc];  //!

  // Sts strip from RocId, NxyterId, NxyterCh
  Int_t fStsStrip[fMaxStsRoc][2][fMaxNyterChannels];

  ClassDef(CbmDaqMapCosy2014,2);

};

#endif



  
