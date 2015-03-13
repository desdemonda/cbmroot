// -------------------------------------------------------------------------
// -----                        CbmTrdDigiDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "CbmTrdDigiDraw.h"

#include "CbmTrdAddress.h"           // for CbmTrdDetectorId
//#include "CbmTrdDetectorId.h"           // for CbmTrdDetectorId
#include "CbmTrdDigi.h"                 // for CbmTrdDigi
#include "CbmTrdDigiPar.h"              // for CbmTrdDigiPar
#include "CbmTrdModule.h"               // for CbmTrdModule

#include "FairEventManager.h"           // for FairEventManager
#include "FairRootManager.h"            // for FairRootManager
#include "FairRunAna.h"                 // for FairRunAna
#include "FairRuntimeDb.h"              // for FairRuntimeDb
#include "FairLogger.h"

#include "Riosfwd.h"                    // for ostream
#include "TClonesArray.h"               // for TClonesArray
#include "TEveBoxSet.h"                 // for TEveBoxSet, etc
#include "TEveManager.h"                // for TEveManager, gEve
#include "TVector3.h"                   // for TVector3

#include <stddef.h>                     // for NULL
#include <iomanip>                      // for setprecision, operator<<
#include <iosfwd>                       // for ostream
#include <iostream>                     // for cout
#include <ostream>                      // for basic_ostream, operator<<, etc
#include <string>                       // for char_traits

using std::cout;
using std::endl;
using std::setprecision;

