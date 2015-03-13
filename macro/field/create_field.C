/**
 * \file create_field.C
 * \brief Macro converts ASCII file with magnetic field to ROOT file.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \author Florian Uhlig <f.uhlig@gsi.de>
 * \date 2012
 * \details The tested input format is the output format of TOSCA with small
 * modifications. The first lines in ten file must hold the following information
 * line 1: field symmetry (nosym, sym2 orsym3)
 * line 2: min value in x-dir, max value in x-dir and number of gridpoints in x-dir  
 * line 3: same information as line 2 for y-dir
 * line 4: same information as line 2 for z-dir
 * line 5 - EOF: field values Bx, By, BZ for all grid points 
 */

#include <iostream>
using std::cout;
using std::endl;

void create_field(TString fileName="CwPolus6_Cs_2", const char* fileType="A", TString symmetry="sym3", TString outputFile="field_v12b.root", TString mapName="field_v12b")
{
	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	if (symmetry == "sym2" ) {
	  CbmFieldMapSym2* fieldMap = new CbmFieldMapSym2(fileName, fileType);
	} else if (symmetry == "sym3" ) {
	  CbmFieldMapSym3* fieldMap = new CbmFieldMapSym3(fileName, fileType);
	} else {
          CbmFieldMap* fieldMap = new CbmFieldMap(fileName, fileType);
	}
	fieldMap->Init();
	fieldMap->Print();
	fieldMap->WriteRootFile(outputFile, mapName);
	fieldMap->Print();

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	cout << endl << endl;
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << outputFile << endl;
	cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
	// ------------------------------------------------------------------------
}
