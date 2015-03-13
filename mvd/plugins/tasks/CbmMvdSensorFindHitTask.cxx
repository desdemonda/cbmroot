// ---------------------------------------------------------------------------------------------
// -----                    CbmMvdSensorFindHitTask source file                            -----
// -----                      Created 11/09/13  by P.Sitzmann                              -----
// -----      				 						   -----
// ---------------------------------------------------------------------------------------------
// Includes from MVD
#include "CbmMvdSensorFindHitTask.h"

#include "CbmMvdGeoPar.h"
#include "CbmMvdHit.h"
#include "CbmMvdCluster.h"
#include "CbmMvdPileupManager.h"
#include "CbmMvdPoint.h"

// Includes from base
#include "FairGeoNode.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmMCTrack.h"
#include "FairLogger.h"

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
CbmMvdSensorFindHitTask::CbmMvdSensorFindHitTask()
  : CbmMvdSensorTask(),
    fAdcDynamic(200),
    fAdcOffset(0),
    fAdcBits(1),
    fAdcSteps(-1),
    fAdcStepSize(-1.),
    fDigis(NULL),
    fHits(NULL),
    fClusters(new TClonesArray("CbmMvdCluster",10000)),
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
  fGausArrayLimit(5000),  
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
  fBranchName("MvdHit"),
  fDigisInCluster(-1),
  fAddNoise(kFALSE),
  inputSet(kFALSE)
{
    CbmMvdCluster* clusterTemp= new CbmMvdCluster;
    fDigisInCluster= clusterTemp->GetMaxDigisInThisObject(); // read the number of memory cells from the cluster object
    delete clusterTemp;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdSensorFindHitTask::CbmMvdSensorFindHitTask(const char* name, Int_t iMode,
			       Int_t iVerbose)
  : CbmMvdSensorTask(),
    fAdcDynamic(200),
    fAdcOffset(0),
    fAdcBits(1),
    fAdcSteps(-1),
    fAdcStepSize(-1.),
    fDigis(NULL),
    fHits(NULL),
    fClusters(new TClonesArray("CbmMvdCluster",10000)),
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
  fGausArrayLimit(5000),  
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
  fBranchName("MvdHit"),
  fDigisInCluster(-1),
  fAddNoise(kFALSE),
  inputSet(kFALSE)
{    
    CbmMvdCluster* clusterTemp= new CbmMvdCluster;
    fDigisInCluster= clusterTemp->GetMaxDigisInThisObject(); // read the number of memory cells from the cluster object
    delete clusterTemp;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMvdSensorFindHitTask::~CbmMvdSensorFindHitTask() {

    if ( fHits ) {
	fHits->Delete();
	delete fHits;
    }

    if ( fClusters ) {
	fClusters->Delete();
	delete fClusters;
    }

    if ( fInputBuffer ) {
	fInputBuffer->Delete();
	delete fInputBuffer;
    }
 

}
// -------------------------------------------------------------------------

// -----    Virtual private method Init   ----------------------------------
void CbmMvdSensorFindHitTask::Init(CbmMvdSensor* mysensor) {


  fSensor = mysensor;
 //cout << "-Start- " << GetName() << ": Initialisation of sensor " << fSensor->GetName() << endl;
   fInputBuffer = new TClonesArray("CbmMvdDigi",10000); 
   fOutputBuffer= new TClonesArray("CbmMvdHit", 10000);
   fHits = new TClonesArray("CbmMvdHit", 10000);
   
 
    //Add charge collection histograms
    fPixelChargeHistos=new TObjArray();

     fTotalChargeInNpixelsArray = new TObjArray();
 Int_t adcMax = fAdcOffset + fAdcDynamic;
    fAdcSteps= (Int_t)TMath::Power(2,fAdcBits);
    fAdcStepSize  = fAdcDynamic/fAdcSteps;

    fGausArray=new Float_t[fGausArrayLimit];
    for(Int_t i=0;i<fGausArrayLimit;i++){fGausArray[i]=gRandom->Gaus(0, fSigmaNoise);};
    fGausArrayIt=0;


initialized = kTRUE;

   //cout << "-Finished- " << GetName() << ": Initialisation of sensor " << fSensor->GetName() << endl;
}
// -------------------------------------------------------------------------

// -----   Virtual public method Reinit   ----------------------------------
InitStatus CbmMvdSensorFindHitTask::ReInit() {
    cout << "-I- " <<"CbmMvdSensorFindHitTask::ReInt---------------"<<endl;
    return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Virtual public method ExecChain   --------------
void CbmMvdSensorFindHitTask::ExecChain() {
  
  Exec();

 
}
// -------------------------------------------------------------------------

// -----   Virtual public method Exec   --------------
void CbmMvdSensorFindHitTask::Exec() {

// if(!inputSet)
//  {
//  fInputBuffer->Clear(); 
//  fInputBuffer->AbsorbObjects(fPreviousPlugin->GetOutputArray());
//  cout << endl << "absorbt object from previous plugin at " << fSensor->GetName() << " got " << fInputBuffer->GetEntriesFast() << " entrys" << endl;
//  }
 if(fInputBuffer->GetEntriesFast() > 0)
  {
fHits->Clear("C");
fOutputBuffer->Clear();
fClusters->Clear("C");
inputSet = kFALSE;
vector<Int_t>* clusterArray=new vector<Int_t>;
        
CbmMvdDigi* digi = NULL;
    
Int_t iDigi=0;
digi = (CbmMvdDigi*) fInputBuffer->At(iDigi);

    
    if(!digi){
	cout << "-E- : CbmMvdSensorFindHitTask - Fatal: No Digits found in this event."<< endl;
    }

Int_t nDigis = fInputBuffer->GetEntriesFast();

//cout << endl << "working with " << nDigis << " entries" << endl;    


    if( fAddNoise == kTRUE ){
      // Generate random number and call it noise
      // add the noise to the charge of the digis

	cout << "-I- " << "CbmMvdSensorFindHitTask: Calling method AddNoiseToDigis()...\n" << endl;

	for ( iDigi=0; iDigi<nDigis; iDigi++ ) {

	    digi  = (CbmMvdDigi*) fInputBuffer->At(iDigi);
	    AddNoiseToDigis(digi);
	}
    }

    Double_t pixelSizeX = digi->GetPixelSizeX();
    Double_t pixelSizeY = digi->GetPixelSizeY();
 
    
    if( fMode == 1 )
      {
       GenerateFakeDigis(pixelSizeX, pixelSizeY); // -------- Create Fake Digis -
	cout << endl << "generate fake digis" << endl;
      }

 

    nDigis = fInputBuffer->GetEntriesFast();
    TArrayS* pixelUsed  = new TArrayS(nDigis);

    for ( iDigi=0; iDigi<nDigis; iDigi++) {
	pixelUsed->AddAt(0,iDigi);
    }

    fDigiMap.clear();
Int_t refId;
	for(Int_t k=0;k<nDigis;k++){

	    digi = (CbmMvdDigi*) fInputBuffer->At(k);
	    refId = digi->GetRefId();
	    if ( refId < 0)
		{
		LOG(FATAL) << "RefID of this digi is -1 this should not happend "<< FairLogger::endl;
		}
	    //apply fNeighThreshold
	   
	    if(GetAdcCharge(digi->GetCharge()) < fNeighThreshold ) continue;

	    pair<Int_t, Int_t> a (digi->GetPixelX(),digi->GetPixelY());
	   // cout << endl << "registerde pixel x:" << digi->GetPixelX() << " y:" << digi->GetPixelY() << endl;
	    fDigiMap[a]=k;
	};


	if( gDebug>0 ){cout << "\n-I- " << GetName() << ": VolumeId " << fSensor->GetVolumeId() << endl;}

	for ( iDigi=0; iDigi<nDigis; iDigi++) {

	    if( gDebug>0 && iDigi%10000==0 ){ cout << "-I- " << GetName() << " Digi:" << iDigi << endl; };

	    digi = (CbmMvdDigi*) fInputBuffer->At(iDigi);
            //cout << endl << "working with pixel x:" << digi->GetPixelX() << " y:" << digi->GetPixelY() << endl;
	 

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
		cout << "-I- " << "CbmMvdSensorFindHitTask: Checking for seed pixels..." << endl;
	    }

	    if( ( GetAdcCharge(digi->GetCharge())>=fSeedThreshold ) && ( pixelUsed->At(iDigi)==kFALSE ) ){
		clusterArray->clear();
		clusterArray->push_back(iDigi);
		

		pixelUsed->AddAt(1,iDigi);

		pair<Int_t, Int_t> a(digi->GetPixelX(), digi->GetPixelY());
    		fDigiMapIt = fDigiMap.find(a);
    		fDigiMap.erase(fDigiMapIt);

		for ( Int_t iCluster=0; iCluster<clusterArray->size(); iCluster++ ){

		    if( gDebug>0 ){
			cout << "-I- " << " CbmMvdSensorFindHitTask: Calling method CheckForNeighbours()..." << endl;
		    }

		    CheckForNeighbours(clusterArray, iCluster, pixelUsed);

		}

		//Calculate the center of gravity of all pixels in the cluster.
		TVector3 pos(0,0,0);
                TVector3 dpos(0,0,0);

		if( gDebug>0 ){ cout << "-I- " << " CbmMvdSensorFindHitTask: Calling method CreateHit()..." << endl; }
		
		CreateHit(clusterArray, pos, dpos); 
		
		
		
		
	    }// if AdcCharge>threshold
		else {//cout << endl << "pixel is with " <<  digi->GetCharge() << " under Threshold or used" << endl;
			}
	}// loop on digis


    //----------------------------------------------------------------------------------
     //------------- End of Detector Loops ----------------------------------------------
    //----------------------------------------------------------------------------------

   // cout << endl << "-I-  End of task " << GetName() << ": Event Nr: " << fNEvent << ", nDIGIS: "<<nDigis << ", nHits:"<<fHits->GetEntriesFast()<<endl;

 
/*cout <<  "registered " << fHits->GetEntriesFast() 
	     << " new hits out of " << fInputBuffer->GetEntriesFast() 
	     << " Digis on sensor " 
	     << fSensor->GetName() << endl;  */

delete pixelUsed;
clusterArray->clear();
delete clusterArray;
fInputBuffer->Clear();

fDigiMap.clear();
}
else {//cout << endl << "No input found." << endl;
     }
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
void CbmMvdSensorFindHitTask::AddNoiseToDigis(CbmMvdDigi* digi){
    Double_t noise  = fGausArray[fGausArrayIt++]; // noise is simulated by a gauss
    if (fGausArrayIt-2>fGausArrayLimit){fGausArrayIt=0;};
    Double_t charge = digi->GetCharge() + noise;
    digi->SetCharge((int)charge);
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
void CbmMvdSensorFindHitTask::GenerateFakeDigis( Double_t pixelSizeX, Double_t pixelSizeY){

    //max index of pixels
    //Int_t nx = TMath::Nint(2*fLayerRadius/pixelSizeX);
    //Int_t ny = TMath::Nint(2*fLayerRadius/pixelSizeY);

    //cdritsa: parametrise geometry: 15/12/08
/*     Double_t layerRadius = station->GetRmax();
//     Double_t layerRadiusInner = station->GetRmin();

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

	    if ( noise>fSeedThreshold && //pixel is not used ???){
                 Int_t nDigis = fInputBuffer->GetEntriesFast();
		 CbmMvdDigi* fakeDigi= 
		       new ((*fInputBuffer)[nDigis]) CbmMvdDigi(station->GetVolumeId(), i,j, noise, pixelSizeX,pixelSizeY);
		 
		 Int_t data[5]; 
		 Float_t data2[5];
		 
		
		       
		       
		}
	}
    }
*/


}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void CbmMvdSensorFindHitTask::CheckForNeighbours(vector<Int_t>* clusterArray, Int_t clusterDigi, TArrayS* pixelUsed)
{
    CbmMvdDigi* seed = (CbmMvdDigi*)fInputBuffer->At(clusterArray->at(clusterDigi));
    //cout << endl << "pixel nr. " << clusterDigi << " is seed" << endl ;
   
    CbmMvdDigi* digiOfInterest;
   
    	
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
	  //cout << endl << "pixel nr. " << i << " is used" << endl ;	  	  
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
	//cout << endl << "pixel nr. " << i << " is used" << endl ;
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
       //cout << endl << "pixel nr. " << i << " is used" << endl ;
	clusterArray->push_back(i);
	pixelUsed->AddAt(1,i); // block pixel for the seed pixel scanner
	fDigiMap.erase(fDigiMapIt); // block pixel for the neighbour pixel scanner
    }

    a = std::make_pair (channelX, channelY+1);
    fDigiMapIt=fDigiMap.find(a);

    if (!(fDigiMapIt == fDigiMap.end()))
    {
	Int_t i=fDigiMap[a];
        //cout << endl << "pixel nr. " << i << " is used" << endl ;
	// Only digis depassing fNeighThreshold are in the map, no cut required
	clusterArray->push_back(i);
	pixelUsed->AddAt(1,i); // block pixel for the seed pixel scanner
	fDigiMap.erase(fDigiMapIt); // block pixel for the neighbour pixel scanner
    }




}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void CbmMvdSensorFindHitTask::CreateHit(vector<Int_t>* clusterArray,  TVector3 &pos, TVector3 &dpos)
{

    //loop on cluster array elements
    //calculate the CoG for this cluster

    Int_t    clusterSize = clusterArray->size();
    Int_t    digiIndex = 0;
    //cout << endl << "try to create hit from " << clusterSize << " pixels" << endl;
    CbmMvdDigi* pixelInCluster = (CbmMvdDigi*) fInputBuffer->At(clusterArray->at(digiIndex));
    Int_t    thisRefID = pixelInCluster->GetRefId();
    while(thisRefID < 0 && digiIndex < clusterSize)
	{
	pixelInCluster = (CbmMvdDigi*) fInputBuffer->At(clusterArray->at(digiIndex));
	thisRefID = pixelInCluster->GetRefId();
	digiIndex++;
        }
    if(thisRefID < 0)
	LOG(FATAL) << "RefID of this digi is -1 this should not happend checked "<< digiIndex << " digis in this Cluster" << FairLogger::endl;
    Int_t detId = pixelInCluster->GetDetectorId();

    // Calculate the center of gravity of the charge of a cluster
    
    ComputeCenterOfGravity(clusterArray, pos, dpos);
    
   
    Int_t indexX, indexY;
    //Double_t x,y;
    Double_t local[2];
    local[0]=pos.X();
    local[1]=pos.Y();
    //cout << endl << "found center of gravity at: " << local[0] << " , " << local[1] << endl;
    
    fSensor->TopToPixel(local, indexX, indexY);
    
    //cout << endl << "Center is on pixel: " << indexX << " , " << indexY << endl;
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
    
    
    // Save hit into array
    Int_t nHits = fHits->GetEntriesFast();
    //cout << endl << "adding new hit to fHits at X: " << pos.X() << " , Y: "<< pos.Y() << " , Z: " << pos.Z() << " , " ;
    new ((*fHits)[nHits]) CbmMvdHit(fSensor->GetStationNr(), pos, dpos, indexX, indexY, nClusters, 0);
    CbmMvdHit* currentHit = new CbmMvdHit;
    currentHit = (CbmMvdHit*) fHits->At(nHits);
    currentHit->SetTimeStamp(fSensor->GetCurrentEventTime());
    currentHit->SetTimeStampError(fSensor->GetIntegrationtime()/2);
    currentHit->SetRefId(thisRefID);
    if (pixelInCluster->GetRefId() < 0)
    cout << endl << "new hit with refID " << pixelInCluster->GetRefId() << " to hit " << nHits << endl;
    
    nHits = fOutputBuffer->GetEntriesFast();
    new((*fOutputBuffer)[nHits]) CbmMvdHit(fSensor->GetStationNr(), pos, dpos, indexX, indexY, nClusters, 0);
    currentHit = (CbmMvdHit*) fOutputBuffer->At(nHits);
    currentHit->SetTimeStamp(fSensor->GetCurrentEventTime());
    currentHit->SetTimeStampError(fSensor->GetIntegrationtime()/2);
    currentHit->SetRefId(pixelInCluster->GetRefId());

  delete digiArray;
}
      
 //--------------------------------------------------------------------------     
    
void CbmMvdSensorFindHitTask::UpdateDebugHistos(vector<Int_t>* clusterArray, Int_t seedIndexX, Int_t seedIndexY){ 
    /************************************************************
    Algorithm for cluster shapes

     ************************************************************/
    
    
    Float_t chargeArray3D[fChargeArraySize][fChargeArraySize];
    Float_t chargeArray[fChargeArraySize*fChargeArraySize];
    Short_t seedPixelOffset=fChargeArraySize/2; // 3 for 7, 2 for 5
    Float_t xCentralTrack, yCentralTrack;
    Float_t clusterCharge=0;
    
    Int_t clusterSize=clusterArray->size();
    
    for(Int_t k=0; k<fChargeArraySize;k++){
      for(Int_t j=0; j<fChargeArraySize; j++){
	chargeArray3D[k][j]=gRandom->Gaus(0,fSigmaNoise);
      }
    }
    
    for(Int_t k=0;k<clusterSize;k++){
      CbmMvdDigi* digi=(CbmMvdDigi*)fInputBuffer->At(clusterArray->at(k));
      
      clusterCharge=clusterCharge+digi->GetCharge();
      
      Int_t relativeX=digi->GetPixelX()+seedPixelOffset-seedIndexX;
      Int_t relativeY=digi->GetPixelY()+seedPixelOffset-seedIndexY;
      
      //for debugging
      //cout << relativeX << " " << relativeY << " " <<digi->GetPixelX()<< " " << seedIndexX << endl;
      
      
      if (relativeX>=0 && relativeX<fChargeArraySize && relativeY>=0 && relativeY<fChargeArraySize){
	chargeArray3D[relativeX][relativeY]=digi->GetCharge();
      }
      
      if((relativeX-seedPixelOffset==0) && (relativeY-seedPixelOffset==0)) {//seed digiArray
	
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

void CbmMvdSensorFindHitTask::ComputeCenterOfGravity(vector<Int_t>* clusterArray, TVector3& pos, 
					    TVector3& dpos){
  Double_t numeratorX  = 0;
  Double_t numeratorY  = 0;
  Double_t denominator = 0;
  Double_t pixelSizeX  = 0;
  Double_t pixelSizeY  = 0;
  Int_t charge;
  Int_t xIndex;
  Int_t yIndex;
  Double_t x,y;
  Double_t layerPosZ=fSensor->GetZ();
  CbmMvdDigi* pixelInCluster;
  Double_t lab[3]={0,0,0};
  
  Int_t clusterSize=clusterArray->size();
   
  for( Int_t iCluster = 0; iCluster<clusterSize; iCluster++ ){
         
    	pixelInCluster = (CbmMvdDigi*) fInputBuffer->At(clusterArray->at(iCluster));
	

	charge      = GetAdcCharge(pixelInCluster->GetCharge());
	xIndex      = pixelInCluster->GetPixelX();
	yIndex      = pixelInCluster->GetPixelY();
	pixelSizeX  = pixelInCluster->GetPixelSizeX();
	pixelSizeY  = pixelInCluster->GetPixelSizeY();

	if(gDebug>0){
	    cout << "-I- " << "CbmMvdSensorFindHitTask:: iCluster= "<<iCluster << " , clusterSize= " << clusterSize << endl;
	    cout << "-I- " << "CbmMvdSensorFindHitTask::xIndex " << xIndex << " , yIndex " << yIndex << " , charge = " << pixelInCluster->GetAdcCharge(fAdcDynamic, fAdcOffset, fAdcBits) << endl;
	}
	
	fSensor->PixelToTop(xIndex, yIndex, lab);
	
	x = lab[0];
	y = lab[1];
	
        //cout << endl << "x = " << x << " y = " << y << endl;
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
	cout << "-I- " << "CbmMvdSensorFindHitTask::=========================\n " << endl;
	cout << "-I- " << "CbmMvdSensorFindHitTask::numeratorX: " <<numeratorX<<" , numeratorY: " <<numeratorY << ", denominator: " << denominator << endl;
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
	cout << "-I- " << "CbmMvdSensorFindHitTask::-----------------------------------" << endl;
	cout << "-I- " << "CbmMvdSensorFindHitTask::X hit= " << fHitPosX << " Y hit= "<<fHitPosY << " Z hit= "<<fHitPosZ <<endl;
	
	cout << "-I- " << "CbmMvdSensorFindHitTask::-----------------------------------\n" << endl;
    }

    // pos = center of gravity (labframe), dpos uncertainty
    pos.SetXYZ(fHitPosX,fHitPosY,fHitPosZ);
    dpos.SetXYZ(fHitPosErrX, fHitPosErrY, fHitPosErrZ);
    
   

}

//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
void CbmMvdSensorFindHitTask::Finish() {
if(fShowDebugHistos){
    cout << "\n============================================================" << endl;
    cout << "-I- " << GetName() << "::Finish: Total events skipped: " << fCounter << endl;
    cout << "============================================================" << endl;
    cout << "-I- Parameters used" << endl;
    cout << "Gaussian noise [electrons]	: " << fSigmaNoise << endl;
    cout << "Noise simulated [Bool]	: " << fAddNoise << endl;
    cout << "Threshold seed [ADC]       : " << fSeedThreshold << endl;
    cout << "Threshold neighbours [ADC]	: " << fNeighThreshold << endl;
    cout << "ADC - Bits			: " << fAdcBits << endl;
    cout << "ADC - Dynamic [electrons]	: " << fAdcDynamic << endl;
    cout << "ADC - Offset [electrons]	: " << fAdcOffset << endl;
    cout << "============================================================" << endl;
    
    
    TH1F* histo;
    TH2F* clusterShapeHistogram;
    
    
    
      
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

Int_t CbmMvdSensorFindHitTask::GetAdcCharge(Float_t charge)
{

    Int_t adcCharge;

    if(charge<fAdcOffset){return 0;};

    adcCharge = int( (charge-fAdcOffset)/fAdcStepSize );
    if ( adcCharge>fAdcSteps-1 ) {adcCharge = fAdcSteps-1;}

    return adcCharge;

}


// -----   Private method Reset   ------------------------------------------
void CbmMvdSensorFindHitTask::Reset() {
    fHits->Clear("C");
    fClusters->Clear("C");
}

// -------------------------------------------------------------------------



ClassImp(CbmMvdSensorFindHitTask);

