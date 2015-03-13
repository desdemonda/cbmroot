#include "CbmMuch.h"

#include "CbmMuchPoint.h"
#include "CbmGeoMuchPar.h"
#include "CbmGeoMuch.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "CbmStack.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "TObjArray.h"
#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include "TGeoBBox.h"
#include "TGeoVolume.h"
#include "TGeoManager.h"
#include "TGeoCompositeShape.h"
#include "TGeoTube.h"
#include "TGeoCone.h"
#include "FairGeoMedia.h"
#include "FairGeoMedium.h"
#include "TGeoBoolNode.h"

#include "CbmMuchAddress.h"
#include "CbmMuchStation.h"
#include "CbmMuchLayer.h"
#include "CbmMuchLayerSide.h"
#include "CbmMuchModule.h"
#include "CbmMuchGeoScheme.h"
#include "TGeoMatrix.h"
#include "CbmMuchModuleGemRadial.h"

#include <iostream>
#include <fstream>
#include <cassert>

#include "TGeoArb8.h"

using std::cout;
using std::endl;

ClassImp(CbmMuch)

  // -----   Default constructor   -------------------------------------------
CbmMuch::CbmMuch() 
: FairDetector(),
  fTrackID(-1),
  fVolumeID(-1),
  fPosIn(),
  fPosOut(),
  fMomIn(),
  fMomOut(),
  fTime(0.),
  fLength(0.),
  fELoss(0.),
  fPosIndex(0),
  fMuchCollection(new TClonesArray("CbmMuchPoint")),
  kGeoSaved(kFALSE),
  flGeoPar(new TList()),
  fPar(NULL)
{
  ResetParameters();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuch::CbmMuch(const char* name, Bool_t active)
  : FairDetector(name, active),
    fTrackID(-1),
    fVolumeID(-1),
    fPosIn(),
    fPosOut(),
    fMomIn(),
    fMomOut(),
    fTime(0.),
    fLength(0.),
    fELoss(0.),
    fPosIndex(0),
    fMuchCollection(new TClonesArray("CbmMuchPoint")),
    kGeoSaved(kFALSE),
    flGeoPar(new TList()),
    fPar(NULL)
{
  ResetParameters();
  flGeoPar->SetName( GetName());
  fVerboseLevel = 1;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuch::~CbmMuch() {

  if ( flGeoPar ) delete flGeoPar;
  if (fMuchCollection) {
    fMuchCollection->Delete();
    delete fMuchCollection;
  }
}
// -------------------------------------------------------------------------


// -----   Public method ProcessHits  --------------------------------------
Bool_t CbmMuch::ProcessHits(FairVolume* vol) {

  if ( gMC->IsTrackEntering() ) {
    fELoss  = 0.;
    fTime   = gMC->TrackTime() * 1.0e09;
    fLength = gMC->TrackLength();
    gMC->TrackPosition(fPosIn);
    gMC->TrackMomentum(fMomIn);
  }

  // Sum energy loss for all steps in the active volume
  fELoss += gMC->Edep();

  // Set additional parameters at exit of active volume. Create CbmMuchPoint.
  if ( gMC->IsTrackExiting()    ||
       gMC->IsTrackStop()       ||
       gMC->IsTrackDisappeared()   ) {
    fTrackID  = gMC->GetStack()->GetCurrentTrackNumber();
    fVolumeID = vol->getMCid();
    Int_t fDetectorId = GetDetId(vol);
    if (fVerboseLevel>2){
      printf(" TrackId: %i",fTrackID);
      printf(" System: %i" , CbmMuchAddress::GetSystemIndex(fDetectorId));
      printf(" Station: %i", CbmMuchAddress::GetStationIndex(fDetectorId));
      printf(" Layer: %i"  , CbmMuchAddress::GetLayerIndex(fDetectorId));
      printf(" Side: %i"   , CbmMuchAddress::GetLayerSideIndex(fDetectorId));
      printf(" Module: %i" , CbmMuchAddress::GetModuleIndex(fDetectorId));
      printf(" Vol %i \n",fVolumeID);
    }
    Int_t iStation = CbmMuchAddress::GetStationIndex(fDetectorId);
    gMC->TrackPosition(fPosOut);
    gMC->TrackMomentum(fMomOut);
    assert(iStation >=0 && iStation < fPar->GetNStations());
    CbmMuchStation* station= (CbmMuchStation*) fPar->GetStations()->At(iStation);
    if (fPosIn.Perp() >station->GetRmax()) {return kFALSE; }
    if (fPosOut.Perp()>station->GetRmax()) {return kFALSE; }
    if (fPosIn.Perp() <station->GetRmin()) {return kFALSE; }
    if (fPosOut.Perp()<station->GetRmin()) {return kFALSE; }

    if (fELoss == 0. ) return kFALSE;
    AddHit(fTrackID, fDetectorId,
      TVector3(fPosIn.X(),   fPosIn.Y(),   fPosIn.Z()),
      TVector3(fPosOut.X(),  fPosOut.Y(),  fPosOut.Z()),
      TVector3(fMomIn.Px(),  fMomIn.Py(),  fMomIn.Pz()),
      TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
      fTime, fLength, fELoss);

    //if (fPosOut.Z()>250) printf("%f\n",fPosOut.Z());

    // Increment number of MuchPoints for this track
    CbmStack* stack = (CbmStack*) gMC->GetStack();
    stack->AddPoint(kMUCH);

    ResetParameters();
  }
  return kTRUE;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuch::BeginEvent() {
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuch::EndOfEvent() {
  if (fVerboseLevel) Print();
  fMuchCollection->Delete();
  ResetParameters();
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
void CbmMuch::Register() {
  FairRootManager::Instance()->Register("MuchPoint", GetName(), fMuchCollection, kTRUE);
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
TClonesArray* CbmMuch::GetCollection(Int_t iColl) const {
  if (iColl == 0) return fMuchCollection;
  else return NULL;
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmMuch::Print() const {
  Int_t nHits = fMuchCollection->GetEntriesFast();
  cout << "-I- CbmMuch: " << nHits << " points registered in this event."
       << endl;
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmMuch::Reset() {
  fMuchCollection->Delete();
  ResetParameters();
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmMuch::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset) {
  Int_t nEntries = cl1->GetEntriesFast();
  cout << "-I- CbmMuch: " << nEntries << " entries to add." << endl;
  TClonesArray& clref = *cl2;
  CbmMuchPoint* oldpoint = NULL;
  for (Int_t i=0; i<nEntries; i++) {
    oldpoint = (CbmMuchPoint*) cl1->At(i);
    Int_t index = oldpoint->GetTrackID() + offset;
    oldpoint->SetTrackID(index);
    new (clref[fPosIndex]) CbmMuchPoint(*oldpoint);
    fPosIndex++;
  }
  cout << " -I- CbmMuch: " << cl2->GetEntriesFast() << " merged entries."
       << endl;
}
// -------------------------------------------------------------------------



// -----   Private method AddHit   --------------------------------------------
CbmMuchPoint* CbmMuch::AddHit(Int_t trackID, Int_t detID, TVector3 posIn,
			      TVector3 posOut, TVector3 momIn,
			      TVector3 momOut, Double_t time,
			      Double_t length, Double_t eLoss) {
  TClonesArray& clref = *fMuchCollection;
  Int_t size = clref.GetEntriesFast();
  if (fVerboseLevel>1)
    cout << "-I- CbmMuch: Adding Point at (" << posIn.X() << ", " << posIn.Y()
	 << ", " << posIn.Z() << ") cm,  detector " << detID << ", track "
	 << trackID << ", energy loss " << eLoss*1e06 << " keV" << endl;

  return new(clref[size]) CbmMuchPoint(trackID, detID, posIn, posOut,
				       momIn, momOut, time, length, eLoss);
}
// ----------------------------------------------------------------------------


#include "FairVolumeList.h"
// ----------------------------------------------------------------------------
void CbmMuch::ConstructGeometry() {
  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  geoFace->addGeoModule(new CbmGeoMuch());
  FairGeoMedia*     media    = geoFace->getMedia();
  FairGeoBuilder*   geobuild = geoLoad->getGeoBuilder();

  FairRun*        fRun = FairRun::Instance();
  if (!fRun) {Fatal("CreateGeometry","No FairRun found"); return;}
  FairRuntimeDb*  rtdb = FairRuntimeDb::instance();
  fPar  = (CbmGeoMuchPar*)(rtdb->getContainer("CbmGeoMuchPar"));
  TObjArray* stations = fPar->GetStations();

  // Create media
  TGeoMedium* mat = NULL;
  TGeoMedium* air          = CreateMedium("air");
  TGeoMedium* carbon       = CreateMedium("MUCHcarbon");
  TGeoMedium* iron         = CreateMedium("MUCHiron");
  TGeoMedium* wolfram      = CreateMedium("MUCHwolfram");
  TGeoMedium* lead         = CreateMedium("MUCHlead");
  TGeoMedium* argon        = CreateMedium("MUCHargon");
  TGeoMedium* supportMat   = CreateMedium("MUCHsupport");
  TGeoMedium* noryl        = CreateMedium("MUCHnoryl");
  TGeoMedium* polyethylene = CreateMedium("MUCHpolyethylene");

  Double_t* buf;

  CbmMuchGeoScheme* fGeoScheme = CbmMuchGeoScheme::Instance();
  fGeoScheme->Init(stations);
  fGeoScheme->ReadGeoFile(fgeoName);
  fGeoScheme->Print();
  fGeoScheme->CreateMuchCave();

  // Create much cave
  Double_t muchZ0 = fGeoScheme->GetMuchCaveZ0();
  TGeoCone* shMuch = fGeoScheme->GetMuchCave();
  TGeoVolume*  voMuch = new TGeoVolume("much",shMuch,air);
  gGeoManager->Node("much",0,"cave",0.,0.,muchZ0,0,kTRUE,buf,0);


  // Create absorbers
  TObjArray* absorbers = fGeoScheme->GetAbsorbers();
  for (Int_t iAbsorber=0;iAbsorber<absorbers->GetEntriesFast();iAbsorber++){
    switch (fGeoScheme->GetAbsorberMat(iAbsorber)) {
      case 'C': mat = carbon;       break;
      case 'I': mat = iron;         break;
      case 'L': mat = lead;         break;
      case 'W': mat = wolfram;      break;
      case 'P': mat = polyethylene; break;
      default :
        mat = iron;
        Warning("CreateGeometry","Absorber material not defined");
    }
    Double_t z0 = fGeoScheme->GetAbsorberZ0(iAbsorber)-muchZ0;
    TGeoCone* shAbs = (TGeoCone*) absorbers->At(iAbsorber);
    TString absorberName = Form("muchabsorber%02i",iAbsorber+1);
    TGeoVolume* voAbs = new TGeoVolume(absorberName,shAbs,mat);
    voAbs->SetLineColor(kRed);
    gGeoManager->Node(absorberName,0,"much",0.,0.,z0,0,kTRUE,buf,0);
  }

  // Create rotation matrices
  Int_t krotZ=0;
  Int_t krotY=0;
  gMC->Matrix(krotY,90,180,90,90,180,0);  // 180 degrees around y axis
  gMC->Matrix(krotZ,90,180,90,270,0,0);   // 180 degrees around z axis

  // Create GEM module shape which common for all module volumes
  Double_t activeLx = fGeoScheme->GetActiveLx();
  Double_t activeLy = fGeoScheme->GetActiveLy();
  Double_t activeLz = fGeoScheme->GetActiveLz();
  Double_t spacerLx = fGeoScheme->GetSpacerLx();
  Double_t spacerLy = fGeoScheme->GetSpacerLy();
  Double_t mDx  = (activeLx+2*spacerLx)/2.;
  Double_t mDy  = (activeLy+2*spacerLy)/2.;
  Double_t mDz  = (activeLz-0.001)/2.;
  TGeoBBox* shModuleFull = new TGeoBBox("shModule", mDx, mDy, mDz);
  TGeoBBox* shActiveFull = new TGeoBBox("shActive", activeLx/2, activeLy/2, activeLz/2);
  TGeoCompositeShape* shSpacerFull = new TGeoCompositeShape("shSpacer","shModule-shActive");

  // Create stations
  for (Int_t iStation=0;iStation<fGeoScheme->GetNStations();iStation++){
    const CbmMuchStation* station = fGeoScheme->GetStation(iStation);

    // Check if
    Int_t nLayers = station->GetNLayers();
    if (nLayers<=0) {
      Warning("CbmMuch","Station layers are not defined for station %02i",iStation);
      continue;
    }

    // Create station volume
    Double_t stRmin = station->GetTubeRmin();
    Double_t stRmax = station->GetTubeRmax();
    Double_t stDz   = station->GetTubeDz();
    TGeoTube* shStation = new TGeoTube(stRmin,shMuch->GetRmax2()-0.001,stDz);
    TString stationName = Form("muchstation%02i",iStation+1);
    TGeoVolume* voSt = new TGeoVolume(stationName,shStation,air);
    gGeoManager->Node(stationName,0,"much",0.,0.,station->GetZ()-muchZ0,0,kTRUE,buf,0);

    // Create support shape
    CbmMuchLayer* layerFirst = station->GetLayer(0);
    Double_t supportDx  = layerFirst->GetSupportDx();
    Double_t supportDy  = layerFirst->GetSupportDy();
    Double_t supportDz  = layerFirst->GetSupportDz();
    TString supportBoxName   = Form("shStation%02iSupportBox",iStation+1);
    TString supportHoleName  = Form("shStation%02iSupportHole",iStation+1);
    TString translationName  = Form("trSt%02i",iStation+1);
    TString supportShapeName = Form("shSt%02iSupport",iStation+1);
    TGeoTube* shSupportHole = new TGeoTube(supportHoleName,0.,stRmin,supportDz+0.001);
    TGeoBBox* shSupportBox  = new TGeoBBox(supportBoxName,supportDx/2.,supportDy,supportDz);
    TGeoTranslation* translation = new TGeoTranslation(translationName,-supportDx/2.,0.,0.);
    translation->RegisterYourself();
    TString expression = supportBoxName+"-"+supportHoleName+":"+translationName;
    TGeoCompositeShape* shSupport = new TGeoCompositeShape(supportShapeName,expression);

    // Create layers
    for (Int_t iLayer=0;iLayer<station->GetNLayers();iLayer++){
      CbmMuchLayer* layer = station->GetLayer(iLayer);
      Double_t layerZ  = layer->GetZtoStationCenter();
      Double_t layerDz = layer->GetDz();
      TGeoTube*   shLayer = new TGeoTube(stRmin,stRmax+50,layerDz);
      TString layerName   = Form("muchstation%02ilayer%i",iStation+1,iLayer+1);
      TGeoVolume* voLayer = new TGeoVolume(layerName,shLayer,air);
      gGeoManager->Node(layerName,0,stationName,0.,0.,layerZ,0,kTRUE,buf,0);

      // Create support
      TString  supportName1  = Form("muchstation%02ilayer%isupport1",iStation+1,iLayer+1);
      TString  supportName2  = Form("muchstation%02ilayer%isupport2",iStation+1,iLayer+1);
      TGeoVolume* voSupport1 = new TGeoVolume(supportName1,shSupport,supportMat);
      TGeoVolume* voSupport2 = new TGeoVolume(supportName2,shSupport,supportMat);
      voSupport1->SetLineColor(kYellow);
      voSupport2->SetLineColor(kYellow);
      if(TMath::Abs(supportDz) > 1e-5) { // Do not create support if it does not exist
        gGeoManager->Node(supportName1,0,layerName,+supportDx/2.,0.,0.,    0,kTRUE,buf,0);
        gGeoManager->Node(supportName2,0,layerName,-supportDx/2.,0.,0.,krotZ,kTRUE,buf,0);
      }

      // Create layer sides
      for (Int_t iSide=0;iSide<2;iSide++){
        const CbmMuchLayerSide* side = layer->GetSide(iSide);
        Char_t cside = (iSide==1) ? 'b' : 'f';

        // Create modules
        for (Int_t iModule=0;iModule<side->GetNModules();iModule++){
          CbmMuchModule* module = side->GetModule(iModule);
          TVector3 pos = module->GetPosition();
          TVector3 size = module->GetSize();
          TString moduleHoleName = Form("shStation%02iModuleHole", iStation);
          TGeoTube* shModuleHole = new TGeoTube(moduleHoleName, 0., stRmin, size[2] / 2. + 0.001);

          if (!station->IsModuleDesign()){ // simple design
            TString moduleBoxName = Form("shStation%02iModuleBox", iStation);
            TString moduleActiveName = Form("shStation%02iModuleActive", iStation);
            expression = moduleBoxName+"-"+moduleHoleName;
            TGeoBBox* shActiveBox = new TGeoBBox(moduleBoxName, size[0] / 2., size[1] / 2., size[2] / 2.);
            TGeoShape* shActive = new TGeoCompositeShape(moduleActiveName, expression);
            TString activeName = Form("muchstation%02ilayer%i%cactive%03i",iStation+1,iLayer+1,cside,iModule+1);
            TGeoVolume* voActive = new TGeoVolume(activeName,shActive,argon);
            gGeoManager->Node(activeName,0,layerName,pos[0],pos[1],pos[2]-layer->GetZ(),0,kTRUE,buf,0);
            AddSensitiveVolume(voActive);
            continue;
          }

          if (module->GetDetectorType()==2) {
            Fatal("CbmMuch","Station %02i - detailed module design not implemented for straws",iStation);
          }

          if (module->GetDetectorType()==3) {
            Double_t dx1 = ((CbmMuchModuleGemRadial*)module)->GetDx1(); 
            Double_t dx2 = ((CbmMuchModuleGemRadial*)module)->GetDx2(); 
            Double_t dy  = ((CbmMuchModuleGemRadial*)module)->GetDy();
            Double_t dz  = ((CbmMuchModuleGemRadial*)module)->GetDz();
            TGeoTrap* shape = new TGeoTrap(dz,0,0,dy,dx1,dx2,0,dy,dx1,dx2,0);
            shape->SetName(Form("shStation%02iLayer%i%cModule%03iActiveNoHole", iStation, iLayer, cside, iModule));

            translationName = Form("tr%02il%i%cm%03i",iStation+1,iLayer+1,cside,iModule+1);
            translation = new TGeoTranslation(translationName,0,-pos.Perp(),0.);
            translation->RegisterYourself();
            expression = Form("shStation%02iLayer%i%cModule%03iActiveNoHole-", iStation, iLayer, cside, iModule)+moduleHoleName+":"+translationName;
            TString moduleActiveName = Form("shStation%02iLayer%i%cModule%03iActive", iStation, iLayer, cside, iModule);
            TGeoCompositeShape* shActive = new TGeoCompositeShape(moduleActiveName,expression);
            Double_t tg = (dx2-dx1)/2/dy;
            Double_t dd1 = fGeoScheme->GetSpacerPhi()*tg;
            Double_t dd2 = fGeoScheme->GetSpacerPhi()*sqrt(1+tg*tg);
            Double_t sdx1 = dx1+dd2-dd1;
            Double_t sdx2 = dx2+dd2+dd1; 
            Double_t sdy  = dy+fGeoScheme->GetSpacerR();
            Double_t sdz  = dz-0.1;
            TGeoTrap* shapeSpacer = new TGeoTrap(sdz,0,0,sdy,sdx1,sdx2,0,sdy,sdx1,sdx2,0);
            shapeSpacer->SetName(Form("shStation%02iLayer%i%cModule%03iFullSpacerNoHole", iStation, iLayer, cside, iModule));
            TGeoCompositeShape* shSpacerFull1 = new TGeoCompositeShape(Form("shStation%02iLayer%i%cModule%03iSpacerNoHole", iStation, iLayer, cside, iModule),
                Form("shStation%02iLayer%i%cModule%03iFullSpacerNoHole-shStation%02iLayer%i%cModule%03iActiveNoHole", iStation, iLayer, cside, iModule, iStation, iLayer, cside, iModule));
            
            expression = Form("shStation%02iLayer%i%cModule%03iSpacerNoHole-", iStation, iLayer, cside, iModule)+moduleHoleName+":"+translationName;
            TGeoCompositeShape* shSpacer = new TGeoCompositeShape(Form("shStation%02iLayer%i%cModule%03iSpacer", iStation, iLayer, cside, iModule),expression);

            TString activeName = Form("muchstation%02ilayer%i%cactive%03i",iStation+1,iLayer+1,cside,iModule+1);
            TString spacerName = Form("muchstation%02ilayer%i%cspacer%03i",iStation+1,iLayer+1,cside,iModule+1);
            TGeoVolume* voActive = new TGeoVolume(activeName,shActive,argon);
            TGeoVolume* voSpacer = new TGeoVolume(spacerName,shSpacer,noryl);
            Double_t phi0 = 360./(layer->GetSide(0)->GetNModules()+layer->GetSide(1)->GetNModules());
            Double_t angle =  phi0*(2*iModule+iSide);
            Int_t krot;
            gMC->Matrix(krot,90,angle-90,90,angle,0,0);
            gGeoManager->Node(activeName,0,layerName,pos[0],pos[1],pos[2]-layer->GetZ(),krot,kTRUE,buf,0);
            gGeoManager->Node(spacerName,0,layerName,pos[0],pos[1],pos[2]-layer->GetZ(),krot,kTRUE,buf,0);
            AddSensitiveVolume(voActive);
            continue;
          }
          
          TGeoShape* shActive = shActiveFull;
          TGeoShape* shSpacer = shSpacerFull;
          Double_t cutRadius = module->GetCutRadius();
          if (cutRadius>0) { // Create composite shape with a hole
            translationName = Form("tr%02il%i%cm%03i",iStation+1,iLayer+1,cside,iModule+1);
            translation = new TGeoTranslation(translationName,-pos[0],-pos[1],0.);
            translation->RegisterYourself();
            expression = "shActive-"+moduleHoleName+":"+translationName;
            TString moduleActiveName = Form("shStation%02iLayer%i%cModule%03iActive", iStation, iLayer, cside, iModule);
            shActive = new TGeoCompositeShape(moduleActiveName,expression);
            expression = "shSpacer-"+moduleHoleName+":"+translationName;
            TString moduleSpacerName = Form("shStation%02iLayer%i%cModule%03iSpacer", iStation, iLayer, cside, iModule);
            shSpacer = new TGeoCompositeShape(moduleSpacerName,expression);
          }

          TString activeName = Form("muchstation%02ilayer%i%cactive%03i",iStation+1,iLayer+1,cside,iModule+1);
          TString spacerName = Form("muchstation%02ilayer%i%cspacer%03i",iStation+1,iLayer+1,cside,iModule+1);
          TGeoVolume* voActive = new TGeoVolume(activeName,shActive,argon);
          TGeoVolume* voSpacer = new TGeoVolume(spacerName,shSpacer,noryl);
          voActive->SetLineColor(kCyan);
          voSpacer->SetLineColor(kMagenta);
          gGeoManager->Node(activeName,0,layerName,pos[0],pos[1],pos[2]-layer->GetZ(),0,kTRUE,buf,0);
          gGeoManager->Node(spacerName,0,layerName,pos[0],pos[1],pos[2]-layer->GetZ(),0,kTRUE,buf,0);
          AddSensitiveVolume(voActive);
        } // modules
      } // sides
    } // layers
  } // stations

  TObjArray* fSensNodes = fPar->GetGeoSensitiveNodes();
  TObjArray* fPassNodes = fPar->GetGeoPassiveNodes();
  TGeoNode* ncave = gGeoManager->GetTopNode();
  TGeoNode* nmuch =(TGeoNode*) ncave->GetNodes()->FindObject("much_0");
  fPassNodes->Add(nmuch);
  //printf("much =%p\n",nmuch);
  TObjArray* nodes = nmuch->GetNodes();
  for (Int_t i=0;i<nodes->GetEntriesFast();i++){
    TGeoNode* node = (TGeoNode*) nodes->At(i);
    TString nodeName = node->GetName();
    //printf("nodeName=%s\n",nodeName.Data());
    if (nodeName.Contains("absorber")) fPassNodes->Add(node);
    if (nodeName.Contains("station")) {
      TObjArray* layers = node->GetNodes();
      for (Int_t l=0;l<layers->GetEntriesFast();l++){
        TGeoNode* layer = (TGeoNode*) layers->At(l);
        if (!TString(layer->GetName()).Contains("layer")) continue;
        TObjArray* layerNodes = layer->GetNodes();
        for (Int_t m=0;m<layerNodes->GetEntriesFast();m++){
          TGeoNode* layerNode = (TGeoNode*)layerNodes->At(m);
          TString layerNodeName = layerNode->GetName();
          if (layerNodeName.Contains("active"))  fSensNodes->Add(layerNode);

          if (layerNodeName.Contains("support")) fPassNodes->Add(layerNode);
        }
      }
    }
  }

  fPar->setChanged();
  fPar->setInputVersion(fRun->GetRunId(),1);
}
// -------------------------------------------------------------------------


Int_t CbmMuch::GetDetId(FairVolume* vol) {
  TString name = vol->GetName();
  Char_t cStationNr[3]= {name[11],name[12],' '};
  Int_t  iStation     = atoi(cStationNr)-1;
  Int_t  iLayer       = TString(name[18]).Atoi()-1;
  Int_t  iSide        = (name[19]=='b') ? 1 : 0;
  Char_t cModuleNr[4] = {name[26],name[27],name[28],' '};
  Int_t  iModule      = atoi(cModuleNr)-1;
  if(iSide!=1 && iSide !=0) printf("side = %i", iSide);
  Int_t detectorId = CbmMuchAddress::GetAddress(iStation,iLayer,iSide,iModule);
  assert(CbmMuchAddress::GetStationIndex(detectorId) == iStation);
  assert(CbmMuchAddress::GetLayerIndex(detectorId) == iLayer);
  assert(CbmMuchAddress::GetLayerSideIndex(detectorId) == iSide);
  assert(CbmMuchAddress::GetModuleIndex(detectorId) == iModule);
  assert(detectorId > 0);
  return detectorId;

}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
TGeoMedium* CbmMuch::CreateMedium(const char* matName){
  FairGeoLoader*    geoLoad   = FairGeoLoader::Instance();
  FairGeoBuilder*   geobuild  = geoLoad->getGeoBuilder();
  FairGeoInterface* geoFace   = geoLoad->getGeoInterface();
  FairGeoMedia*     media     = geoFace->getMedia();
  FairGeoMedium*    medium = media->getMedium(matName);
  if (!medium) gLogger->Fatal(MESSAGE_ORIGIN,"%s not defined in media file",matName);
  Int_t kMat = geobuild->createMedium(medium);
  return gGeoManager->GetMedium(kMat);
}
// -------------------------------------------------------------------------

