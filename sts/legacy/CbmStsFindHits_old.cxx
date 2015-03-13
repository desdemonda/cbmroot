/**
 * \file CbmStsFindHits_old.cxx
 **/

#include "legacy/CbmStsFindHits_old.h"

#include "CbmGeoStsPar.h"
#include "CbmStsCluster.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsHit.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"
#include "CbmStsAddress.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"

#include "TStopwatch.h"

using namespace std;

CbmStsFindHits_old::CbmStsFindHits_old()
    : FairTask("CbmStsFindHits_old", 1)
    , fGeoPar(NULL)
    , fDigiPar(NULL)
    , fClusters(NULL)
    , fHits(NULL)
    , fDigiScheme(NULL)
    , fClusterMapF()
    , fClusterMapB()
{
}

CbmStsFindHits_old::~CbmStsFindHits_old()
{
    if (fHits)
    {
        fHits->Delete();
        delete fHits;
    }
}

void CbmStsFindHits_old::Exec(Option_t* opt)
{

	  LOG(INFO) << "FindHits_old:: exec" << FairLogger::endl;

	  TStopwatch timer;
	  timer.Start();
    // Clear output array
    fHits->Delete();

    SortClusters();

    // Find hits in sectors
    Int_t nofStations = fDigiScheme->GetNStations();
    for (Int_t iStation = 0; iStation < nofStations; iStation++)
    {
        CbmStsStation* station = fDigiScheme->GetStation(iStation);

        Int_t nofSectors = station->GetNSectors();
        for (Int_t iSector = 0; iSector < nofSectors; iSector++)
        {
            CbmStsSector* sector = station->GetSector(iSector);
            if (fClusterMapF.find(sector) == fClusterMapF.end())
            {
                continue;
            }
            const set<Int_t>& frontSet = fClusterMapF[sector];

            if ((sector->GetType() == 2 || sector->GetType() == 3) && (fClusterMapB.find(sector) == fClusterMapB.end()))
            {
                continue;
            }
            const set<Int_t>& backSet = fClusterMapB[sector];

            FindHits(station, sector, frontSet, backSet);
        } // Sector loop
    }     // Station loop

    timer.Stop();
    static Int_t eventNo = 0;
    LOG(INFO) << "CbmStsFindHits_old::Exec: eventNo=" << eventNo++
    		      << ", time " << timer.RealTime() << FairLogger::endl;
}

void CbmStsFindHits_old::SetParContainers()
{
    FairRuntimeDb* db = FairRunAna::Instance()->GetRuntimeDb();
    if (db == NULL)
        LOG(FATAL) << "CbmStsFindHits_old::SetParContainers: No runtime database" << FairLogger::endl;

    // Get STS geometry parameter container
    fGeoPar = (CbmGeoStsPar*)db->getContainer("CbmGeoStsPar");

    // Get STS digitisation parameter container
    fDigiPar = (CbmStsDigiPar*)db->getContainer("CbmStsDigiPar");
}

InitStatus CbmStsFindHits_old::Init()
{
    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if (ioman == NULL)
        LOG(FATAL) << "CbmStsFindHits_old::Init: No FairRootManager" << FairLogger::endl;

    fClusters = (TClonesArray*)ioman->GetObject("StsCluster");
    if (fClusters == NULL)
        LOG(FATAL) << "CbmStsFindHits_old::Init: No StsCluster" << FairLogger::endl;

    // Register output array
    fHits = new TClonesArray("CbmStsHit", 1000);
    ioman->Register("StsHit", "Hit in STS", fHits, kTRUE);

    // Build digitisation scheme
    fDigiScheme = new CbmStsDigiScheme();
    if (!fDigiScheme->Init(fGeoPar, fDigiPar))
        return kERROR;

    // Create sectorwise cluster sets
    MakeSets();

    return kSUCCESS;
}

void CbmStsFindHits_old::MakeSets()
{
    fClusterMapF.clear();
    fClusterMapB.clear();
    Int_t nofStations = fDigiScheme->GetNStations();
    for (Int_t iStation = 0; iStation < nofStations; iStation++)
    {
        CbmStsStation* station = fDigiScheme->GetStation(iStation);
        Int_t nofSectors = station->GetNSectors();
        for (Int_t iSector = 0; iSector < nofSectors; iSector++)
        {
            CbmStsSector* sector = station->GetSector(iSector);
            fClusterMapF[sector] = set<Int_t>();
            if (sector->GetType() == 2 || sector->GetType() == 3)
            {
                fClusterMapB[sector] = set<Int_t>();
            }
        }
    }
}

