//
// PairAnalysis styler functions wrapped in a namespace
// 
//
// Authors: 
//   Julian Book <Julian.Book@cern.ch>

#include <TROOT.h>
#include <TStyle.h>
#include <TGaxis.h>
#include <TAttMarker.h>
#include <TAttLine.h>
#include <TAttFill.h>
#include <TColor.h>
#include <TH1.h>
#include <TPad.h>
#include "PairAnalysisStyler.h"

// const Int_t PairAnalysisStyler::Marker[] = {kOpenCircle,
// 					   kOpenDiamond,
// 					   kOpenSquare,
// 					   kOpenCross,
// 					   kOpenStar,
// 					   kMultiply,
// 					   kPlus,
// 					   kStar };

TStyle *PairAnalysisStyler::fUserDielStyle=0x0;   // user defined style
void PairAnalysisStyler::SetStyle(TStyle *userStyle) {
  //
  // set and load user defined style
  //
  fUserDielStyle=userStyle;
  LoadStyle();
}

//_____________________________________________________________________________
void PairAnalysisStyler::LoadStyle() {
  //
  // load style
  //

  //  return;
  // load style defined by the user
  if(fUserDielStyle) { fUserDielStyle->cd(); gROOT->ForceStyle(); }
  else {

    Int_t font      = 42;
    Int_t bgrdcolor = 0; // set to -1 for transparaent, will causes warnings

    TStyle *defaultSty = new TStyle("default","default");
    defaultSty->SetPaperSize(TStyle::kA4);
    defaultSty->SetTextFont(font);

    // For the canvas:
    defaultSty->SetCanvasBorderMode(0);
    defaultSty->SetCanvasColor(bgrdcolor); //transparent
    defaultSty->SetCanvasDefH(600); //Height of canvas
    defaultSty->SetCanvasDefW(600); //Width of canvas
    defaultSty->SetCanvasDefX(0);   //POsition on screen
    defaultSty->SetCanvasDefY(0);

    // For the Pad:
    defaultSty->SetPadBorderMode(0);
    // defaultSty->SetPadBorderSize(Width_t size = 1);
    defaultSty->SetPadColor(bgrdcolor);        //transparent
    defaultSty->SetPadGridX(false);
    defaultSty->SetPadGridY(false);
    defaultSty->SetGridColor(0);
    defaultSty->SetGridStyle(3);
    defaultSty->SetGridWidth(1);

    // For the frame:
    defaultSty->SetFrameBorderMode(0);
    defaultSty->SetFrameBorderSize(1);
    defaultSty->SetFrameFillColor(bgrdcolor);  //transparent
    defaultSty->SetFrameFillStyle(0);
    defaultSty->SetFrameLineColor(1);
    defaultSty->SetFrameLineStyle(1);
    defaultSty->SetFrameLineWidth(1);

    // Margins:
    defaultSty->SetPadTopMargin(0.075);
    defaultSty->SetPadBottomMargin(0.18);
    defaultSty->SetPadLeftMargin(0.18); //0.13
    defaultSty->SetPadRightMargin(0.075);

    // For the histo:
    defaultSty->SetHistFillColor(bgrdcolor);    //transparent
    // defaultSty->SetHistFillStyle(0);
    defaultSty->SetHistLineColor(1);
    defaultSty->SetHistLineStyle(0);
    defaultSty->SetHistLineWidth(2);
    // defaultSty->SetLegoInnerR(Float_t rad = 0.5);
    // defaultSty->SetNumberContours(Int_t number = 20);
    defaultSty->SetHistMinimumZero();
    //    defaultSty->SetEndErrorSize(2);
    defaultSty->SetErrorX(0.);
    defaultSty->SetMarkerStyle(kOpenCircle); // Kind of dot used for points
    defaultSty->SetMarkerSize(1.5);
    
    //For the fit/function:
    defaultSty->SetOptFit(1);
    defaultSty->SetFitFormat("5.4g");
    defaultSty->SetFuncColor(2);
    defaultSty->SetFuncStyle(1);
    defaultSty->SetFuncWidth(1);
    
    //For the date:
    defaultSty->SetOptDate(12); // bottom right only date
    // defaultSty->SetDateX(Float_t x = 0.01);
    // defaultSty->SetDateY(Float_t y = 0.01);
    defaultSty->GetAttDate()->SetTextColor(kGray);
    defaultSty->GetAttDate()->SetTextFont(font);
    defaultSty->GetAttDate()->SetTextSize(0.02);

    // For the statistics box:
    defaultSty->SetOptFile(0);
    defaultSty->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
    //  defaultSty->SetOptStat("n"); // only name
    defaultSty->SetStatColor(kWhite);
    defaultSty->SetStatFont(font);
    defaultSty->SetStatFontSize(0.02);
    defaultSty->SetStatTextColor(kGray);
    defaultSty->SetStatFormat("6.4g");
    defaultSty->SetStatBorderSize(1);
    defaultSty->SetStatH(0.1);
    defaultSty->SetStatW(0.5);
    // defaultSty->SetStatStyle(Style_t style = 1001);
    defaultSty->SetStatX(0.01);
    defaultSty->SetStatY(0.01);
    
    // For the Global title:
    defaultSty->SetOptTitle(0);
    defaultSty->SetTitleFont(font,"T");
    defaultSty->SetTitleSize(0.05,"T");
    defaultSty->SetTitleColor(bgrdcolor,"T");
    defaultSty->SetTitleTextColor(1);
    defaultSty->SetTitleFillColor(bgrdcolor);  //transparent
    defaultSty->SetTitleFontSize(0.06);
    // defaultSty->SetTitleH(0); // Set the height of the title box
    defaultSty->SetTitleW(0.5); // Set the width of the title box
    defaultSty->SetTitleX(0.25); // Set the position of the title box
    // defaultSty->SetTitleY(0.985); // Set the position of the title box
    // defaultSty->SetTitleStyle(Style_t style = 1001);
    // defaultSty->SetTitleBorderSize(2);

    // For the axis titles:
    defaultSty->SetTitleColor(1, "XYZ");
    defaultSty->SetTitleFont(font, "XYZ");
    defaultSty->SetTitleSize(0.06, "XYZ");             //
    // defaultSty->SetTitleXSize(Float_t size = 0.02); // Another way to set the size?
    // defaultSty->SetTitleYSize(Float_t size = 0.02);
    defaultSty->SetTitleXOffset(1.0); //  defaultSty->SetTitleXOffset(1.08);
    defaultSty->SetTitleYOffset(1.25); //  defaultSty->SetTitleYOffset(1.5);
    //defaultSty->SetTitleOffset(1.0, "XYZ"); // Another way to set the Offset

    // For the axis labels:
    defaultSty->SetLabelColor(1, "XYZ");
    defaultSty->SetLabelFont(font, "XYZ");
    defaultSty->SetLabelOffset(0.007, "XYZ");
    defaultSty->SetLabelSize(0.04, "XYZ"); //  defaultSty->SetLabelSize(0.06, "XYZ");

    // For the axis:
    defaultSty->SetHistMinimumZero();
    defaultSty->SetAxisColor(1, "XYZ");
    //    defaultSty->SetStripDecimals(kTRUE);
    //    defaultSty->SetTickLength(0.03, "XYZ");
    defaultSty->SetNdivisions(810, "XYZ");  //  defaultSty->SetNdivisions(505, "XYZ");
    defaultSty->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
    defaultSty->SetPadTickY(1);

    // Change for log plots:
    defaultSty->SetOptLogx(0);
    defaultSty->SetOptLogy(0);
    defaultSty->SetOptLogz(0);

    // For the colored palette
    defaultSty->SetPalette(1,0);

    // For the legends:
    defaultSty->SetLegendFillColor(bgrdcolor);
    defaultSty->SetLegendFont(font);

    //                                       Additions -- transparent style
    defaultSty->SetFillStyle(4000);
    defaultSty->SetStatStyle(0);
    defaultSty->SetTitleStyle(0);
    defaultSty->SetCanvasBorderSize(0);
    defaultSty->SetFrameBorderSize(0);
    defaultSty->SetLegendBorderSize(0);
    defaultSty->SetStatBorderSize(0);
    defaultSty->SetTitleBorderSize(0);

    // set current style to default
    //  gROOT->SetStyle(sty);
    defaultSty->cd();
    gROOT->ForceStyle();
    fUserDielStyle=defaultSty;

    // axis
    TGaxis::SetMaxDigits(4);
  }

}

