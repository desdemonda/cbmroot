/**
 * \file CbmStsClusterFinder_old.cxx
 **/
#include "TClonesArray.h"
#include "TH1S.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "CbmGeoStsPar.h"
#include "CbmStsAddress.h"
#include "CbmStsCluster.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "legacy/CbmStsClusterFinder_old.h"
#include "CbmStsHit.h"
#include "CbmStsSector.h"
#include "CbmStsStation_old.h"
#include "CbmTimeSlice.h"
#include "TMath.h"
#include "TStopwatch.h"



CbmStsClusterFinder_old::CbmStsClusterFinder_old()
    : FairTask("CbmStsClusterFinder_old", 1)
    , fDigis(NULL)
    , fTimeSlice(NULL)
    , fClusterCandidates(NULL)
    , fClusters(NULL)
    , fInputMode(0)
    , fGeoPar(NULL)
    , fDigiPar(NULL)
    , fDigiScheme(NULL)
    , fDigiMapF()
    , fDigiMapB()
{
}

CbmStsClusterFinder_old::~CbmStsClusterFinder_old()
{
    if (fClusterCandidates)
    {
        fClusterCandidates->Delete();
        delete fClusterCandidates;
    }
    if (fClusters)
    {
        fClusters->Delete();
        delete fClusters;
    }
}

void CbmStsClusterFinder_old::Exec(Option_t* opt)
{

	  TStopwatch timer;
	  timer.Start();

    // Clear output arrays
    fClusters->Delete();
    fClusterCandidates->Delete();

    SortDigis();

    // Find hits in sectors
    Int_t nofStations = fDigiScheme->GetNStations();
    for (Int_t iStation = 0; iStation < nofStations; iStation++)
    {
        CbmStsStation_old* station = fDigiScheme->GetStation(iStation);
        Int_t nofSectors = station->GetNSectors();
        for (Int_t iSector = 0; iSector < nofSectors; iSector++)
        {
            CbmStsSector* sector = station->GetSector(iSector);
            if (fDigiMapF.find(sector) == fDigiMapF.end())
            {
                continue;
            }
            const set<Int_t>& frontSet = fDigiMapF[sector];
            FindClusters(iStation + 1, iSector + 1, 0, frontSet);
            if (sector->GetType() == 2 || sector->GetType() == 3)
            {
                if (fDigiMapB.find(sector) == fDigiMapB.end())
                {
                    continue;
                }
            }
            const set<Int_t>& backSet = fDigiMapB[sector];
            FindClusters(iStation + 1, iSector + 1, 1, backSet);
        } // Sector loop
    } // Station loop

    AnalyzeClusters();

    timer.Stop();

    static Int_t eventNo = 0;
    LOG(INFO) << "CbmStsClusterFinder_old::Exec: eventNo=" << eventNo++
    		      << ", time " << timer.RealTime() << FairLogger::endl;
    LOG(INFO) << "CbmStsClusterFinder_old::Exec: fDigis.Size=" << fDigis->GetEntries() << FairLogger::endl;
    LOG(INFO) << "CbmStsClusterFinder_old::Exec: fClusterCandidates.Size=" << fClusterCandidates->GetEntries() << FairLogger::endl;
    LOG(INFO) << "CbmStsClusterFinder_old::Exec: fClusters.Size=" << fClusters->GetEntries() << FairLogger::endl;

}

void CbmStsClusterFinder_old::SetParContainers()
{
    FairRuntimeDb* db = FairRunAna::Instance()->GetRuntimeDb();
    if (NULL == db)
        LOG(FATAL) << "CbmStsClusterFinder_old::SetParContainers: No runtime database" << FairLogger::endl;
    fGeoPar = (CbmGeoStsPar*)db->getContainer("CbmGeoStsPar");
    fDigiPar = (CbmStsDigiPar*)db->getContainer("CbmStsDigiPar");
}

