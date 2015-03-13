#include "CbmTrdDigitizerPRF.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdPoint.h"
#include "CbmTrdDigi.h"
#include "CbmTrdDigiMatch.h"
#include "CbmTrdModule.h"
#include "CbmTrdRadiator.h"
#include "CbmTrdGeoHandler.h"

#include "CbmMCTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TF1.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TStopwatch.h"

#include "TPRegexp.h"

//#include "sqlite/sqlite3.h" // used for the lookup table option

#include <iostream>
#include <cmath>
using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::fabs;
// ---- Default constructor -------------------------------------------
CbmTrdDigitizerPRF::CbmTrdDigitizerPRF()
  : FairTask("TrdCluster"),
    Digicounter(-1),
    fLayerZ(),
    fTime(-1.),
    fModuleType(-1),
    fModuleCopy(-1),
    fModuleID(-1),
    fMCindex(-1),
    fEfficiency(1.),
    fTrdPoints(NULL),
    fDigiCollection(new TClonesArray("CbmTrdDigi")),
    fDigiMatchCollection(NULL),
    fMCStacks(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fRadiators(NULL),
    fGeoHandler(new CbmTrdGeoHandler()),
    //fDigiMap(),
    //fDigiMapIt(),
    fModuleClusterMap(),
    fModuleClusterMapIt(),
    //fdigi(NULL),
    //fdigiMatch(NULL),
    fDebug(false),
    fOneClusterPerPoint(false),
    fWireQuantisation(false)
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdDigitizerPRF::CbmTrdDigitizerPRF(const char *name, const char *title,
					     CbmTrdRadiator *radiator, Bool_t oneClusterPerPoint, Bool_t wireQuantisation)
  :FairTask(name),
   Digicounter(-1),
   fLayerZ(),
   fTime(-1.),
   fModuleType(-1),
   fModuleCopy(-1),
   fModuleID(-1),
   fMCindex(-1),
   fEfficiency(1.),
   fTrdPoints(NULL),
   fDigiCollection(NULL),
   fDigiMatchCollection(NULL),
   fMCStacks(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fRadiators(radiator),
   fGeoHandler(new CbmTrdGeoHandler()),
   //fDigiMap(),
   //fDigiMapIt(),
   fModuleClusterMap(),
   fModuleClusterMapIt(),
   //fdigi(NULL),
   //fdigiMatch(NULL),
   fDebug(false),
   fOneClusterPerPoint(oneClusterPerPoint),
   fWireQuantisation(wireQuantisation)
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdDigitizerPRF::~CbmTrdDigitizerPRF()
{
  FairRootManager *ioman =FairRootManager::Instance();
  ioman->Write();

  for (fModuleClusterMapIt = fModuleClusterMap.begin();
       fModuleClusterMapIt != fModuleClusterMap.end(); ++fModuleClusterMapIt) {
    fModuleClusterMapIt->second->SectorSizeX.clear();
    fModuleClusterMapIt->second->SectorSizeY.clear();
    fModuleClusterMapIt->second->PadSizeX.clear();
    fModuleClusterMapIt->second->PadSizeY.clear();
    fModuleClusterMapIt->second->SecxPad.clear();
    fModuleClusterMapIt->second->SecyPad.clear();
    for (Int_t x = 0; x < fModuleClusterMapIt->second->PadPlane.size(); x++) {
      for(Int_t y = 0; y < fModuleClusterMapIt->second->PadPlane[x].size(); y++) {
	fModuleClusterMapIt->second->PadPlane[x][y]->MCIndex.clear();
	delete fModuleClusterMapIt->second->PadPlane[x][y];
      }
      fModuleClusterMapIt->second->PadPlane[x].clear();
    }
    fModuleClusterMapIt->second->PadPlane.clear();

    delete fModuleClusterMapIt->second;
  }
  fModuleClusterMap.clear();

  //fDigiCollection->Clear("C");
  fDigiCollection->Delete();
  delete fDigiCollection;
  //fDigiMatchCollection->Clear("C");
  fDigiMatchCollection->Delete();
  delete fDigiMatchCollection;
  fTrdPoints->Clear("C");
  fTrdPoints->Delete();
  delete fTrdPoints;
  fMCStacks->Clear("C");
  fMCStacks->Delete();
  delete fMCStacks;
  if(fDigiPar)
    delete fDigiPar;
  if(fModuleInfo)
    delete fModuleInfo;
  //delete fdigi;
  //delete fdigiMatch;
}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdDigitizerPRF::SetParContainers()
{
    cout<<" * CbmTrdDigitizerPRF * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdDigitizerPRF::ReInit(){

  cout<<" * CbmTrdClusterizer * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdDigitizerPRF::Init()
{

  cout<<" * CbmTrdDigitizerPRF * :: Init() "<<endl;

  FairRootManager *ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
    
  fTrdPoints=(TClonesArray *)ioman->GetObject("TrdPoint"); 
  if ( ! fTrdPoints ) {
    cout << "-W CbmTrdClusterFast::Init: No TrdPoints array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }

  fMCStacks = (TClonesArray*)ioman->GetObject("MCTrack");
  if ( ! fMCStacks ) {
    cout << "-W CbmTrdClusterFast::Init: No MCTrack array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }

  fDigiCollection = new TClonesArray("CbmTrdDigi", 100);
  ioman->Register("TrdDigi","TRD Digis",fDigiCollection,kTRUE);

  fDigiMatchCollection = new TClonesArray("CbmTrdDigiMatch", 100);
  ioman->Register("TrdDigiMatch","TRD Digis",fDigiMatchCollection,kTRUE);

  fGeoHandler->Init();

  fRadiators->Init();

  return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdDigitizerPRF::Exec(Option_t * option)
{
  TStopwatch timer;
  timer.Start();
  cout << "================CbmTrdDigitizerPRF=====================" << endl;
  printf("  OneClusterPerPoint: %i\n  WireQuantisation: %i\n\n",(Int_t)fOneClusterPerPoint,(Int_t)fWireQuantisation);
  Digicounter = 0;
  CbmTrdPoint *pt = NULL;
  CbmMCTrack *p = NULL;
  fDebug = false;//true;
  /*
  // Module infos are stored to ModuleClusterMap whenever a module is used for the first time in each event
  // Digis are buffered in the ModuleClusterMap until the end of event and than copied to die digicollection
  // Digis are only calculated for three central pads in x and y (start up version)
  // Afterwards can be extended to n-pad (worse, easier to implement) cluster for x and y or a maximum distance to cluster center (better, more flexible)
  // Dont forget to erase the ModuleClusterMap after each event!!!
  // First step: digis are estmated using prf for the position coresponding to center between entrance and exit point
  // Second step: path between entrance and exit is splitted into slithes
  */
  TVector3 mom;
  Double_t ELoss;
  Double_t ELossTR;     // TR energy loss for e- & e+
  Double_t ELossdEdX;   // ionization energy loss
  Int_t nCount = 0;
  Int_t nEntries = fTrdPoints->GetEntries();
  cout << " Found " << nEntries << " MC-Points in Collection of TRD" << endl;
  for (int j = 0; j < nEntries ; j++ ){
    //cout << " " << j << endl;
    pt = (CbmTrdPoint*) fTrdPoints->At(j);
    if(NULL == pt) {
      cout << " no point found " << endl;
    }
    if(NULL == pt) continue;

    pt->Momentum(mom);
    fMCindex=pt->GetTrackID();
    p = (CbmMCTrack*) fMCStacks->At(fMCindex);
    if(NULL == p) {
      cout << " no point found " << endl;
    }

    if(NULL == p) continue;

    Int_t pdgCode = p->GetPdgCode();
    ELoss = pt->GetEnergyLoss();
    if(TMath::Abs(pdgCode) == 11){      
      ELoss +=  fRadiators->GetTR(mom);
    }
    // Calculate point in the middle of the detector. This is
    // for safety reasons, because the point at exit is slightly
    // outside of the active volume. If one does not use a point
    // in the detector one will get a wrong volume from the
    // geomanager. Navigate to the correct volume to get all
    // necessary information about this volume
    // --------------->[cm]<---------------
    Double_t x_in   = pt->GetXIn();
    Double_t x_out  = pt->GetXOut();
    Double_t y_in   = pt->GetYIn();
    Double_t y_out  = pt->GetYOut();
    Double_t z_in   = pt->GetZIn();
    Double_t z_out  = pt->GetZOut();
    Double_t x_mean = (x_in + x_out)/2.;
    Double_t y_mean = (y_in + y_out)/2.;
    Double_t z_mean = (z_in + z_out)/2.;
    // --------------->[mm]<---------------
 
    gGeoManager->FindNode(x_mean, y_mean, z_mean);

    // Get the local point in local MC coordinates from
    // the geomanager. This coordinate system is rotated
    // if the chamber is rotated. This is corrected in 
    // GetModuleInformationFromDigiPar(..) to have a
    // the same local coordinate system in all the chambers
    const Double_t *global_point = gGeoManager->GetCurrentPoint();
    Double_t local_point[3];
    Double_t local_meanLL[3];
    Double_t local_meanC[3];
    Double_t global_meanLL[3];//[cm]
    Double_t global_meanC[3];
    Double_t local_inLL[3];
    Double_t local_inC[3];
    Double_t global_inLL[3];//[cm]
    Double_t global_inC[3];
    Double_t local_outLL[3];
    Double_t local_outC[3];
    Double_t global_outLL[3];//[cm]
    Double_t global_outC[3];

    global_meanC[0]= x_mean;//[cm]      
    global_inC[0]  = x_in  ;//[cm]      
    global_outC[0] = x_out ;//[cm]

    global_meanC[1]= y_mean;//[cm]      
    global_inC[1]  = y_in  ;//[cm]      
    global_outC[1] = y_out ;//[cm]

    global_meanC[2]= z_mean;//[cm]      
    global_inC[2]  = z_in  ;//[cm]      
    global_outC[2] = z_out ;//[cm]

    gGeoManager->MasterToLocal(global_point, local_point);
    gGeoManager->MasterToLocal(global_meanC, local_meanC);
    gGeoManager->MasterToLocal(global_inC, local_inC);
    gGeoManager->MasterToLocal(global_outC, local_outC);

    for ( Int_t idim = 0; idim < 3; idim++)
      {
	local_meanC[idim]  *= 10; //[cm]->[mm]
	local_inC[idim]    *= 10; //[cm]->[mm]
	local_outC[idim]   *= 10; //[cm]->[mm]

	global_meanC[idim] *= 10; //[cm]->[mm]
	global_inC[idim]   *= 10; //[cm]->[mm]
	global_outC[idim]  *= 10; //[cm]->[mm]
      }

    fModuleID = pt->GetDetectorID();
    GetModuleInformationFromDigiPar(fModuleID);


    TransformC2LL(local_meanC, local_meanLL);
    TransformC2LL(local_inC,   local_inLL);
    TransformC2LL(local_outC,  local_outLL);

     if (!fOneClusterPerPoint)
      SplitPathSlices(local_inLL, local_outLL, ELoss, j);
    else{
      Double_t padDisplacementLL[2] = {0, 0};
      Int_t PadMax[2] = {-1, -1};
      GetClusterDisplacement(local_meanLL, padDisplacementLL, PadMax);
      CalcDigisOnPadPlane(padDisplacementLL, PadMax, ELoss, j);
    }
  }
  //if (NULL != p) delete p;
  //if (NULL != pt) delete pt;
  
  Int_t iDigi = 0;
  CbmTrdDigi* fdigi = NULL;
  //CbmTrdDigiMatch *fdigiMatch = NULL;
  
  for (fModuleClusterMapIt = fModuleClusterMap.begin(); fModuleClusterMapIt != fModuleClusterMap.end(); fModuleClusterMapIt++) {
    if (fDebug)
      printf("L%i M%i\n    (%3i,%3i) ",(*fModuleClusterMapIt).second->Layer,(*fModuleClusterMapIt).first,(Int_t)(*fModuleClusterMapIt).second->PadPlane.size(),(Int_t)(*fModuleClusterMapIt).second->PadPlane[Int_t((*fModuleClusterMapIt).second->PadPlane.size()) -1].size());
    for (Int_t xPad = 0; xPad < (*fModuleClusterMapIt).second->PadPlane.size(); xPad++) {
      //printf("\n");
      for (Int_t yPad = 0; yPad < (*fModuleClusterMapIt).second->PadPlane[xPad].size(); yPad++) {
	//printf("%.1e ",(*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->Charge);
	if ((*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->Charge > 0.0) {
	  if (fDebug)
	    printf("xPad%i yPad%i  ",xPad,yPad);
	  /*
	    CbmTrdDigi* fdigi = new CbmTrdDigi(
	    (*fModuleClusterMapIt).first, 
	    xPad, 
	    yPad, 
	    (*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->Charge, 
	    fTime, 
	    (*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex[0]//iDigi // ?????????????????
	    );
	  
	    for (Int_t i = 1; i < (*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex.size(); i++)
	    fdigi->AddMCIndex((*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex[i]);
	  */
	  new ((*fDigiCollection)[iDigi]) CbmTrdDigi(
						     (*fModuleClusterMapIt).first, 
						     xPad, 
						     yPad, 
						     (*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->Charge, 
						     fTime, 
						     (*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex[0]//iDigi // ?????????????????
						     );
	  
	  fdigi = (CbmTrdDigi*)((*fDigiCollection)[iDigi]);
	  for (Int_t i = 1; i < (*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex.size(); i++)
	    fdigi->AddMCIndex((*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex[i]);
	  
	  
	  CbmTrdDigiMatch *fdigiMatch = new ((*fDigiMatchCollection)[iDigi]) CbmTrdDigiMatch(); 

	  for (Int_t i = 0; i < (*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex.size(); i++)
	    Int_t bla = fdigiMatch->AddPoint((*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex[i]);
	  
	  iDigi++;
	  //delete fdigi;
	  //delete fdigiMatch;
	}
      }
    }
    if (fDebug)
      printf("\n");
  }

  //delete fdigi;
  //delete fdigiMatch;
  printf(" Added %d Digis to Collection of TRD\n\n",iDigi);
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");

  //fIntegralTest->Draw();
}
  // --------------------------------------------------------------------
  void CbmTrdDigitizerPRF::CalcDigisOnPadPlane(Double_t* clusterPosInPadLL, Int_t* PadMax, Double_t ELoss, Int_t pointId)
  {
    if (PadMax[0] >= (Int_t)fModuleClusterMap[fModuleID]->PadPlane.size() || PadMax[1] >= (Int_t)fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]].size()) {
      printf("\n\n ------  ERROR: CalcDigisOnPadPlane: PadMax out of module.\n Module size: (%3i,%3i)  PadMax(%3i,%3i)   ------\n\n",(Int_t)fModuleClusterMap[fModuleID]->PadPlane.size(), (Int_t)fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]].size(),PadMax[0],PadMax[1]);
    }
    else
      {
	//const Int_t fnPadCluster = 11; // has to be odd
	Int_t xStart = PadMax[0] - fnPadCluster / 2;
	Int_t xStop  = PadMax[0] + fnPadCluster / 2 + 1;
	Int_t yStart = PadMax[1] - fnPadCluster / 2;
	Int_t yStop  = PadMax[1] + fnPadCluster / 2 + 1;
	if(fDebug/* || fModuleClusterMap[fModuleID]->Layer%2 > 0*/)
	  printf("L%i M%i\n   ",fModuleClusterMap[fModuleID]->Layer,fModuleID);

	if(fDebug/* || fModuleClusterMap[fModuleID]->Layer%2 > 0*/)
	  printf("PadPlane(%3i,%3i)(%3i,%3i)  PadMax(%3i,%3i)     ",(Int_t)fModuleClusterMap[fModuleID]->PadPlane.size(), (Int_t)fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]].size(),fModuleClusterMap[fModuleID]->nxPad,fModuleClusterMap[fModuleID]->nyPad,PadMax[0],PadMax[1]);
	Double_t xPosC = -clusterPosInPadLL[0] + 0.5 * fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]][PadMax[1]]->SizeX;
	Double_t yPosC = -clusterPosInPadLL[1] + 0.5 * fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]][PadMax[1]]->SizeY;
	//if(fDebug || fModuleClusterMap[fModuleID]->Layer%2 > 0)
	//printf ("x(%3i,%3i) y(%3i,%3i)      ",xStart,xStop,yStart,yStop);
	if (xStart < 0)
	  xStart = 0;
	if (xStart > fModuleClusterMap[fModuleID]->nxPad)
	  xStart = fModuleClusterMap[fModuleID]->nxPad;
	if (xStop < 0)
	  xStop = 0;
	if (xStop > fModuleClusterMap[fModuleID]->nxPad)
	  xStop = fModuleClusterMap[fModuleID]->nxPad;
	if (yStart < 0)
	  yStart = 0;
	if (yStart > fModuleClusterMap[fModuleID]->nyPad)
	  yStart = fModuleClusterMap[fModuleID]->nyPad;
	if (yStop < 0)
	  yStop = 0;
	if (yStop > fModuleClusterMap[fModuleID]->nyPad)
	  yStop = fModuleClusterMap[fModuleID]->nyPad;
	if(fDebug/* || fModuleClusterMap[fModuleID]->Layer%2 > 0*/)
	  printf ("x(%3i,%3i) y(%3i,%3i)\n",xStart,xStop,yStart,yStop);

	if(fDebug/* || fModuleClusterMap[fModuleID]->Layer%2 > 0*/)
	  printf("LL%.2f,%.2f C:%.2f,%.2f ",clusterPosInPadLL[0],clusterPosInPadLL[1],xPosC,yPosC);

	for (Int_t ix = xStart; ix < PadMax[0]; ix++)
	  xPosC -= fModuleClusterMap[fModuleID]->PadPlane[ix][PadMax[1]]->SizeX;
	for (Int_t iy = yStart; iy < PadMax[1]; iy++)
	  yPosC -= fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]][iy]->SizeY;

	if(fDebug/* || fModuleClusterMap[fModuleID]->Layer%2 > 0*/)
	  printf("C:%.2f,%.2f ",xPosC,yPosC);

	Double_t xPosCStart = xPosC;
	Double_t yPosCStart = yPosC;

	for (Int_t y = yStart; y < yStop; y++) {
	  if(fDebug/* || fModuleClusterMap[fModuleID]->Layer%2 > 0*/)
	    printf("y%3i: ",y);
	  for (Int_t x = xStart; x < xStop; x++) {
	    if(fDebug/* || fModuleClusterMap[fModuleID]->Layer%2 > 0*/)
	      printf("x%3i, ",x);
	    if(fDebug/* || fModuleClusterMap[fModuleID]->Layer%2 > 0*/)
	      printf("C:%.2f,%.2f [%.2f,%.2f] ",xPosC,yPosC,fModuleClusterMap[fModuleID]->PadPlane[x][y]->SizeX,fModuleClusterMap[fModuleID]->PadPlane[x][y]->SizeY);
	    fModuleClusterMap[fModuleID]->PadPlane[x][y]->Charge += 
	      CalcMathieson(xPosC, fModuleClusterMap[fModuleID]->PadPlane[x][y]->SizeX, fModuleClusterMap[fModuleID]->h) * 
	      CalcMathieson(yPosC, fModuleClusterMap[fModuleID]->PadPlane[x][y]->SizeY, fModuleClusterMap[fModuleID]->h) *
	      ELoss;
	    fModuleClusterMap[fModuleID]->PadPlane[x][y]->MCIndex.push_back(/*fMCindex*/pointId);
	    xPosC += fModuleClusterMap[fModuleID]->PadPlane[x][y]->SizeX; 
	    if(fDebug /*|| fModuleClusterMap[fModuleID]->Layer%2 > 0*/)
	      printf("c%.0e ",fModuleClusterMap[fModuleID]->PadPlane[x][y]->Charge);
	  }
	  if(fDebug/* || fModuleClusterMap[fModuleID]->Layer%2 > 0*/)
	    printf("\n");
	  xPosC = xPosCStart;
	  yPosC += fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]][y]->SizeY;
	}
      }
  }

  // --------------------------------------------------------------------
  void CbmTrdDigitizerPRF::InitPadPlane(ClusterModule* mCluster)
  {
    Int_t iXSector = 0;
    Int_t iYSector = 0;
    Int_t secXPad = 0;
    Int_t secYPad = 0;
    if(fDebug)
      printf("    (%3i,%3i) %3i\n",mCluster->nxPad,mCluster->nyPad,(Int_t)mCluster->PadPlane.size());
    mCluster->PadPlane.resize(mCluster->nxPad);
    for (Int_t xPads = 0; xPads < mCluster->nxPad; xPads++){
      mCluster->PadPlane[xPads].resize(mCluster->nyPad);
      iYSector = 0;
      if (xPads == mCluster->SecxPad[iXSector]){
	iXSector++;
	secXPad = 0;
      }
    
      for (Int_t yPads = 0; yPads < mCluster->nyPad; yPads++){
      
	if (yPads == mCluster->SecyPad[iYSector]){
	  iYSector++;
	  secYPad = 0;
	}   

	ClusterPad* pad = new ClusterPad;    
	mCluster->PadPlane[xPads][yPads] = pad;
      
	mCluster->PadPlane[xPads][yPads]->SizeX  = mCluster->PadSizeX[iXSector];
	mCluster->PadPlane[xPads][yPads]->SizeY  = mCluster->PadSizeY[iYSector];
	mCluster->PadPlane[xPads][yPads]->Charge = 0.0;

	if(fDebug /*|| fModuleClusterMap[fModuleID]->Layer%2 > 0*/)
	  printf("xS%i yS%i xP%i yP%i nxP%i nyP%i sxP%i syP%i pxS%.2f pyS%.2f\n",iXSector,iYSector,xPads,yPads,mCluster->nxPad,mCluster->nyPad,secXPad,secYPad,mCluster->PadSizeX[iXSector],mCluster->PadSizeY[iYSector]);
	secYPad++;
      }

      secXPad++;
    }
    if(fDebug)
      printf("    (%3i,%3i) (%3i,%3i)\n",mCluster->nxPad,mCluster->nyPad,(Int_t)mCluster->PadPlane.size(),(Int_t)mCluster->PadPlane[0].size());
  }
  // --------------------------------------------------------------------

