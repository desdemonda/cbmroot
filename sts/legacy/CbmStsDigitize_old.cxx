//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsDigitize source file             -----
// -----                  Created 08/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------

/** Legacy software. This class is replaced by CbmStsDigitize (completely
 ** new implementation).
 **/

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TRandom3.h"

#include "TGeoManager.h"
#include "TGeoNode.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from STS
#include "CbmGeoStsPar.h"
#include "CbmStsAddress.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsDigitize_old.h"
#include "CbmStsPoint.h"
#include "legacy/CbmStsSensor_old.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"

#include "CbmMatch.h"
#include "CbmLink.h"

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>

using namespace std;

CbmStsDigitize_old::CbmStsDigitize_old()
    : FairTask("CbmStsDigitize")
    , fGeoPar(NULL)
    , fDigiPar(NULL)
    , fPoints(NULL)
    , fDigis(NULL)
    , fDigiMatches(NULL)
    , fRealistic(kFALSE)
    , fDigiScheme(NULL)
    , fNEvents(0)
    , fNPoints(0)
    , fNDigisFront(0)
    , fNDigisBack(0)
    , fStep(0.001)
    , fEnergyLossToSignal(280000000.)
    , fFThreshold(4.0)
    , fBThreshold(4.0)
    , fFNoiseWidth(0.1)
    , fBNoiseWidth(0.1)
    , fStripDeadTime(10)
    , fFNofBits(12)
    , fBNofBits(12)
    , fFNofElPerAdc(10.)
    , fBNofElPerAdc(10.)
    , fFNofSteps(0)
    , fBNofSteps(0)
    , fStripSignalF(NULL)
    , fStripSignalB(NULL)
    , fTime(0.)
    , fTimer()
    , fFChannelPointsMap()
    , fBChannelPointsMap()
    , fPointMap()
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmStsDigitize_old::~CbmStsDigitize_old()
{
    if (fGeoPar)
        delete fGeoPar;
    if (fDigiPar)
        delete fDigiPar;
    if (fDigis)
    {
        fDigis->Delete();
        delete fDigis;
    }
    if (fDigiMatches)
    {
        fDigiMatches->Delete();
        delete fDigiMatches;
    }
    if (fDigiScheme)
        delete fDigiScheme;
    Reset();
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmStsDigitize_old::Exec(Option_t* opt)
{
    // Reset all eventwise counters
    fTimer.Start();
    Reset();

    Int_t nPoints = 0;
    Int_t nDigisF = 0;
    Int_t nDigisB = 0;

    map<CbmStsSensor_old*, set<Int_t> >::iterator mapIt;
    for (mapIt = fPointMap.begin(); mapIt != fPointMap.end(); mapIt++)
        ((*mapIt).second).clear();

    for (Int_t iPoint = 0; iPoint < fPoints->GetEntriesFast(); iPoint++)
    {
        CbmStsPoint* point = (CbmStsPoint*)fPoints->At(iPoint);

        Double_t xin = point->GetXIn();
        Double_t yin = point->GetYIn();
        Double_t zin = point->GetZIn();
        gGeoManager->FindNode(xin, yin, zin);
        TGeoNode* curNode = gGeoManager->GetCurrentNode();

        CbmStsSensor_old* sensor = NULL;
        if (fDigiScheme->IsNewGeometry())
        {
            TString curPath = fDigiScheme->GetCurrentPath();
            sensor = fDigiScheme->GetSensorByName(curPath);
        }
        else
            sensor = fDigiScheme->GetSensorByName(curNode->GetName());

        if (fPointMap.find(sensor) == fPointMap.end())
        {
            cerr << "-E- " << fName << "::Exec:: sensor " << curNode->GetName() << " not found in digi scheme!" << endl;
            continue;
        }
        fPointMap[sensor].insert(iPoint);
        LOG(DEBUG2) << GetName() << ": Point in sensor " << sensor->GetName()
        		        << FairLogger::endl;
        nPoints++;
    }

    for (Int_t iStation = fDigiScheme->GetNStations(); iStation > 0;)
    {
        CbmStsStation* station = fDigiScheme->GetStation(--iStation);
        for (Int_t iSector = station->GetNSectors(); iSector > 0;)
        {
            CbmStsSector* sector = station->GetSector(--iSector);

            map<Int_t, set<Int_t> >::iterator mapCh;

            for (mapCh = fFChannelPointsMap.begin(); mapCh != fFChannelPointsMap.end(); mapCh++)
                ((*mapCh).second).clear();
            for (mapCh = fBChannelPointsMap.begin(); mapCh != fBChannelPointsMap.end(); mapCh++)
                ((*mapCh).second).clear();

            // simulating detector+cables+electronics noise
            // should be more sophisticated...
            // the question is: sectorwise or sensorwise???
            Int_t nChannels = sector->GetNChannelsFront();

            //-----aplying noise on every channel-----
            for (Int_t iChannel = nChannels; iChannel > 0;)
            {
                // 	fStripSignalF[--iChannel] = fGen->Landau(.1,.02);
                // 	fStripSignalB[  iChannel] = fGen->Landau(.1,.02);
                // 	fStripSignalF[--iChannel] = 0.;
                // 	fStripSignalB[  iChannel] = 0.;
                fStripSignalF[--iChannel] = TMath::Abs(gRandom->Gaus(0., fFNoiseWidth));
                fStripSignalB[iChannel] = TMath::Abs(gRandom->Gaus(0., fBNoiseWidth));
            }

            for (Int_t iSensor = sector->GetNSensors(); iSensor > 0;)
            {
                CbmStsSensor_old* sensor = sector->GetSensor(--iSensor);

                ProduceHitResponse(sensor);
            }

            Int_t stationNr = sector->GetStationNr();
            Int_t sectorNr = sector->GetSectorNr();
            Int_t sectorDetId = sector->GetDetectorId();

            for (Int_t ifstr = 0; ifstr < nChannels; ifstr++)
            {
                if (fStripSignalF[ifstr] < (fFThreshold * 1000.))
                    continue; // threshold cut

                //-----random strip inefficiency-----
                Double_t generator;
                generator = gRandom->Rndm() * 100.;
                if (generator < (fStripDeadTime / 100.) * occupancy[iStation][iSector][ifstr / 125])
                    continue;
                //-----------------------------------

                Int_t digiFSignal = 1 + (Int_t)((fStripSignalF[ifstr]) / fFNofElPerAdc);
                if (digiFSignal >= fFNofSteps)
                    digiFSignal = fFNofSteps - 1;
                /*
                new ((      *fDigis)[fNDigis]) CbmStsDigi(stationNr, sectorNr,
                                      0, ifstr,
                                      digiFSignal, 0);
                                      */
                UInt_t address = CbmStsAddress::GetAddress(stationNr, 0, 0, 0, 0, 0, ifstr);
                Int_t size = fDigis->GetEntriesFast();
                new ((*fDigis)[size]) CbmStsDigi(address, 0, digiFSignal, sectorNr);
                LOG(DEBUG3) << GetName() << ": New digi at address " << address
                		        << ", sector " << sectorNr << " front side, channel "
                		        << ifstr << ", ADC " << digiFSignal << FairLogger::endl;

                set<Int_t> chPnt = fFChannelPointsMap[ifstr];
                if (chPnt.size() == 0)
                {
                    new ((*fDigiMatches)[size]) CbmMatch();
                }
                else
                {
                    CbmMatch* match = NULL;
                    set<Int_t>::const_iterator it1 = chPnt.begin();
                    for (; it1 != chPnt.end(); it1++)
                    {
                        Int_t pnt = (*it1);
                        const CbmStsPoint* point = static_cast<const CbmStsPoint*>(fPoints->At(pnt));
                        if (it1 == chPnt.begin())
                            match = new ((*fDigiMatches)[size]) CbmMatch();
                         match->AddLink(CbmLink(point->GetEnergyLoss(), pnt));
                    }
                }
                nDigisF++;
            }

            for (Int_t ibstr = 0; ibstr < nChannels; ibstr++)
            {
                if (fStripSignalB[ibstr] < (fBThreshold * 1000.))
                    continue; // threshold cut

                //-----random strip inefficiency-----
                Double_t generator;
                generator = gRandom->Rndm() * 100.;
                if (generator < (fStripDeadTime / 100.) * occupancy[iStation][iSector][ibstr / 125])
                    continue;
                //-----------------------------------

                Int_t digiBSignal = 1 + (Int_t)((fStripSignalB[ibstr]) / fBNofElPerAdc);
                if (digiBSignal >= fBNofSteps)
                    digiBSignal = fBNofSteps - 1;
                /*
                new ((      *fDigis)[fNDigis]) CbmStsDigi(stationNr, sectorNr,
                                      1, ibstr,
                                      digiBSignal, 0);
                                      */
                // Use here the backward-compatibel constructor of CbmStsDigi.
                UInt_t address = CbmStsAddress::GetAddress(stationNr, 0, 0, 0, 0, 1, ibstr);
                Int_t size = fDigis->GetEntriesFast();
                new ((*fDigis)[size]) CbmStsDigi(address, 0, digiBSignal, sectorNr);
                LOG(DEBUG3) << GetName() << ": New digi at address " << address
                		        << ", sector " << sectorNr << " back side, channel "
                		        << ibstr << ", ADC " << digiBSignal << FairLogger::endl;

                set<Int_t> chPnt = fBChannelPointsMap[ibstr];
                if (chPnt.size() == 0)
                {
                    new ((*fDigiMatches)[size]) CbmMatch();
                }
                else
                {
                    CbmMatch* match = NULL;
                    set<Int_t>::const_iterator it1 = chPnt.begin();
                    for (; it1 != chPnt.end(); it1++)
                    {
                        Int_t pnt = (*it1);
                        const CbmStsPoint* point = static_cast<const CbmStsPoint*>(fPoints->At(pnt));
                        if (it1 == chPnt.begin())
                            match = new ((*fDigiMatches)[size]) CbmMatch();
                         match->AddLink(CbmLink(point->GetEnergyLoss(), pnt));
                    }
                }
                nDigisB++;
            }
        }
    }

    fTimer.Stop();
    cout << "+ " << flush;
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8) << fixed << right << fTimer.RealTime() << " s, digis " << nDigisF << " / "
         << nDigisB << endl;

    fNEvents += 1.;
    fNPoints += Double_t(nPoints);
    fNDigisFront += Double_t(nDigisF);
    fNDigisBack += Double_t(nDigisB);
    fTime += fTimer.RealTime();

    static Int_t eventNo = 0;
    cout << "CbmStsDigitize::Exec: eventNo=" << eventNo++ << " nofDigis=" << fDigis->GetEntriesFast() << " nofDigiMatches=" << fDigiMatches->GetEntriesFast() << std::endl;
}
// -------------------------------------------------------------------------

