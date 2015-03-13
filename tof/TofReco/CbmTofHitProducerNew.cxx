// --------------------------------------------------------------------------
// -----                 Class CbmTofHitProducerNew                       ------
// -----           Created  by E. Cordier 14/09/05                     ------
// -----           Modified by D. Gonzalez-Diaz 07/09/06               ------
// -----           Modified by D. Gonzalez-Diaz 02/02/07               ------
// -----           modified nh 24/10/2012
// --------------------------------------------------------------------------

#include "CbmTofHitProducerNew.h"

#include "CbmMCTrack.h"       
#include "CbmTofPoint.h"      // in cbmdata/tof
#include "CbmTofHit.h"        // in cbmdata/tof
#include "CbmTofGeoHandler.h" // in tof/TofTools
#include "CbmTofCell.h"       // in tof/TofData
#include "CbmTofDigiPar.h"    // in tof/TofParam

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TRandom.h"
#include "TString.h"
#include "TVector3.h"
#include "TSystem.h"
#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------

CbmTofHitProducerNew::CbmTofHitProducerNew()
  : FairTask("CbmTofHitProducerNew"),
    fVerbose(1),
    fParFileName(""),
    fTofPoints(NULL),
    fMCTracks(NULL),
    fHitCollection(NULL),
    X(),
    Dx(),
    Y(),
    Dy(),
    Z(),
    Ch(),
    ActSMtypMax(),
    ActnSMMax(),
    ActnModMax(),
    ActnCellMax(),
    tl(),
    tr(),
    trackID_left(),
    trackID_right(),
    point_left(),
    point_right(),
    fSigmaT(0.),
    fSigmaEl(0.),
    fSigmaXY(0.),
    fSigmaY(0.),
    fSigmaZ(0.),
    fVersion(""),
    fNHits(-1),
    fGeoHandler(new CbmTofGeoHandler()),
    fDigiPar(NULL),
    fCellInfo(NULL),
    fParInitFromAscii(kTRUE)
{
}


// ---- Constructor ----------------------------------------------------

CbmTofHitProducerNew::CbmTofHitProducerNew(const char *name, Int_t verbose)
  :FairTask(TString(name),verbose),
   fVerbose(1),
   fParFileName(""),
   fTofPoints(NULL),
   fMCTracks(NULL),
   fHitCollection(NULL),
   X(),
   Dx(),
   Y(),
   Dy(),
   Z(),
   Ch(),
   ActSMtypMax(),
   ActnSMMax(),
   ActnModMax(),
   ActnCellMax(),
   tl(),
   tr(),
   trackID_left(),
   trackID_right(),
   point_left(),
   point_right(),
   fSigmaT(0.),
   fSigmaEl(0.),
   fSigmaXY(0.),
   fSigmaY(0.),
   fSigmaZ(0.),
   fVersion(""),
   fNHits(-1),
   fGeoHandler(new CbmTofGeoHandler()),
   fDigiPar(NULL),
   fCellInfo(NULL),
   fParInitFromAscii(kTRUE)
{
  cout << "CbmTofHitProducerNew instantiated with verbose = "<<fVerbose<<endl;
}


// ---- Destructor ----------------------------------------------------

CbmTofHitProducerNew::~CbmTofHitProducerNew()
{
//	FairRootManager *fManager =FairRootManager::Instance();
//	fManager->Write();
  if (fGeoHandler) {
    delete fGeoHandler;
  }
}

// --------------------------------------------------
void CbmTofHitProducerNew::SetParContainers()
{
  if (!fParInitFromAscii) {
    LOG(INFO)<<" Get the digi parameters for tof"<<FairLogger::endl;

    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTofDigiPar*)
               (rtdb->getContainer("CbmTofDigiPar"));

  }
}

// --------------------------------------------------------------------
InitStatus CbmTofHitProducerNew::ReInit()
{
  if (!fParInitFromAscii) {
    LOG(INFO)<<"Reinitialize the digi parameters for tof"<<FairLogger::endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTofDigiPar*)
               (rtdb->getContainer("CbmTofDigiPar"));

  }
}

