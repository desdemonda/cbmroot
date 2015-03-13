{

  TFile* f = new TFile("data/test.mc.root");
  TTree *t=f->Get("cbmsim") ;
  TClonesArray *fT=new TClonesArray("TGeoTrack");

  t->SetBranchAddress("GeoTracks",&fT) ;

  TFile* p = new TFile("data/params.root");
  p->Get("FairBaseParSet");
   
  gGeoManager->DefaultColors(); // repair buggy coloring

 TCanvas* c1 = new TCanvas("glcanvas", "openGL", 100, 100, 800, 800);
 c1->SetFillColor(10);

 gGeoManager->GetTopVolume()->Draw();
 
 TGeoTrack *tr;
 Int_t color;
 TObjArray *TrList= gGeoManager->GetListOfTracks(); 
 gGeoManager->SetAnimateTracks();

 for (Int_t j=0; j< t->GetEntriesFast(); j++)	{
 	t->GetEntry(j);
        Double_t *point;
	for (Int_t i=0; i<fT->GetEntriesFast(); i++)	{
    		tr=(TGeoTrack *)fT->At(i);
		if(tr->GetPDG()==2212){
                  Int_t Np=tr->GetNpoints();
                  FairVTrack *pt = new FairVTrack(Np); 
                  pt->SetLineWidth(2);
                  pt->SetTrack(tr);
                  pt->SetParticle((TParticle *)tr->GetParticle());
                  for (Int_t n=0; n<Np; n++){
                     point=tr->GetPoint(n);
                     pt->SetPoint(n,point[0],point[1],point[2]);
		  }
                  pt->Draw();
		}
	}
  }

}

