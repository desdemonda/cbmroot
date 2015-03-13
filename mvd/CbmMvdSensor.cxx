// -------------------------------------------------------------------------
// -----                     CbmMvdSensor source file                  -----
// -----                  Created 31/01/11  by M. Deveaux              -----
// -------------------------------------------------------------------------

#include "CbmMvdSensor.h"
//#include "CbmMvdMimosa26AHR.h"
#include "TGeoManager.h"

//---Plugins


#include "plugins/buffers/CbmMvdSensorFrameBuffer.h"
#include "plugins/buffers/CbmMvdSensorTrackingBuffer.h"
#include "plugins/tasks/CbmMvdSensorDigitizerTask.h"
#include "plugins/tasks/CbmMvdSensorClusterfinderTask.h"
#include "plugins/tasks/CbmMvdSensorFindHitTask.h"
#include "plugins/tasks/CbmMvdSensorHitfinderTask.h"
//---Plugins


#include <iostream>
#include "FairLogger.h"

using std::cout;
using std::endl;



// -----   Default constructor   -------------------------------------------
CbmMvdSensor::CbmMvdSensor() 
  : TNamed(),
    fStationNr(0),
    fSensorNr(0),
    fVolumeId(0),
    fDetectorID(-1),
    fDigiPlugin(-1),
    fHitPlugin(-1),
    fClusterPlugin(-1),
    fVolName(""),
    fNodeName(""),
    foutputDigis(NULL),
    foutputDigiMatch(NULL),
    foutputCluster(NULL),
    foutputBuffer(NULL),
    fcurrentPoints(NULL),
    fcurrentEventTime(0.),
    fShape(NULL),
    fMCMatrix(NULL),
    fRecoMatrix(NULL),
    fAlignmentCorr(NULL),
    fTempCoordinate(),
    fSensorPosition(),
    fSensorData(new CbmMvdSensorDataSheet()),
    fSensorMap(),
    fPluginArray(new TObjArray(1)),
    fSensorStartTime(0.),
    initialized(kFALSE),
    epsilon()
{
cout << "-W- " << GetName() << ": MVD-Sensor initialized without technical data.";
cout << " Assuming default sensor." << endl;

cout << "-W- " << GetName() << ": MVD-Sensor initialized without geometry data. ";
cout << " Must be added manually before using this class." << endl;

cout << "-W- " << GetName() << ": MVD-Sensor initialized without valid start time. ";
cout << " Must be added manually before using this class." << endl;

}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdSensor::CbmMvdSensor(const char* name, CbmMvdSensorDataSheet* dataSheet, TString volName, TString nodeName, Int_t stationNr, Int_t volumeId, Double_t sensorStartTime)
  : TNamed(name, ""), 
    fStationNr(),
    fSensorNr(stationNr),
    fVolumeId(volumeId),
    fDetectorID(DetectorId(stationNr)),
    fDigiPlugin(-1),
    fHitPlugin(-1),
    fClusterPlugin(-1),
    fVolName(volName),
    fNodeName(nodeName),
    foutputDigis(NULL),
    foutputDigiMatch(NULL),
    foutputCluster(NULL),
    foutputBuffer(NULL),
    fcurrentPoints(NULL),
    fcurrentEventTime(0.),
    fShape(NULL),
    fMCMatrix(NULL),
    fRecoMatrix(NULL),
    fAlignmentCorr(NULL),
    fTempCoordinate(),
    fSensorPosition(),
    fSensorData(dataSheet),
    fSensorMap(),
    fPluginArray(new TObjArray(1)),
    fSensorStartTime(sensorStartTime),
    initialized(kFALSE),
    epsilon()
{

}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMvdSensor::~CbmMvdSensor() {
  delete fSensorData;
}
// -------Setters -----------------------------------------------------

