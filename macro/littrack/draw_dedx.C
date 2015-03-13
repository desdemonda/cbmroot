/**
 * \file draw_dedx.C
 * \brief Macro draws energy losses.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 **/

void draw_dedx() {
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	CbmLitCheckEnergyLossMuons checker;
	checker.SetMaterial("iron");
	checker.Check();

//	CbmLitCheckBrem checker;
//	checker.SetMaterial("iron");
//	checker.Check();
}