// ---- Init ----------------------------------------------------------

InitStatus CbmTofHitProducerNew::Init()
{

  cout << "nh - version of CbmTofHitProducerNew initializing with file " << fParFileName << endl;
  FairRootManager *fManager = FairRootManager::Instance();

    fTofPoints  = (TClonesArray *) fManager->GetObject("TofPoint");
    fMCTracks   = (TClonesArray *) fManager->GetObject("MCTrack");
    
    // Initialize the TOF GeoHandler
    Bool_t isSimulation=kFALSE;
    Int_t bla = fGeoHandler->Init(isSimulation);
    Int_t   nsmtyp=10, nsm=255, nmodules=10, ncells=255;
    Int_t iCh=0; // channel identifier 
  
    //    Initialize the matrixes [make this index visible in all the macro]. FIXME

    for(int t=0;t<nsmtyp;t++){
     for(int i=0;i<nsm;i++){
      for(int j=0;j<nmodules;j++){
	for(int k=0;k<ncells;k++){
	  X[t][i][j][k] = -1;
	  Y[t][i][j][k] = -1;
	  Z[t][i][j][k] = -1;
	  Dx[t][i][j][k]= -1;
	  Dy[t][i][j][k]= -1;
	  Ch[t][i][j][k]= -1;
	}
      }
     }
    }

// initialize accounting variables    
    ActSMtypMax=0;
    for (Int_t i=0; i<=maxSMtyp; i++){
	ActnSMMax[i]=0;
	ActnModMax[i]=0;
	ActnCellMax[i]=0;
    }

    if (fParInitFromAscii) {
      FILE *par;

    //Reading the parameter file. In the future this must be done in a different way.
 
      char    header='#';
      int     module, cell, smodule, smtype;
      Float_t X_tmp, Y_tmp, Z_tmp, Dx_tmp, Dy_tmp;

      TString tofGeoFile = gSystem->Getenv("VMCWORKDIR");
//      tofGeoFile += "/parameters/tof/tof_standard.geom.par";
      if (fParFileName==""){
	tofGeoFile += "/parameters/tof/par_tof.txt";
      }else{
	tofGeoFile += fParFileName;
      }
      cout << "<I> CbmTofHitProducerNew::Read parameters from "<< fParFileName 
           << "," <<tofGeoFile << endl;
      par=fopen(tofGeoFile,"r");
   
      if(par==NULL) {
      printf("\n ERROR WHILE OPENING THE PARAMETER FILE IN TOF HIT PRODUCER!");
      return kFATAL;
    }
     

    //Skip the header. In the future the header structure must be defined. FIXME
    while (fscanf(par,"%c",&header)>=0){
//      cout << "ReadH "<< header << endl;
      if((int)(header-'0')==0) break;
    }

    //Read the first line
//    fscanf(par,"%5d %5d %5d %2d %5.1f %5.1f %5.1f %2.1f %2.1f ", 
    fscanf(par,"%d %d %d %d %f %f %f %f %f ", 
    &smodule, &module, &cell, &smtype, &X_tmp, &Y_tmp, &Z_tmp, &Dx_tmp, &Dy_tmp);
//    cout << "Read " << smodule << " mod " << module << " cell "<< cell << " SMtyp " <<  smtype << " X,Y " << X_tmp <<","<< Y_tmp <<endl;
    if(smtype>ActSMtypMax)             ActSMtypMax=smtype;
    if(smodule>ActnSMMax[ActSMtypMax]) ActnSMMax[ActSMtypMax]=smodule;
    if(module>ActnModMax[ActSMtypMax]) ActnModMax[ActSMtypMax]=module;
    if(cell>ActnCellMax[ActSMtypMax])  ActnCellMax[ActSMtypMax]=cell;
    
    X[smtype][smodule][module][cell]    = X_tmp/10.;
    Y[smtype][smodule][module][cell]    = Y_tmp/10.;
    Z[smtype][smodule][module][cell]    = Z_tmp/10.;
    Dx[smtype][smodule][module][cell]   = Dx_tmp/10.;
    Dy[smtype][smodule][module][cell]   = Dy_tmp/10.;
    Ch[smtype][smodule][module][cell]   = iCh++;
    
    //Read all the lines
     
    while(fscanf(par,"%5d %5d %5d %2d %f %f %f %f %f", 
      &smodule, &module, &cell, &smtype, &X_tmp, &Y_tmp, &Z_tmp, &Dx_tmp, &Dy_tmp)>=0){ 
//      cout << "Read " << smodule << " mod " << module << " cell "<< cell << " SMtyp " <<  smtype << " X,Y " << X_tmp <<","<< Y_tmp <<endl;
  
      if(smtype>ActSMtypMax)             ActSMtypMax=smtype;
      if(smodule>ActnSMMax[ActSMtypMax]) ActnSMMax[ActSMtypMax]=smodule;
      if(module>ActnModMax[ActSMtypMax]) ActnModMax[ActSMtypMax]=module;
      if(cell>ActnCellMax[ActSMtypMax])  ActnCellMax[ActSMtypMax]=cell;

      X[smtype][smodule][module][cell]     = X_tmp/10.;  // units are cm 
      Y[smtype][smodule][module][cell]     = Y_tmp/10.;
      Z[smtype][smodule][module][cell]     = Z_tmp/10.;
      Dx[smtype][smodule][module][cell]    = Dx_tmp/10.;
      Dy[smtype][smodule][module][cell]    = Dy_tmp/10.;      
      Ch[smtype][smodule][module][cell]    = iCh++;
    }

    fclose(par);
    cout << "Filled position array with ActSMtypMax " << ActSMtypMax 
         << " and " << iCh << " active Channels "<< endl;
   } else {
     cout <<" InitParametersFromContainer "<<endl;
     InitParametersFromContainer();
   }

    cout << "-I- CbmTofHitProducerNew: found following setup: "<<endl; 
    Int_t nCh=0;
    for (Int_t i=0; i<=ActSMtypMax; i++){
	cout << " SMtype " << i <<" nsmod " << ActnSMMax[i]+1 
             << " nmod "<< ActnModMax[i]+1  << " ncell " << ActnCellMax[i] << endl;
	nCh += (ActnSMMax[i]+1) * (ActnModMax[i]+1) * ActnCellMax[i] * 2;
    }
    
    fHitCollection = new TClonesArray("CbmTofHit");
    fManager->Register("TofHit","Tof",fHitCollection, kTRUE);
     
    cout << "-I- CbmTofHitProducerNew: Initialization successful for " 
         << nCh <<" electronics channels"<< endl;

    return kSUCCESS;
}

