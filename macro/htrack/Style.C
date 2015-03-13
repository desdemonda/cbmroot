
  //////////////////////////////////////////////////////////////////////
  //
  // ROOT style macro for the TDR
  //
  //////////////////////////////////////////////////////////////////////


void Style() {

  gSystem->Load("libPostscript.so");
  gStyle->SetCanvasBorderMode(-1);
  gStyle->SetCanvasBorderSize(1);
  gStyle->SetCanvasColor(10);
  gStyle->SetFrameFillColor(10);
  gStyle->SetFrameBorderSize(1);
  gStyle->SetFrameBorderMode(-1);
  gStyle->SetFrameLineWidth(1.2);
  gStyle->SetFrameLineColor(1);
  gStyle->SetHistFillColor(0);
  gStyle->SetHistLineWidth(3.5);
  gStyle->SetHistLineColor(1);
  gStyle->SetPadColor(10);
  gStyle->SetPadBorderSize(1);
  gStyle->SetPadBorderMode(-1);
  gStyle->SetStatColor(10);
  gStyle->SetTitleColor(10);
  gStyle->SetTitleFont(22);
  gStyle->SetLabelFont(22,"X");
  gStyle->SetLabelFont(22,"Y");
  gStyle->SetStatFont(22);
  gStyle->SetTitleOffset(1.2,"X");
  gStyle->SetTitleOffset(1.2,"Y");
  gStyle->SetOptDate(0);
  gStyle->SetOptTitle(1);
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1);

}
