#ifndef CBMFIBERHODOMAPPING_H
#define CBMFIBERHODOMAPPING_H 1


#include "TObject.h"
#include "TString.h"


class CbmFiberHodoMapping : public TObject
{


 public:

  /**   Default constructor   **/
  CbmFiberHodoMapping();


  /**   Destructor   **/
  virtual ~CbmFiberHodoMapping();

  /**  Fiber Hodoscope station from ROC Id  **/
  Int_t GetFiberHodoStation(Int_t rocId);

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
  
  Int_t fFiberHodoFiber[128]; //!  /** Mapping from fiber hodoscope feb channel to fiber number **/
  Int_t fFiberHodoPlane[128]; //! /** Mapping from fiber hodoscope feb channel to plane number 1=X, 2=Y **/
  Int_t fFiberHodoPixel[128]; //! /** Mapping from fiber hodoscope feb channel to pixel number **/

  ClassDef(CbmFiberHodoMapping,1);

};

#endif



  