void CbmTrdDigitizerPRF::GetModuleInformationFromDigiPar(Int_t VolumeID)
{
  //cout << "GetModuleInformationFromDigiPar" << endl;
  // fPos is >0 for x and y and not rotated
  // origin of the local coordinate system in 
  // the lower left corner of the chamber, 
  // x to the right (small side of the pads), y up  

  //cout << "---ModuleID: " << VolumeID << endl;

  fModuleInfo = fDigiPar->GetModule(VolumeID);
  if (fModuleInfo != NULL)
    {
      Int_t detID = fModuleInfo->GetDetectorId();

      if (detID != VolumeID ){
	cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
      }

      fModuleType = fGeoHandler->GetModuleType(detID);
      fModuleCopy = fGeoHandler->GetModuleCopyNr(detID);
      //---------------------------------------------------------------------------------------------------
      std::map<Int_t, ClusterModule* >::iterator it = fModuleClusterMap.find(detID);
      if (it == fModuleClusterMap.end()) {
	//cout << "------new ModuleID: " << VolumeID << endl;
	ClusterModule *mCluster = new ClusterModule;
	fModuleClusterMap[detID] = mCluster;
	/*
	  add a matrix 0 -> nrow and ncol for pad width and height of each pad to get wride of the padsizematrix !!!! 
	*/
	Float_t averagePadSizeX;
	Float_t averagePadSizeY;
	mCluster -> Station = fGeoHandler->GetStation(detID);
	mCluster -> Layer = fGeoHandler->GetLayer(detID);
	mCluster -> moduleId = detID;//moduleId;
	
	mCluster -> h = 10 * fModuleInfo->GetAnodeWireToPadPlaneDistance();
	mCluster -> AnodeWireOffset = 10 * fModuleInfo->GetAnodeWireOffset();
	mCluster -> AnodeWireSpacing = 10 * fModuleInfo->GetAnodeWireSpacing();

	mCluster -> ModulePositionX = (Int_t)(10 * fModuleInfo->GetX());
	mCluster -> ModulePositionY = (Int_t)(10 * fModuleInfo->GetY());
	mCluster -> ModulePositionZ = (Int_t)(10 * fModuleInfo->GetZ());
	mCluster -> ModuleSizeX = (fModuleInfo->GetSizeX()) * 10. * 2;
	mCluster -> ModuleSizeY = (fModuleInfo->GetSizeY()) * 10. * 2;
	mCluster -> nxPad = fModuleInfo->GetNofColumns();
	mCluster -> nyPad = fModuleInfo->GetNofRows();
	mCluster -> NoSectors = fModuleInfo->GetNofSectors();
	if(fDebug)
	  printf("S%i L%i M%i Msx%.2f Msy%.2f\n",mCluster -> Station,mCluster -> Layer,mCluster -> moduleId,mCluster -> ModuleSizeX,mCluster -> ModuleSizeY);

	const Int_t NoSectors = fModuleInfo->GetNofSectors();
	mCluster -> SectorSizeX.resize(NoSectors);
	mCluster -> SectorSizeY.resize(NoSectors);
	mCluster -> PadSizeX.resize(NoSectors);
	mCluster -> PadSizeY.resize(NoSectors);
	mCluster -> SecyPad.resize(NoSectors);
	mCluster -> SecxPad.resize(NoSectors);      

	for (Int_t i = 0; i < NoSectors; i++) {
	  mCluster -> SectorSizeX[i] = 10 * fModuleInfo->GetSectorSizeX(i);
	  mCluster -> SectorSizeY[i] = 10 * fModuleInfo->GetSectorSizeY(i);	
	  mCluster -> PadSizeX[i]    = 10 * fModuleInfo->GetPadSizeX(i);
	  mCluster -> PadSizeY[i]    = 10 * fModuleInfo->GetPadSizeY(i);
	  //}

	  //for (Int_t i = 0; i < NoSectors; i++) {
	  mCluster -> SecyPad[i]      = Int_t(mCluster->SectorSizeY[i] / mCluster->PadSizeY[i]);
	  mCluster -> SecxPad[i]      = Int_t(mCluster->SectorSizeX[i] / mCluster->PadSizeX[i]);
	  if (i > 0) {
	    if (mCluster -> SectorSizeX[i] == mCluster -> ModuleSizeX){
	      mCluster -> SectorSizeX[i] = 0.0;
	      mCluster -> SecxPad[i] = 0;
	      mCluster -> PadSizeX[i] = 0.0;
	    }
	    if (mCluster -> SectorSizeY[i] == mCluster -> ModuleSizeY){
	      mCluster -> SectorSizeY[i] = 0.0;
	      mCluster -> SecyPad[i] = 0;
	      mCluster -> PadSizeY[i] = 0.0;
	    }
	  }
	  if(fDebug)
	    printf("       S%i Ssx%.2f Ssy%.2f Psx%.2f Psy%.2f nPx%i nPy%i\n" ,i,mCluster -> SectorSizeX[i],mCluster -> SectorSizeY[i],mCluster -> PadSizeX[i],mCluster -> PadSizeY[i],mCluster -> SecxPad[i],mCluster -> SecyPad[i]);
	}

	 
	averagePadSizeX /= NoSectors;
	averagePadSizeY /= NoSectors;

	InitPadPlane(mCluster);
      }
    }
  else
    {
      printf("fModuleInfo == NULL\n");
    }
}
  // --------------------------------------------------------------------
  void CbmTrdDigitizerPRF::GetModuleInformation(){
    //cout << "GetModuleInformation" << endl;
    // Extract the information about station, layer, module type
    // and cpoy number of the module from the full path to the
    // node.
    // The full path is tokenized at the "/" which diide the different
    // levels of the geometry.
    // Knowing the nameing scheme of the volumes one gets the required
    // information with simple string manipulation.
    // This is probably not the fastes way, but the speed has to be checked.
    // The methode works only for versions of Root > 5.20.0, before the
    // class TStringTocken is not implemented

    TString path = gGeoManager->GetPath();
    cout<<"Path: "<<path<<endl;
    TStringToken* bla = new TStringToken(path,"/");

    while (bla->NextToken()) {
      if (bla->Contains("layer")) {
	TString bla3 = (TString) *bla;
	Ssiz_t pos = bla3.Last('_');
	Ssiz_t substringLength=bla3.Length()-pos-1;
	TString bla2 = bla3((bla3.Last('_')+1),substringLength);
	TString bla1 = bla3(3,1);
	fModuleClusterMap[fModuleID]->Station=bla1.Atoi();
	fModuleClusterMap[fModuleID]->Layer=bla2.Atoi();
      }
      if (bla->Contains("mod")){
	TString bla3 = (TString) *bla;
	Ssiz_t pos = bla3.Last('_');
	Ssiz_t substringLength=bla3.Length()-pos-1;
	TString bla2 = bla3(pos+1,substringLength);
	substringLength=pos-7;
	TString bla1 = bla3(7,substringLength);     
	fModuleType = bla1.Atoi();
	fModuleCopy = bla2.Atoi();
	break;
      } 
    }
  }
  // --------------------------------------------------------------------
  void CbmTrdDigitizerPRF::AddDigi(const Int_t pointID, Int_t ixPad, Int_t iyPad, Int_t nxPad, Int_t nyPad, Double_t iCharge)
  {

  }
  // --------------------------------------------------------------------
