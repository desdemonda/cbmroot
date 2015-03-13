// -------------------------------------------------------------------------
// -----                  CbmMvdSensorClusterfinderTask source file    -----
// -----                  Created 03.12.2014 by P. Sitzmann            -----
// -------------------------------------------------------------------------

#include "CbmMvdSensorClusterfinderTask.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "FairLogger.h"



// -----   Default constructor   -------------------------------------------
CbmMvdSensorClusterfinderTask::CbmMvdSensorClusterfinderTask() 
  : CbmMvdSensorTask(),
    fAdcDynamic(200),
    fAdcOffset(0),
    fAdcBits(1),
    fAdcSteps(-1),
    fAdcStepSize(-1.),
    fDigis(NULL),
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
    fVerbose(0),
    fSigmaNoise(15.),
    fSeedThreshold(1.),
    fNeighThreshold(1.),
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
    fAddNoise(kFALSE),
    inputSet(kFALSE),
    ftempPixelMap()
{
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMvdSensorClusterfinderTask::~CbmMvdSensorClusterfinderTask() 
{
;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
CbmMvdSensorClusterfinderTask::CbmMvdSensorClusterfinderTask(const char* name, Int_t iMode,
			       Int_t iVerbose)
  : CbmMvdSensorTask(),
    fAdcDynamic(200),
    fAdcOffset(0),
    fAdcBits(1),
    fAdcSteps(-1),
    fAdcStepSize(-1.),
    fDigis(NULL),
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
    fVerbose(iVerbose),
    fSigmaNoise(15.),
    fSeedThreshold(1.),
    fNeighThreshold(1.),
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
    fAddNoise(kFALSE),
    inputSet(kFALSE),
    ftempPixelMap()
{   
}
// -------------------------------------------------------------------------




// -----    Virtual private method Init   ----------------------------------
void CbmMvdSensorClusterfinderTask::Init(CbmMvdSensor* mysensor) {


  fSensor = mysensor;
 //cout << "-Start- " << GetName() << ": Initialisation of sensor " << fSensor->GetName() << endl;
   fInputBuffer = new TClonesArray("CbmMvdDigi",10000); 
   fOutputBuffer= new TClonesArray("CbmMvdCluster", 10000);
   
 
    //Add charge collection histograms
    fPixelChargeHistos=new TObjArray();

     fTotalChargeInNpixelsArray = new TObjArray();
 Int_t adcMax = fAdcOffset + fAdcDynamic;
    fAdcSteps= (Int_t)TMath::Power(2,fAdcBits);
    fAdcStepSize  = fAdcDynamic/fAdcSteps;


initialized = kTRUE;

if(fShowDebugHistos)
{
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
	histo=new TH1F(histoName,histoName,200,0,200);
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
}
   //cout << "-Finished- " << GetName() << ": Initialisation of sensor " << fSensor->GetName() << endl;

}
// -------------------------------------------------------------------------

// -----   Virtual public method Reinit   ----------------------------------
Bool_t CbmMvdSensorClusterfinderTask::ReInit() {
    cout << "-I- " <<"CbmMvdSensorClusterfinderTask::ReInt---------------"<<endl;
    return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Virtual public method ExecChain   --------------
void CbmMvdSensorClusterfinderTask::ExecChain() {
  
  Exec();

 
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------
void CbmMvdSensorClusterfinderTask::Exec() {   
if(fInputBuffer->GetEntriesFast() > 0)
  {
fOutputBuffer->Delete();
inputSet = kFALSE;
vector<Int_t>* clusterArray=new vector<Int_t>;
        
CbmMvdDigi* digi;
    
Int_t iDigi=0;
digi = (CbmMvdDigi*) fInputBuffer->At(iDigi);
Int_t number = 0;
    
    if(!digi){
	cout << "-E- : CbmMvdSensorFindHitTask - Fatal: No Digits found in this event."<< endl;
    }

Int_t nDigis = fInputBuffer->GetEntriesFast();
Double_t pixelSizeX = digi->GetPixelSizeX();
Double_t pixelSizeY = digi->GetPixelSizeY();
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
	    //cout << endl << "registerde pixel x:" << digi->GetPixelX() << " y:" << digi->GetPixelY() << endl;
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
			cout << "-I- " << " CbmMvdSensorClusterfinderTask: Calling method CheckForNeighbours()..." << endl;
		    }

		    CheckForNeighbours(clusterArray, iCluster, pixelUsed);
		     //cout << endl << "checked for neighbours, create cluster" << endl;   

		}
		
			Int_t i=0;
			Int_t pixelCharge;
			Int_t pixelX;
			Int_t pixelY;
			pair<Int_t, Int_t> pixelCoords;
    			Int_t clusterSize=clusterArray->size();
			Int_t nClusters = fOutputBuffer->GetEntriesFast();
			//cout << endl << "new cluster: " << nClusters << endl;
			CbmMvdCluster* clusterNew=new ((*fOutputBuffer)[nClusters]) CbmMvdCluster();
	 		clusterNew->SetDetectorId(fSensor->GetDetectorID());
			clusterNew->SetStationNr(fSensor->GetStationNr());
   			for(i=0;i<clusterSize;i++) 
			{
	      		CbmMvdDigi* digiInCluster = (CbmMvdDigi*) fInputBuffer->At(clusterArray->at(i));
			clusterNew->AddDigi(digiInCluster->GetRefId());
			pixelCoords = std::make_pair(digiInCluster->GetPixelX(),digiInCluster->GetPixelY());
			pixelCharge = digiInCluster->GetCharge();
			ftempPixelMap[pixelCoords] = pixelCharge;
			}
			clusterNew->SetPixelMap(ftempPixelMap);
			ftempPixelMap.clear();

	  		if(fShowDebugHistos) UpdateDebugHistos(clusterNew);	
    				
	    }// if AdcCharge>threshold
		else {//cout << endl << "pixel is with " <<  digi->GetCharge() << " under Threshold or used" << endl;
			}
	}// loop on digis


delete pixelUsed;
clusterArray->clear();
delete clusterArray;
fInputBuffer->Delete();

fDigiMap.clear();
}
else {//cout << endl << "No input found." << endl;
     }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMvdSensorClusterfinderTask::CheckForNeighbours(vector<Int_t>* clusterArray, Int_t clusterDigi, TArrayS* pixelUsed)
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
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Int_t CbmMvdSensorClusterfinderTask::GetAdcCharge(Float_t charge)
{

    Int_t adcCharge;

    if(charge<fAdcOffset){return 0;};

    adcCharge = int( (charge-fAdcOffset)/fAdcStepSize );
    if ( adcCharge>fAdcSteps-1 ) {adcCharge = fAdcSteps-1;}

    return adcCharge;

}
// -------------------------------------------------------------------------

void CbmMvdSensorClusterfinderTask::UpdateDebugHistos(CbmMvdCluster* cluster){ 
    /************************************************************
    Algorithm for cluster shapes

     ************************************************************/
    Float_t chargeArray3D[fChargeArraySize][fChargeArraySize];
    Float_t chargeArray[fChargeArraySize*fChargeArraySize];
    Short_t seedPixelOffset=fChargeArraySize/2; // 3 for 7, 2 for 5
    Int_t   seedIndexX, seedIndexY;
    Float_t seedCharge =0.;
    Float_t clusterCharge = cluster->GetClusterCharge();
    std::map < std::pair<Int_t, Int_t>,Int_t> clusterMap = cluster->GetPixelMap();
    
    for(Int_t k=0; k<fChargeArraySize;k++){
      for(Int_t j=0; j<fChargeArraySize; j++){
	chargeArray3D[k][j]=gRandom->Gaus(0,fSigmaNoise);
      }
    }
    for(std::map < std::pair<Int_t, Int_t>,Int_t>::iterator iter = clusterMap.begin(); iter != clusterMap.end(); iter++)
	{
	if(iter->second > seedCharge)
		{
		seedCharge = iter->second;
		seedIndexX = iter->first.first;
		seedIndexY = iter->first.second;
		}
	}
 //cout << endl << "seed pixel with " << seedCharge << " charge" << endl;
     for(std::map < std::pair<Int_t, Int_t>,Int_t>::iterator iter = clusterMap.begin(); iter != clusterMap.end(); iter++){
      
      Int_t relativeX=iter->first.first+seedPixelOffset-seedIndexX;
      Int_t relativeY=iter->first.second+seedPixelOffset-seedIndexY;
      
      if(fVerbose > 1)cout << relativeX << " " << relativeY << " " <<iter->first.first<< " " << seedIndexX << endl;
      
      
      if (relativeX>=0 && relativeX<fChargeArraySize && relativeY>=0 && relativeY<fChargeArraySize){
	chargeArray3D[relativeX][relativeY]=iter->second;
      }
      
      if((relativeX-seedPixelOffset==0) && (relativeY-seedPixelOffset==0)) {//seed digiArray
	
      }

    
    }
    
    if(fVerbose > 1)
	{
    for(Int_t i=0;i<fChargeArraySize;i++)
    {for (Int_t j=0;j<fChargeArraySize;j++) {cout << chargeArray3D[i][j] << " " ;}
     cout << endl;
    } 
    

    }
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
    } 
}