void CbmMvdSensor::SetAlignment(TGeoHMatrix* alignmentMatrix) {
  
  if (fAlignmentCorr) {delete fAlignmentCorr;};
  fAlignmentCorr=(TGeoHMatrix*)alignmentMatrix->Clone(fVolName+"_AlignmentData");
  
  
}


// -------Initialization tools  -----------------------------------------------------
Int_t CbmMvdSensor::ReadSensorGeometry(TString volName, TString nodeName) {
   
  //cout << "-I- " << GetName() << " : Searching for station: " << volName << endl;
  //cout << "-I- nodeName is " << nodeName << endl;
  
  
  Int_t volId    = -1;
  TGeoVolume* volume;
   //cout << gGeoManager->GetNodeId()<<endl;
  volId = gGeoManager->GetUID(volName); //search for node in geometry data base
  Double_t* tempCoordinate; 

  if ( !(volId > -1) ) {
    cout << "-W- " << GetName() << ": Station " << volName << " not found, ignored for now." << endl;
  }
  else { // volume found

      // Get shape parameters
      volume   = gGeoManager    ->GetVolume(volName.Data());
      fShape = (TGeoBBox*) volume ->GetShape();
      
      //fDetectorID = volume->GetIndex(volume->GetNode(nodeName));
      //cout << fDetectorID << endl;
     // cout<< "This is the volume:" <<endl;
     // volume->Dump();
     // cout<< "This is the shape:" << endl;
     // fShape->Dump();
  }
  
  
  Bool_t nodeFound = gGeoManager->cd(nodeName.Data());

  if ( ! nodeFound ) {
        
        
	
	cout << "-W- " << GetName() << "::SetMvdGeometry: Node " << nodeName
	     << " not found in geometry!" << endl;
	cout << " Ignored for now." << endl;
	cout << endl;
      }    
  else { 
    if(fMCMatrix) {delete fMCMatrix;}; //delete local copy of the position information
 
    fMCMatrix=(TGeoHMatrix*)(gGeoManager->GetCurrentMatrix())->Clone(volName+"_MC_Matrix");
    //fMCMatrix->Print();
    
    fMCMatrix->SetName(volName+"_MC_Matrix");
   
    if (!fRecoMatrix) {
        Double_t pre[3], past[3];
	//The initial guess on the reconstructed position is that the MC-position is correct
	//Short cut to misalignment, add a small error
	fRecoMatrix=(TGeoHMatrix*)fMCMatrix->Clone(volName+"_Reco_Matrix");
        PixelToTop(0, 0, pre);
        tempCoordinate=fRecoMatrix->GetTranslation();
	for(Int_t i=0;i<3;i++){tempCoordinate[i]=tempCoordinate[i]+epsilon[i];}
	fRecoMatrix->SetTranslation(tempCoordinate);
        PixelToTop(0, 0, past);
	//cout << endl << "shifted pixel 0 from: " << pre[0] << ", " << pre[1] << " to " << past[0] << ", " << past[1] << endl; 
    }
     else
	{tempCoordinate=fRecoMatrix->GetTranslation();}
    if (!fAlignmentCorr){
	//If no knowledge on the reco matrix is available there is plausibly no correction data.
	fAlignmentCorr=new TGeoHMatrix(volName+"_AlignmentData");
    }
  }


 for(Int_t i=0;i<3;i++){fSensorPosition[i]=tempCoordinate[i];}
}

//-------------------------------------------------------------------

