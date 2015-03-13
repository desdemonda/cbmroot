#include "TPolyLine.h"
void much_draw_sectors(){
  TString digiFile = "data5/much_digi.root";

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
  muchlibs();

  TFile* f = new TFile(digiFile.Data());
  f->ls();
  TObjArray* stations = (TObjArray*) f->Get("stations");
  for (Int_t st=0;st<stations->GetEntriesFast();st++){
    if (st>0) continue;
    TCanvas* c1 = new TCanvas(Form("station%i",st+1),Form("station%i",st+1),800,800);
    c1->SetFillColor(0);
    c1->Range(-100,-100,100,100);//(-27,-2,0,25);
    CbmMuchStation* station = (CbmMuchStation*) stations->At(st);
    CbmMuchLayer* layer = station->GetLayer(0);
    for (Int_t s=1;s>=0;s--){
      CbmMuchLayerSide* side = layer->GetSide(s);
      for (Int_t m=0;m<side->GetNModules();m++) {
        CbmMuchModuleGem* module = (CbmMuchModuleGem*)side->GetModule(m);
        module->SetFillStyle(0);
        module->Draw();
        for (Int_t j=0;j<module->GetNSectors();j++){
          CbmMuchSector* sector = module->GetSector(j);
          // Reject incomplete sectors by size
          Double_t secLx = sector->GetSize()[0];
          Double_t secLy = sector->GetSize()[1];
          Double_t less = TMath::Min(secLx, secLy);
          Double_t more = TMath::Max(secLx, secLy);
          Int_t nReal = Int_t((more+1e-5)/less);
          Int_t nPow = Int_t(TMath::Log2(nReal) + 1e-2);
          Double_t more1 = less*TMath::Power(2,nPow);
          if(TMath::Abs(more1-more ) > 1e-5 || sector->GetNChannels() < module->GetNSectorChannels())
            sector->SetFillColor(kRed);
          else
            sector->SetFillColor(s ? kOrange : kYellow);
          sector->Draw("f");
          sector->Draw();
        } // sectors
      } // modules
    } // sides
    TArc* arc1 = new TArc(0.,0.,station->GetRmin());
    arc1->SetFillColor(10);
    arc1->Draw("f");
    TArc* arc2 = new TArc(0.,0.,station->GetRmax());
    arc2->SetFillStyle(0);
    arc2->Draw();


  }//stations
}
