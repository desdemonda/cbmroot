/**
 * \file CbmTrdCreateDigiPar.h
 * \brief Assign pad layout to TRD Modules.
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \date 06/06/2008
 *
 * Updated 20/05/2013 by Andrey Lebedev <andrey.lebedev@gsi.de>
 *
 * Create digitisation parameter map.
 * Read pad geometry from CbmTrdPads.h,
 * assign pad layout to sectors in TRD modules.
 */

#ifndef CBMTRDCREATEDIGIPAR_H
#define CBMTRDCREATEDIGIPAR_H

#include "FairTask.h"

#include "TArrayD.h"

#include <map>

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdGeoHandler;

/**
 * \class CbmTrdCreateDigiPar
 * \brief Assign pad layout to TRD Modules.
 */
class CbmTrdCreateDigiPar : public FairTask
{
public:

   /**
    * \brief Default constructor.
    **/
   CbmTrdCreateDigiPar();

   /**
    * \brief Destructor.
    **/
   virtual ~CbmTrdCreateDigiPar();

   /**
    * \breif Inherited from FairTask.
    **/
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    **/
   virtual void SetParContainers();

   /**
    * \brief Inherited from FairTask.
    **/
   virtual void Exec(Option_t* option);

   /**
    * \breif Inherited from FairTask.
    **/
   virtual void Finish();

private:

   void CreateModule(
         const TString& path);

   void FillModuleMap();

   void FillDigiPar();

   Int_t fMaxSectors; // Maximum number of sectors for all modules

   // Map of Unique TRD Module Id to corresponding CbmTrdModule
   std::map<Int_t, CbmTrdModule*> fModuleMap;

   CbmTrdDigiPar* fDigiPar; // pointer to digi parameters

   CbmTrdGeoHandler* fGeoHandler;

   CbmTrdCreateDigiPar(const CbmTrdCreateDigiPar&);
   CbmTrdCreateDigiPar& operator=(const CbmTrdCreateDigiPar&);

   ClassDef(CbmTrdCreateDigiPar, 4)
};
#endif //CBMTRDCREATEDIGIPAR_H
