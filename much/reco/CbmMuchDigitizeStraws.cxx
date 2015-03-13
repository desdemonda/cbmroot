/**
 * \file CbmMuchDigitizeStraws.cxx
 **/

#include "CbmMuchDigitizeStraws.h"
#include "CbmMuchPoint.h"
#include "CbmMuchModuleStraws.h"
#include "CbmMuchStrawDigi.h"
#include "CbmMuchDigi.h"
#include "FairRootManager.h"
#include "TClonesArray.h"

#include <iostream>

// -------------------------------------------------------------------------
CbmMuchDigitizeStraws::CbmMuchDigitizeStraws(const char* digiFileName)
    : FairTask("CbmMuchDigitizeStraws")
    , fGeoScheme(CbmMuchGeoScheme::Instance())
    , fDigiFile(digiFileName)
    , fPoints(NULL)
    , fDigis(NULL)
    , fDigiMatches(NULL)
    , fDimens(1)
{
}

// -------------------------------------------------------------------------
CbmMuchDigitizeStraws::~CbmMuchDigitizeStraws()
{
    /*
     if (fDigis) {
        fDigis->Delete();
        delete fDigis;
     }
     if (fDigiMatches) {
        fDigiMatches->Delete();
        delete fDigiMatches;
     }
    */
}

// -------------------------------------------------------------------------
void CbmMuchDigitizeStraws::Exec(Option_t* opt)
{

    if (fDimens == 2)
    {
        Exec2D();
        return;
    }

    fDigis->Clear();
    fDigiMatches->Clear();

    Int_t nofNotUsable = 0; // Counter for not usable points
    Int_t nofFailed = 0;
    Int_t iDigi = 0; // Digi counter
    Int_t nofPoints = fPoints->GetEntriesFast();
    for (Int_t iPoint = 0; iPoint < nofPoints; iPoint++)
    {
        const CbmMuchPoint* point = static_cast<const CbmMuchPoint*>(fPoints->At(iPoint));

        // Take only usable points
        if (!point || !point->IsUsable())
        {
            nofNotUsable++;
            continue;
        }

        CbmMuchModule* module = fGeoScheme->GetModuleByDetId(point->GetDetectorID());
        if (!module)
        {
            nofFailed++;
            continue;
        }
        // Process only appropriate module types
        if (module->GetDetectorType() != 2)
            continue;

        // Produce straw digi
        Double_t x = (point->GetXIn() + point->GetXOut()) / 2.;
        Double_t y = (point->GetYIn() + point->GetYOut()) / 2.;
        Double_t z = (point->GetZIn() + point->GetZOut()) / 2.;
        CbmMuchStrawDigi* digi = new ((*fDigis)[iDigi]) CbmMuchStrawDigi(point->GetDetectorID(), x, y, z, point->GetTime());
        CbmMuchDigiMatch* match = new ((*fDigiMatches)[iDigi]) CbmMuchDigiMatch();
        match->AddLink(CbmLink(1., iPoint));
        iDigi++;
    }

    static Int_t eventNo = 0;
    LOG(INFO) << "CbmMuchDigitizeStraws::Exec eventNo=" << eventNo++ << " points=" << nofPoints << " failed=" << nofFailed << " not usable=" << nofNotUsable
              << " digis=" << fDigis->GetEntriesFast() << FairLogger::endl;
}

// -------------------------------------------------------------------------
void CbmMuchDigitizeStraws::Exec2D()
{
    // Digitize in 2-D case (to repeat data flow for GEMs)

    Int_t nofNotUsable = 0; // Counter for not usable points
    Int_t nofFailed = 0;
    //  Int_t iDigi = 0; // Digi counter
    Int_t nofPoints = fPoints->GetEntriesFast();
    Int_t nofCreatedDigis = 0;

    for (Int_t iPoint = 0; iPoint < nofPoints; iPoint++)
    {
        const CbmMuchPoint* point = static_cast<const CbmMuchPoint*>(fPoints->At(iPoint));

        // Take only usable points
        if (!point || !point->IsUsable())
        {
            nofNotUsable++;
            continue;
        }

        CbmMuchModule* module = fGeoScheme->GetModuleByDetId(point->GetDetectorID());
        if (!module)
        {
            nofFailed++;
            continue;
        }

//        if (point->GetZ() > 300.) {
//           Int_t detId = point->GetDetectorID();
//
//           Int_t stationId = CbmMuchAddress::GetStationIndex(detId);
//           Int_t layerId = CbmMuchAddress::GetLayerIndex(detId);
//           Int_t sideId = CbmMuchAddress::GetLayerSideIndex(detId);
//           Int_t moduleId = CbmMuchAddress::GetModuleIndex(detId);
//           LOG(INFO) << point->GetZ() << " " << module->GetDetectorType() << " |" << stationId << ", " << layerId << ", " << sideId << ", " << moduleId << "|\n";
//        }

        // Process only appropriate module types
        if (module->GetDetectorType() != 2)
            continue;

        //Int_t stationId = CbmMuchAddress::GetStationIndex(point->GetDetectorID());
        //if (stationId < 4) continue;

        // Produce MUCH digi
        Int_t sizeDigis = fDigis->GetEntriesFast();
        CbmMuchDigi* digi = new ((*fDigis)[sizeDigis]) CbmMuchDigi(point->GetDetectorID(), 1., point->GetTime());
        // cout << point->GetDetectorID() << " " << digi->GetAddress() << " " << iPoint << endl;
        CbmMuchDigiMatch* match = new ((*fDigiMatches)[sizeDigis]) CbmMuchDigiMatch();
        match->AddLink(CbmLink(1., iPoint));
        //    iDigi++;
        nofCreatedDigis++;
    }

    LOG(INFO) << "CbmMuchDigitizeStraws::Exec2D: nofPoints=" << fPoints->GetEntriesFast()
          << " nofDigis=" << fDigis->GetEntriesFast() << " nofDigiMatches=" << fDigiMatches->GetEntriesFast()
          << " nofCreatedDigis=" << nofCreatedDigis << " nofFailed=" << nofFailed << " nofNotUsable=" << nofNotUsable << "\n";
}

// -------------------------------------------------------------------------
void CbmMuchDigitizeStraws::Finish()
{
}

// -------------------------------------------------------------------------
InitStatus CbmMuchDigitizeStraws::Init()
{
    FairRootManager* ioman = FairRootManager::Instance();

    // Initialize GeoScheme
    TFile* oldfile = gFile;
    TFile* file = new TFile(fDigiFile);
    TObjArray* stations = (TObjArray*)file->Get("stations");
    file->Close();
    file->Delete();
    gFile = oldfile;
    fGeoScheme->Init(stations);

    fPoints = (TClonesArray*)ioman->GetObject("MuchPoint");
    if (fDimens == 1)
    {
        // 1-D readout
        fDigis = new TClonesArray("CbmMuchStrawDigi", 1000);
        ioman->Register("MuchStrawDigi", "MUCH", fDigis, kTRUE);
        fDigiMatches = new TClonesArray("CbmMuchDigiMatch", 1000);
        ioman->Register("MuchStrawDigiMatch", "MUCH", fDigiMatches, kTRUE);
    }
    else
    {
        // Get from the GEM part
        fDigis = (TClonesArray*)FairRootManager::Instance()->GetObject("MuchDigi");
        fDigiMatches = (TClonesArray*)FairRootManager::Instance()->GetObject("MuchDigiMatch");
    }

    return kSUCCESS;
}

ClassImp(CbmMuchDigitizeStraws)