//--------------------------------------------------------------------------
void CbmMvdSensorClusterfinderTask::Finish() {
   
if(fShowDebugHistos)
	{  
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
      	TCanvas* canvas2=new TCanvas("HitFinderCharge","HitFinderCharge");
      	canvas2->Divide(2,2);
      	canvas2->cd(1);
     	if(fChargeArraySize<=7)
		{
		clusterShapeHistogram= new TH2F("MvdClusterShape", "MvdClusterShape", fChargeArraySize, 0, fChargeArraySize,fChargeArraySize, 0, fChargeArraySize);
		for (Int_t i=0;i<fChargeArraySize*fChargeArraySize; i++) 
			{		
	  		histo= (TH1F*)fPixelChargeHistos->At(i);
	 		Float_t charge= histo->GetMean();
	  		//cout <<i << " Charge " << charge << " xCluster: " << i%fChargeArraySize << " yCluster: " << i/fChargeArraySize << endl;
	  		//histo->Fit("landau");
	  		//TF1* fitFunction= histo->GetFunction("landau");
 	  		//Double_t MPV=fitFunction->GetParameter(1);
	  		clusterShapeHistogram->Fill(i%fChargeArraySize,i/fChargeArraySize,charge);
			}
      		}
     	clusterShapeHistogram->Draw("Lego2");
     	canvas2->cd(2);
      	histo= (TH1F*)fPixelChargeHistos->At(50);
      	histo->Draw();
      	canvas2->cd(3);      
      	histo= (TH1F*)fPixelChargeHistos->At(51);
      	histo->Draw();
	canvas2->cd(4);
	//fFullClusterHisto->Draw(); 
	}

}
//--------------------------------------------------------------------------
ClassImp(CbmMvdSensorClusterfinderTask)
