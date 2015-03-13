// -------------------------------------------------------------------------
// -----                    CbmMvdClusterfinder source file                -----
// -------------------------------------------------------------------------

// Includes from MVD
#include "CbmMvdClusterfinder.h"
#include "CbmMvdPoint.h"
#include "plugins/tasks/CbmMvdSensorClusterfinderTask.h"
#include "SensorDataSheets/CbmMvdMimosa26AHR.h"
#include "tools/CbmMvdGeoHandler.h"

// Includes from FAIR
#include "FairRootManager.h"
#include "FairModule.h"

// Includes from ROOT
#include "TClonesArray.h"


// Includes from C++
#include <iomanip>
#include <iostream>

// -----   Default constructor   ------------------------------------------
CbmMvdClusterfinder::CbmMvdClusterfinder() 
  : FairTask("MVDClusterfinder"),
    fMode(0),
    fDetector(NULL),
    fInputDigis(NULL),
    fCluster(NULL),
    fClusterPluginNr(),
    fBranchName(""),
    fTimer(),
    fRandGen(),
    fShowDebugHistos(kFALSE)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMvdClusterfinder::CbmMvdClusterfinder(const char* name, Int_t iMode, Int_t iVerbose) 
  : FairTask(name, iVerbose),
    fMode(iMode),
    fDetector(NULL),
    fInputDigis(NULL),
    fCluster(NULL),
    fClusterPluginNr(0),
    fBranchName("MvdDigi"),
    fTimer(),
    fRandGen(),
    fShowDebugHistos(kFALSE)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMvdClusterfinder::~CbmMvdClusterfinder() {
 
if ( fCluster) 
    {
    fCluster->Delete();
    delete fCluster;
    }
}
// -----------------------------------------------------------------------------

// -----   Exec   --------------------------------------------------------------
void CbmMvdClusterfinder::Exec(Option_t* opt){
// --- Start timer
fTimer.Start();
	
fCluster->Delete();
if(fInputDigis && fInputDigis->GetEntriesFast() > 0)
   {
   if(fVerbose) cout << "//----------------------------------------//";
   if(fVerbose) cout << endl << "Send Input" << endl;
   fDetector->SendInputDigis(fInputDigis);
   if(fVerbose) cout << "Execute ClusterPlugin Nr. "<< fClusterPluginNr << endl;
   fDetector->Exec(fClusterPluginNr);
   if(fVerbose) cout << "End Chain" << endl;
   if(fVerbose) cout << "Start writing Cluster" << endl;  
   fCluster->AbsorbObjects(fDetector->GetOutputCluster(),0,fDetector->GetOutputCluster()->GetEntriesFast()-1); 
   if(fVerbose) cout << "Total of " << fCluster->GetEntriesFast() << " Cluster in this Event" << endl;
   if(fVerbose) cout  << "//----------------------------------------//" << endl ;
   LOG(INFO) << "+ " << setw(20) << GetName() << ": Created: " 
        << fCluster->GetEntriesFast() << " cluster in " 
        << fixed << setprecision(6) << fTimer.RealTime() << " s" << FairLogger::endl;
   }

fTimer.Stop();
}
// -----------------------------------------------------------------------------

// -----   Init   --------------------------------------------------------------
InitStatus CbmMvdClusterfinder::Init() {
  cout << "-I- " << GetName() << ": Initialisation..." << endl;
  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << "-I- Initialising " << GetName() << " ...." << endl;

    // **********  RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if ( ! ioman ) 
	{
	cout << "-E- " << GetName() << "::Init: No FairRootManager!" << endl;
	return kFATAL;
	}

    // **********  Get input arrays
    fInputDigis = (TClonesArray*) ioman->GetObject("MvdDigi"); 

    if (! fInputDigis ) {
      LOG(ERROR) << "No MvdDigi branch found. There was no MVD in the simulation. Switch this task off" << FairLogger::endl;
      return kERROR;
    }
   
    // **********  Register output array
    fCluster = new TClonesArray("CbmMvdCluster", 10000);
    ioman->Register("MvdCluster", "Mvd Clusters", fCluster, kTRUE);

    fDetector = CbmMvdDetector::Instance();
    
    if(!fDetector)
    	{
	if(fVerbose) cout << endl << "-I- Try to load CbmMvdDetector -I- " << endl; 
        GetMvdGeometry(); 
	fDetector = CbmMvdDetector::Instance();
	if(fDetector->GetSensorArraySize() > 1)
		{
		 if(fVerbose) cout << endl << "-I- succesfully loaded Geometry from file -I-" << endl;
		}
	else
		{
		LOG(FATAL) <<  "Geometry couldn't be loaded from file. No MVD digitizer available."
	        << FairLogger::endl;
		}
	} 

    CbmMvdSensorClusterfinderTask* clusterTask = new CbmMvdSensorClusterfinderTask();
   
    fDetector->AddPlugin(clusterTask);
    fClusterPluginNr = (UInt_t) (fDetector->GetPluginArraySize());
    if(fShowDebugHistos)fDetector->ShowDebugHistos();
    fDetector->Init();
   

    // Screen output
    cout << GetName() << " initialised with parameters: " << endl;
    //PrintParameters();
    cout << "---------------------------------------------" << endl;

       
    return kSUCCESS;
}

// -----   Virtual public method Reinit   ----------------------------------
InitStatus CbmMvdClusterfinder::ReInit() {

    return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Virtual method Finish   -----------------------------------------
void CbmMvdClusterfinder::Finish() {
    fDetector->Finish();
    PrintParameters();

}					       
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmMvdClusterfinder::Reset() {
    fCluster->Delete();

}
// -------------------------------------------------------------------------  

// -----   Private method GetMvdGeometry   ---------------------------------
void CbmMvdClusterfinder::GetMvdGeometry() {

CbmMvdDetector* Detector = new CbmMvdDetector("A");
CbmMvdGeoHandler* mvdHandler = new CbmMvdGeoHandler();
mvdHandler->Init();
mvdHandler->Fill();
Detector->PrintParameter();
}
// -------------------------------------------------------------------------  



// -----   Private method PrintParameters   --------------------------------
void CbmMvdClusterfinder::PrintParameters() {
    
    cout.setf(ios_base::fixed, ios_base::floatfield);
    cout << "============================================================" << endl;
    cout << "============== Parameters Clusterfinder ====================" << endl;
    cout << "============================================================" << endl;
    cout << "=============== End Task ===================================" << endl;
 
}
// -------------------------------------------------------------------------  



ClassImp(CbmMvdClusterfinder);
