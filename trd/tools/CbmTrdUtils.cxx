/**
 * \file CbmTrdUtils.cxx
 * \author Cyrano Bergmann
 * \date 18/11/13
 **/

#include "CbmTrdUtils.h"
#include "CbmTrdAddress.h"
#include "TMath.h"
#include "TCanvas.h"
CbmTrdUtils::CbmTrdUtils() : fColors(), fZLevel() {}
CbmTrdUtils::~CbmTrdUtils(){}

void CbmTrdUtils::InitColorVector(Bool_t logScale, Double_t min, Double_t max){
  fColors.clear();
  fZLevel.clear();
  for (Int_t i = 0; i < TColor::GetNumberOfColors(); i++){
    fColors.push_back(TColor::GetColorPalette(i));
    if (logScale)
      fZLevel.push_back(min + TMath::Power(10, TMath::Log10(max) / TColor::GetNumberOfColors() * i));// log scale
    else
      fZLevel.push_back(min + (max / TColor::GetNumberOfColors() * i)); // lin scale
  }
}
Int_t CbmTrdUtils::GetColorCode(Double_t value){
  Int_t j = 0;
  while ((value > fZLevel[j]) && (j < (Int_t)fZLevel.size())){
    //printf ("              %i<%i %i    %E <= %E\n",j,(Int_t)fZLevel.size(),fColors[j], rate, fZLevel[j]);
    j++;
  }
  if (j >= (Int_t)fZLevel.size())
    return 2;
  else
    return fColors[j];
}

TPolyLine *CbmTrdUtils::CreateTriangularPad(Int_t column, Int_t row, Double_t value, Double_t min_range, Double_t max_range, Bool_t logScale){
  InitColorVector(logScale, min_range, max_range);
  const Int_t nCoordinates = 4;
  Double_t x[nCoordinates];
  Double_t y[nCoordinates];
  if (row%2 != 0){
    x[0] = column-0.5;
    y[0] = row-1.5;
    x[1] = column+0.5;
    y[1] = row+0.5;
    x[2] = column-0.5;
    y[2] = row+0.5;
    x[3] = column-0.5;
    y[3] = row-1.5;
  } else {
    x[0] = column-0.5;
    y[0] = row-0.5;
    x[1] = column+0.5;
    y[1] = row-0.5;
    x[2] = column+0.5;
    y[2] = row+1.5;
    x[3] = column-0.5;
    y[3] = row-0.5;
  }
  TPolyLine *pad = new TPolyLine(nCoordinates,x,y);
  pad->SetLineColor(1);
  pad->SetFillColor(GetColorCode(value));

  return pad;
}
TPolyLine *CbmTrdUtils::CreateRectangularPad(Int_t column, Int_t row, Double_t value, Double_t min_range, Double_t max_range, Bool_t logScale){
  InitColorVector(logScale, min_range, max_range);
  const Int_t nCoordinates = 5;
  Double_t x[nCoordinates];
  Double_t y[nCoordinates];
  x[0] = column-0.5;
  y[0] = row-0.5;
  x[1] = column-0.5;
  y[1] = row+0.5;
  x[2] = column+0.5;
  y[2] = row+0.5;
  x[3] = column+0.5;
  y[3] = row-0.5;
  x[4] = column-0.5;
  y[4] = row-0.5;
  TPolyLine *pad = new TPolyLine(nCoordinates,x,y);
  pad->SetLineColor(1);
  pad->SetFillColor(GetColorCode(value));

  return pad;
}



   void CbmTrdUtils::NiceTH1(TH1 *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle) 
  {
    h->SetStats(kFALSE);
    h->SetMarkerStyle(mStyle);
    h->SetMarkerSize(mSize);  
    h->SetMarkerColor(color);
    h->SetLineColor(color);  
    h->GetXaxis()->SetLabelSize(0.03);
    h->GetYaxis()->SetLabelSize(0.03);
    //h->GetZaxis()->SetLabelSize(0.03);
    h->GetXaxis()->SetTitleSize(0.035);
    h->GetXaxis()->SetTitleOffset(1.25);
    h->GetYaxis()->SetTitleSize(0.035);
    h->GetYaxis()->SetTitleOffset(1.25);
    //h->GetZaxis()->SetTitleSize(0.035);
    //h->GetZaxis()->SetTitleOffset(-2);
    h->SetXTitle(xTitle);
    h->SetYTitle(yTitle);
  }

   void CbmTrdUtils::NiceTH2(TH2 *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle, TString zTitle) 
  {
  h->SetStats(kFALSE);
    h->SetMarkerStyle(mStyle);
    h->SetMarkerSize(mSize);  
    h->SetMarkerColor(color);
    h->SetLineColor(color);  
    h->GetXaxis()->SetLabelSize(0.03);
    h->GetYaxis()->SetLabelSize(0.03);
    h->GetZaxis()->SetLabelSize(0.03);
    h->GetXaxis()->SetTitleSize(0.035);
    h->GetXaxis()->SetTitleOffset(1.5);
    h->GetYaxis()->SetTitleSize(0.035);
    h->GetYaxis()->SetTitleOffset(1.5);
    h->GetZaxis()->SetTitleSize(0.035);
    h->GetZaxis()->SetTitleOffset(1.25);
    h->SetXTitle(xTitle);
    h->SetYTitle(yTitle);
    h->SetZTitle(zTitle);
  }

   void CbmTrdUtils::NiceTH3(TH3 *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle, TString zTitle) 
  {
  h->SetStats(kFALSE);
    h->SetMarkerStyle(mStyle);
    h->SetMarkerSize(mSize);  
    h->SetMarkerColor(color);
    h->SetLineColor(color);  
    h->GetXaxis()->SetLabelSize(0.03);
    h->GetYaxis()->SetLabelSize(0.03);
    h->GetZaxis()->SetLabelSize(0.03);
    h->GetXaxis()->SetTitleSize(0.035);
    h->GetXaxis()->SetTitleOffset(1.25);
    h->GetYaxis()->SetTitleSize(0.035);
    h->GetYaxis()->SetTitleOffset(1.25);
    h->GetZaxis()->SetTitleSize(0.035);
    h->GetZaxis()->SetTitleOffset(1.25);
    h->SetXTitle(xTitle);
    h->SetYTitle(yTitle);
    h->SetZTitle(zTitle);
  }