void CbmTofHitProducerNew::InitParametersFromContainer()
{

   //    Initialize the matrixes [make this index visible in all the macro]. FIXME
    Int_t   nsmtyp=10, nsm=255, nmodules=10, ncells=255;

    for(int t=0;t<nsmtyp;t++){
     for(int i=0;i<nsm;i++){
      for(int j=0;j<nmodules;j++){
	for(int k=0;k<ncells;k++){
	  X[t][i][j][k] = -1;
	  Y[t][i][j][k] = -1;
	  Z[t][i][j][k] = -1;
	  Dx[t][i][j][k]= -1;
	  Dy[t][i][j][k]= -1;
	  Ch[t][i][j][k]= -1;
	}
      }
     }
    }

   Int_t nrOfCells = fDigiPar->GetNrOfModules();
   LOG(INFO)<<"Parameter container contain "<<nrOfCells<<" cells."<<FairLogger::endl;

   for (Int_t icell = 0; icell < nrOfCells; ++icell) {

     Int_t cellId = fDigiPar->GetCellId(icell); // cellId is assigned in CbmTofCreateDigiPar
     fCellInfo =fDigiPar->GetCell(cellId);

     Int_t smtype  = fGeoHandler->GetSMType(cellId);
     Int_t smodule = fGeoHandler->GetSModule(cellId);
     Int_t module  = fGeoHandler->GetCounter(cellId);
     Int_t cell    = fGeoHandler->GetCell(cellId);

     Double_t x = fCellInfo->GetX();
     Double_t y = fCellInfo->GetY();
     Double_t z = fCellInfo->GetZ();
     Double_t dx = fCellInfo->GetSizex();
     Double_t dy = fCellInfo->GetSizey();

     if(icell < 0){
       cout << "-I- InitPar "<<icell<<" Id: "<<cellId
	    << " "<< cell << " tmcs: "<< smtype <<" "<<smodule<<" "<<module<<" "<<cell   
            << " x="<<Form("%6.2f",x)<<" y="<<Form("%6.2f",y)<<" z="<<Form("%6.2f",z)
            <<" dx="<<dx<<" dy="<<dy<<endl;
     }
    if(smtype>ActSMtypMax)        ActSMtypMax=smtype;
    if(smodule>ActnSMMax[smtype]) ActnSMMax[smtype]=smodule;
    if(module>ActnModMax[smtype]) ActnModMax[smtype]=module;
    if(cell>ActnCellMax[smtype])  ActnCellMax[smtype]=cell;
 
     X[smtype][smodule][module][cell] = x;
     Y[smtype][smodule][module][cell] = y;
     Z[smtype][smodule][module][cell] = z;
     Dx[smtype][smodule][module][cell]= dx;
     Dy[smtype][smodule][module][cell]= dy;
     Ch[smtype][smodule][module][cell]= icell;
   }
}

