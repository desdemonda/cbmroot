{
  TFile* f = new TFile("data/test.mc.root");
  TTree *t=f->Get("cbmsim") ;

  TClonesArray *fT=new TClonesArray("TGeoTrack");
  t->SetBranchAddress("GeoTracks",&fT);

  TFile* p = new TFile("data/params.root");
  p->Get("FairBaseParSet");

// TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");
 TCanvas* c1 = new TCanvas("c1", "", 100, 100, 800, 800);
 c1->SetFillColor(10);

 gGeoManager->GetTopVolume()->Draw("same");
 TGeoTrack *tr;
 TGeoTrack *tr1;

 Int_t events=1;

 for (Int_t j=0; j< events; j++)	{
 	t->GetEntry(j);
	for (Int_t i=0; i<fT->GetEntriesFast(); i++)	{
    		tr=(TGeoTrack *)fT->At(i);
     		tr->Draw("same");
	}

 }

}