// -----   Private method ProduceHitResponse   --------------------------------
void CbmStsDigitize_old::ProduceHitResponse(CbmStsSensor_old* sensor)
{
    set<Int_t> pSet;
    if (fPointMap.find(sensor) == fPointMap.end())
    {
        cerr << "-E- " << fName << "::Exec:: sensor"
             << " not found in digi scheme!" << endl;
        return;
    }
    pSet = fPointMap[sensor];

    Int_t iPoint = -1;
    CbmStsPoint* point = NULL;

    set<Int_t>::iterator it1;

    for (it1 = pSet.begin(); it1 != pSet.end(); it1++)
    {
        iPoint = (*it1);
        point = (CbmStsPoint*)fPoints->At(iPoint);

        point->Print();
        Double_t xin = point->GetXIn();
        Double_t yin = point->GetYIn();
        Double_t zin = point->GetZIn();

        Double_t xvec = point->GetXOut() - xin;
        Double_t yvec = point->GetYOut() - yin;
        Double_t zvec = point->GetZOut() - zin;

        Int_t nofSteps = (Int_t)(TMath::Sqrt(xvec * xvec + yvec * yvec + zvec * zvec) / fStep + 1);

        Double_t stepEL = fEnergyLossToSignal * point->GetEnergyLoss() / (nofSteps + 1);

        xvec = xvec / ((Double_t)nofSteps);
        yvec = yvec / ((Double_t)nofSteps);
        zvec = zvec / ((Double_t)nofSteps);

        for (Int_t istep = 0; istep <= nofSteps; istep++)
        {

            Int_t iIChan = sensor->GetFrontChannel(xin, yin, zin);
            LOG(DEBUG4) << GetName() << ": Front x " << xin << " y " << yin
            		        << " channel " << iIChan << FairLogger::endl;

            if (iIChan != -1)
            {
                fStripSignalF[iIChan] += stepEL;
                fFChannelPointsMap[iIChan].insert(iPoint);
            }

            iIChan = sensor->GetBackChannel(xin, yin, zin);
            LOG(DEBUG4) << GetName() << ": Back x " << xin << " y " << yin
            		        << " channel " << iIChan << FairLogger::endl;

            if (iIChan != -1)
            {
                fStripSignalB[iIChan] += stepEL;
                fBChannelPointsMap[iIChan].insert(iPoint);
            }

            xin += xvec;
            yin += yvec;
            zin += zvec;
        }
    }
}