// ---- Exec ----------------------------------------------------------

void CbmTofHitProducerNew::Exec(Option_t * option)
{
  fHitCollection->Clear();
  fNHits = -1;              //Must start in -1

  CbmTofPoint *pt;
  CbmMCTrack  *mc;
  
  Int_t nTofPoint = fTofPoints->GetEntries();  
  Int_t nMCTracks = fMCTracks ->GetEntries();
  Int_t tof_tracks = 0, tof_tracks_vert = 0, tof_tracks_local = 0;

  cout << "-I- CbmTofHitProducerNew(Exec): " << nTofPoint
       << " points in Tof for this event with " << nMCTracks
       << " MC tracks "<< endl;

  //Some numbers on TOF distributions

  for(Int_t p=0;p<nMCTracks;p++) {
    mc = (CbmMCTrack*) fMCTracks->At(p);
    if(mc->GetNPoints(kTOF)>0) tof_tracks++;
    if(mc->GetNPoints(kTOF)>0 && mc->GetMotherId()==-1) tof_tracks_vert++;
  }
  
  cout << "-I- CbmTofHitProducerNew : " << tof_tracks << " tracks in Tof " << endl;
  cout << "-I- CbmTofHitProducerNew : " << tof_tracks_vert << " tracks in Tof from vertex" << endl;
//  cout << "-I- CbmTofHitProducerNew : " << tof_tracks-tof_tracks_local
//       << " tracks in Tof able to produce a hit" << endl;

  TVector3 pos;

  Double_t xHit, yHit, zHit, tHit, xHitErr, yHitErr, zHitErr;
  Double_t tl_new, tr_new;
  Double_t Dz=2.04;  //FIXME: Introduce also Dz and Z as (constant) parameters 
  Double_t sigma_T=0.098, sigma_Y=0.7, sigma_t_gap, t_o, T_smearing = 0, sigma_el=0.04, 
    vprop = 15., Pgap = 0.75;
  //time[ns], position[cm], velocity[cm/ns]
  //FIXME: these parameters must be provided externally

  Int_t trackID, smtype, smodule, module, cell, gap, flag, ref;
  
  if  (fSigmaT>0.) sigma_T=fSigmaT;   // single gap resolution 
  else fSigmaT=sigma_T;               // take default 

  if  (fSigmaEl>0.) sigma_el=fSigmaEl;   // electronics channel resolution 
  else fSigmaEl=sigma_el;                // take default

  //Here check for the validity of the parameters
  if(fSigmaY>1) cout<<"UNREALISTIC TOF POSITION RESOLUTION!! (HitProducer may crash)"<<endl;
  if((fSigmaT<0.01 && fSigmaT>0)||fSigmaT>0.2) cout<<"UNREALISTIC TOF RESOLUTION!! (HitProducer may crash)"<<endl;

  //Parameterizations. Now they depend on the geometry/algorithm. FIXME

//  if(fSigmaY!=0)    sigma_el = sqrt(2.)*fSigmaY/vprop*1.3;
//  else              sigma_el = sqrt(2.)*sigma_Y/vprop*1.3;

//  if(fSigmaT!=0)  {  
//    sigma_t_gap = sqrt(pow(fSigmaT/(0.5*pow(ngaps,-0.361) + 0.40),2)-1.4/1.3*pow(sigma_el,2));
//    t_o = 1.93*fSigmaT;
//  }
//  else {
//    sigma_t_gap = sqrt(pow(sigma_T/(0.5*pow(ngaps,-0.361) + 0.40),2)-1.4/1.3*pow(sigma_el,2));
//    t_o = 1.93*sigma_T;
//  }

  //Initialization of cell times

  for(Int_t t=0;t<=ActSMtypMax;t++){
   for(Int_t i=0;i<ActnSMMax[t]+1;i++){
    for(Int_t j=0;j<ActnModMax[t]+1;j++){
     for(Int_t k=0;k<ActnCellMax[t];k++){
      tl[t][i][j][k]= 1e+5;
      tr[t][i][j][k]= 1e+5;
     }
    }
   }
  }

// Loop over the TOF points
//  nTofPoint=1; //debugging 
  
  for (Int_t j=0; j < nTofPoint; j++ ) {
    // Probability that the avalanche is detected
    if(gRandom->Uniform(1)>Pgap)     continue;
    // Get a pointer to the TOF point      
    pt = (CbmTofPoint*) fTofPoints->At(j);
    if(pt == NULL) {
      cout<<"Be careful: hole in the CbmTofPoint TClonesArray!"<<endl;
      continue;
    }
    //Reject particles produced in the last 4 cm. Better job must be done here. For example:
    //it could better to go up to the parent particle and get its trackID, then the
    //secondary is processed. FIXME.
    //if(pt->GetTrackID() < 0) continue;
    mc = (CbmMCTrack*) fMCTracks-> At(pt->GetTrackID()); // pointer to MC - info
    //if((mc->GetStartZ())>996) continue;
    //Get relevant information from the point

    trackID = pt->GetTrackID();
    Int_t detID = pt->GetDetectorID();

    cell    = fGeoHandler->GetCell(detID);
    module  = fGeoHandler->GetCounter(detID);
    gap     = fGeoHandler->GetGap(detID);
    smodule = fGeoHandler->GetSModule(detID);
    smtype  = fGeoHandler->GetSMType(detID);
    Int_t cellID = fGeoHandler->GetCellId(detID);
   //    fCellInfo =fDigiPar->GetCell(cellID);

    if( //0){
          smtype<0.  || smtype>ActSMtypMax 
       || smodule<0. || smodule>ActnSMMax[smtype]
       || module<0.  || module>ActnModMax[smtype]
       || cell<0.    || cell>ActnCellMax[smtype]
       || Dx[smtype][smodule][module][cell]<0.
       )
    {
     LOG(INFO)<<"-E- TofHitProducerNew: detId: "<< detID <<" SMType: "<<smtype;
     LOG(INFO)<<" SModule: "<<smodule<<" of "<<ActnSMMax[smtype]+1;
     LOG(INFO)<<" Module: "<<module<<" of "<<ActnModMax[smtype]+1;
     LOG(INFO)<<" Gap: "<<gap;
     LOG(INFO)<<" Cell: "<<cell<<" of "<<ActnCellMax[smtype] <<FairLogger::endl;
     continue;
    }
    pt->Position(pos);

    if(fVerbose >2) {
      pt->Print(""); //FIXME
      cout << endl;
    }

//    T_smearing      = gRandom->Gaus(t_o, sigma_t_gap);
    T_smearing      = gRandom->Gaus(1.21*sigma_T, sigma_T);

    Float_t X_local = pos.X()-X[smtype][smodule][module][cell];
    Float_t Y_local = pos.Y()-Y[smtype][smodule][module][cell];
    
    tl_new = pt->GetTime() + T_smearing - Y_local/vprop
           + gRandom->Gaus(0,sigma_el);
    tr_new = pt->GetTime() + T_smearing + Y_local/vprop
           + gRandom->Gaus(0,sigma_el);

    if(fVerbose >1 || TMath::Abs(X_local)>1.5) {
      cout << "-W- TofHitProNew " << j <<". Poi," 
         << " TID:" << trackID 
	 << " detID: " << detID
	 << " cellID: " << cellID
         << " SMtype: " << smtype 
         << " SM: " << smodule 
         << " Mod: " << module 
         << " Str: " << cell 
         << " G: " << gap 
	   << " posX " << Form("%6.2f",pos.X())<<","<< Form("%6.2f",X[smtype][smodule][module][cell])
	   << " posY " << Form("%6.2f",pos.Y())<<","<< Form("%6.2f",Y[smtype][smodule][module][cell])
	   << " tl " << Form("%6.2f",tl_new)   << " tr " << Form("%6.2f",tr_new)
         << endl; 

      if(TMath::Abs(X_local)>1.5) {
        continue; //prevent crashes 
      }
    }
    
    //Take the fastest time from all the points/gaps in this cell
    
    if(tl_new<tl[smtype][smodule][module][cell]) {
      tl[smtype][smodule][module][cell]             = tl_new;
      trackID_left[smtype][smodule][module][cell]   = trackID;
      point_left[smtype][smodule][module][cell]     = j;
    }

    if(tr_new<tr[smtype][smodule][module][cell]) {
      tr[smtype][smodule][module][cell]             = tr_new;
      trackID_right[smtype][smodule][module][cell]  = trackID;
      point_right[smtype][smodule][module][cell]    = j;
    }
    //X and Y depend on the orientation of the cell. FIXME
  } // end of loop on TOF points 

// Extract TOF Hits
  
  Int_t nFl1=0;
  Int_t nFl2=0;
  Int_t ii=0;

  //  fVerbose=3;  // debugging 

  for(Int_t t=0;t<=ActSMtypMax;t++){
   for(Int_t i=0;i<ActnSMMax[t]+1;i++){
    for(Int_t j=0;j<ActnModMax[t]+1;j++){
     for(Int_t k=0;k<ActnCellMax[t];k++){
       //      cout <<"-D- HitProd: tijk "<<t<<" "<<i<<" "<<j<<" "<<k<<" "<<tl[t][i][j][k]<<" "<<tr[t][i][j][k]<<endl;
//Increase the counter for the TofHit TClonesArray if the first time a hit is attached to this cell
    
      if( tl[t][i][j][k]<1e+5 
       && tr[t][i][j][k]<1e+5) {
       fNHits++;

       xHit    = X[t][i][j][k]; 
       xHitErr = Dx[t][i][j][k]/sqrt(12.);
       zHit    = Z[t][i][j][k];
       zHitErr = Dz/sqrt(12.);
       yHit    = (tr[t][i][j][k] - tl[t][i][j][k])*vprop*0.5
               + Y[t][i][j][k];
       
    //Reference to the point that contributes to the left side.
       yHitErr = sigma_el*vprop;
       tHit    = 0.5*(tl[t][i][j][k] + tr[t][i][j][k]);
       ref     = point_left[t][i][j][k];
       pt      = (CbmTofPoint*) fTofPoints->At(ref);
       if(trackID_left[t][i][j][k]==trackID_right[t][i][j][k]){
	   flag = 1; nFl1++;
           // Check consistency
           if(fVerbose >2) {
           pt->Position(pos);
           cout << " pos check for point "<<ref<<" x:  "<< xHit << " " << pos.X() 
                << " y: " << yHit << " " << pos.Y() << endl;   
	   }
       }
       else {
	   flag = 2; nFl2++;
       }
       
       TVector3 hitPos(xHit, yHit, zHit);
       TVector3 hitPosErr(xHitErr, yHitErr, zHitErr);
       Int_t iCh = Ch[t][i][j][k];
       if(fVerbose >1) {
	 //       if(1) {
	 cout << ii++ << " Add hit smt " << t << " sm " << i << " mod " << j << " str " << k
	 <<" Ch " << iCh
         <<" tl " << tl[t][i][j][k] << " tr " << tr[t][i][j][k] 
	 <<" xh " << xHit << " yh " << yHit << " fl "<< flag << " refPoi " << ref 
	 <<" TID "<<  trackID_left[t][i][j][k] <<","<<trackID_right[t][i][j][k]<< endl;
       }
       
       AddHit(pt->GetDetectorID(), hitPos, hitPosErr, ref, tHit, flag, iCh);

      }
     }
    }
   }
  }  
  cout << "-I- CbmTofHitProducerNew : " << fNHits+1
       << " hits in Tof created, "<< nFl1<< " single, "<< nFl2<< " multiple hits " << endl;

}
// ---- Add Hit to HitCollection --------------------------------------