void CbmMvdSensor::Init(){
  

      
ReadSensorGeometry(fVolName, fNodeName);  
	

 if(!initialized)
{
  foutputDigis = new TClonesArray("CbmMvdDigi",1000);
  foutputDigiMatch = new TClonesArray("CbmMatch", 1000);
  foutputBuffer = new TClonesArray("CbmMvdHit", 1000);
  foutputCluster = new TClonesArray("CbmMvdCluster", 1000);
  //cout << endl << " init TClonesArrays" << endl;
 }
  
  Int_t nPlugin=fPluginArray->GetEntriesFast();
  CbmMvdSensorPlugin* pluginFirst;
  CbmMvdSensorPlugin* pluginNext;
  
  CbmMvdSensorFrameBuffer* framebuffer;
  CbmMvdSensorDigitizerTask* digitask;
  CbmMvdSensorClusterfinderTask* clustertask;
  CbmMvdSensorHitfinderTask* hitfindertask;

  CbmMvdSensorTrackingBuffer* trackingbuffer;
  CbmMvdSensorFindHitTask* findertask;
  
  TClonesArray* dataArray;
  
  for(Int_t i=0;i<nPlugin;i++)
     {
      pluginFirst=(CbmMvdSensorPlugin*)fPluginArray->At(i);
      if( pluginFirst->GetPluginType() == buffer)
          {
           const TString framename = "CbmMvdSensorFrameBuffer";
           const TString trackingname = "CbmMvdSensorTrackingBuffer";

	   
           if ( pluginFirst->ClassName() == framename)
	       {
		
                framebuffer = (CbmMvdSensorFrameBuffer*)fPluginArray->At(i);
		if(! framebuffer->IsInit())
               		framebuffer->Init(this);
               }
                if ( pluginFirst->ClassName() == trackingname)
	       {
		
                trackingbuffer = (CbmMvdSensorTrackingBuffer*)fPluginArray->At(i);
		if(! trackingbuffer->IsInit())
               		 trackingbuffer->Init(this);
               }
              
	  }
    if(pluginFirst->GetPluginType() == task)
      {
      const TString digitizername = "CbmMvdSensorDigitizerTask";
      const TString findername = "CbmMvdSensorFindHitTask";
      const TString clustername = "CbmMvdSensorClusterfinderTask";
      const TString hitname = "CbmMvdSensorHitfinderTask";

      if (pluginFirst->ClassName()  == digitizername)
	  {
	  digitask = (CbmMvdSensorDigitizerTask*)fPluginArray->At(i);
	  if(! digitask->IsInit())
        	  digitask->Init(this);
          fDigiPlugin = i;
	  } 
	 
      else if (pluginFirst->ClassName() == findername)
	       {
		findertask = (CbmMvdSensorFindHitTask*)fPluginArray->At(i);
		if(! findertask->IsInit())
               		 findertask->Init(this); 
		 fHitPlugin = i;
	       }
      else if (pluginFirst->ClassName() == clustername)
	       {
		clustertask = (CbmMvdSensorClusterfinderTask*)fPluginArray->At(i);
		if(! clustertask->IsInit())
               		 clustertask->Init(this); 
		 fClusterPlugin = i;
	       }
        else if (pluginFirst->ClassName() == hitname)
	       {
		hitfindertask = (CbmMvdSensorHitfinderTask*)fPluginArray->At(i);
		if(! hitfindertask->IsInit())
               		 hitfindertask->Init(this); 
		 fHitPlugin = i;
	       }
     }
     }
  //Link data chain
  if(nPlugin>1){
    
    
    
    for(Int_t i=0;i<nPlugin-1;i++){
      
      pluginFirst=(CbmMvdSensorPlugin*)fPluginArray->At(i);
      pluginNext =(CbmMvdSensorPlugin*)fPluginArray->At(i+1);
      
      //Rules: The output array of the previous plugin is input of the next
      dataArray=pluginFirst->GetOutputArray();
      pluginNext->SetInputArray(dataArray);
      
      //The plugin knows its precessor and sucessor to communicate
      pluginFirst->SetNextPlugin(pluginNext);
      pluginNext->SetPreviousPlugin(pluginFirst);
    }
  }
  
  
  if(nPlugin == 0)
      {
       cout << endl << "No Plugins on this Sensor " << endl;
       pluginFirst = NULL;
      }
initialized = kTRUE;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMvdSensor::ShowDebugHistos(){
if(fDetectorID == 1)
	{
	CbmMvdSensorPlugin* pluginLast;
	pluginLast=(CbmMvdSensorPlugin*)fPluginArray->At(fPluginArray->GetEntriesFast()-1);
	cout << endl << "Set debug mode on Plugin " << fPluginArray->GetEntriesFast()-1 << " at sensor " << GetName() << endl;
	pluginLast->ShowDebugHistos();
	}
}


// -------------------------------------------------------------------------
void CbmMvdSensor::SendInput(CbmMvdPoint* point){
  
      CbmMvdSensorPlugin* pluginFirst;
      
      CbmMvdSensorFrameBuffer* framebuffer;
      CbmMvdSensorDigitizerTask* digitask;

    pluginFirst=(CbmMvdSensorPlugin*)fPluginArray->At(0);
      if( pluginFirst->GetPluginType() == buffer)
          {
           TString framename = "CbmMvdSensorFrameBuffer";
           if ( pluginFirst->ClassName() == framename)
	       {
		
                framebuffer = (CbmMvdSensorFrameBuffer*)fPluginArray->At(0);
                framebuffer->SetInput(point);
               }
               
               else
		{
		LOG(FATAL) << "Invalid input typ" << FairLogger::endl;
		}
	  }
    else if(pluginFirst->GetPluginType() == task)
      {
      TString digitizername = "CbmMvdSensorDigitizerTask";
        
      if (pluginFirst->ClassName()  == digitizername)
	  {
	  digitask = (CbmMvdSensorDigitizerTask*)fPluginArray->At(0);
          digitask->SetInput(point);
	  } 
	
	   else 
	       {
		LOG(FATAL) << "Invalid input typ" << FairLogger::endl;
		 
	       }
	
     }
     else
     {
      cout << endl << "ERROR!! undefind plugin!" << endl; 
     }
      
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMvdSensor::SendInputDigi(CbmMvdDigi* digi)
{
if(fClusterPlugin != -1)
	{
	CbmMvdSensorClusterfinderTask* clustertask = (CbmMvdSensorClusterfinderTask*)fPluginArray->At(fClusterPlugin);
	clustertask->SetInputDigi(digi);
	}
else if(fHitPlugin != -1)
	{
	CbmMvdSensorFindHitTask* findertask = (CbmMvdSensorFindHitTask*)fPluginArray->At(fHitPlugin);
	findertask->SetInputDigi(digi);
	}
else
	{
	cout << endl << "Somthing seems fishy here" << endl;
	}
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void CbmMvdSensor::SendInputCluster(CbmMvdCluster* cluster)
{
if(fHitPlugin != -1)
	{
	CbmMvdSensorHitfinderTask* findertask = (CbmMvdSensorHitfinderTask*)fPluginArray->At(fHitPlugin);
       	findertask->SetInputCluster(cluster);
	}
else
	{
	cout << endl << "Somthing seems fishy here" << endl;
	}
}
// -------------------------------------------------------------------------

void CbmMvdSensor::ExecChain(){
      
  FairRunSim* run = FairRunSim::Instance();
  FairRunAna* ana = FairRunAna::Instance();
if(run)
	{
	FairEventHeader* event = run->GetEventHeader();
	fcurrentEventTime = event->GetEventTime();
	 }
if(ana)
	 {
	FairEventHeader* event = ana->GetEventHeader();
	fcurrentEventTime = event->GetEventTime();
	}
foutputDigis->Clear("C");
foutputDigiMatch->Clear("C");
foutputCluster->Clear("C");
foutputBuffer->Clear("C");


   CbmMvdSensorPlugin* plugin;
   Int_t nPlugin=fPluginArray->GetEntriesFast();
  
   plugin=(CbmMvdSensorPlugin*)fPluginArray->At(0);
   
    plugin->ExecChain();
  //cout << endl << "is plugin ready? "<< plugin->PluginReady() << " on sensor "<< this->GetName()  << endl;    
  if(plugin->PluginReady()) 
    {
    //cout << endl << "exec chain on sensor "<< this->GetName() << endl;
    for(Int_t i=1; i<nPlugin; i++)
      {
	//cout << endl << "exec plugin " << i << " on sensor "<< this->GetName() << endl;
	plugin=(CbmMvdSensorPlugin*)fPluginArray->At(i);
	plugin->ExecChain();
      }
    }
}
// -------------------------------------------------------------------------

void CbmMvdSensor::Exec(UInt_t nPlugin){
  foutputDigis->Clear("C");
foutputDigiMatch->Clear("C");
foutputCluster->Clear("C");
foutputBuffer->Clear("C");
  Int_t nPluginMax=fPluginArray->GetEntriesFast();
  
  if(nPlugin>nPluginMax) {Fatal(GetName()," Error - Called non-existing plugin");}
  
  CbmMvdSensorPlugin* plugin=(CbmMvdSensorPlugin*)fPluginArray->At(nPlugin);
  
  plugin->Exec();
}
// -------------------------------------------------------------------------
void CbmMvdSensor::ExecTo(UInt_t nPlugin)
{
FairRunSim* run = FairRunSim::Instance();
FairPrimaryGenerator* gen = run->GetPrimaryGenerator();
FairMCEventHeader* event = gen->GetEvent();

foutputDigis->Delete();
foutputCluster->Delete();
foutputDigiMatch->Clear("C");
foutputBuffer->Clear("C");

fcurrentEventTime = event->GetT();
  
   CbmMvdSensorPlugin* plugin;
   Int_t maxPlugin=fPluginArray->GetEntriesFast();
  
   plugin=(CbmMvdSensorPlugin*)fPluginArray->At(0);
if(nPlugin < maxPlugin)
{
    plugin->ExecChain();
  //cout << endl << "is plugin ready? "<< plugin->PluginReady() << " on sensor "<< this->GetName()  << endl;    
  if(plugin->PluginReady()) 
    {
    //cout << endl << "exec chain on sensor "<< this->GetName() << endl;
    for(Int_t i=1; i<=nPlugin; i++)
      {
	//cout << endl << "exec plugin " << i << " on sensor "<< this->GetName() << endl;
	plugin=(CbmMvdSensorPlugin*)fPluginArray->At(i);
	plugin->ExecChain();
      }
    }
}
else {cout << endl << "nPlugin to large" << endl;}

}


// -------------------------------------------------------------------------
void CbmMvdSensor::ExecFrom(UInt_t nPlugin)
{
FairRunSim* run = FairRunSim::Instance();
FairPrimaryGenerator* gen = run->GetPrimaryGenerator();
FairMCEventHeader* event = gen->GetEvent();

fcurrentEventTime = event->GetT();
  
   CbmMvdSensorPlugin* plugin;
   Int_t maxPlugin=fPluginArray->GetEntriesFast();
  
   plugin=(CbmMvdSensorPlugin*)fPluginArray->At(nPlugin);
if(nPlugin <= maxPlugin)
{
    plugin->ExecChain();
  //cout << endl << "is plugin ready? "<< plugin->PluginReady() << " on sensor "<< this->GetName()  << endl;    
  if(plugin->PluginReady()) 
    {
    //cout << endl << "exec chain on sensor "<< this->GetName() << endl;
    for(Int_t i=nPlugin+1; i<maxPlugin; i++)
      {
	//cout << endl << "exec plugin " << i << " on sensor "<< this->GetName() << endl;
	plugin=(CbmMvdSensorPlugin*)fPluginArray->At(i);
	plugin->ExecChain();
      }
    }
}
else {cout << endl << "nPlugin to large" << endl;}

}

// -------------------------------------------------------------------------   
TClonesArray* CbmMvdSensor::GetOutputArray(Int_t nPlugin){

if(nPlugin == fHitPlugin)
	GetOutputBuffer();
else if (nPlugin == fDigiPlugin)
	  {
          CbmMvdSensorDigitizerTask* digiplugin = (CbmMvdSensorDigitizerTask*)fPluginArray->At(nPlugin);
	  Int_t ArrayLength = digiplugin->GetOutputArray()->GetEntriesFast()-1;
	  if(ArrayLength >= 0)
		{
	  	foutputDigis->AbsorbObjects(digiplugin->GetOutputArray(),0,ArrayLength);
	  	foutputDigiMatch->AbsorbObjects(digiplugin->GetMatchArray(),0,ArrayLength);
		//cout << endl << "got digis " << foutputDigis->GetEntriesFast() << " and matches " << foutputDigiMatch->GetEntriesFast() << endl;
		}
	  return(foutputDigis);
	  } 
else if (nPlugin == fClusterPlugin)
	  {
	  CbmMvdSensorClusterfinderTask* clusterplugin = (CbmMvdSensorClusterfinderTask*)fPluginArray->At(nPlugin);
          Int_t ArrayLength = clusterplugin->GetOutputArray()->GetEntriesFast()-1;
	  if(ArrayLength >= 0)
	  	foutputCluster->AbsorbObjects(clusterplugin->GetOutputArray(),0,ArrayLength);
	  return(foutputCluster);
	  } 
else
	{ 
	LOG(FATAL) << "undefined plugin called" << FairLogger::endl;
	}
}    
// -------------------------------------------------------------------------   

// -------------------------------------------------------------------------   
Int_t CbmMvdSensor::GetOutputArrayLen(Int_t nPlugin){

GetOutputArray(nPlugin); //** make sure that the arrays are filled 

if (nPlugin == fDigiPlugin)
	  {
	  return(foutputDigis->GetEntriesFast()-1);
	  }  
else if (nPlugin == fClusterPlugin)
	  {
	  return(foutputCluster->GetEntriesFast()-1);
	  } 
else if (nPlugin == fHitPlugin)
	  {
	  return(foutputBuffer->GetEntriesFast()-1);
	  } 

else
	{ 
	LOG(FATAL) << "undefined plugin called" << FairLogger::endl;
	}

}    
// -------------------------------------------------------------------------   

// -------------------------------------------------------------------------   
TClonesArray* CbmMvdSensor::GetOutputMatch(){

  return(foutputDigiMatch);

}    
// ------------------------------------------------------------------------- 


// -------------------------------------------------------------------------
TClonesArray* CbmMvdSensor::GetOutputBuffer(){



  CbmMvdSensorPlugin* plugin=(CbmMvdSensorPlugin*)fPluginArray->At(fPluginArray->GetLast());
  foutputBuffer->AbsorbObjects(plugin->GetOutputArray(),0,plugin->GetOutputArray()->GetEntriesFast()-1);
  return (foutputBuffer);
     
} 
// -------------------------------------------------------------------------   
    
    

// -----  Coordinate Transformations --------------------------------

void CbmMvdSensor::LocalToTop	(Double_t* local, Double_t* lab){
  fMCMatrix->LocalToMaster(local, lab);
};
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMvdSensor::TopToLocal	(Double_t* lab, Double_t* local){
  fMCMatrix->MasterToLocal(lab, local); 
  //cout << endl << "local 0 nach TopToLocal " << local[0] << endl;
  //cout << endl << "local 1 nach TopToLocal " << local[1] << endl;
};
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMvdSensor::LocalToPixel	(Double_t* local, Int_t &pixelNumberX, Int_t &pixelNumberY){
  
  //Compute position of the frame relativ to the border of the matrix
  //which contains the pixel (0/0)
  Double_t x=local[0] + (fSensorData->GetPixelSignX() * GetDX());
  //cout << "From " << local[0] << " to Double_t x " << x << endl;
  Double_t y=local[1] + (fSensorData->GetPixelSignY() * GetDY());
  //cout << "From " << local[1] << " to Double_t y " << y << endl;
  //Compute the number of the pixel hit.
  //Note: substract 0.5 to move from border to center of pixel
  pixelNumberX= Int_t(x/fSensorData->GetPixelPitchX()-0.5);
  //if (pixelNumberX < 0) cout << "pixelNumberX = " << pixelNumberX << " on Sensor " << this->GetName() << endl;
  pixelNumberY= Int_t(y/fSensorData->GetPixelPitchY()-0.5);
  //if (pixelNumberY < 0) cout << "pixelNumberY = " << pixelNumberY << " on Sensor " << this->GetName() << endl;
};
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmMvdSensor::PixelToLocal	(Int_t pixelNumberX, Int_t pixelNumberY, Double_t* local){
  
  //Compute distance from border of the pixel matrix
  //Ignore the direction of pixel numbering so far
  //By definiton (x,y) is in the center of the pixel
  
  Double_t x = (pixelNumberX + 0.5) * fSensorData->GetPixelPitchX();
  Double_t y = (pixelNumberY + 0.5) * fSensorData->GetPixelPitchY();
  
  //Perform coordinate transformation from border of matrix to center of volume
  local[0]= x - fSensorData->GetPixelSignX()*GetDX();
  local[1]= y - fSensorData->GetPixelSignY()*GetDY();
 
  local[2]=0; //per definition always at the sensor surface;
};
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmMvdSensor::PixelToTop	(Int_t pixelNumberX, Int_t pixelNumberY, Double_t* lab) {
  PixelToLocal (pixelNumberX, pixelNumberY,fTempCoordinate);
  LocalToTop(fTempCoordinate, lab);
};
// -------------------------------------------------------------------------


void CbmMvdSensor::TopToPixel (Double_t* lab, Int_t &pixelNumberX, Int_t &pixelNumberY){
  TopToLocal(lab, fTempCoordinate);
  LocalToPixel(fTempCoordinate, pixelNumberX, pixelNumberY);
};
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
Int_t CbmMvdSensor::GetFrameNumber  (Int_t pixelNumberY, Double_t absoluteTime){
  

  Double_t timeSinceStart= absoluteTime - fSensorStartTime;
  
  Double_t timeInUnitsOfFrames= timeSinceStart/fSensorData->GetIntegrationTime();
  
  // Get time substracting the full frames (140.3 -> 0.3)
  Double_t timeSinceStartOfTheFrame= timeInUnitsOfFrames - (Int_t)timeInUnitsOfFrames;
  
  Int_t rowUnderReadout = Int_t(fSensorData->GetNPixelsY() * timeSinceStartOfTheFrame);
 
  
  return (Int_t)(timeInUnitsOfFrames + (pixelNumberY<rowUnderReadout));
//   
  /* This statement is equivalent to:
  if (pixelNumberY<rowUnderReadout) {return timeInUnitsOfFrames;}
  else				    {return timeInUnitsOfFrames+1;}
  */
}
// -------------------------------------------------------------------------  


// -----   Public method Print   -------------------------------------------
void CbmMvdSensor::Print(Option_t* opt) const {
  cout << " --- " << GetName() << ", sensor name" << fVolName << endl;
  cout << " MC - ID: " << fVolumeId << endl;
  cout << "---------------------------------------------------------" << endl;
  cout << " Position information: " << endl;
  fRecoMatrix->Print();
  cout << " --------------------------------------------------------" << endl;
  cout << " Technical information: " << endl;
  fSensorData->Print("");
  cout     << " , MC Id " << fVolumeId <<endl;
  cout << "---------------------------------------------------------" << endl;
}
// -------------------------------------------------------------------------

//-----------------------------------------------------------------------
void CbmMvdSensor::Finish(){
  CbmMvdSensorPlugin* plugin;
  Int_t nPlugin=fPluginArray->GetEntriesFast();
  for(Int_t i=0; i<nPlugin; i++)
      {
	plugin=(CbmMvdSensorPlugin*)fPluginArray->At(i);
	plugin->Finish();
      }
}  
// -------------------------------------------------------------------------  


ClassImp(CbmMvdSensor)
