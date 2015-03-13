// -------------------------------------------------------------------------
// -----                CbmPsdIdealDigitizer source file             -----
// -----                  Created 15/05/12  by     Alla & SELIM & FLORIAN               -----
// -------------------------------------------------------------------------
#include <iostream>

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "CbmPsdDigi.h"
#include "CbmPsdIdealDigitizer.h"
#include "CbmPsdPoint.h"
#include "TMath.h"

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdIdealDigitizer::CbmPsdIdealDigitizer() :
  FairTask("Ideal Psd Digitizer",1),
  fNDigis(0),
  fPointArray(NULL),
  fDigiArray(NULL)
{ 
  //  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsdIdealDigitizer::~CbmPsdIdealDigitizer() 
{
  if ( fDigiArray ) {
    fDigiArray->Delete();
    delete fDigiArray;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmPsdIdealDigitizer::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
  {
      LOG(FATAL) << "CbmPsdIdealDigitizer::Init: RootManager not instantised!" << FairLogger::endl;    //FLORIAN
      return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("PsdPoint");
  if ( ! fPointArray )
  {
      LOG(FATAL) << "CbmPsdIdealDigitizer::Init: No PSDPoint array!" << FairLogger::endl;             //FLORIAN
      return kERROR;
  }

  // Create and register output array
  fDigiArray = new TClonesArray("CbmPsdDigi", 1000);
  ioman->Register("PsdDigi", "PSD", fDigiArray, kTRUE);

  cout << "-I- CbmPsdIdealDigitizer: Intialisation successfull " << kSUCCESS<< endl;
  return kSUCCESS;

}


// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmPsdIdealDigitizer::Exec(Option_t* opt) {

  cout<<" CbmPsdIdealDigitizer::Exec begin "<<endl;
  // Reset output array
   if ( ! fDigiArray ) Fatal("Exec", "No PsdDigi array");
  Reset();  // SELIM: reset!!!
   
  // Declare some variables
  CbmPsdPoint* point = NULL;
  Int_t modID   = -1;        // module ID
  Int_t scinID = -1;        // #sciillator
  Double_t x, y, z;         // Position
 
 //const Int_t NB_PSD_MODS = 45;//marina: if we have mod #45 with small hole
  //const Int_t NB_PSD_MODS = 100;
  //const Int_t NB_PSD_SECT = 100;
  //const Int_t NB_PSD_MODS = 44;
  //const Int_t NB_PSD_SECT = 10;

  //Double_t edep[100][100];               //SELIM: 49 modules, including central & corner modules (rejected in analysis/flow/eventPlane.cxx)
  //memset(edep, 0, 10000*sizeof(Double_t));
  
  Double_t edep[NB_PSD_SECT][NB_PSD_MODS];               //SELIM: 49 modules, including central & corner modules (rejected in analysis/flow/eventPlane.cxx)
  memset(edep, 0,(NB_PSD_SECT*NB_PSD_MODS)*sizeof(Double_t));

  TVector3 pos;       // Position vector
  fNDigis=0;

  //for (Int_t imod=0; imod<100; imod++)                   //SELIM: 49 modules, including central & corner modules (rejected in analysis/flow/eventPlane.cxx)
  for (Int_t imod=0; imod<NB_PSD_MODS; imod++)//marina
  {
    for (Int_t isec=0; isec<NB_PSD_SECT; isec++)
      {
	  edep[isec][imod] = 0.;
      }
  }

  // Loop over ZdcPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  cout<<" nPoints "<<nPoints<<endl;

  //Int_t modID_min, modID_max;
  //modID_min = 100;
  //modID_max = 0;

  Int_t sec;

  for (Int_t iPoint=0; iPoint<nPoints; iPoint++)
  {
      point = (CbmPsdPoint*) fPointArray->At(iPoint);
      if ( ! point ) continue;

      // Detector ID
      //modID = point->GetModuleID();                   // !!!!! correction SELIM: scintID <-> modID !!!!!!
      //scinID = point->GetDetectorID();
      //edep[sec][modID] += point->GetEnergyLoss();

      modID = point->GetModuleID(); //marina  1-44 (45)
      scinID = point->GetDetectorID();//1-60 
  
      //sec = (Int_t)((scinID-1)/6);   //marina   0-9 
      sec = (Int_t)((scinID-1)/6)+1;   //marina   1-10 
      edep[sec-1][modID-1] += (Double_t) point->GetEnergyLoss();
      //cout <<"MARINA modID,scinID " <<modID <<" " <<scinID <<" " <<sec <<endl;
      
      //if ( sec > modID_max) modID_max = sec;
      //if ( sec < modID_min) modID_min = sec;
  }// Loop over MCPoints

  //cout << "modID in: " << modID_min << ", " << modID_max << endl;

  //for (Int_t imod=1; imod<50; imod++)              //SELIM: 49 modules, including central & corner modules (rejected in analysis/flow/eventPlane.cxx)
      for (Int_t imod=0; imod<NB_PSD_MODS; imod++)//marina
  {                  
      for (Int_t isec=0; isec<NB_PSD_SECT; isec++)
      {
	  //if (edep[isec][imod]<=0.) cout << "!!  edep  !! : " << edep[isec][imod] << endl;
	  if ( edep[isec][imod] <= 0. ) continue;
	  else {
	      new ((*fDigiArray)[fNDigis]) CbmPsdDigi(isec+1, imod+1, edep[isec][imod]);
	      fNDigis++;
	      //cout <<"MARINA CbmPsdIdealDigitizer " <<fNDigis <<" " <<isec+1 <<" " <<imod+1 <<" " <<edep[isec][imod] <<endl; 
	  }
      }   // section
  }//module

  // Event summary
  cout << "-I- CbmPsdIdealDigitizer: " <<fNDigis<< " CbmPsdDigi created." << endl;

}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmPsdIdealDigitizer::Reset() {
 fNDigis = 0;
 if ( fDigiArray ) fDigiArray->Delete();

}


ClassImp(CbmPsdIdealDigitizer)