InitStatus CbmStsClusterFinder_old::Init()
{

    // Get I/O manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman)
        LOG(FATAL) << "CbmStsClusterFinder_old::Init: No FairRootManager" << FairLogger::endl;

    // Get input source of StsDigi (time slice or event)
    fInputMode = -1;
    fTimeSlice = (CbmTimeSlice*) ioman->GetObject("TimeSlice.");
    if ( fTimeSlice ) {
    	LOG(INFO) << GetName() << ": operating on time slice." << FairLogger::endl;
    	fInputMode = 1;
    }
    else {
    LOG(INFO) << GetName() << ": operating on event data." << FairLogger::endl;
     fDigis = (TClonesArray*)ioman->GetObject("StsDigi");
     fInputMode = 0;
    }
    if ( fInputMode == -1 )
    	LOG(FATAL) << "CbmStsClusterFinder_old::Init: No StsDigi array" << FairLogger::endl;

    fClusterCandidates = new TClonesArray("CbmStsCluster", 30000);
    ioman->Register("StsClusterCand", "Cluster in STS", fClusterCandidates, kTRUE);

    fClusters = new TClonesArray("CbmStsCluster", 30000);
    ioman->Register("StsCluster", "Cluster in STS", fClusters, kTRUE);

    // Build digitisation scheme
    fDigiScheme = new CbmStsDigiScheme();
    if (!fDigiScheme->Init(fGeoPar, fDigiPar))
        return kERROR;

    // Create sectorwise digi sets
    MakeSets();

    return kSUCCESS;
}

void CbmStsClusterFinder_old::MakeSets()
{
    fDigiMapF.clear();
    fDigiMapB.clear();
    Int_t nofStations = fDigiScheme->GetNStations();
    for (Int_t iStation = 0; iStation < nofStations; iStation++)
    {
        CbmStsStation_old* station = fDigiScheme->GetStation(iStation);
        Int_t nofSectors = station->GetNSectors();
        for (Int_t iSector = 0; iSector < nofSectors; iSector++)
        {
            CbmStsSector* sector = station->GetSector(iSector);
            fDigiMapF[sector] = set<Int_t>();
            if (sector->GetType() == 2 || sector->GetType() == 3)
            {
                fDigiMapB[sector] = set<Int_t>();
            }
        }
    }
}

void CbmStsClusterFinder_old::SortDigis()
{
    MakeSets();

    Int_t nofDigis = 0;
    switch (fInputMode) {
    	case 0: nofDigis = fDigis->GetEntriesFast(); break;
    	case 1: nofDigis = fTimeSlice->GetDataSize(kSTS); break;
    	default: nofDigis = 0; break;
    }

    for (Int_t iDigi = 0; iDigi < nofDigis; iDigi++)
    {
    	  CbmStsDigi* digi = NULL;
    	  if ( fInputMode == 0 )
    	  	digi = static_cast<CbmStsDigi*> (fDigis->At(iDigi));
    	  else if ( fInputMode == 1 )
    	  	digi = static_cast<CbmStsDigi*> (fTimeSlice->GetData(kSTS, iDigi));

        const Int_t stationNr = CbmStsAddress::GetElementId(digi->GetAddress(), kStsStation);
        const Int_t sectorNr = digi->GetSectorNr();
        const Int_t iSide = CbmStsAddress::GetElementId(digi->GetAddress(), kStsSide);
        CbmStsSector* sector = fDigiScheme->GetSector(stationNr, sectorNr);

        if (iSide == 0)
        {
            if (fDigiMapF.find(sector) == fDigiMapF.end())
            {
                continue;
            }
            fDigiMapF[sector].insert(iDigi);
        }
        else if (iSide == 1)
        {
            if (fDigiMapB.find(sector) == fDigiMapB.end())
            {
                continue;
            }
            fDigiMapB[sector].insert(iDigi);
        }
    }
}