void CbmTrdUtils::NiceTProfile(TProfile *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle) 
{
  h->SetStats(kFALSE);
  h->SetMarkerStyle(mStyle);
  h->SetMarkerSize(mSize);  
  h->SetMarkerColor(color);
  h->SetLineColor(color);  
  h->GetXaxis()->SetLabelSize(0.03);
  h->GetYaxis()->SetLabelSize(0.03);
  //h->GetZaxis()->SetLabelSize(0.03);
  h->GetXaxis()->SetTitleSize(0.035);
  h->GetXaxis()->SetTitleOffset(1.25);
  h->GetYaxis()->SetTitleSize(0.035);
  h->GetYaxis()->SetTitleOffset(1.25);
  //h->GetZaxis()->SetTitleSize(0.035);
  //h->GetZaxis()->SetTitleOffset(-2);
  h->SetXTitle(xTitle);
  h->SetYTitle(yTitle);
}

   void CbmTrdUtils::NiceLegend(TLegend *l)
  {
    l->SetLineColor(0);
    l->SetLineStyle(0);
    l->SetFillStyle(0);
    l->SetTextSize(0.03);
  }

void CbmTrdUtils::Statusbar(Int_t i, Int_t n) {
  if (int(i * 100 / float(n)) - int((i-1) * 100 / float(n)) >= 1) {
    if (int(i * 100 / float(n)) == 1 || i == 1 || i == 0) 
      cout << "[" << flush;
    cout << "-" << flush;
    if (int(i * 10 / float(n)) - int((i-1) * 10 / float(n)) >= 1) 
      cout << "|";
    if (int(i * 100 / float(n)) >=99) 
      cout << "]" <<endl;
  }
}