void PairAnalysisStyler::Style(TObject *obj, Int_t idx) {
  //
  // style object according to index
  //
  if(!obj) return;

  if (obj->InheritsFrom(TH1::Class()) && gPad) {
    if(gPad->GetLogx()) {
      dynamic_cast<TH1*>(obj)->GetXaxis()->SetMoreLogLabels(kTRUE);
      dynamic_cast<TH1*>(obj)->GetXaxis()->SetNoExponent(kTRUE);
    }
    if(gPad->GetLogy()) {
      dynamic_cast<TH1*>(obj)->GetYaxis()->SetMoreLogLabels(kTRUE);
      dynamic_cast<TH1*>(obj)->GetYaxis()->SetNoExponent(kTRUE);
    }
    if(gPad->GetLogz()) {
      dynamic_cast<TH1*>(obj)->GetZaxis()->SetMoreLogLabels(kTRUE);
      dynamic_cast<TH1*>(obj)->GetZaxis()->SetNoExponent(kTRUE);
    }
  }

  if (obj->InheritsFrom(TAttMarker::Class())) {
    dynamic_cast<TAttMarker*>(obj)->SetMarkerSize(2.);
    dynamic_cast<TAttMarker*>(obj)->SetMarkerStyle(Marker[idx%kNMaxMarker]);
    dynamic_cast<TAttMarker*>(obj)->SetMarkerColor(Color[idx%kNMaxColor]);
  }
  if (obj->InheritsFrom(TAttLine::Class())) {
    dynamic_cast<TAttLine*>(obj)->SetLineWidth(2);
    dynamic_cast<TAttLine*>(obj)->SetLineStyle(Line[(idx%10)%kNMaxLine]);
    //    dynamic_cast<TAttLine*>(obj)->SetLineColor(Color[idx%kNMaxColor]);
    if(idx>=10) {
      idx=idx%10;
      dynamic_cast<TAttLine*>(obj)->SetLineColor(TColor::GetColorBright(Color[idx]));
    }
    else
      dynamic_cast<TAttLine*>(obj)->SetLineColor(TColor::GetColorDark(Color[idx%kNMaxColor]));
  }
  // if (obj->InheritsFrom(TAttFill::Class()))
  //   dynamic_cast<TAttFill*>(obj)->SetFillStyle();
  //   dynamic_cast<TAttFill*>(obj)->SetFillColor();

}