void CbmStsClusterFinder_old::FindClusters(Int_t stationNr, Int_t sectorNr, Int_t iSide, const set<Int_t>& digiSet)
{
    CbmStsCluster* clusterCand = NULL;

    Int_t lastDigiNr = -1;
    Int_t lastChannelId = -1;
    Double_t lastDigiSig = 100000.;
    Int_t clusterMaxNr = -1;
    Int_t clusterMaxPos = -1;
    Double_t clusterMaxSig = -1.;
    Int_t startChannelId = 0;

    set<Int_t>::const_iterator it = digiSet.begin();
    set<Int_t>::const_iterator it_end = digiSet.end();

    map<Int_t, Int_t> m;
    map<Int_t, Int_t>::const_iterator p;
    for (; it != it_end; ++it)
    {
        Int_t digiIndex = (*it);
        const CbmStsDigi* digi = static_cast<const CbmStsDigi*>(fDigis->At(digiIndex));
        Int_t channelId = CbmStsAddress::GetElementId(digi->GetAddress(), kStsChannel);
        m[channelId]=digiIndex;
      
    }
/*    
    for (; it != it_end; ++it)
    {
        Int_t digiIndex = (*it);
*/
    for(p=m.begin();p!=m.end();++p)
    {
        Int_t digiIndex = p->second;
        const CbmStsDigi* digi = static_cast<const CbmStsDigi*>(fDigis->At(digiIndex));

        Int_t channelId = CbmStsAddress::GetElementId(digi->GetAddress(), kStsChannel);
        Double_t digiSig = digi->GetCharge();

        if (lastChannelId == -1)
        {
            Int_t size = fClusterCandidates->GetEntriesFast();
            clusterCand = new ((*fClusterCandidates)[size]) CbmStsCluster();
            clusterCand->SetAddress(digi->GetAddress());
            startChannelId = channelId;
        }

        if (channelId == lastChannelId + 1)
        {
            // if the signal is larger that last one and the previous one is smaller than maximum
            if (digiSig > lastDigiSig && lastDigiSig < clusterMaxSig && digiSig != clusterMaxSig)
            {
                Int_t size = fClusterCandidates->GetEntriesFast();
                clusterCand = new ((*fClusterCandidates)[size]) CbmStsCluster();
                clusterCand->SetAddress(digi->GetAddress());
                clusterCand->AddDigi(lastDigiNr);

                clusterMaxPos = -1;
                clusterMaxSig = -1.;
                startChannelId = channelId;
            }
        }
        else if (lastChannelId >= 0)
        {
            Int_t size = fClusterCandidates->GetEntriesFast();
            clusterCand = new ((*fClusterCandidates)[size]) CbmStsCluster();
            clusterCand->SetAddress(digi->GetAddress());

            clusterMaxPos = -1;
            clusterMaxSig = -1.;
            startChannelId = channelId;
        }

        if (digiSig > clusterMaxSig)
        {
            clusterMaxNr = digiIndex;
            clusterMaxPos = channelId;
            clusterMaxSig = digiSig;
        }

        Int_t clusterWidth = lastChannelId - startChannelId + 1;
        if (clusterWidth < 90)
        {
            clusterCand->AddDigi(digiIndex);
        }
        else
        {
            Int_t size = fClusterCandidates->GetEntriesFast();
            clusterCand = new ((*fClusterCandidates)[size]) CbmStsCluster();
            clusterCand->SetAddress(digi->GetAddress());
            clusterCand->AddDigi(digiIndex);
            clusterWidth = lastChannelId - startChannelId + 1;

            clusterMaxPos = -1;
            clusterMaxSig = -1.;
            startChannelId = channelId;
        }

        lastDigiNr = digiIndex;
        lastChannelId = channelId;
        lastDigiSig = digiSig;
    }
}

void CbmStsClusterFinder_old::AnalyzeClusters()
{
    Int_t nofClusterCandidates = fClusterCandidates->GetEntriesFast();
    for (Int_t iclus = 0; iclus < nofClusterCandidates; iclus++)
    {
        AnalyzeCluster(iclus);
    }
}

void CbmStsClusterFinder_old::AnalyzeCluster(Int_t clusterId)
{
    const CbmStsCluster* clusterCand = static_cast<const CbmStsCluster*>(fClusterCandidates->At(clusterId));

    Double_t chanNr = 0;
    Double_t chanADC = 0.;
    Double_t sumW = 0;
    Double_t sumWX = 0;
    Double_t error = 0;
    Int_t nofDigis = clusterCand->GetNofDigis();
    for (Int_t i = 0; i < nofDigis; ++i)
    {
        CbmStsDigi* digi = (CbmStsDigi*)fDigis->At(clusterCand->GetDigi(i));
        chanNr = (Double_t)(CbmStsAddress::GetElementId(digi->GetAddress(), kStsChannel));
        chanADC = digi->GetCharge();
        sumW += chanADC;
        sumWX += chanNr * chanADC;
        error += (chanADC * chanADC);
    }

    if (sumW > 50.)
    {
        Int_t size = fClusters->GetEntriesFast();
        CbmStsCluster* cluster = new ((*fClusters)[size]) CbmStsCluster();

        for (Int_t i = 0; i < nofDigis; ++i)
        {
           Int_t digiIndex = clusterCand->GetDigi(i);
            const CbmStsDigi* digi = static_cast<const CbmStsDigi*>(fDigis->At(digiIndex));
            if (i == 0)
            {
                cluster->SetAddress(digi->GetAddress());
                cluster->SetSectorNr(digi->GetSectorNr());
            }
            cluster->AddDigi(digiIndex);
        }
        cluster->SetMean(sumWX / sumW);
        cluster->SetMeanError((1. / (sumW)) * TMath::Sqrt(error));
    }
}

void CbmStsClusterFinder_old::Finish()
{
}

ClassImp(CbmStsClusterFinder_old)
