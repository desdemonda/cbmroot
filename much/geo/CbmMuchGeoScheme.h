// -------------------------------------------------------------------------
// -----                   CbmMuchGeoScheme header file                -----
// -----                  Created 18/02/08  by E. Kryshen              -----
// -------------------------------------------------------------------------

/** CbmMuchGeoScheme
 *@author Evgeny Kryshen <e.kryshen@gsi.de>
 *@since 18.02.08
 *@version 1.0
 *
 */

#ifndef CbmMuchGeoScheme_H
#define CbmMuchGeoScheme_H 1

#include "CbmDetectorList.h"
#include "CbmMuchAddress.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TArrayD.h"
#include "TArrayI.h"
#include "TArrayC.h"
#include "TGeoCone.h"
#include <vector>
#include <map>

class CbmMuchStation;
class CbmMuchLayer;
class CbmMuchLayerSide;
class CbmMuchModule;
class CbmMuchModuleGem;
class CbmMuchPad;

using std::vector;
using std::map;

class CbmMuchGeoScheme: public TObject {

  public:

    /** Destructor.     */
    ~CbmMuchGeoScheme();

    static CbmMuchGeoScheme* Instance();
    /** Gets whether the geometry scheme is initialized. */
    Bool_t IsInitialized() { return fInitialized; }

    // Get geometry objects by indices
    CbmMuchStation* GetStation(Int_t iStation) const;
    CbmMuchLayer* GetLayer(Int_t iStation, Int_t iLayer) const;
    CbmMuchLayerSide* GetLayerSide(Int_t iStation, Int_t iLayer, Bool_t iSide) const;
    CbmMuchModule* GetModule(Int_t iStation, Int_t iLayer, Bool_t iSide, Int_t iModule) const;

    // Get geometry objects by detector id
    CbmMuchStation* GetStationByDetId(Int_t detId) const;
    CbmMuchLayer* GetLayerByDetId(Int_t detId) const;
    CbmMuchLayerSide* GetLayerSideByDetId(Int_t detId) const;
    CbmMuchModule* GetModuleByDetId(Int_t detId) const;

    static Int_t GetStationIndex(Int_t address)   { return CbmMuchAddress::GetElementId(address,kMuchStation);   }
    static Int_t GetLayerIndex(Int_t address)     { return CbmMuchAddress::GetElementId(address,kMuchLayer);     }
    static Int_t GetLayerSideIndex(Int_t address) { return CbmMuchAddress::GetElementId(address,kMuchLayerSide); }

    
    Int_t      GetNStations() const {return fStations->GetEntries();}
    Int_t      GetNAbsorbers() const {return fNabs;}
    TObjArray* GetStations() const {return fStations;}
    TObjArray* GetAbsorbers() const {return fAbsorbers;}
    TGeoCone*  GetMuchCave() const {return fMuchCave;}
    Double_t   GetMuchCaveZ0() const {return fMuchZ1+fMuchCave->GetDz();}
    Double_t   GetAbsorberZ0(Int_t i) const {return fMuchZ1+fAbsorberZ1[i]+((TGeoCone*) fAbsorbers->At(i))->GetDz();}
    Char_t     GetAbsorberMat(Int_t i) const {return fAbsorberMat[i];}

    Double_t GetActiveLx() const {return fActiveLx;}
    Double_t GetActiveLy() const {return fActiveLy;}
    Double_t GetActiveLz() const {return fActiveLz;}
    Double_t GetSpacerLx() const {return fSpacerLx;}
    Double_t GetSpacerLy() const {return fSpacerLy;}
    Double_t GetSpacerR() const {return fSpacerR;}
    Double_t GetSpacerPhi() const {return fSpacerPhi;}
    Double_t GetLayersDz(Int_t st) const {return fLayersDz[st];}

    void Init(TObjArray* stations);
    void Init(TString digiFileName);
    void CreatePointArrays();
    void CreateHitArrays();
    void CreateClusterArrays();
    void ClearPointArrays();
    void ClearHitArrays();
    void ClearClusterArrays();
    vector<CbmMuchModule*>    GetModules() const;
    vector<CbmMuchModuleGem*>    GetGemModules() const;
    vector<CbmMuchModule*>    GetModules(Int_t iStation) const;
    vector<CbmMuchLayerSide*> GetLayerSides(Int_t iStation) const;
    Int_t GetLayerSideNr(Int_t detId) const;

    void ReadGeoFile(const char* geoName);
    void Print();
    void CreateMuchCave();


  private:
    CbmMuchGeoScheme();
    void InitModules();
    void CreateAbsorbers();
    void CreateStations();
    CbmMuchStation* CreateStationGem(Int_t st);
    CbmMuchStation* CreateStationStraw(Int_t st);

    Int_t Intersect(Float_t x, Float_t y, Float_t dx, Float_t dy, Float_t r);
    static CbmMuchGeoScheme* fInstance;
    static Bool_t fInitialized;        // Defines whether the instance was initialized
    static Bool_t fModulesInitialized; // Defines whether grid of the instance was initialized

    vector<vector<CbmMuchModule*> > fModules;      //!
    vector<vector<CbmMuchLayerSide*> > fSides;     //!
    map<Int_t,Int_t> fMapSides;

    TObjArray* fStations; //!
    TObjArray* fAbsorbers;//!
    TGeoCone* fMuchCave; //!

    // Input parameters
    Double_t fMuchZ1; // MuchCave Zin position [cm]
    Double_t fAcceptanceTanMin; // Acceptance tangent min
    Double_t fAcceptanceTanMax; // Acceptance tangent max
    Int_t fNabs; // Number of absorbers
    Int_t fNst; // Number of stations

    // GEM module parameters
    Double_t fActiveLx; // Active volume lx [cm]
    Double_t fActiveLy; // Active volume ly [cm]
    Double_t fActiveLz; // Active volume lz [cm]
    Double_t fSpacerLx; // Spacer lx [cm]
    Double_t fSpacerLy; // Spacer ly [cm]
    Double_t fOverlapY; // Overlap along y axis [cm]

    // Straw module parameters
    Double_t fStrawLz;

    // Sector-type module parameters
    TArrayI fNSectorsPerLayer; // Number of sectors per layer
    Double_t fActiveLzSector;  // Active volume thickness [cm]
    Double_t fSpacerR;         // Spacer width in R [cm]
    Double_t fSpacerPhi;       // Spacer width in Phi [cm]
    Double_t fOverlapR;        // Overlap in R direction [cm]

    TArrayD fAbsorberZ1; // Absorber Zin position [cm] in the cave reference frame
    TArrayD fAbsorberLz; // Absorber thickness [cm]
    TArrayC fAbsorberMat; // Absorber material
    TArrayD fStationZ0; // Station Zceneter [cm] in  the cave reference frame
    TArrayI fNlayers; // Number of layers
    TArrayI fDetType; // Detector type
    TArrayD fLayersDz; // Distance between layers [cm]
    TArrayD fSupportLz; // Support thickness [cm]
    TArrayI fModuleDesign; /* 1 - detailed design (modules at two sides)
     * 0 - simple design (1 module per layer)     */

    CbmMuchGeoScheme(const CbmMuchGeoScheme&);
    CbmMuchGeoScheme& operator=(const CbmMuchGeoScheme&);

    ClassDef(CbmMuchGeoScheme,1);
};

#endif