void CbmTrdDigitizerPRF::SplitPathSlices(Double_t* local_inLL, Double_t* local_outLL, Double_t ELoss, Int_t j)
{
  const Float_t clusterDistance = 7.5*0.55; //[mm]
  Double_t trackProjectionLength = TMath::Sqrt(pow(local_outLL[0] - local_inLL[0],2) + pow(local_outLL[1] - local_inLL[1],2));  
  Int_t nCluster =  trackProjectionLength / clusterDistance;
  if (nCluster < 1)
    nCluster = 1;

  ELoss /= Float_t(nCluster);

  Double_t pathSlice[2] = {
    (local_outLL[0] - local_inLL[0]) / Float_t(nCluster),
    (local_outLL[1] - local_inLL[1]) / Float_t(nCluster)
  };

  for (Int_t iCluster = 0; iCluster < nCluster; iCluster++) {
    Double_t local_clusterLL[2] = {
      local_inLL[0] + (iCluster * pathSlice[0]) + (0.5 * pathSlice[0]),
      local_inLL[1] + (iCluster * pathSlice[1]) + (0.5 * pathSlice[1])
    };
    Double_t padDisplacementLL[2] = {0.0, 0.0};
    Int_t PadMax[2] = {-1, -1};
    GetClusterDisplacement(local_clusterLL, padDisplacementLL, PadMax);
    //printf("%.2f %.2f    ",padDisplacementLL[0],padDisplacementLL[1]);
    if (fWireQuantisation)
      WireQuantisation(padDisplacementLL, fModuleClusterMap[fModuleID]->AnodeWireOffset, fModuleClusterMap[fModuleID]->AnodeWireSpacing, fModuleClusterMap[fModuleID]->PadPlane[0][0]->SizeX, fModuleClusterMap[fModuleID]->PadPlane[0][0]->SizeY);
    //printf("%.2f %.2f    \n",padDisplacementLL[0],padDisplacementLL[1]);
    CalcDigisOnPadPlane(padDisplacementLL, PadMax, ELoss, j);
    //if (local_clusterLL[0] > local_outLL[0] || local_clusterLL[1] > local_outLL[1])
    //printf(" ERROR::Overshoot:(%.2f,%.2f)_in (%.2f,%.2f)_out (%.2f,%.2f)_cluster (%.2f,%.2f)_slice\n",local_inLL[0],local_inLL[1],local_outLL[0],local_outLL[1],local_clusterLL[0],local_clusterLL[1],pathSlice[0],pathSlice[1]);
  }
}
  // --------------------------------------------------------------------
