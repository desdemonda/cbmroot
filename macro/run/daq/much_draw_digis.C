Bool_t IsSelected(Int_t detId, Int_t iStation,Int_t iLayer,Int_t iSide);

void much_draw_digis(
  TString mcFileName   = "data/mc.root",
  TString digiFileName = "data/much_digi_sector.root",
  TString rcFileName   = "data/hits.root",
  Int_t iEvent = 0,
  Int_t iStationSelected = 0,
  Int_t iLayerSelected = 2,
  Int_t iSideSelected = 0, // 0 - front, 1 - back, 2 - both
  Double_t xmin = -250,
  Double_t xmax = 250,
  Double_t ymin = -250,
  Double_t ymax = 250
){
  TString paramDir = gSystem->Getenv("VMCWORKDIR");
  TString digiFileName = paramDir+"/parameters/much/much_v12c.digi.root";

  
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();

  // init digi scheme
  TFile* digiFile = new TFile(digiFileName);
  TObjArray* stations = (TObjArray*) digiFile->Get("stations");
  CbmMuchGeoScheme* digiScheme = CbmMuchGeoScheme::Instance();
  digiScheme->Init(stations);
  
  // get TClonesArrays from trees
  TFile* mcFile = new TFile(mcFileName);
  TFile* rcFile = new TFile(rcFileName);
  TTree* mcTree = mcFile ? (TTree*) mcFile->Get("cbmsim") : 0;
  TTree* rcTree = rcFile ? (TTree*) rcFile->Get("cbmsim") : 0;
  TClonesArray* points = new TClonesArray("CbmMuchPoint");
  TClonesArray* digis  = new TClonesArray("CbmMuchDigi");
  TClonesArray* hits   = new TClonesArray("CbmMuchPixelHit");
  if (mcTree) mcTree->SetBranchAddress("MuchPoint",&points);
  if (rcTree) rcTree->SetBranchAddress("MuchDigi",&digis);
  if (rcTree) rcTree->SetBranchAddress("MuchPixelHit",&hits);
  if (mcTree) mcTree->GetEntry(iEvent);
  if (rcTree) rcTree->GetEntry(iEvent);
  

  // Draw pads
  TCanvas* c1 = new TCanvas("station","station",1000*(xmax-xmin)/(ymax-ymin),1000);
  c1->Range(xmin,ymin,xmax,ymax);
  for (Int_t iSide=1;iSide>=0;iSide--){
    if (iSideSelected!=2 && iSide!=iSideSelected)continue;
    CbmMuchLayerSide* side = digiScheme->GetLayerSide(iStationSelected,iLayerSelected,iSide);
    for (Int_t iModule=0;iModule<side->GetNModules();++iModule) {
      CbmMuchModuleGem* module = (CbmMuchModuleGem*) side->GetModule(iModule);
      if (!module) continue;
      module->Draw();
      printf("%i\n",module->GetNPads());
    }
  }
  return;
  // Mark fired pads
  for (Int_t i=0;i<digis->GetEntriesFast();i++){
    CbmMuchDigi* digi = (CbmMuchDigi*) digis->At(i);
    // Filter out points
    Int_t detId = digi->GetDetectorId();
    if (!IsSelected(detId,iStationSelected,iLayerSelected,iSideSelected)) continue;
    CbmMuchModuleGem* module = (CbmMuchModuleGem*)digiScheme->GetModuleByDetId(detId);
    if (!module) continue;
    module->SetPadFired(digi->GetChannelId(),i,digi->GetADCCharge());
  }

  // Draw points
  for (Int_t i=0;i<points->GetEntriesFast();i++){
    CbmMuchPoint* point = (CbmMuchPoint*) points->At(i);
    Int_t detId = point->GetDetectorId();
    if (!IsSelected(detId,iStationSelected,iLayerSelected,iSideSelected)) continue;
    TLine* l = new TLine(point->GetXIn()-0.1,point->GetYIn()-0.1,point->GetXOut()+0.1,point->GetYOut()+0.1);
    l->SetLineColor(kBlack);
    l->SetLineWidth(3);
    l->Draw();
  }

  // Draw hits
  for (Int_t i=0;i<hits->GetEntriesFast();i++){
    CbmMuchPixelHit* hit = (CbmMuchPixelHit*) hits->At(i);
    Int_t detId = hit->GetDetectorId();
    if (!IsSelected(detId,iStationSelected,iLayerSelected,iSideSelected)) continue;
    TEllipse* p = new TEllipse(hit->GetX(),hit->GetY(),0.08);
    p->SetFillColor(kRed);
    p->SetLineColor(kRed);
    p->Draw();
  }

  
  // Draw a hole
  TArc* holeArc = new TArc(0.,0.,digiScheme->GetStation(iStationSelected)->GetRmin());
  holeArc->Draw();
  c1->Print("station.png");
}

Bool_t IsSelected(Int_t detId, Int_t iStationSelected,Int_t iLayerSelected,Int_t iSideSelected){
  Int_t iStation = CbmMuchGeoScheme::GetStationIndex(detId);
  Int_t iLayer   = CbmMuchGeoScheme::GetLayerIndex(detId);
  Int_t iSide    = CbmMuchGeoScheme::GetLayerSideIndex(detId);
  if (iStation!=iStationSelected) return 0;
  if (iLayer!=iLayerSelected) return 0;
  if (iSideSelected==2) return 1;
  if (iSide!=iSideSelected) return 0;
  return 1;
}
