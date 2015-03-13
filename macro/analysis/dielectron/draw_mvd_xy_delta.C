void draw_mvd_xy_delta()
{
   TString fileName = "/lustre/cbm/user/ebelolap/aug11/25gev/70field/deltasource/mc.delta.root";
   TFile *file = new TFile(fileName);

   gStyle->SetCanvasColor(kWhite);
   gStyle->SetFrameFillColor(kWhite);
   gStyle->SetPadColor(kWhite);
   gStyle->SetStatColor(kWhite);
   gStyle->SetTitleFillColor(kWhite);
   gStyle->SetPalette(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);

   TString fileName= "delta_xy_mvd1_70";
   TString fileNameGif = fileName + ".png";
   TString fileNameEps = fileName + ".eps";

   cbmsim->Draw("MvdPoint.fY:MvdPoint.fX", "MvdPoint.fZ<6","COLZ");
   TH2F* htemp = (TH2F*)gPad->GetPrimitive("htemp"); // empty, but has axes
   htemp->GetXaxis()->SetTitle("X [cm]");
   htemp->GetYaxis()->SetTitle("Y [cm]");
   htemp->SetMaximum(250);
   gPad->SetGridx(true);
   gPad->SetGridy(true);

   c1->SaveAs(fileNameGif);
   c1->SaveAs(fileNameEps);
}
