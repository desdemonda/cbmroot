/** CbmMuchModule.h
 *
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds geometry parameters of much modules
 **
 **/

#ifndef CBMMUCHMODULE_H
#define CBMMUCHMODULE_H 1

#include "CbmMuchGeoScheme.h"

#include "TObject.h"
#include "TVector3.h"
#include "TPave.h"
#include "map"

using std::multimap;
using std::pair;

class CbmMuchModule : public TPave{
public:
  /** Default constructor **/
  CbmMuchModule();
  /** Standard constructor
   *@param iStation  Station index
   *@param iLayer    Layer index
   *@param iSide     Defines side of the layer (0 - Front, 1 - Back)
   *@param iModule   Module index
   *@param position  Location of the module center in global c.s. (all dimensions in [cm])
   *@param size      Size of the module (all dimensions in [cm])
   *@param cutRadius Radius of the cut (if any, otherwise = -1.) [cm]
   **/
  CbmMuchModule(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius);
  /** Destructor **/
  virtual ~CbmMuchModule(){}

  /** Accessors */
  Int_t    GetDetectorId()    const { return fDetectorId; }
  Double_t GetCutRadius()     const { return fCutRadius; }
  TVector3 GetSize()          const { return fSize; }
  TVector3 GetPosition()      const { return fPosition; }
  Int_t    GetDetectorType()  const { return fDetectorType; }
  TClonesArray* GetPoints()   const { return fPoints;     }
  TClonesArray* GetHits()     const { return fHits;       }
  TClonesArray* GetClusters() const { return fClusters;   }

  virtual Bool_t InitModule() {return kTRUE;}
  virtual void DrawModule(Color_t color) {}

  void SetPoints(TClonesArray* points)     { fPoints   = points;   }
  void SetHits  (TClonesArray* hits)       { fHits     = hits;     }
  void SetClusters(TClonesArray* clusters) { fClusters = clusters; }
  /** */
  void AddDigi(Double_t time,Int_t id) { fDigis.insert(pair<Double_t,Int_t>(time,id)); }
  /** */
   void ClearDigis() { fDigis.clear(); }
   /** */
   multimap<Double_t,Int_t> GetDigis() { return fDigis; } 

protected:
  Int_t                  fDetectorId;            // Unique detector ID
  Int_t                  fDetectorType;          // Detector type
  Double_t               fCutRadius;             // Radius of the cut (if any, otherwise = -1.) [cm]
  TVector3               fSize;                  // Size vector of the module (all dimensions in [cm])
  TVector3               fPosition;              // Location vector of the module center in global c.s. (all dimensions in [cm])
  TClonesArray*          fPoints;                //!
  TClonesArray*          fHits;                  //!
  TClonesArray*          fClusters;              //!
  multimap<Double_t,Int_t> fDigis;               //!
 
 private:
  CbmMuchModule(const CbmMuchModule&);
  CbmMuchModule& operator=(const CbmMuchModule&);

  ClassDef(CbmMuchModule,2);
};
#endif


