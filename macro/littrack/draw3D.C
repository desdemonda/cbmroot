/**
 * \file draw3D.C
 * \brief Macro draws 3D view of CBM detector using TGeo functionality.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 **/

void draw3D()
{
   TString dir = "/Users/andrey/Development/cbm/d/events/sts_tof/";;
	TString parFile = dir + "param.0000.root";

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	TFile* f = new TFile(parFile);
	f->Get("FairBaseParSet");
	gGeoManager->SetVisLevel(4);

	// Check overlaps
//	gGeoManager->CheckOverlaps(0.0001);
//	gGeoManager->PrintOverlaps();

	TGeoVolume* master = gGeoManager->GetMasterVolume();
	master->Draw();//"ogl");
}
