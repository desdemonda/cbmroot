#include "TStyle.h"

void much_histo_style(){
  gStyle->SetCanvasColor(10);
  gStyle->SetFrameFillColor(10);
  gStyle->SetHistLineWidth(2);
  gStyle->SetPadColor(10);
  gStyle->SetStatColor(10);
  gStyle->SetPalette(1);
//  gStyle->SetTextSize(0.05);
//  gStyle->SetTitleSize(0.1);
//  gStyle->SetLabelSize(0.05, "X");
//  gStyle->SetLabelSize(0.05, "Y");
//  gStyle->SetTitleXSize(0.05);
//  gStyle->SetTitleYSize(0.05);
  gStyle->SetTitleXOffset(1.2);
  gStyle->SetTitleYOffset(1.2);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(10);
  gStyle->SetPadBorderSize(0);

  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);
  //gStyle->SetGridWidth(1);

  gStyle->SetStripDecimals(0);
  gStyle->SetPadBottomMargin(0.12);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadLeftMargin(0.12);
  gStyle->SetPadRightMargin(0.04);

}