void CbmTrdDigitizerPRF::WireQuantisation(Double_t* local_point, Double_t AnodeWireOffset, Double_t AnodeWireSpacing, Double_t PadSizex, Double_t PadSizey)
  {
 Double_t local_point_temp[2] = {local_point[0], local_point[1]};
  if (AnodeWireOffset > 0.0 && AnodeWireSpacing > 0.0) {
    if (PadSizex < PadSizey) { // get anode wire orientation from pad aspect ratio
      local_point[1] = Int_t(((local_point_temp[1] - AnodeWireOffset) / AnodeWireSpacing) + 0.5) * AnodeWireSpacing;
    }
    else {
      local_point[0] = Int_t(((local_point_temp[0] - AnodeWireOffset) / AnodeWireSpacing) + 0.5) * AnodeWireSpacing;
    }
  }
  else {
    printf("  ERROR:: AnodeWireOffset and AnodeWireSpacing not set. WireQuantisation can not be used.\n");
  }
  }
  // --------------------------------------------------------------------
  void CbmTrdDigitizerPRF::ChargeConservation()
  {

  }
  // --------------------------------------------------------------------
Double_t CbmTrdDigitizerPRF::CalcMathieson(Double_t x, Double_t W, Double_t h)
  {
    Float_t K3 = 0.525;  //Mathieson parameter for 2nd MuBu prototype -> Parametrisation for chamber parameter
    //Float_t K3 = (-0.7/1.6 * (h/s) + 0.7) + ((exp(-9.74350e+02 * ra/s) * 5.64791e-01 + 3.32737e-01));// aproximation of 'E. Mathieson 'Cathode Charge Distributions in Multiwire Chambers' Nuclear Instruments and Methods in Physics Research A270,1988
    //K3 = 5.25407e-01; // MS 336 fit
    //K3 = 6.14949e-01; // MS 444 fit

    //Float_t K2 = 3.14159265 / 2.* ( 1. - sqrt(K3)/2.);
    //Float_t K1 = (K2 * sqrt(K3)) / (4. * atan(sqrt(K3)));
    //Float_t W = 5;
    //Float_t par = 1;
    //Float_t h = 3;
    Double_t SqrtK3 = sqrt(K3);
    /*
      Char_t formula[500];
      sprintf(formula," -1. / (2. * atan(sqrt(%f))) * (atan(sqrt(%f) *tanh(3.14159265 * (-2. + sqrt(%f) ) * (%f + 2.* x * %f) / (8.* %f) )) +  atan(sqrt(%f) *  tanh(3.14159265 * (-2. + sqrt(%f) ) * (%f - 2.* x * %f) / (8.* %f) )) )",K3,K3,K3,W,par,h,K3,K3,W,par,h);
      TF1* mathiesonPRF = new TF1("mathieson",formula, -15, 15);
    */
    Double_t mathiesonPRF = fabs(
				 -1. / (2. * atan(SqrtK3)) * (
							      atan(SqrtK3 * tanh(TMath::Pi() * (-2. + SqrtK3 ) * (W + 2.* x) / (8.* h) )) +  
							      atan(SqrtK3 * tanh(TMath::Pi() * (-2. + SqrtK3 ) * (W - 2.* x) / (8.* h) )) 
							      )
				 );
    /*
      -1. / (2. * atan(sqrt(K3))) * (
      atan(sqrt(K3) * tanh(3.14159265 * (-2. + sqrt(K3) ) * (W + 2.* x) / (8.* h) )) +
      atan(sqrt(K3) * tanh(3.14159265 * (-2. + sqrt(K3) ) * (W - 2.* x) / (8.* h) )) 
      );
    */
    return mathiesonPRF;
  }
  // --------------------------------------------------------------------
  void CbmTrdDigitizerPRF::TransformLL2C(Double_t* LLCoordinate, Double_t* CCoordinate)
  {
    //Transform coordinates from module coordinate system with origin in the Lower Left (LL) corner to the
    //coordinate system with origin in the Center (C) of the module
 
    CCoordinate[0] = LLCoordinate[0] - 0.5 * fModuleClusterMap[fModuleID]->ModuleSizeX;
    CCoordinate[1] = LLCoordinate[1] - 0.5 * fModuleClusterMap[fModuleID]->ModuleSizeY;
  }
  // --------------------------------------------------------------------
  void CbmTrdDigitizerPRF:: TransformC2LL(Double_t* CCoordinate, Double_t* LLCoordinate)
  {
    //Transform the coordinates from coordinate system with origin in the Center (C) of the module 
    // to the coordinate system with origin in the Lower Left (LL) corner

    LLCoordinate[0] = CCoordinate[0] + 0.5 * fModuleClusterMap[fModuleID]->ModuleSizeX;
    LLCoordinate[1] = CCoordinate[1] + 0.5 * fModuleClusterMap[fModuleID]->ModuleSizeY;
    
  }
  // --------------------------------------------------------------------
  void CbmTrdDigitizerPRF::GetClusterDisplacement(Double_t* clusterPosInModuleLL, Double_t* clusterPosInPadLL, Int_t* PadMax)
  {
    PadMax[0] = 0; // Fired pad in x-direction
    PadMax[1] = 0; // "      "   " y-direction
    Double_t tempPosX = clusterPosInModuleLL[0];
    Double_t tempPosY = clusterPosInModuleLL[1];
    Int_t ixSector;
    Int_t iySector;
    for (ixSector = 0; ixSector < fModuleClusterMap[fModuleID]->NoSectors; ixSector++)
      {
	if (tempPosX > fModuleClusterMap[fModuleID]->SectorSizeX[ixSector])
	  {
	    tempPosX -= fModuleClusterMap[fModuleID]->SectorSizeX[ixSector];
	    PadMax[0] += fModuleClusterMap[fModuleID]->SecxPad[ixSector];
	    if (fDebug)
	      printf(" %.2f %i |",tempPosX,PadMax[0]);
	  }
	else
	  {
	    PadMax[0] += Int_t(tempPosX / Float_t(fModuleClusterMap[fModuleID]->PadSizeX[ixSector]));
	    tempPosX -= Int_t(tempPosX / Float_t(fModuleClusterMap[fModuleID]->PadSizeX[ixSector])) * fModuleClusterMap[fModuleID]->PadSizeX[ixSector];
	    if (fDebug)
	      printf(" %.2f %i || X\n",tempPosX,PadMax[0]);
	    //printf("x ready\n");
	    break;
	  }
      }  
    for (iySector = 0; iySector < fModuleClusterMap[fModuleID]->NoSectors; iySector++)
      {
	if (tempPosY > fModuleClusterMap[fModuleID]->SectorSizeY[iySector])
	  {
	    tempPosY -= fModuleClusterMap[fModuleID]->SectorSizeY[iySector];
	    PadMax[1] += fModuleClusterMap[fModuleID]->SecyPad[iySector];
	    if (fDebug)
	      printf(" %.2f %i |",tempPosY,PadMax[1]);
	  }
	else
	  {
	    PadMax[1] += Int_t(tempPosY / Float_t(fModuleClusterMap[fModuleID]->PadSizeY[iySector]));
	    tempPosY -= Int_t(tempPosY / Float_t(fModuleClusterMap[fModuleID]->PadSizeY[iySector])) * fModuleClusterMap[fModuleID]->PadSizeY[iySector];	  
	    if (fDebug)
	      printf(" %.2f %i || Y\n",tempPosY,PadMax[1]);
	    //printf("y ready\n");
	    break;
	  }
      }

    clusterPosInPadLL[0] = tempPosX;// - Int_t(tempPosX / Float_t(fModuleClusterMap[fModuleID]->PadSizeX[ixSector])) * fModuleClusterMap[fModuleID]->PadSizeX[ixSector];
    clusterPosInPadLL[1] = tempPosY;// - Int_t(tempPosY / Float_t(fModuleClusterMap[fModuleID]->PadSizeY[iySector])) * fModuleClusterMap[fModuleID]->PadSizeY[iySector];
    if (fDebug){
      if ( PadMax[0] == 0 || PadMax[1] == 0 )
	printf("-------------------> ");
      printf("(%.2f,%.2f) (%.2f,%.2f) (%i,%i) (%i,%i)\n",clusterPosInModuleLL[0],clusterPosInModuleLL[1],clusterPosInPadLL[0],clusterPosInPadLL[1],PadMax[0],PadMax[1],ixSector,iySector);
    }
  }
  // --------------------------------------------------------------------
  Int_t CbmTrdDigitizerPRF::GetxPad(Double_t tempPosX)/*tempPosX has to be in LL module coordinate-systems in [mm]*/
  {
    Int_t ixPad = 0;
    for (Int_t iSector = 0; iSector < fModuleClusterMap[fModuleID]->NoSectors; iSector++)
      {
	if (tempPosX > fModuleClusterMap[fModuleID]->SectorSizeX[iSector])
	  {
	    tempPosX -= fModuleClusterMap[fModuleID]->SectorSizeX[iSector];
	    ixPad += fModuleClusterMap[fModuleID]->SecxPad[iSector];
	  }
	else
	  {
	    ixPad += Int_t(tempPosX / Float_t(fModuleClusterMap[fModuleID]->PadSizeX[iSector]));
	    break;
	  }
      }
    return ixPad;
  }

  // --------------------------------------------------------------------
  Int_t CbmTrdDigitizerPRF::GetyPad(Double_t tempPosY)/*tempPosY has to be in LL module coordinate-systems in [mm]*/
  {
    //cout << "GetyPad" << endl;
    Int_t iyPad = 0;
    for (Int_t iSector = 0; iSector < fModuleClusterMap[fModuleID]->NoSectors; iSector++)
      {
	if (tempPosY > fModuleClusterMap[fModuleID]->SectorSizeY[iSector])
	  {
	    tempPosY -= fModuleClusterMap[fModuleID]->SectorSizeY[iSector];
	    iyPad += fModuleClusterMap[fModuleID]->SecyPad[iSector];
	  }
	else
	  {
	    iyPad += Int_t(tempPosY / Float_t(fModuleClusterMap[fModuleID]->PadSizeY[iSector]));
	    break;
	  }
      }
    return iyPad;
  }

  // ---- FinishTask-----------------------------------------------------
  void CbmTrdDigitizerPRF::FinishEvent()
  {

    for (fModuleClusterMapIt = fModuleClusterMap.begin();
	 fModuleClusterMapIt != fModuleClusterMap.end(); ++fModuleClusterMapIt) {
      fModuleClusterMapIt->second->SectorSizeX.clear();
      fModuleClusterMapIt->second->SectorSizeY.clear();
      fModuleClusterMapIt->second->PadSizeX.clear();
      fModuleClusterMapIt->second->PadSizeY.clear();
      fModuleClusterMapIt->second->SecxPad.clear();
      fModuleClusterMapIt->second->SecyPad.clear();
      for (Int_t x = 0; x < fModuleClusterMapIt->second->PadPlane.size(); x++) {
	for(Int_t y = 0; y < fModuleClusterMapIt->second->PadPlane[x].size(); y++) {
	  fModuleClusterMapIt->second->PadPlane[x][y]->MCIndex.clear();
	  delete fModuleClusterMapIt->second->PadPlane[x][y];
	}
	fModuleClusterMapIt->second->PadPlane[x].clear();
      }
      fModuleClusterMapIt->second->PadPlane.clear();

      delete fModuleClusterMapIt->second;
    }
    fModuleClusterMap.clear();

    fDigiCollection->Delete();
    fDigiMatchCollection->Delete();
    fTrdPoints->Clear();
    fMCStacks->Clear();
    /*
      fDigiCollection->Delete();
      fDigiMatchCollection->Delete();
      fTrdPoints->Delete();
      fMCStacks->Delete();
    */
  }

  // ---- Register ------------------------------------------------------
  void CbmTrdDigitizerPRF::Register()
  {
    FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
    FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
  }
  // --------------------------------------------------------------------

  ClassImp(CbmTrdDigitizerPRF)
