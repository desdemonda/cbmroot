#include "drawStyle.C"

void plot()
{
    TPstyle();

    gROOT->LoadMacro("$(VMCWORKDIR)/gconfig/basiclibs.C");
    basiclibs();

    char *ver = getenv("CBMVER");

    char str[1000];
    sprintf(str, "/misc/kresan/mc/%s/urqmd/auau/2gev/centr/\
urqmd.auau.2gev.centr.0000.mc.root", ver);
    TFile *file = new TFile(str);

    TGeoManager *geoMan = (TGeoManager*) file->Get("CBMGeom");

    TCanvas *c1 = new TCanvas("c1", "CBM Detector",
			      10, 10, 510, 510);

    geoMan->GetMasterVolume()->Draw("same");
}



