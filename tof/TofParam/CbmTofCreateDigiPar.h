// --------------------------------------------------------------------------
// -----          Header for the CbmTrdCreateDigiPar               ------
// -----              Created 06.06.08 by F.Uhlig                      ------
// --------------------------------------------------------------------------



#ifndef CBMTOFCREATEDIGIPAR_H
#define CBMTOFCREATEDIGIPAR_H

#include "FairTask.h"

#include "TArrayD.h"

#include <map>
#include <vector>

class CbmTofDigiPar;
class CbmTofCell;
class CbmTofGeoHandler;

class CbmTofCreateDigiPar : public FairTask {

  public:

    /** Default constructor **/
    CbmTofCreateDigiPar();

    /** Standard constructor **/
    CbmTofCreateDigiPar(const char *name, const char *title="CBM Task");

    /** Destructor **/
    virtual ~CbmTofCreateDigiPar();

    /** Initialisation **/
    virtual InitStatus ReInit();
    virtual InitStatus Init();
    virtual void SetParContainers();

    /** Executed task **/
    virtual void Exec(Option_t * option);

    /** Finish (called after each event) **/
    virtual void FinishEvent(){;}
 
   /** Finish task (called after all event) **/
    virtual void FinishTask();


private:

    void FillCellInfoFromGeoHandler(TString FullPath);
    void FillDigiPar();

    void FillCellMapRootGeometry();
    void FillCellMapAsciiGeometry();


    Int_t fSMType;      //super module type (needed for root geometry)
    Int_t fSModule;     //super module NR
    Int_t fCounter;     //module/counter number
    Int_t fGap;         //gap number (probably not needed)
    Int_t fCell;        //cell number
    Int_t fRegion;      //region (needed for ascii geometry)

    Double_t fSizex;
    Double_t fSizey;
    Double_t fX;
    Double_t fY;
    Double_t fZ;

    Int_t   fDetID; //Unique number for detector element (including gap)
    Int_t   fCellID; //Unique number for a detector cell (8 gaps add up to one cell)

    /** Map of Unique Tof Cell Id to corresponding TofCell **/
    std::map<Int_t, std::vector<CbmTofCell*> > fCellMap;                  //!
    std::map<Int_t, std::vector<CbmTofCell*> >::iterator fCellMapIt;      //!

    CbmTofDigiPar *fDigiPar;    //! pointer to digi parameters

    CbmTofGeoHandler* fGeoHandler;
  
    CbmTofCreateDigiPar(const CbmTofCreateDigiPar&);
    CbmTofCreateDigiPar& operator=(const CbmTofCreateDigiPar&);

    ClassDef(CbmTofCreateDigiPar,1)

    };
#endif //CBMTOFCREATEDIGIPAR_H
