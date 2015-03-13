#include "TColor.h"

#include "TH2.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TLine.h"
#include "TFile.h"
#include "TPaveText.h"
#include <iostream>






TFile *file = new TFile("CbmTrdHitRateTest.root","READ");

void hitrateDisplay(){
 
  TCanvas *layerSelection = new TCanvas("layerSelection","layerSelection",800,600);
  layerSelection->Divide(4,3);
  TString name;
  Int_t position = 0;
  for (Int_t s = 1; s < 4; s++){
    for (Int_t l = 1; l < 5; l++){
      position++;
      name.Form("S%i_L%i",s,l);
      TPaveText *text = new TPaveText(.1,.1,.9,.9);
      text->AddText(name.Data());
      layerSelection->cd(position)->SetName(name);
      //layerSelection->cd((s-1)*3+l)->SetFillColor((s-1)*3+l);
      //layerSelection->cd((s-1)*3+l)->Draw();
      text->Draw();
      layerSelection->cd((s-1)*3+l)->Update();
    }
  }

  layerSelection->AddExec("ex","LayerClicked()");
}

void LayerClicked() {
  int event = gPad->GetEvent();
  TObject *select = gPad->GetSelected();
  if (select->InheritsFrom("TPad")) 
    if ( TString(select->GetName()) != "c1_layerSelection"){
      TPad *layer = (TPad*)select;
      TString name;
      name.Form("c1_%s",TString(layer->GetName()).Data());
      //cout << name << endl;
      TCanvas *moduleSelection = (TCanvas*)file->Get(name);
      if (moduleSelection){
	moduleSelection->Draw();
	moduleSelection->AddExec("ex","ModuleClicked()");
      }
    }
}
void ModuleClicked() {
  int event = gPad->GetEvent();
  TObject *select = gPad->GetSelected();
  if (select->InheritsFrom("TBox")){
    TBox *module = (TBox*)select;
    TString name;
    name.Form("Module%iHitPad",module->GetUniqueID());
    cout << name << endl;
    TH2F *moduleHitRate = (TH2F*)file->Get(name);
    if (moduleHitRate){
      TCanvas *hitrate = new TCanvas("hitrate","hitrate",800,600);
      hitrate->cd()->SetLogx(1);
      hitrate->cd()->SetLogy(1);
      //moduleHitRate->GetxAxis()->SetRangeUser(1,1e5);
      //moduleHitRate->GetyAxis()->SetRangeUser(0.1,100);
      moduleHitRate->Draw();
    }
  }
 }