// -----   Default constructor   -------------------------------------------
CbmTrdDigiDraw::CbmTrdDigiDraw() 
  : FairTask(),
    fVerbose(0),
    fPointList(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fEventManager(NULL),
    fq(NULL),
    fColor(),
    fStyle(),
    fActiveLayers()
{
}
// -------------------------------------------------------------------------

                                      

// -----   Standard constructor   ------------------------------------------
CbmTrdDigiDraw::CbmTrdDigiDraw(const char* name, Color_t color ,Style_t mstyle,Int_t iVerbose) 
  : FairTask(name, iVerbose),
    fVerbose(iVerbose),
    fPointList(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fEventManager(NULL),
    fq(NULL),
    fColor(color),
    fStyle(mstyle),
    fActiveLayers()
{
  for(Int_t i=0; i<15; i++){
    fActiveLayers[i]=kTRUE;
  } 
}
// ----  Initialisation  ----------------------------------------------
void CbmTrdDigiDraw::SetParContainers()
{
    cout<<" * CbmTrdDigiDraw * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// -------------------------------------------------------------------------
InitStatus CbmTrdDigiDraw::Init()
{
  if(fVerbose>1) cout<<  "CbmTrdDigiDraw::Init()" << endl;
   FairRootManager* fManager = FairRootManager::Instance();
   fPointList = (TClonesArray *)fManager->GetObject(GetName());
   if(fPointList==0){
      cout << "CbmTrdDigiDraw::Init()  branch " << GetName() << " Not found! Task will be deactivated "<< endl; 
      SetActive(kFALSE);
    }
   if(fVerbose>2)  cout<<  "CbmTrdDigiDraw::Init() get track list" <<  fPointList<< endl;
   fEventManager =FairEventManager::Instance();
   if(fVerbose>2) cout<<  "CbmTrdDigiDraw::Init() get instance of FairEventManager " << endl;
   fq =0;
      
  // gEve->AddElement(fq, fEventManager ); 
   return kSUCCESS;
}
// -------------------------------------------------------------------------  
void CbmTrdDigiDraw::Exec(Option_t* option)
{
  if (IsActive()){
    
    CbmTrdDigi *digi=0;
    
    Int_t npoints=fPointList->GetEntriesFast();
    Reset();

    TEveBoxSet *q = new TEveBoxSet(GetName(),"");
    q->Reset(TEveBoxSet::kBT_AABox, kTRUE, npoints);
    q->SetMainColor(kBlue);

    //   q->SetMainAlpha(1.);
    //   q->SetMarkerSize(1.5);
    //   q->SetMarkerStyle(fStyle);
    
    //   q->SetDefDepth();
    //   q->SetDefHeight();
    //   q->SetDefDepth();
    
   Int_t refCounter=0;

   for (Int_t i=0; i<npoints; ++i) {   
     
     digi =  (CbmTrdDigi*) fPointList->At(i);
     

     if(digi!=0) {
       
    
       Double_t ELoss = digi-> GetCharge();

       Int_t address = digi->GetAddress();
       Int_t Col = CbmTrdAddress::GetColumnId(address);
       Int_t Row = CbmTrdAddress::GetRowId(address);
       Int_t Sector = CbmTrdAddress::GetSectorId(address);
       Int_t moduleId = CbmTrdAddress::GetModuleAddress(address);
       Int_t layerId = CbmTrdAddress::GetLayerId(address);

       fModuleInfo = fDigiPar->GetModule(moduleId);

       TVector3 posHit, padSize;
       fModuleInfo->GetPosition(moduleId, Sector, Col, Row, posHit, padSize);

       // The given point is used as the edge of the box but it is
       // the middle point of the pad. So we have to do a transformation
     
      
       Double_t X = posHit.X()-(padSize.X()/2);
       Double_t Y = posHit.Y()-(padSize.Y()/2);


       LOG(DEBUG1)<<"*** CbmTrdHitProducerDigi::CalculateHitPosition ***"<<FairLogger::endl;
       LOG(DEBUG1)<<"Col: "<< Col <<FairLogger::endl;
       LOG(DEBUG1)<<"Row: "<< Row <<FairLogger::endl;
       LOG(DEBUG1)<<setprecision(5)<<"fPadX: "<< padSize.X() <<FairLogger::endl;
       LOG(DEBUG1)<<setprecision(5)<<"fPadY: "<< padSize.Y() <<FairLogger::endl;
       LOG(DEBUG1)<<setprecision(5)<<"fPosX: "<< posHit.X() <<FairLogger::endl;
       LOG(DEBUG1)<<setprecision(5)<<"fPosY: "<< posHit.Y() <<FairLogger::endl;
       LOG(DEBUG1)<<setprecision(5)<<"fPosZ: "<< posHit.Z() <<FairLogger::endl;
       
       if ( fActiveLayers[layerId]) {
	 q->AddBox(X,Y,posHit.Z(), 
                   padSize.X(), padSize.Y(), 0.);
	 refCounter++;
       }
     }
     
   }
   gEve->AddElement(q);
   gEve->Redraw3D(kFALSE); 
   fq=q;
   }
   
}
// -----   Destructor   ----------------------------------------------------
CbmTrdDigiDraw::~CbmTrdDigiDraw() 
{ 
}
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
  /** Action after each event**/
void CbmTrdDigiDraw::Finish()
{
}
// -------------------------------------------------------------------------
void CbmTrdDigiDraw::Reset()
{
     if(fq!=0){
	   fq->Reset();
       gEve->RemoveElement(fq, fEventManager );
	 }
}

void CbmTrdDigiDraw::SetActiveLayer(Bool_t Layer1, Bool_t Layer2,
				    Bool_t Layer3, Bool_t Layer4,
				    Bool_t Layer5, Bool_t Layer6,
				    Bool_t Layer7, Bool_t Layer8,
				    Bool_t Layer9, Bool_t Layer10,
				    Bool_t Layer11, Bool_t Layer12,
				    Bool_t Layer13, Bool_t Layer14,
				    Bool_t Layer15) 

{
  fActiveLayers[0]=Layer1;
  fActiveLayers[1]=Layer2;
  fActiveLayers[2]=Layer3;
  fActiveLayers[3]=Layer4;
  fActiveLayers[4]=Layer5;
  fActiveLayers[5]=Layer6;
  fActiveLayers[6]=Layer7;
  fActiveLayers[7]=Layer8;
  fActiveLayers[8]=Layer9;
  fActiveLayers[9]=Layer10;
  fActiveLayers[10]=Layer11;
  fActiveLayers[11]=Layer12;
  fActiveLayers[12]=Layer13;
  fActiveLayers[13]=Layer14;
  fActiveLayers[14]=Layer15;
}


ClassImp(CbmTrdDigiDraw)