void CbmStsFindHits_old::SortClusters()
{
    MakeSets();

    Int_t nofClusters = fClusters->GetEntriesFast();
    for (Int_t iCluster = 0; iCluster < nofClusters; ++iCluster)
    {
        const CbmStsCluster* cluster = (CbmStsCluster*)fClusters->At(iCluster);
        Int_t stationNr = CbmStsAddress::GetElementId(cluster->GetAddress(), kStsStation);
        Int_t sectorNr = cluster->GetSectorNr();
        Int_t side = CbmStsAddress::GetElementId(cluster->GetAddress(), kStsSide);
        const CbmStsSector* sector = fDigiScheme->GetSector(stationNr, sectorNr);
        if (side == 0)
        {
            if (fClusterMapF.find(sector) == fClusterMapF.end())
            {
                continue;
            }
            fClusterMapF[sector].insert(iCluster);
        }
        else if (side == 1)
        {
            if (fClusterMapB.find(sector) == fClusterMapB.end())
            {
                continue;
            }
            fClusterMapB[sector].insert(iCluster);
        }
    }
}

void CbmStsFindHits_old::FindHits(CbmStsStation* station, CbmStsSector* sector, const set<Int_t>& frontSet, const set<Int_t>& backSet)
{
    Int_t sectorType = sector->GetType();

    // ----- Type 1 : Pixel sector   ---------------------------------------
    if (sectorType == 1)
    {
        Fatal("FindHits", "Sorry, not implemented yet");
    } // Pixel sensor
    // ---------------------------------------------------------------------

    // -----  Type 2: Strip sector OSU   -----------------------------------
    else if (sectorType == 2)
    {
        Fatal("FindHits", "Sorry, not implemented yet");
    } // Strip OSU
    // ---------------------------------------------------------------------

    // -----  Type 3: Strip sector GSI   -----------------------------------
    else if (sectorType == 3)
    {
        static const Double_t SQRT12 = TMath::Sqrt(12.);
        static const Double_t SQRT6 = TMath::Sqrt(12.);

        const Double_t rotation = sector->GetRotation();
        const Double_t dx = sector->GetDx();
        const Double_t dy = sector->GetDy();
        const Double_t stereoB = sector->GetStereoB();
        const Double_t stereoF = sector->GetStereoF();

        const Double_t sinRotation = TMath::Sin(rotation);
        const Double_t cosRotation = TMath::Cos(rotation);
        const Double_t tanStereoB = TMath::Tan(stereoB);

        Double_t vX = 0, vY = 0, vXY = 0;
        if (stereoF == 0. && stereoB * 180 / TMath::Pi() < 80)
        {
            vX = dx / SQRT12;
            vY = dx / SQRT6 / TMath::Abs(tanStereoB);
            vXY = -1. * dx * dx / 12. / tanStereoB;
        }
        else if (stereoF == 0. && stereoB * 180 / TMath::Pi() > 80)
        {
            vX = dx / SQRT12;
            vY = dx / SQRT12;
            vXY = 0.;
        }
        else
        {
            vX = dx / SQRT12;
            vY = dx / SQRT12 / TMath::Abs(tanStereoB);
            vXY = 0.;
        }

        set<Int_t>::const_iterator itf = frontSet.begin();
        set<Int_t>::const_iterator itf_end = frontSet.end();
        for (; itf != itf_end; itf++)
        {
            Int_t frontClusterId = (*itf);
            const CbmStsCluster* frontCluster = static_cast<const CbmStsCluster*>(fClusters->At(frontClusterId));
            Double_t frontChannel = frontCluster->GetMean();

            set<Int_t>::const_iterator itb = backSet.begin();
            set<Int_t>::const_iterator itb_end = backSet.end();
            for (; itb != itb_end; itb++)
            {
                Int_t backClusterId = (*itb);
                const CbmStsCluster* backCluster = static_cast<const CbmStsCluster*>(fClusters->At(backClusterId));
                Double_t backChannel = backCluster->GetMean();

                Double_t xHit = 0., yHit = 0, zHit = 0;
                Int_t sensorDetId = sector->IntersectClusters(frontChannel, backChannel, xHit, yHit, zHit);

                if (sensorDetId == -1)
                    continue;

                TVector3 pos(xHit, yHit, zHit);

                Double_t vXTemp, vYTemp, vXYTemp;

                Double_t wX = vX * vX * cosRotation * cosRotation - 2. * vXY * cosRotation * sinRotation + vY * vY * sinRotation * sinRotation;
                Double_t wY = vX * vX * sinRotation * sinRotation + 2. * vXY * cosRotation * sinRotation + vY * vY * cosRotation * cosRotation;
                Double_t wXY = (vX * vX - vY * vY) * cosRotation * sinRotation + vXY * (cosRotation * cosRotation - sinRotation * sinRotation);
                Double_t sigmaX = TMath::Sqrt(wX);
                Double_t sigmaY = TMath::Sqrt(wY);
                TVector3 dpos;

                if (stereoF == 0. && stereoB * 180 / TMath::Pi() < 80)
                { // 0&15 case

                    vXTemp = vX * frontCluster->GetMeanError();
                    vYTemp = (vX / (TMath::Tan(stereoB))) * TMath::Sqrt(frontCluster->GetMeanError() * frontCluster->GetMeanError() +
                                                                        backCluster->GetMeanError() * backCluster->GetMeanError());
                    vXYTemp = -vXTemp * vXTemp / (TMath::Tan(stereoB));

                    wX = vXTemp * vXTemp * cosRotation * cosRotation - 2. * vXYTemp * cosRotation * sinRotation + vYTemp * vYTemp * sinRotation * sinRotation;
                    wY = vXTemp * vXTemp * sinRotation * sinRotation + 2. * vXYTemp * cosRotation * sinRotation + vYTemp * vYTemp * cosRotation * cosRotation;
                    wXY = (vXTemp * vXTemp - vYTemp * vYTemp) * cosRotation * sinRotation + vXYTemp * (cosRotation * cosRotation - sinRotation * sinRotation);
                    dpos.SetXYZ(TMath::Sqrt(wX), TMath::Sqrt(wY), 0.);
                }
                else if (stereoF == 0. && stereoB * 180 / TMath::Pi() > 80)
                { // 0&90 case

                    vXTemp = vX * frontCluster->GetMeanError();
                    vYTemp = vX * backCluster->GetMeanError();
                    vXYTemp = 0.;

                    wX = vXTemp * vXTemp * cosRotation * cosRotation - 2. * vXYTemp * cosRotation * sinRotation + vYTemp * vYTemp * sinRotation * sinRotation;
                    wY = vXTemp * vXTemp * sinRotation * sinRotation + 2. * vXYTemp * cosRotation * sinRotation + vYTemp * vYTemp * cosRotation * cosRotation;
                    wXY = (vXTemp * vXTemp - vYTemp * vYTemp) * cosRotation * sinRotation + vXYTemp * (cosRotation * cosRotation - sinRotation * sinRotation);
                    dpos.SetXYZ(TMath::Sqrt(wX), TMath::Sqrt(wY), 0.);
                }
                else
                { // 7.5&-7.5 case

                    vXTemp = (vX / 2.) * TMath::Sqrt(frontCluster->GetMeanError() * frontCluster->GetMeanError() +
                                                     backCluster->GetMeanError() * backCluster->GetMeanError());
                    vYTemp = (vX / (2. * (TMath::Tan(stereoB)))) * TMath::Sqrt(frontCluster->GetMeanError() * frontCluster->GetMeanError() +
                                                                               backCluster->GetMeanError() * backCluster->GetMeanError());
                    vXYTemp = (vX * vX / (4. * (TMath::Tan(stereoB)))) *
                              (backCluster->GetMeanError() * backCluster->GetMeanError() - frontCluster->GetMeanError() * frontCluster->GetMeanError());

                    wX = vXTemp * vXTemp * cosRotation * cosRotation - 2. * vXYTemp * cosRotation * sinRotation + vYTemp * vYTemp * sinRotation * sinRotation;
                    wY = vXTemp * vXTemp * sinRotation * sinRotation + 2. * vXYTemp * cosRotation * sinRotation + vYTemp * vYTemp * cosRotation * cosRotation;
                    wXY = (vXTemp * vXTemp - vYTemp * vYTemp) * cosRotation * sinRotation + vXYTemp * (cosRotation * cosRotation - sinRotation * sinRotation);
                    dpos.SetXYZ(TMath::Sqrt(wX), TMath::Sqrt(wY), 0.);
                }

                Int_t size = fHits->GetEntriesFast();
                new ((*fHits)[size]) CbmStsHit(frontCluster->GetAddress(), pos, dpos, wXY, frontClusterId, backClusterId, frontClusterId, backClusterId, sector->GetSectorNr());
            } // back side strip loop
        }     // front side strip loop
    }         // strip GSI
}

void CbmStsFindHits_old::Finish()
{
}

ClassImp(CbmStsFindHits_old)