// -----   Private method SetParContainers   -------------------------------
void CbmStsDigitize_old::SetParContainers()
{

    // Get run and runtime database
    FairRunAna* run = FairRunAna::Instance();
    if (!run)
        Fatal("SetParContainers", "No analysis run");

    FairRuntimeDb* db = run->GetRuntimeDb();
    if (!db)
        Fatal("SetParContainers", "No runtime database");

    // Get STS geometry parameter container
    fGeoPar = (CbmGeoStsPar*)db->getContainer("CbmGeoStsPar");

    // Get STS digitisation parameter container
    fDigiPar = (CbmStsDigiPar*)db->getContainer("CbmStsDigiPar");
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmStsDigitize_old::Init()
{
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        LOG(FATAL) << "CbmStsDigitize::Init: No FairRootManager" << FairLogger::endl;
    fPoints = (TClonesArray*)ioman->GetObject("StsPoint");

    // Register output array StsDigi
    fDigis = new TClonesArray("CbmStsDigi", 1000);
    ioman->Register("StsDigi", "Digital response in STS", fDigis, kTRUE);

    // Register output array StsDigiMatches
    fDigiMatches = new TClonesArray("CbmMatch", 1000);
    ioman->Register("StsDigiMatch", "Digi Match in STS", fDigiMatches, kTRUE);

    /// FIXME: Hardcoded array size! Why 2000?
    fStripSignalF = new Double_t[2000];
    fStripSignalB = new Double_t[2000];

    fFNofSteps = (Int_t)TMath::Power(2, (Double_t)fFNofBits);
    fBNofSteps = (Int_t)TMath::Power(2, (Double_t)fBNofBits);

    fDigiScheme = new CbmStsDigiScheme();
    if (fDigiScheme->Init(fGeoPar, fDigiPar))
    {
        MakeSets();
    }
    else
    {
        return kERROR;
    }

    return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsDigitize_old::ReInit()
{

    // Clear digitisation scheme
    fDigiScheme->Clear();

    // Build new digitisation scheme
    if (fDigiScheme->Init(fGeoPar, fDigiPar))
    {
        MakeSets();
        return kSUCCESS;
    }

    return kERROR;
}
// -------------------------------------------------------------------------

// -----   Private method MakeSets   ---------------------------------------
void CbmStsDigitize_old::MakeSets()
{

    fPointMap.clear();
    Int_t nStations = fDigiScheme->GetNStations();

    Double_t fSectorWidth = 0.;

    for (Int_t iStation = 0; iStation < nStations; iStation++)
    {
        CbmStsStation* station = fDigiScheme->GetStation(iStation);
        Int_t nSectors = station->GetNSectors();
        for (Int_t iSector = 0; iSector < nSectors; iSector++)
        {
            CbmStsSector* sector = station->GetSector(iSector);
            Int_t nSensors = sector->GetNSensors();
            for (Int_t iSensor = 0; iSensor < nSensors; iSensor++)
            {
                CbmStsSensor_old* sensor = sector->GetSensor(iSensor);
                set<Int_t> a;
                fPointMap[sensor] = a;
                fSectorWidth = 10. * sensor->GetLx();

                Int_t nofChips = (Int_t)(TMath::Ceil(fSectorWidth / 7.5)); // fwidth in mm, 7.5mm = 125(channels)*60mum(pitch)
                Int_t lastChip = (Int_t)(TMath::Ceil(10. * fSectorWidth));
                lastChip = lastChip % 75;
                lastChip = (Int_t)(lastChip / .6);
                //     cout << nofChips << " chips on " << iStation+1 << " " << iSector+1 << endl;
                TString addInfo = "";
                if (nofChips != 8)
                {
                    addInfo = Form(", only %d strips", lastChip);
                    //	cout << fSectorWidth << " -> " << addInfo.Data() << endl;
                }

                for (Int_t iChip = 0; iChip < nofChips; iChip++)
                {
                    occupancy[iStation][iSector][iChip] = 3.;
                    //           cout << "OCCUPANCY [" << iStation+1 << "][" << iSector+1 << "][" << iChip << "] "<< occupancy [iStation][iSector][iChip] << "%"
                    // << endl;
                }
            }
        }
    }
    fFChannelPointsMap.clear();
    fBChannelPointsMap.clear();
    for (Int_t ichan = 2000; ichan > 0;)
    {
        set<Int_t> a;
        fFChannelPointsMap[--ichan] = a;
        set<Int_t> b;
        fBChannelPointsMap[ichan] = b;
    }
}
// -------------------------------------------------------------------------
void CbmStsDigitize_old::MakeSets1()
{ // with occupancy file - default not used

    fPointMap.clear();
    Int_t nStations = fDigiScheme->GetNStations();

    TH1F* fhFNofDigisPChip[10][1000][20];
    TH1F* fhBNofDigisPChip[10][1000][20];
    TString qaFileName;
    qaFileName = "occup.sts.reco.root";
    cout << "Occupancy read from file: \"" << qaFileName.Data() << "\"" << endl;
    TFile* occuF = TFile::Open(qaFileName.Data());

    TString directoryName = "STSFindHitsQA";

    Double_t fSectorWidth = 0.;

    for (Int_t iStation = 0; iStation < nStations; iStation++)
    {
        CbmStsStation* station = fDigiScheme->GetStation(iStation);
        Int_t nSectors = station->GetNSectors();

        for (Int_t iSector = 0; iSector < nSectors; iSector++)
        {
            CbmStsSector* sector = station->GetSector(iSector);
            Int_t nSensors = sector->GetNSensors();
            Int_t nChannels = sector->GetNChannelsFront();

            for (Int_t iSensor = 0; iSensor < nSensors; iSensor++)
            {
                CbmStsSensor_old* sensor = sector->GetSensor(iSensor);
                set<Int_t> a;
                fPointMap[sensor] = a;
                fSectorWidth = 10. * sensor->GetLx();

                Int_t nofChips = (Int_t)(TMath::Ceil(fSectorWidth / 7.5)); // fwidth in mm, 7.5mm = 125(channels)*60mum(pitch)
                Int_t lastChip = (Int_t)(TMath::Ceil(10. * fSectorWidth));
                lastChip = lastChip % 75;
                lastChip = (Int_t)(lastChip / .6);
                //     cout << nofChips << " chips on " << iStation+1 << " " << iSector+1 << endl;
                TString addInfo = "";
                if (nofChips != 8)
                {
                    addInfo = Form(", only %d strips", lastChip);
                    //	cout << fSectorWidth << " -> " << addInfo.Data() << endl;
                }

                for (Int_t iChip = 0; iChip < nofChips; iChip++)
                {
                    fhFNofDigisPChip[iStation][iSector][iChip] = (TH1F*)occuF->Get(
                        Form("%s/Station%d/hNofFiredDigisFSt%dSect%dChip%d", directoryName.Data(), iStation + 1, iStation + 1, iSector + 1, iChip + 1));
                    fhBNofDigisPChip[iStation][iSector][iChip] = (TH1F*)occuF->Get(
                        Form("%s/Station%d/hNofFiredDigisBSt%dSect%dChip%d", directoryName.Data(), iStation + 1, iStation + 1, iSector + 1, iChip + 1));
                    occupancy[iStation][iSector][iChip] = 100. * fhFNofDigisPChip[iStation][iSector][iChip]->GetMean() / 125.;
                    occupancy[iStation][iSector][iChip] = 100. * fhBNofDigisPChip[iStation][iSector][iChip]->GetMean() / 125.;
                    // 	    if ( !occuF ) {
                    // 	      occupancy [iStation][iSector][iChip] = 3.;
                    // 	    }
                    //           cout << "OCCUPANCY [" << iStation+1 << "][" << iSector+1 << "][" << iChip << "] "<< occupancy [iStation][iSector][iChip] << "%"
                    // << endl;
                }
            }
        }
    }
    fFChannelPointsMap.clear();
    fBChannelPointsMap.clear();
    for (Int_t ichan = 2000; ichan > 0;)
    {
        set<Int_t> a;
        fFChannelPointsMap[--ichan] = a;
        set<Int_t> b;
        fBChannelPointsMap[ichan] = b;
    }
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmStsDigitize_old::Reset()
{
    fFChannelPointsMap.clear();
    fBChannelPointsMap.clear();
    if (fDigis)
        fDigis->Delete();
    if (fDigiMatches)
        fDigiMatches->Delete();
}
// -------------------------------------------------------------------------

// -----   Virtual method Finish   -----------------------------------------
void CbmStsDigitize_old::Finish()
{
    cout << endl;
    cout << "============================================================" << endl;
    cout << "===== " << fName << ": Run summary " << endl;
    cout << "===== " << endl;
    cout << "===== Events processed          : " << setw(8) << fNEvents << endl;
    cout.setf(ios_base::fixed, ios_base::floatfield);
    cout << "===== Real time per event       : " << setw(8) << setprecision(4) << fTime / fNEvents << " s" << endl;
    cout << "===== StsPoints per event       : " << setw(8) << setprecision(2) << fNPoints / fNEvents << endl;
    cout << "===== StsDigis per event        : " << setw(8) << setprecision(2) << (fNDigisFront + fNDigisBack) / fNEvents << endl;
    cout << "===== Front digis per point     : " << setw(8) << setprecision(2) << fNDigisFront / (fNPoints) << endl;
    cout << "===== Back digis per point      : " << setw(8) << setprecision(2) << fNDigisBack / (fNPoints) << endl;
    cout << "============================================================" << endl;
}
// -------------------------------------------------------------------------

ClassImp(CbmStsDigitize_old)