void CbmTofHitProducerNew::AddHit(Int_t detID, TVector3 &posHit, TVector3 &posHitErr,
			       Int_t ref, Double_t tHit, Int_t flag, Int_t iChannel)
{
//  new((*fHitCollection)[fNHits]) CbmTofHit(detID, posHit, posHitErr, ref, tHit, flag, iChannel);
  new((*fHitCollection)[fNHits]) CbmTofHit(detID, posHit, posHitErr, ref, tHit, flag);

  if(fVerbose > 1) {
    CbmTofHit* tofHit = (CbmTofHit*) fHitCollection->At(fNHits);
    tofHit->Print();
    cout << endl;
  }
}


// ---- Finish --------------------------------------------------------

void CbmTofHitProducerNew::Finish()
{
}


// ---- SetSigmaT -----------------------------------------------------

void CbmTofHitProducerNew::SetSigmaT(Double_t sigma)
{
    fSigmaT = sigma;
}


// ---- SetSigmaEl -----------------------------------------------------

void CbmTofHitProducerNew::SetSigmaEl(Double_t sigma)
{
    fSigmaEl = sigma;
}

// ---- SetSigmaXY -----------------------------------------------------

void CbmTofHitProducerNew::SetSigmaXY(Double_t sigma)
{
    fSigmaXY = sigma;
}

// ---- SetSigmaY -----------------------------------------------------

void CbmTofHitProducerNew::SetSigmaY(Double_t sigma)
{
    fSigmaY = sigma;
}


// ---- SetSigmaZ -----------------------------------------------------

void CbmTofHitProducerNew::SetSigmaZ(Double_t sigma)
{
    fSigmaZ = sigma;
}


// ---- GetSigmaT -----------------------------------------------------

Double_t CbmTofHitProducerNew::GetSigmaT()
{  
    return  fSigmaT;
}


// ---- GetSigmaEl -----------------------------------------------------

Double_t CbmTofHitProducerNew::GetSigmaEl()
{  
    return  fSigmaEl;
}

// ---- GetSigmaXY -----------------------------------------------------

Double_t CbmTofHitProducerNew::GetSigmaXY()
{  
    return  fSigmaXY;
}

// ---- GetSigmaY -----------------------------------------------------

Double_t CbmTofHitProducerNew::GetSigmaY()
{  
    return  fSigmaY;
}

// ---- GetSigmaZ -----------------------------------------------------

Double_t CbmTofHitProducerNew::GetSigmaZ()
{  
    return  fSigmaZ;
}


ClassImp(CbmTofHitProducerNew)


