//
//  plot pad size of layers in cm2 in png files
//  Nov 2012
//
//  run as follows:
//  root -l -b -q plot_pad_size_in_layer.C++\(\"trd.v13/trd_v13b.digi.par\"\)
//  display *.png
//
//  20131028 - DE - patch for digi.par files now containing fOrientation
//

#include "Riostream.h"
#include "TRint.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TString.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>

void plot_pad_size_in_layer(TString digiPar="trd.v13/trd_v13g.digi.par", Int_t nlines=1, Int_t nrows_in_sec=0, Int_t alllayers=1) 
{

  gStyle->SetPalette(1,0);
  gROOT->SetStyle("Plain");
  gStyle->SetPadTickX(1);                        
  gStyle->SetPadTickY(1); 
  gStyle->SetOptStat(kFALSE);
  gStyle->SetOptTitle(kFALSE);

  Bool_t read = false;
  TH2I *fLayerDummy = new TH2I("LayerDummy","",1200,-600,600,1000,-500,500);
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

  TString title;
  TString title1, title2, title3;
  TString buffer;
  TString firstModule = "";
  Int_t blockCounter(0), startCounter(0); // , stopCounter(0);
  Double_t msX(0), msY(0), mpX(0), mpY(0), mpZ(0), psX(0), psY(0);
  Double_t ps1X(0), ps1Y(0), ps2X(0), ps2Y(0), ps3X(0), ps3Y(0);
  Int_t modId(0), layerId(0);
  Double_t sec1(0), sec2(0), sec3(0);
  Double_t row1(0), row2(0), row3(0);
  std::map<float, TCanvas*> layerView;// map key is z-position of modules
  std::map<float, TCanvas*>::iterator it;
  ifstream digipar;

  digipar.open(digiPar.Data(), ifstream::in);
  while (digipar.good()) {
    digipar >> buffer;
    //cout << "(" << blockCounter << ")    " << buffer << endl;
    if (blockCounter == 19)
      firstModule = buffer;
    if (buffer == (firstModule + ":")){
      //cout << buffer << " <===========================================" << endl;
      read = true;
    }
    if (read) {
      startCounter++;
      if (startCounter == 1)   // position of module position in x
      {
	modId = buffer.Atoi();
        layerId = (modId & (15 << 4)) >> 4;  // from CbmTrdAddress.h
      }

      if (startCounter == 5)   // position of module position in x
	mpX = buffer.Atof();
      if (startCounter == 6)   // position of module position in y
	mpY = buffer.Atof();
      if (startCounter == 7)   // position of module position in z
	mpZ = buffer.Atof();
      if (startCounter == 8)   // position of module size in x
	msX = buffer.Atof();
      if (startCounter == 9)   // position of module size in y
	msY = buffer.Atof();

      if (startCounter == 12)   // sector 1 size in y
	sec1 = buffer.Atof();
      if (startCounter == 13)   // position of pad size in x - do not take the backslash (@14)
	ps1X = buffer.Atof();
      if (startCounter == 15)   // position of pad size in y
	ps1Y = buffer.Atof();

      if (startCounter == 17)   // sector 2 size in y
	sec2 = buffer.Atof();
      if (startCounter == 18)   // position of pad size in x
      {
	ps2X = buffer.Atof();
        psX = ps2X;   // for backwards compatibility - sector 2 is default sector
      }
      if (startCounter == 19)   // position of pad size in y
      {
	ps2Y = buffer.Atof();
        psY = ps2Y;   // for backwards compatibility - sector 2 is default sector
      }

      if (startCounter == 21)   // sector 3 size in y
	sec3 = buffer.Atof();
      if (startCounter == 22)   // position of pad size in x
	ps3X = buffer.Atof();
      if (startCounter == 23)   // position of pad size in y
	ps3Y = buffer.Atof();

//      if (startCounter == 23)   // last element
//      {
//        printf("moduleId         : %d, %d\n", modId, layerId);
//        printf("pad size sector 1: (%.2f cm, %.2f cm) pad area: %.2f cm2\n", ps1X, ps1Y, ps1X*ps1Y);
//        printf("pad size sector 2: (%.2f cm, %.2f cm) pad area: %.2f cm2\n", ps2X, ps2Y, ps2X*ps2Y);
//        printf("pad size sector 3: (%.2f cm, %.2f cm) pad area: %.2f cm2\n", ps3X, ps3Y, ps3X*ps3Y);
//        printf("rows per sector  : %.1f %.1f %.1f\n", sec1/ps1Y, sec2/ps2Y, sec3/ps3Y);
//        printf("\n");
//      }

      //printf("module position: (%.1f, %.1f, %.1f) module size: (%.1f, %.1f) pad size: (%.2f, %.2f) pad area: %.2f\n",mpX,mpY,mpZ,2*msX,2*msY,psX,psY,psX*psY);

      if (startCounter == 23) { // if last element is reached
	startCounter = 0; // reset

        if ( alllayers == 0 )   
          if ( !((layerId == 0) || (layerId == 4) || (layerId == 8)) )   // plot only 1 layer per station
            continue;

        row1 = sec1 / ps1Y;
        row2 = sec2 / ps2Y;
        row3 = sec3 / ps3Y;

	it = layerView.find(mpZ);
	if (it == layerView.end()){	
	  //	  title.Form("pad_size_layer_at_z_%.2fm",mpZ);  
	  title.Form("%02d_pad_size_layer%02d", layerId, layerId);  
	  layerView[mpZ] = new TCanvas(title,title,1200,1000);
	  fLayerDummy->DrawCopy("");

        // now print cm2 in the center
	layerView[mpZ]->cd();
	title.Form("cm^{2}");  // print cm2
	TPaveText *text = new TPaveText(0 - 28.5,
					0 - 28.5,
					0 + 28.5,
					0 + 28.5
					);
	text->SetFillStyle(1001);
	text->SetLineColor(1);
        text->SetFillColor(kWhite);
	text->AddText(title);
	text->Draw("same");
	}

        // print pad size in each module
	layerView[mpZ]->cd();
	//	title.Form("%2.0fcm^{2}",psX*psY);  // print pad size
	//	title.Form("%.0f",psX*psY);  // print pad size - 1 digit
	TPaveText *text = new TPaveText(mpX - msX,
					mpY - msY,
					mpX + msX,
					mpY + msY
					);
	text->SetFillStyle(1001);
	text->SetLineColor(1);
	//        text->SetFillColor(kViolet);

	// vary background color
//        if ((int)(psX*psY+.5) == 2)
//        {
//          text->SetFillColor(kOrange + 9);
//        } 
//        else 
        if (psX*psY <= 1.1)
        {
          text->SetFillColor(kOrange + 10);
        }
        else if (psX*psY <= 2.1)
        {
          text->SetFillColor(kOrange -  3);
        }
        else if (psX*psY <= 3.1)
        {
          text->SetFillColor(kOrange -  4);
        }
        else if (psX*psY <= 5)
        {
          text->SetFillColor(kOrange + 10 - ((int)(psX*psY+.5)-1) * 2);
//        printf("%2.1f: %d\n", psX*psY, 10 - ((int)(psX*psY+.5)-1) * 2);
        } 
        else if (psX*psY <= 10)
	{
          text->SetFillColor(kSpring + 10 - ((int)(psX*psY+.5)-4) * 2);
//        printf("%2.1f: %d\n", psX*psY, 10 - ((int)(psX*psY+.5)-4) * 2);
        } 
        else if (psX*psY > 10)
	{
          text->SetFillColor(kGreen);
//        printf("%2.1f: %s\n", psX*psY, "green");
        } 

        if (nrows_in_sec == 1)   // print number of rows in sector
	{
   	  title1.Form("%3.1f - %2.0f", ps1X*ps1Y, row1);  // print pad size and nrows - 2 digits - sector 1
	  title2.Form("%3.1f - %2.0f", ps2X*ps2Y, row2);  // print pad size and nrows - 2 digits - sector 2
	  title3.Form("%3.1f - %2.0f", ps3X*ps3Y, row3);  // print pad size and nrows - 2 digits - sector 3
	}
        else
	{
   	  title1.Form("%3.1f",ps1X*ps1Y);  // print pad size - 2 digits - sector 1
	  title2.Form("%3.1f",ps2X*ps2Y);  // print pad size - 2 digits - sector 2
	  title3.Form("%3.1f",ps3X*ps3Y);  // print pad size - 2 digits - sector 3
	}

        if (nlines==1)   // plot pad size for central sector only
	{
	  text->AddText(title2);
        }
        else   // plot pad size for all 3 sectors
	{
	  text->AddText(title1);
	  text->AddText(title2);
	  text->AddText(title3);
        }
	text->Draw("same");
	//layerView[mpZ]->Update();
      }
    }  
    blockCounter++;
  }
  digipar.close();

  for (it = layerView.begin(); it != layerView.end(); it++) 
    it->second->SaveAs(TString(it->second->GetTitle())+".png");

}
