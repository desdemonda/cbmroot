/** CbmMuchPad.h
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 02.08.12
 *@version 2.0
*
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 10.11.07
 *@version 1.0
 **
 ** Class describing a standalone pad
 **/
#ifndef CBMMUCHPAD_H
#define CBMMUCHPAD_H 1

#include "TArrayL64.h"
#include "CbmMuchAddress.h"
class CbmMuchSector;
class CbmMuchDigi;
class CbmMuchDigiMatch;
#include "CbmMuchModuleGem.h"
#include "vector"
using std::vector;

class CbmMuchPad {

public:
  CbmMuchPad();
  CbmMuchPad(Int_t address, Double_t x, Double_t y, Double_t dx, Double_t dy);
  virtual ~CbmMuchPad(){};
  Int_t    GetAddress()        const { return fAddress; }
  Double_t GetX()              const { return fX; }
  Double_t GetY()              const { return fY; }
  Double_t GetDx()             const { return fDx; }
  Double_t GetDy()             const { return fDy; }
  Double_t GetDxy()            const { return 0.; }
  Int_t GetDigiIndex()         const { return fDigiIndex; }
  CbmMuchDigi* GetDigi()  const { return fDigi; }
  CbmMuchDigiMatch* GetMatch() const { return fMatch; }
  vector<CbmMuchPad*> GetNeighbours() const { return fNeighbours; }
  void SetNeighbours(vector<CbmMuchPad*> neighbours) { fNeighbours = neighbours; }
  void SetDigiIndex(Int_t iDigi)             { fDigiIndex = iDigi; }
  virtual void SetFired(Int_t iDigi, Int_t ADCcharge, Int_t nADCChannels=256){}

  Int_t GetSectorIndex()  { return CbmMuchAddress::GetElementId(fAddress,kMuchSector);  }
  Int_t GetChannelIndex() { return CbmMuchAddress::GetElementId(fAddress,kMuchChannel); }
protected:
  Int_t               fAddress;    // Detector ID (including module number)
  Double_t            fX;          // X-coordinate of the pad center
  Double_t            fY;          // Y-coordinate of the pad center
  Double_t            fDx;         // X-pad width
  Double_t            fDy;         // Y-pad width
  Int_t               fDigiIndex;  // Index of the corresponding CbmMuchDigi (if any)
  CbmMuchDigi*        fDigi;       //! pointer to current digi
  CbmMuchDigiMatch*   fMatch;      //! pointer to current digi match
  vector<CbmMuchPad*> fNeighbours; //! Array of neighbour pads

 private:
  CbmMuchPad(const CbmMuchPad&);
  CbmMuchPad operator=(const CbmMuchPad&);
};
#endif
