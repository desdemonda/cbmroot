/**
 * \file create_field.C
 * \brief Macro converts ASCII file with magnetic field to ROOT file.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 */

#include <iostream>
using std::cout;
using std::endl;

void create_field()
{
	// Input ASCII file
	const char* fieldASCII = "AMFz";
	// Output ROOT file
	const char* fieldROOT = "~/Development/cbm/trunk/cbmroot/input/AMFz.root";
	const char* mapName = "AMFz";

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	//CbmFieldMapSym2* fieldMap = new CbmFieldMapSym2(fieldASCII, "A");
	CbmFieldMap* fieldMap = new CbmFieldMap(fieldASCII, "A");
	fieldMap->Init();
	fieldMap->Print();
	fieldMap->WriteRootFile(fieldROOT, mapName);
	fieldMap->Print();

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << fieldROOT << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	// ------------------------------------------------------------------------
}
