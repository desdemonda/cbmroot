// --------------------------------------------------------------------------------------------
// -----                    CbmMvdFindHits source file                 --------
// -----                  Created 06/05/08  by C. Dritsa               -----
// -----   Code tuning and maintainance M.Deveaux 01/07/2010     -----
// ---------------------------------------------------------------------------------------------
// Includes from MVD
#include "CbmMvdGeoPar.h"
#include "CbmMvdHit.h"
#include "CbmMvdCluster.h"
#include "CbmMvdHitMatch.h"
#include "CbmMvdPileupManager.h"
#include "CbmMvdPoint.h"
#include "CbmMvdDigitizeL.h"
#include "CbmMvdFindHits.h"
#include "CbmMvdStation.h"

// Includes from base
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmMCTrack.h"

// Includes from ROOT
#include "TGeoManager.h"
#include "TGeoTube.h"
#include "TArrayD.h"
#include "TObjArray.h"
#include "TRefArray.h"
#include "TCanvas.h"

#include "TClonesArray.h"

#include "TRandom3.h"
#include "TString.h"
#include "TVector3.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"

// Includes from C++
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

using std::cout;
using std::endl;
using std::map;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::pair;
using std::setprecision;
using std::ios_base;
using std::vector;


// -----   Default constructor   ------------------------------------------
CbmMvdFindHits::CbmMvdFindHits()
  : FairTask("MVDHitFinder"),
    fAdcDynamic(150),
    fAdcOffset(0),
    fAdcBits(0),
    fAdcSteps(-1),
    fAdcStepSize(-1),
    fDigis(NULL),
    fHits(new TClonesArray("CbmMvdHit")),
    fClusters(new TClonesArray("CbmMvdCluster")),
    fMatches(new TClonesArray("CbmMvdHitMatch")),
    fDigiMatch(NULL),
    fPixelChargeHistos(NULL),
    fTotalChargeInNpixelsArray(NULL),
    fResolutionHistoX(NULL),
    fResolutionHistoY(NULL),
    fResolutionHistoCleanX(NULL),
    fResolutionHistoCleanY(NULL),
    fResolutionHistoMergedX(NULL),
  fResolutionHistoMergedY(NULL),
  fBadHitHisto(NULL),
  fGausArray(NULL),
  fGausArrayIt(-1),
  fGausArrayLimit(500000),
  fDigiMap(),
  fDigiMapIt(),
  h(NULL),
  h3(NULL),
  h1(NULL),
  h2(NULL),
  Qseed(NULL),
  fFullClusterHisto(NULL),
  c1(NULL),
  fNEvent(0),
  fMode(0),
  fCounter(0),
  fSigmaNoise(15.),
  fSeedThreshold(1.),
  fNeighThreshold(1.),
  fShowDebugHistos(kFALSE),
  fUseMCInfo(kFALSE),
  fLayerRadius(0.),
  fLayerRadiusInner(0.),
  fLayerPosZ(0.),
  fHitPosX(0.),
  fHitPosY(0.),
  fHitPosZ(0.),
  fHitPosErrX(0.0005),
  fHitPosErrY(0.0005),
  fHitPosErrZ(0.0),
  fBranchName("MvdDigi"),
  fBranchNameMatch("MvdDigiMatch"),
  fDigisInCluster(-1),
  fAddNoise(kFALSE),
  fStationMap()
{
    CbmMvdCluster* clusterTemp= new CbmMvdCluster;
    fDigisInCluster= clusterTemp->GetMaxDigisInThisObject(); // read the number of memory cells from the cluster object
    delete clusterTemp;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdFindHits::CbmMvdFindHits(const char* name, Int_t iMode,
			       Int_t iVerbose)
  : FairTask(name, iVerbose),
    fAdcDynamic(150),
    fAdcOffset(0),
    fAdcBits(0),
    fAdcSteps(-1),
    fAdcStepSize(-1),
    fDigis(NULL),
    fHits(new TClonesArray("CbmMvdHit")),
    fClusters(new TClonesArray("CbmMvdCluster")),
    fMatches(new TClonesArray("CbmMvdHitMatch")),
    fDigiMatch(NULL),
    fPixelChargeHistos(NULL),
    fTotalChargeInNpixelsArray(NULL),
    fResolutionHistoX(NULL),
    fResolutionHistoY(NULL),
    fResolutionHistoCleanX(NULL),
    fResolutionHistoCleanY(NULL),
    fResolutionHistoMergedX(NULL),
  fResolutionHistoMergedY(NULL),
  fBadHitHisto(NULL),
  fGausArray(NULL),
  fGausArrayIt(-1),
  fGausArrayLimit(500000),
  fDigiMap(),
  fDigiMapIt(),
  h(NULL),
  h3(NULL),
  h1(NULL),
  h2(NULL),
  Qseed(NULL),
  fFullClusterHisto(NULL),
  c1(NULL),
  fNEvent(0),
  fMode(iMode),
  fCounter(0),
  fSigmaNoise(15.),
  fSeedThreshold(1.),
  fNeighThreshold(1.),
  fShowDebugHistos(kFALSE),
  fUseMCInfo(kFALSE),
  fLayerRadius(0.),
  fLayerRadiusInner(0.),
  fLayerPosZ(0.),
  fHitPosX(0.),
  fHitPosY(0.),
  fHitPosZ(0.),
  fHitPosErrX(0.0005),
  fHitPosErrY(0.0005),
  fHitPosErrZ(0.0),
  fBranchName("MvdDigi"),
  fBranchNameMatch("MvdDigiMatch"),
  fDigisInCluster(-1),
  fAddNoise(kFALSE),
  fStationMap()
{    
    CbmMvdCluster* clusterTemp= new CbmMvdCluster;
    fDigisInCluster= clusterTemp->GetMaxDigisInThisObject(); // read the number of memory cells from the cluster object
    delete clusterTemp;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMvdFindHits::~CbmMvdFindHits() {

    if ( fHits ) {
	fHits->Delete();
	delete fHits;
    }

    if ( fClusters ) {
	fClusters->Delete();
	delete fClusters;
    }

    if ( fDigis ) {
	fDigis->Delete();
	delete fDigis;
    }
    if ( fMatches ) {
	fMatches->Delete();
	delete fMatches;
    }

}
// -------------------------------------------------------------------------

// -----    Virtual private method Init   ----------------------------------
InitStatus CbmMvdFindHits::Init() {

  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << "-I- Initialising " << GetName() << " ...." << endl; 

    //11.12.08 cdritsa: start: copy Volker's code to parametrise mvd-geometry
    // *****  Get MVD geometry
    Int_t nStations = GetMvdGeometry();
    if ( ! nStations ) {
	cout << "-W- " << GetName() << "::Init: No MVD stations in geometry!"
	    << endl << "   Task will be inactive!" << endl;
	fActive = kFALSE;
        return kERROR;
    }
    //11.12.08 cdritsa: end-------------------------


    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if ( ! ioman ) {
	cout << "-E- " << GetName() << "::Init: No FairRootManager!" << endl;
	return kFATAL;
    }

    fDigis = (TClonesArray*) ioman->GetObject(fBranchName);
    /*fDigiMatch= (TClonesArray*) ioman->GetObject(fBranchNameMatch);
    if(fDigiMatch){fUseMCInfo=kTRUE;} else {*/fUseMCInfo=kFALSE;
    
 
    Register();
    //if(fShowDebugHistos){
    fResolutionHistoX=new TH1F("SinglePointResolution_X","SinglePointResolution_X",10000,-0.0100,0.0100);
    fResolutionHistoY=new TH1F("SinglePointResolution_Y","SinglePointResolution_Y",10000,-0.0100,0.0100);
    fResolutionHistoCleanX=new TH1F("SinglePointResolution_X_Clean","SinglePointResolution_X_Clean",10000,-0.0100,0.0100);
    fResolutionHistoCleanY=new TH1F("SinglePointResolution_Y_Clean","SinglePointResolution_Y_Clean",10000,-0.0100,0.0100);
    fResolutionHistoMergedX=new TH1F("SinglePointResolution_X_Merged","SinglePointResolution_X_Merged",10000,-0.0100,0.0100);
    fResolutionHistoMergedY=new TH1F("SinglePointResolution_Y_Merged","SinglePointResolution_Y_Merged",10000,-0.0100,0.0100);
    fBadHitHisto            = new TH2F("BadHits","Hits above 0.003cm",1000,-2.5,2.5,1000,-2.5,2.5);
    fFullClusterHisto = new TH1F("ChargeOfAllPixels","ChargeOfAllPixels",12000,0,12000);
    //}

    TH1F* histo;
    TH1F* histoTotalCharge;
    char* histoName= new char[20];
    char* histoTotalChargeName= new char[50];

    //Add charge collection histograms
    fPixelChargeHistos=new TObjArray();
    for (Int_t i=0; i<49; i++)
    {
	sprintf(histoName,"ChargePixel%i",i+1);
	histo=new TH1F(histoName,histoName,12000,0,12000);
	fPixelChargeHistos->AddLast(histo);
    };

     fTotalChargeInNpixelsArray = new TObjArray();
    for (Int_t i=0; i<49; i++)
    {
	sprintf(histoTotalChargeName,"totalChargeInNPixels%i",i+1);
	histoTotalCharge=new TH1F(histoTotalChargeName,histoTotalChargeName,12000,0,12000);
	fTotalChargeInNpixelsArray->AddLast(histoTotalCharge);
    };

    //Number 49
    histo = new TH1F("ChargePixelSeed","ChargePixelSeed",200,0,14000);
    fPixelChargeHistos->AddLast(histo);
    //Number 50
    histo = new TH1F("ChargePixel9of49","ChargePixel 9 Of 49",200,0,14000);
    fPixelChargeHistos->AddLast(histo);
    //Number 51
    histo = new TH1F("ChargePixel25of49","ChargePixel 25 Of 49",200,0,14000);
    fPixelChargeHistos->AddLast(histo);
    //Number 52
    histo = new TH1F("ChargePixel49of49","ChargePixel 49 Of 49",200,0,14000);
    fPixelChargeHistos->AddLast(histo);
    //Number 53
    histo = new TH1F("ChargePixel9of49Sorted","ChargePixel 9 Of 49 Sorted",200,0,14000);
    fPixelChargeHistos->AddLast(histo);
    //Number 54
    histo = new TH1F("ChargePixel25of49Sorted","ChargePixel 25 Of 49 Sorted",200,0,14000);
    fPixelChargeHistos->AddLast(histo);
    //Number 55
    histo = new TH1F("ChargePixel49of49Sorted","ChargePixel 49 Of 49 Sorted",49,0.5,49.5);
    fPixelChargeHistos->AddLast(histo);
    //Number 56
    //histo = new TH1F("ChargePixel49Of49Sorted","ChargePixel 49 Of 49 Sorted",49,0.5,49.5);
    //fPixelChargeHistos->AddLast(histo);

    Qseed = new TH1F("Qseed","ChargeOnSeedPixel",1000,0,12000);

    cout << "------- CbmMvdFindHits::Init completed ------" << endl;
    cout << "---------------------------------------------" << endl;

   
    Int_t adcMax = fAdcOffset + fAdcDynamic;
    fAdcSteps= (Int_t)TMath::Power(2,fAdcBits);
    fAdcStepSize  = fAdcDynamic/fAdcSteps;

    fGausArray=new Float_t[fGausArrayLimit];
    for(Int_t i=0;i<fGausArrayLimit;i++){fGausArray[i]=gRandom->Gaus(0, fSigmaNoise);};
    fGausArrayIt=0;

    delete histoName;
    delete histoTotalChargeName;

    return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Virtual public method Reinit   ----------------------------------
InitStatus CbmMvdFindHits::ReInit() {
    cout << "-I- " <<"CbmMvdFindHits::ReInt---------------"<<endl;
}
// -------------------------------------------------------------------------


// -----   Virtual public method Exec   --------------
void CbmMvdFindHits::Exec(Option_t* opt) {


    if( fDigis->GetEntriesFast() == 0 ){
	cout << "-I- " << GetName() << "::Exec: No digis found, event skipped. " << endl;
        fCounter++;
	return;
    }

    Reset(); //clear arrays

    //cdritsa: 15/12/08 start
    map<Int_t, CbmMvdStation*>::iterator stationIt;
    for (stationIt=fStationMap.begin(); stationIt!=fStationMap.end();
	 stationIt++) (*stationIt).second->Clear();
    //cdritsa: 15/12/08 end

    fNEvent++;

    vector<Int_t>* clusterArray=new vector<Int_t>;
        
    CbmMvdDigi* digi = NULL;
    
    Int_t iDigi=0;
    digi = (CbmMvdDigi*) fDigis->At(iDigi);
    
    
    if(!digi){
	cout << "-E- : CbmMvdFindHits - Fatal: No Digits found in this event."<< endl;
    }


    Int_t nDigis = fDigis->GetEntriesFast();


    //----------------------------------------------------------------------------------
    // Generate random number and call it noise
    // add the noise to the charge of the digis
    //----------------------------------------------------------------------------------
    if( fAddNoise == kTRUE ){

	cout << "-I- " << "CbmMvdFindHits: Calling method AddNoiseToDigis()...\n" << endl;

	for ( iDigi=0; iDigi<nDigis; iDigi++ ) {

	    digi  = (CbmMvdDigi*) fDigis->At(iDigi);
	    AddNoiseToDigis(digi);
	}
    }
    //----------------------------------------------------------------------------------
    //----------------------------------------------------------------------------------

    Double_t pixelSizeX = digi->GetPixelSizeX();;
    Double_t pixelSizeY = digi->GetPixelSizeY();;

    //----------------------------------------------------------------------------------
    // -------- Create Fake Digis ------------------------------------------------------
    //----------------------------------------------------------------------------------
    if( fMode == 1 ){

    	for (stationIt=fStationMap.begin(); stationIt!=fStationMap.end(); stationIt++) {

	    CbmMvdStation* station = (*stationIt).second;

	    Double_t layerRadius = station->GetRmax();

    	    if( gDebug>0 ){
		cout << "-I- " << GetName() << ": Fakes: stationIt: " << station->GetVolumeId() << " , layerRadius " << layerRadius << endl;
		cout << "-I- " << GetName() << ": Calling method GenerateFakeDigis()..." << endl;
	    }

	    GenerateFakeDigis(station,pixelSizeX, pixelSizeY);
	;
	}

    }

    //----------------------------------------------------------------------------------


    nDigis = fDigis->GetEntriesFast();
    TArrayS* pixelUsed  = new TArrayS(nDigis);

    for ( iDigi=0; iDigi<nDigis; iDigi++) {
	pixelUsed->AddAt(0,iDigi);
    }


    if( gDebug>0 ){
	cout << GetName() << ": NDigis now " << nDigis << endl;
	cout << GetName() << ": CbmMvdFindHits: Entering loop on Mvd stations ..." << endl;
    }

    //----------------------------------------------------------------------------------
    //-------------- Loop over MVD stations  -------------------------------------------
    //----------------------------------------------------------------------------------

    for (stationIt=fStationMap.begin(); stationIt!=fStationMap.end(); stationIt++) {

	CbmMvdStation* station = (*stationIt).second;

        Double_t layerRadius = station->GetRmax();

	if( gDebug>0 ){
	    cout << "-I- " << GetName() << ": station->GetVolumeId(): " << station->GetVolumeId() << " , layerRadius " << layerRadius << endl;
	}


	// DigiMap MDX
	fDigiMap.clear();

	for(Int_t k=0;k<nDigis;k++){

	    digi = (CbmMvdDigi*) fDigis->At(k);
	    
	    if(GetAdcCharge(digi->GetCharge()) < fNeighThreshold ) continue;

	    pair<Int_t, Int_t> a (digi->GetPixelX(),digi->GetPixelY());
	    fDigiMap[a]=k;
	};


	if( gDebug>0 ){cout << "\n-I- " << GetName() << ": VolumeId " << station->GetVolumeId() << endl;}

	for ( iDigi=0; iDigi<nDigis; iDigi++) {

	    if( gDebug>0 && iDigi%10000==0 ){ cout << "-I- " << GetName() << " Digi:" << iDigi << endl; };

	    digi = (CbmMvdDigi*) fDigis->At(iDigi);

	    if ( digi->GetStationNr() != station->GetStationNr() ) { continue; }


	    /*
	     ---------------------------------------------------------
	     check if digi is above threshold (define seed pixel)
	     then check for neighbours.
	     Once the cluster is created (seed and neighbours)
	     calculate the position of the hit
	     using center of gravity (CoG) method.
	     ---------------------------------------------------------
	     */

	    if( gDebug>0 ){
		cout << "-I- " << "CbmMvdFindHits: Checking for seed pixels..." << endl;
	    }

	    if( ( GetAdcCharge(digi->GetCharge())>=fSeedThreshold ) && ( pixelUsed->At(iDigi)==kFALSE ) ){
cout << endl << "found pixel" << endl;
		clusterArray->clear();
		clusterArray->push_back(iDigi);
		

		pixelUsed->AddAt(1,iDigi);

		pair<Int_t, Int_t> a(digi->GetPixelX(), digi->GetPixelY());
    		fDigiMapIt = fDigiMap.find(a);
		if (fDigiMapIt != fDigiMap.end())
    		fDigiMap.erase(fDigiMapIt);

		for ( Int_t iCluster=0; iCluster<clusterArray->size(); iCluster++ ){

		    if( gDebug>0 ){
			cout << "-I- " << " CbmMvdFindHits: Calling method CheckForNeighbours()..." << endl;
		    }

		    CheckForNeighbours(clusterArray, iCluster, pixelUsed);

		}

		//Calculate the center of gravity of all pixels in the cluster.
		TVector3 pos(0,0,0);
                TVector3 dpos(0,0,0);

		if( gDebug>0 ){ cout << "-I- " << " CbmMvdFindHits: Calling method CreateHit()..." << endl; }
		
		CreateHit(clusterArray, station ,pos, dpos); // Add cluster to array. Return pointer for filling the CbmMvdHitMatch
		
		
		
		
	    }// if AdcCharge>threshold
	}// loop on digis
    }// loop on detectors

    //----------------------------------------------------------------------------------
     //------------- End of Detector Loops ----------------------------------------------
    //----------------------------------------------------------------------------------

    cout << "-I-  End of task " << GetName() << ": Event Nr: " << fNEvent << ", nDIGIS: "<<nDigis << ", nHits:"<<fHits->GetEntriesFast()<<endl;

    delete pixelUsed;
    delete clusterArray;



}


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// -----  Private method GetMvdGeometry ------------------------------------

/**  The method assumes the following convention:
 **  --- The MVD stations are of the shape TGeoTube.
 **  --- The name of a station is mvdstationxx, where xx is the number
 **      of the station.
 **  --- The stations are numbered from 0 on consecutively.
 **  V. Friese, 4 December 2008
 **/


Int_t CbmMvdFindHits::GetMvdGeometry() {
  cout << "-I- " << GetName() << " : Reading MVD geometry..." << endl;
  Int_t iStation =  1;
  Int_t volId    = -1;
  fStationMap.clear();

    do {

    // Volume name according to convention
    TString volName  = Form("mvdstation%02i", iStation);
    volId = gGeoManager->GetUID(volName);
    if ( volId > -1 ) {

      // Get shape parameters
      TGeoVolume* volume = gGeoManager->GetVolume(volName.Data());
      TGeoTube* tube = (TGeoTube*) volume->GetShape();
      Double_t rmin = tube->GetRmin();
      Double_t rmax = tube->GetRmax();
      Double_t d    = tube->GetDz();

      // Full path to node 
      TString nodeName = "/cave_1/pipevac1_0/" + volName + "_0";

  TString mother;
  TString pipeName = "pipevac1";
  Int_t pipeID;
  TGeoNode* pipeNode;
  TString motherName; 
  mother = "cave1/pipevac1";

      if (!gGeoManager->CheckPath(mother.Data()))
         {
	gGeoManager->CdTop();
	gGeoManager->CdDown(0);
	motherName=gGeoManager->GetPath();
	mother = motherName;
	mother += "/";
	mother += pipeName;
	mother += "_0/mvd_v07a_0/";
	gGeoManager->CdTop();
	}
      else
	mother = "cave_1/pipevac1_0/";

       nodeName = mother + volName + "_0";

 cout << endl << "MotherNode is : " << mother << endl;

      // Get z position of node
      Bool_t nodeFound = gGeoManager->cd(nodeName.Data());

	if ( ! nodeFound ) { // no node at all found
	cout << "-E- " << GetName() << "::SetMvdGeometry: Node " << nodeName
	     << " not found in geometry!" << endl;
	Fatal("SetMvdGeometry", "Node not found");
      }
      Double_t local[3] = {0., 0., 0.};  // Local centre of volume
      Double_t global[3];                // Global centre of volume
      gGeoManager->LocalToMaster(local, global);
      Double_t z = global[2];


      // Check for already existing station with the same ID
      // (Just in case, one never knows...)
      if ( fStationMap.find(iStation) != fStationMap.end() ) {
	cout << "-E- " << GetName() << "::GetMvdGeometry: " 
	     << "Volume ID " << iStation << " already in map!" << endl;
	Fatal("GetMvdGeometry", "Double station number in TGeoManager!");
      }

      // Create new CbmMvdStation and add it to the map
      fStationMap[iStation] = new CbmMvdStation(volName.Data(), iStation, volId,
					     z, d, rmin, rmax);
      fStationMap[iStation]->Print();

      iStation++;

    }     // Volume found

  } while ( volId > -1 );

   
  return iStation - 1;
}


//--------------------------------------------------------------------------


//--------------------------------------------------------------------------

void CbmMvdFindHits::AddNoiseToDigis(CbmMvdDigi* digi){
    Double_t noise  = fGausArray[fGausArrayIt++]; // noise is simulated by a gauss
    if (fGausArrayIt-2>fGausArrayLimit){fGausArrayIt=0;};
    Double_t charge = digi->GetCharge() + noise;
    digi->SetCharge((int)charge);
}


//--------------------------------------------------------------------------


//--------------------------------------------------------------------------

void CbmMvdFindHits::GenerateFakeDigis(CbmMvdStation* station, Double_t pixelSizeX, Double_t pixelSizeY){

    //max index of pixels
    //Int_t nx = TMath::Nint(2*fLayerRadius/pixelSizeX);
    //Int_t ny = TMath::Nint(2*fLayerRadius/pixelSizeY);

    //cdritsa: parametrise geometry: 15/12/08
    Double_t layerRadius = station->GetRmax();
    Double_t layerRadiusInner = station->GetRmin();

    Int_t nx = int(2*layerRadius/pixelSizeX);
    Int_t ny = int(2*layerRadius/pixelSizeY);

    Double_t x;
    Double_t y;
    Double_t distance2;
    Double_t noise;
    Double_t r2       = layerRadius*layerRadius;
    Double_t r2_inner = layerRadiusInner*layerRadiusInner;

    for( Int_t i=0; i<nx; i++){

	x = (i+0.5)*pixelSizeX - layerRadius;

	for( Int_t j=0; j<ny; j++){

	    y = (j+0.5)*pixelSizeY - layerRadius;

	    distance2 =  x*x + y*y ;


	    if(  distance2>r2 || distance2<r2_inner )  continue;

	   noise  = fGausArray[fGausArrayIt++]; // noise is simulated by a gauss
    	   if (fGausArrayIt-2>fGausArrayLimit){fGausArrayIt=0;}; 

	    if ( noise>fSeedThreshold /*&& pixel is not used ???*/){
                 Int_t nDigis = fDigis->GetEntriesFast();
		 CbmMvdDigi* fakeDigi= 
		       new ((*fDigis)[nDigis]) CbmMvdDigi(station->GetVolumeId(), i,j, noise, pixelSizeX,pixelSizeY);
		 
		 Int_t data[5]; 
		 Float_t data2[5];
		 
		 CbmMvdDigiMatch* fakeDigiMatch=
		       new ((*fDigiMatch)[nDigis]) CbmMvdDigiMatch(0., -1, -1, -1);
		       
		       
		}
	}
    }



}

//--------------------------------------------------------------------------


//--------------------------------------------------------------------------

void CbmMvdFindHits::CheckForNeighbours(vector<Int_t>* clusterArray, Int_t clusterDigi, TArrayS* pixelUsed)
{
    CbmMvdDigi* seed = (CbmMvdDigi*)fDigis->At(clusterArray->at(clusterDigi));
    CbmMvdDigiMatch* seedMatch;
    if (fUseMCInfo){seedMatch = (CbmMvdDigiMatch*)fDigiMatch->At(clusterArray->at(clusterDigi));}
    
    CbmMvdDigi* digiOfInterest;
    CbmMvdDigiMatch* digiMatchOfInterest;
    	
    // Remove Seed Pixel from list of non-used pixels
    Int_t channelX=seed->GetPixelX();
    Int_t channelY=seed->GetPixelY();
    pair<Int_t, Int_t> a(channelX, channelY);
    
    // Find first neighbour
    
    a=std::make_pair(channelX+1, channelY);
    fDigiMapIt=fDigiMap.find(a);

    if (!(fDigiMapIt == fDigiMap.end()))
    	{ 
	  Int_t i=fDigiMap[a];
	  	  	  
	  // Only digis depassing fNeighThreshold are in the map, no cut required
	  clusterArray->push_back(i);
	  	  
	  pixelUsed->AddAt(1,i); // block pixel for the seed pixel scanner
	  fDigiMap.erase(fDigiMapIt); // block pixel for the neighbour pixel scanner
	 } 
	  
    a = std::make_pair(channelX-1, channelY);
    fDigiMapIt = fDigiMap.find(a);

    if (!(fDigiMapIt == fDigiMap.end()))
    {
	Int_t i=fDigiMap[a];
	// Only digits depassing fNeighThreshold are in the map, no cut required
	clusterArray->push_back(i);
	pixelUsed->AddAt(1,i); // block pixel for the seed pixel scanner
	fDigiMap.erase(fDigiMapIt); // block pixel for the neighbour pixel scanner
    }

    a = std::make_pair(channelX, channelY-1);
    fDigiMapIt=fDigiMap.find(a);
    if (!(fDigiMapIt == fDigiMap.end()))
    {
	Int_t i=fDigiMap[a];
	// Only digits depassing fNeighThreshold are in the map, no cut required
	clusterArray->push_back(i);
	pixelUsed->AddAt(1,i); // block pixel for the seed pixel scanner
	fDigiMap.erase(fDigiMapIt); // block pixel for the neighbour pixel scanner
    }

    a = std::make_pair (channelX, channelY+1);
    fDigiMapIt=fDigiMap.find(a);

    if (!(fDigiMapIt == fDigiMap.end()))
    {
	Int_t i=fDigiMap[a];
	// Only digis depassing fNeighThreshold are in the map, no cut required
	clusterArray->push_back(i);
	pixelUsed->AddAt(1,i); // block pixel for the seed pixel scanner
	fDigiMap.erase(fDigiMapIt); // block pixel for the neighbour pixel scanner
    }




}

//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
void CbmMvdFindHits::CreateHit(vector<Int_t>* clusterArray, CbmMvdStation* station, TVector3 &pos, TVector3 &dpos)
{

    //loop on cluster array elements
    //calculate the CoG for this cluster

    Int_t    clusterSize = clusterArray->size();
    
    
    Double_t layerRadius = station->GetRmax();
    Double_t layerPosZ = station->GetZ();

    CbmMvdDigi* pixelInCluster = (CbmMvdDigi*) fDigis->At(clusterArray->at(0));
    CbmMvdDigiMatch* pixelInClusterMatch;
    if(fUseMCInfo) {pixelInClusterMatch= (CbmMvdDigiMatch*)fDigiMatch->At(clusterArray->at(0));};
    
    Int_t detId = pixelInCluster->GetDetectorId();

    // Calculate the center of gravity of the charge of a cluster
    
    ComputeCenterOfGravity(clusterArray, pos, dpos, station);
    
    //if (fUseMCInfo)
    //{if(TMath::Abs(pos.x()-pixelInClusterMatch->GetPointX(0)) > (pixelInCluster->GetPixelSizeX()/2.)){
    //  cout << pos.x() << " " << pixelInClusterMatch->GetPointX(0) << endl;}}

    Int_t indexX, indexY;
    //Double_t x,y;
    
    LabToIndex(pos.X(), pos.Y(), pixelInCluster->GetPixelSizeX(),pixelInCluster->GetPixelSizeY(), indexX, indexY, station);
    //IndexToLab(indexX, indexY,pixelInCluster->GetPixelSizeX(), pixelInCluster->GetPixelSizeY(), x,y, station);
    
    //if (TMath::Abs(pos.X()-x)>(pixelInCluster->GetPixelSizeX()/2)) {cout << "Transformation Error X " << endl;}
    //if (TMath::Abs(pos.Y()-y)>(pixelInCluster->GetPixelSizeY()/2)) {cout << "Transformation Error Y " << endl;}
    
    if (fShowDebugHistos) {UpdateDebugHistos(clusterArray, station,indexX,indexY );};
    
    //Fill HitClusters
    
    Int_t i=0;
    Int_t* digiArray=new Int_t[fDigisInCluster];
    
    for(i=0;i<fDigisInCluster;i++){digiArray[i]=0;};
    
    Int_t latestClusterIndex=-1;
    Int_t digisInArray=0;
    
    CbmMvdCluster* latestCluster= NULL;
    Int_t nClusters=-1;
    
    for(i=0;i<clusterSize;i++) {
      
      digiArray[i%fDigisInCluster]=clusterArray->at(i);
      digisInArray=digisInArray+1;
      
      if(digisInArray==fDigisInCluster){ // intermediate buffer full

	  nClusters = fClusters->GetEntriesFast();
	  CbmMvdCluster* clusterNew=new ((*fClusters)[nClusters]) CbmMvdCluster(digiArray, digisInArray, 
									      clusterSize, latestClusterIndex);
	  if(latestCluster){latestCluster->SetNeighbourUp(nClusters);}
	  latestCluster      = clusterNew;
	  latestClusterIndex = nClusters;
	  digisInArray=0;
	
      }
    }
    
    if(digisInArray!=0){
      nClusters = fClusters->GetEntriesFast();
      CbmMvdCluster* clusterNew=new ((*fClusters)[nClusters]) CbmMvdCluster(digiArray, digisInArray, 									      			      clusterSize, latestClusterIndex);
      clusterNew->SetNeighbourUp(-1);
      if(latestCluster){latestCluster->SetNeighbourUp(nClusters);};
    };
    
    delete digiArray;
    
    // Save hit into array
    Int_t nHits = fHits->GetEntriesFast();
    new ((*fHits)[nHits]) CbmMvdHit(station->GetStationNr(), pos, dpos, indexX, indexY, nClusters, 0);
    
    // Fill CbmmvdHitMatch, obsolet.
    CbmMvdDigi* digiOfInterest;
    
    if(fUseMCInfo){
        Int_t digiIndexX, digiIndexY;
	Int_t k=-1;
	Bool_t digiIsCentral=kFALSE;
	do {
	  k++;
	  digiOfInterest=(CbmMvdDigi*) fDigis->At(clusterArray->at(k));
	  digiIndexX=digiOfInterest->GetPixelX();
	  digiIndexY=digiOfInterest->GetPixelY();
	  digiIsCentral=(digiIndexX==indexX)&&(digiIndexY==indexY);
	  
	}
	while ((!digiIsCentral) && (k<clusterArray->size()));
	if (digiIsCentral) {
	  CbmMvdDigiMatch* centralDigiMatch= (CbmMvdDigiMatch*) fDigiMatch->At(clusterArray->at(k));
	  new ((*fMatches)[nHits]) CbmMvdHitMatch(0, 0, centralDigiMatch->GetDominatorTrackID(),
						  centralDigiMatch->GetDominatorPointID(), 
						  centralDigiMatch->GetNContributors());  
	  
	} 
	else {
	  new ((*fMatches)[nHits]) CbmMvdHitMatch(0., 0, -1, -1);  
	}
    }
	  
}
      
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------      
    
void CbmMvdFindHits::UpdateDebugHistos(vector<Int_t>* clusterArray, CbmMvdStation* station,Int_t seedIndexX, Int_t seedIndexY){    
    
    /************************************************************

    Algorithm for cluster shapes

     ************************************************************/
    
    
    Float_t chargeArray3D[fChargeArraySize][fChargeArraySize];
    Float_t chargeArray[fChargeArraySize*fChargeArraySize];
    Short_t seedPixelOffset=fChargeArraySize/2; // 3 for 7, 2 for 5
    Float_t xCentralTrack, yCentralTrack;
    CbmMvdDigiMatch* centralDigiMatch=0;
    Float_t clusterCharge=0;
    
    Int_t clusterSize=clusterArray->size();
    
    for(Int_t k=0; k<fChargeArraySize;k++){
      for(Int_t j=0; j<fChargeArraySize; j++){
	chargeArray3D[k][j]=gRandom->Gaus(0,fSigmaNoise);
      }
    }
    
    for(Int_t k=0;k<clusterSize;k++){
      CbmMvdDigi* digi=(CbmMvdDigi*)fDigis->At(clusterArray->at(k));
      
      clusterCharge=clusterCharge+digi->GetCharge();
      
      Int_t relativeX=digi->GetPixelX()+seedPixelOffset-seedIndexX;
      Int_t relativeY=digi->GetPixelY()+seedPixelOffset-seedIndexY;
      
      //for debugging
      //cout << relativeX << " " << relativeY << " " <<digi->GetPixelX()<< " " << seedIndexX << endl;
      
      
      if (relativeX>=0 && relativeX<fChargeArraySize && relativeY>=0 && relativeY<fChargeArraySize){
	chargeArray3D[relativeX][relativeY]=digi->GetCharge();
      }
      
      if((relativeX-seedPixelOffset==0) && (relativeY-seedPixelOffset==0)) {//seed digiArray
	centralDigiMatch = (CbmMvdDigiMatch*) fDigiMatch->At(clusterArray->at(k));
	if(centralDigiMatch){
	  xCentralTrack=centralDigiMatch->GetDominatorX();
	  yCentralTrack=centralDigiMatch->GetDominatorY();
	}
      }

    
    }
    
    //for debugging
    //for(Int_t i=0;i<fChargeArraySize;i++)
    //{for (Int_t j=0;j<fChargeArraySize;j++) {cout << chargeArray3D[i][j] << " " ;}
    // cout << endl;
    //} 
    
    //cout << endl;
    //Fatal("Break","Break");
    
    fFullClusterHisto->Fill(clusterCharge);
    
    for(Int_t k=0; k<fChargeArraySize;k++){
      for(Int_t j=0; j<fChargeArraySize; j++){
	chargeArray[fChargeArraySize*k+j]=chargeArray3D[k][j];
      }
    }
    
    Int_t qSeed=chargeArray3D[seedPixelOffset][seedPixelOffset];
    Int_t q9=0;
    
    for(Int_t k=seedPixelOffset-1; k<seedPixelOffset+1;k++){
      for(Int_t j=seedPixelOffset-1; j<seedPixelOffset+1; j++){
	q9=q9+chargeArray3D[k][j];
      }
    };
    
    Int_t counter=0;
    
    if(fChargeArraySize<=7){
      for(Int_t i=0;i<(fChargeArraySize*fChargeArraySize);i++){
	((TH1F*) fPixelChargeHistos->At(i))->Fill(chargeArray[i]);
	//cout << counter++<<" Charge: " << chargeArray[i]<< endl;
      };
    };
    
    //cout << "End of Cluster: "<<fChargeArraySize*fChargeArraySize << endl;
    
    Int_t q25=0;
    Int_t q49=0;
    
   
    
    for(Int_t k=seedPixelOffset-2; k<seedPixelOffset+2;k++){
	for(Int_t j=seedPixelOffset-2; j<seedPixelOffset+2; j++){q25=q25+chargeArray3D[k][j];}
    };
    
    if (fChargeArraySize>= 7) {
      for(Int_t k=seedPixelOffset-3; k<seedPixelOffset+3;k++){
	for(Int_t j=seedPixelOffset-3; j<seedPixelOffset+3; j++){q49=q49+chargeArray3D[k][j];}
      }
    }  

    ((TH1F*) fPixelChargeHistos->At(49))->Fill(qSeed);
    ((TH1F*) fPixelChargeHistos->At(50))->Fill(q9);
    ((TH1F*) fPixelChargeHistos->At(51))->Fill(q25);
    ((TH1F*) fPixelChargeHistos->At(52))->Fill(q49);

    if( fHitPosX-xCentralTrack>0.003 && fHitPosZ<6 ) { fBadHitHisto->Fill(fHitPosX,fHitPosY); }
    
    fResolutionHistoX->Fill(fHitPosX-xCentralTrack);
    fResolutionHistoY->Fill(fHitPosY-yCentralTrack);
    
    
    if (centralDigiMatch){
      if((centralDigiMatch->GetNContributors())==1) {
	fResolutionHistoCleanX->Fill(fHitPosX-xCentralTrack);
	fResolutionHistoCleanY->Fill(fHitPosY-yCentralTrack);

      }
      else {
	fResolutionHistoMergedX->Fill(fHitPosX-xCentralTrack);
	fResolutionHistoMergedY->Fill(fHitPosY-yCentralTrack);
	
	
	
      };
      
    };
	
	//Prepare selection of crowns for charge bow histograms
	
  
    
    Int_t orderArray[fChargeArraySize*fChargeArraySize];
    
    TMath::Sort(fChargeArraySize*fChargeArraySize,chargeArray,orderArray,kTRUE);
    
    Float_t qSort=0;
    for (Int_t i=0; i<9; i++){ qSort+=chargeArray[orderArray[i]]; };
    ((TH1F*) fPixelChargeHistos->At(53))->Fill(qSort);
    
    for (Int_t i=9; i<25; i++) { qSort+=chargeArray[orderArray[i]]; };
    ((TH1F*) fPixelChargeHistos->At(54))->Fill(qSort);
		
    TH1F* histoTotalCharge;
    qSort=0;
    for (Int_t i=0; i<fChargeArraySize*fChargeArraySize; i++){
      qSort+=chargeArray[orderArray[i]];
      ((TH1F*) fPixelChargeHistos->At(55))->Fill(i+1,qSort);
      histoTotalCharge =(TH1F*) fTotalChargeInNpixelsArray->At(i) ;
      histoTotalCharge->Fill(qSort);
    };

  
   	
  
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------

void CbmMvdFindHits::ComputeCenterOfGravity(vector<Int_t>* clusterArray, TVector3& pos, 
					    TVector3& dpos,CbmMvdStation* station){
  
  Double_t numeratorX  = 0;
  Double_t numeratorY  = 0;
  Double_t denominator = 0;
  Double_t pixelSizeX  = 0;
  Double_t pixelSizeY  = 0;
  Int_t charge;
  Int_t xIndex;
  Int_t yIndex;
  Double_t x,y;
  Double_t layerPosZ=station->GetZ();
  Double_t layerRadius=station->GetRmax();
  CbmMvdDigi* pixelInCluster;
  
  Int_t clusterSize=clusterArray->size();
   
  for( Int_t iCluster = 0; iCluster<clusterSize; iCluster++ ){
         
    	pixelInCluster = (CbmMvdDigi*) fDigis->At(clusterArray->at(iCluster));
	

	charge      = GetAdcCharge(pixelInCluster->GetCharge());
	xIndex      = pixelInCluster->GetPixelX();
	yIndex      = pixelInCluster->GetPixelY();
	pixelSizeX  = pixelInCluster->GetPixelSizeX();
	pixelSizeY  = pixelInCluster->GetPixelSizeY();

	if(gDebug>0){
	    cout << "-I- " << "CbmMvdFindHits:: iCluster= "<<iCluster << " , clusterSize= " << clusterSize << endl;
	    cout << "-I- " << "CbmMvdFindHits::xIndex " << xIndex << " , yIndex " << yIndex << " , charge = " << pixelInCluster->GetAdcCharge(fAdcDynamic, fAdcOffset, fAdcBits) << endl;
	}
	
	IndexToLab(xIndex, yIndex,pixelSizeX, pixelSizeY, x,y, station);
	
	//Calculate x,y coordinates of the pixel in the detector ref frame
	//Double_t x = ( 0.5+double(xIndex) )*pixelSizeX;
	//Double_t y = ( 0.5+double(yIndex) )*pixelSizeY;

	Double_t xc = x*charge;
	Double_t yc = y*charge;

	
	numeratorX   += xc;
	numeratorY   += yc;
	denominator  += charge;

    }

    if(gDebug>0){
	cout << "-I- " << "CbmMvdFindHits::=========================\n " << endl;
	cout << "-I- " << "CbmMvdFindHits::numeratorX: " <<numeratorX<<" , numeratorY: " <<numeratorY << ", denominator: " << denominator << endl;
    }

    //Calculate x,y coordinates of the pixel in the laboratory ref frame
    if(denominator!=0) {
	fHitPosX = (numeratorX/denominator);
	fHitPosY = (numeratorY/denominator);
	fHitPosZ = layerPosZ;
    }else{
	fHitPosX = 0;
	fHitPosY = 0;
	fHitPosZ = 0;
    }
    if(gDebug>0){
	cout << "-I- " << "CbmMvdFindHits::-----------------------------------" << endl;
	cout << "-I- " << "CbmMvdFindHits::X hit= " << fHitPosX << " Y hit= "<<fHitPosY << " Z hit= "<<fHitPosZ <<endl;
	cout << "-I- " << "CbmMvdFindHits::X seed " <<int( (fHitPosX+layerRadius)/pixelSizeX )<< " , Y Seed = " << int( (fHitPosY+layerRadius)/pixelSizeY ) << " , charge = " << pixelInCluster->GetAdcCharge(fAdcDynamic, fAdcOffset, fAdcBits)<< endl;
	cout << "-I- " << "CbmMvdFindHits::-----------------------------------\n" << endl;
    }

    // pos = center of gravity (labframe), dpos uncertainty
    pos.SetXYZ(fHitPosX,fHitPosY,fHitPosZ);
    dpos.SetXYZ(fHitPosErrX, fHitPosErrY, fHitPosErrZ);
    
   

}

void CbmMvdFindHits::IndexToLab(Int_t indexX, Int_t indexY, Double_t pixelSizeX, Double_t pixelSizeY, 
				Double_t& labX, Double_t& labY, CbmMvdStation* station){
  
  Float_t layerRadius= station->GetRmax();
  labX = ( 0.5+double(indexX) )*pixelSizeX - layerRadius;
  labY = ( 0.5+double(indexY) )*pixelSizeY - layerRadius;
   
}

void CbmMvdFindHits::LabToIndex(Double_t labX, Double_t labY, Double_t pixelSizeX, Double_t pixelSizeY, Int_t& indexX, Int_t& indexY, CbmMvdStation* station){
  Float_t layerRadius= station->GetRmax();
  
  indexX = int( (fHitPosX+layerRadius)/pixelSizeX );  // find index of seed pixel in the detector frame
  indexY = int( (fHitPosY+layerRadius)/pixelSizeY );
  
}

// -----   Private method Register   ---------------------------------------
void CbmMvdFindHits::Register() {
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman) Fatal("Register",
		      "No FairRootManager");
  ioman->Register("MvdHit", "MVD", fHits, kTRUE);
  ioman->Register("MvdCluster", "MVD", fClusters, kTRUE);
  ioman->Register("MvdHitMatch", "MVD Hit Match", fMatches, kTRUE);


}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
void CbmMvdFindHits::Finish() {
    cout << "\n============================================================" << endl;
    cout << "-I- " << GetName() << "::Finish: Total events skipped: " << fCounter << endl;
    cout << "============================================================" << endl;
    cout << "-I- Parameters used" << endl;
    cout << "Gaussian noise [electrons]	: " << fSigmaNoise << endl;
    cout << "Noise simulated [Bool]	        : " << fAddNoise << endl;
    cout << "Threshold seed [ADC]            : " << fSeedThreshold << endl;
    cout << "Threshold neighbours [ADC]	: " << fNeighThreshold << endl;
    cout << "ADC - Bits			: " << fAdcBits << endl;
    cout << "ADC - Dynamic [electrons]	: " << fAdcDynamic << endl;
    cout << "ADC - Offset [electrons]	: " << fAdcOffset << endl;
    cout << "============================================================" << endl;
    
    
    TH1F* histo;
    TH2F* clusterShapeHistogram;
    
    
    if(fShowDebugHistos){
      
      TCanvas* canvas2=new TCanvas("HitFinderCharge","HitFinderCharge");
      //cout <<fChargeArraySize << endl;
      canvas2->Divide(2,2);
      canvas2->cd(1);
      
      
      if(fChargeArraySize<=7){
	clusterShapeHistogram= new TH2F("MvdClusterShape", "MvdClusterShape", fChargeArraySize, 0, fChargeArraySize,fChargeArraySize, 0, fChargeArraySize);
	
	for (Int_t i=0;i<fChargeArraySize*fChargeArraySize; i++) {
	  histo= (TH1F*)fPixelChargeHistos->At(i);
	  Float_t charge= histo->GetMean();
	  //cout <<i << " Charge " << charge << " xCluster: " << i%fChargeArraySize << " yCluster: " << i/fChargeArraySize << endl;
	  //histo->Fit("landau");
	  //TF1* fitFunction= histo->GetFunction("landau");
// 	  Double_t MPV=fitFunction->GetParameter(1);
	  clusterShapeHistogram->Fill(i%fChargeArraySize,i/fChargeArraySize,charge);
	  //canvas2->cd(i);
	  //histo->Draw();
	}
      }
      
      clusterShapeHistogram->Draw("Lego2");
      canvas2->cd(2);
      histo= (TH1F*)fPixelChargeHistos->At(24);
      histo->Draw();
      //cout <<"Mean charge" << histo->GetMean() << endl;
  /*    
      TCanvas* canvas=new TCanvas("HitFinderCanvas","HitFinderCanvas");
      canvas->Divide (2,3);
      canvas->cd(1);
      fResolutionHistoX->Draw();
      fResolutionHistoX->Write();
      canvas->cd(2);
      fResolutionHistoY->Draw();
      fResolutionHistoY->Write();
      canvas->cd(3);
      ((TH1F*)fPixelChargeHistos->At(49))->Draw();
      ((TH1F*)fPixelChargeHistos->At(49))->Fit("landau");
      canvas->cd(4);
      fFullClusterHisto->Draw();
      canvas->cd(5);
      ((TH1F*)fTotalChargeInNpixelsArray->At(0))->Draw();
      ((TH1F*)fTotalChargeInNpixelsArray->At(0))->Fit("landau");
	//fResolutionHistoMergedX->Write();
      canvas->cd(6);
      clusterShapeHistogram->Draw("Lego2");
	//fResolutionHistoMergedY->Draw();
	//fResolutionHistoMergedY->Write();*/
  
  

    }
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
Int_t CbmMvdFindHits::GetAdcCharge(Float_t charge)
{

    Int_t adcCharge;

    if(charge<fAdcOffset){return 0;};

    adcCharge = int( (charge-fAdcOffset)/fAdcStepSize );
    if ( adcCharge>fAdcSteps-1 ) {adcCharge = fAdcSteps-1;}

    return adcCharge;

}
//--------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmMvdFindHits::Reset() {
    fHits->Clear("C");
    fClusters->Clear("C");
    fMatches->Clear("C");
}

// -------------------------------------------------------------------------



ClassImp(CbmMvdFindHits);