Int_t CbmTrdUtils::GetModuleType(Int_t moduleAddress, CbmTrdModule *fModuleInfo, CbmTrdDigiPar *fDigiPar){
  Int_t type = -1;
  fModuleInfo = fDigiPar->GetModule(moduleAddress);
  Int_t nRows = fModuleInfo->GetNofRows();
  Int_t nCols = fModuleInfo->GetNofColumns();
 
  if (nCols == 80){
    if (nRows == 36)
      type = 1;
    else if (nRows == 20)
      type = 2;
    else if (nRows == 12)
      type = 3;
    else if (nRows == 8)
      type = 4;
  } else if (nCols == 128) {
    if (nRows == 24)
      type = 5;
    else if (nRows == 16)  // from v14d
      type = 6;
    else if (nRows == 12)  // till v14c
      type = 6;
    else if (nRows == 8)
      type = 7;
    else if (nRows == 6)
      type = 8;
  }
  if (type == -1) printf("ERROR::CbmTrdUtils:GetModuleType: nRows:%2i nCols:%3i\n",nRows,nCols);
  return type;
}

   
  void CbmTrdUtils::CreateLayerView(std::map<Int_t/*moduleAddress*/, TH1*>& Map, CbmTrdModule *fModuleInfo, CbmTrdDigiPar *fDigiPar, const TString folder, const TString pics, const TString zAxisTitle, const Double_t fmax, const Double_t fmin, const Bool_t logScale){
    TString title(""), name("");
    //name.Form("_TH_%.2EGeV_",fTriggerThreshold);
    TPaveText *text=NULL;
    std::map<Int_t, TH1*>::iterator MapIt;
    InitColorVector(logScale, fmin, fmax);
    TH2I* fLayerDummy = new TH2I("LayerDummy","",1,-600,600,1,-500,500);
    fLayerDummy->SetXTitle("x-coordinate [cm]");
    fLayerDummy->SetYTitle("y-coordinate [cm]");
    fLayerDummy->GetXaxis()->SetLabelSize(0.02);
    fLayerDummy->GetYaxis()->SetLabelSize(0.02);
    fLayerDummy->GetZaxis()->SetLabelSize(0.02);
    fLayerDummy->GetXaxis()->SetTitleSize(0.02);
    fLayerDummy->GetXaxis()->SetTitleOffset(1.5);
    fLayerDummy->GetYaxis()->SetTitleSize(0.02);
    fLayerDummy->GetYaxis()->SetTitleOffset(2);
    fLayerDummy->GetZaxis()->SetTitleSize(0.02);
    fLayerDummy->GetZaxis()->SetTitleOffset(-2);
    fLayerDummy->SetStats(kFALSE);
    fLayerDummy->SetContour(99);
    fLayerDummy->Fill(0.,0.,0.);
    std::map<Int_t, TCanvas*> fLayerMap;
    std::map<Int_t, TCanvas*>::iterator fLayerMapIt; 
    /*
      gDirectory->pwd();
      if (!gDirectory->Cd(folder)) 
      gDirectory->mkdir(folder);
      gDirectory->Cd(folder);
    */
    for (MapIt = Map.begin(); MapIt != Map.end(); MapIt++) {
      Double_t value = MapIt->second->GetMean(1);
      Double_t valueE = MapIt->second->GetRMS(1);
      fModuleInfo = fDigiPar->GetModule(MapIt->first);
      Int_t Station  = CbmTrdAddress::GetLayerId(MapIt->first) / 4 + 1;//fGeoHandler->GetStation(moduleId);
      Int_t Layer    = CbmTrdAddress::GetLayerId(MapIt->first) % 4 + 1;//fGeoHandler->GetLayer(moduleId);
      Int_t combiId = 10 * Station + Layer;
      if (fLayerMap.find(combiId) == fLayerMap.end()){
	title.Form("Station%i_Layer%i",Station,Layer);
	fLayerMap[combiId] =  new TCanvas(title,title,1200,1000);
	fLayerDummy->SetZTitle(zAxisTitle);
	fLayerDummy->GetZaxis()->SetRangeUser(fmin,fmax);
	fLayerDummy->Draw("colz");
      }
      fLayerMap[combiId]->cd();
 
      text = new TPaveText(fModuleInfo->GetX()-fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()-fModuleInfo->GetSizeY(),
			   fModuleInfo->GetX()+fModuleInfo->GetSizeX(),
			   fModuleInfo->GetY()+fModuleInfo->GetSizeY());
      text->SetFillStyle(1001);
      text->SetLineColor(1);

      text->SetFillColor(GetColorCode(value));
  
      if (GetColorCode(value)<65)
	text->SetTextColor(kWhite);
      title.Form("%.1f#pm%.1f",value,valueE);
      text->AddText(title);
      text->Draw("same");
    }
    for (fLayerMapIt = fLayerMap.begin(); fLayerMapIt != fLayerMap.end(); fLayerMapIt++) {
      //fLayerMapIt->second->Write("", TObject::kOverwrite);
      title.Form("pics/TrdQa%s_S%i_L%i_%s.pdf",pics.Data(),fLayerMapIt->first/10,fLayerMapIt->first-(fLayerMapIt->first/10)*10,name.Data());
      fLayerMapIt->second->SaveAs(title);
      title.ReplaceAll("pdf","png");
      fLayerMapIt->second->SaveAs(title);
    }
    for (MapIt = Map.begin(); MapIt != Map.end(); MapIt++) {
      //MapIt->second->Write("", TObject::kOverwrite);
    }
    //gDirectory->Cd("..");
  }
