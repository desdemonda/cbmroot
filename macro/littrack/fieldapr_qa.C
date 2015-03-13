/**
 * \file fieldapr_qa.C
 * \brief Macro runs CbmLitFieldApproximationQa task which checks field map and its approximation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 **/

#include <iostream>

void fieldapr_qa(Int_t nEvents = 1)
{
   TString script = TString(gSystem->Getenv("LIT_SCRIPT"));

   TString dir = "$VMCWORKDIR/macro/littrack/commit_tests/events_electron/"; // Output directory
   TString mcFile = dir + "mc.0000.root"; // MC transport file
   TString parFile = dir + "param.0000.root"; // Parameter file
   TString fieldaprQaFile = dir + "fieldapr.qa.0000.root";// Output file
   TString resultDir = "./test/";

   if (script == "yes") {
      mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
      parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
      fieldaprQaFile = TString(gSystem->Getenv("LIT_FIELDAPR_QA_FILE"));
      resultDir = TString(gSystem->Getenv("LIT_RESULT_DIR"));
   }

	TStopwatch timer;
	timer.Start();

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	run->SetInputFile(mcFile);
	run->SetOutputFile(fieldaprQaFile);
	// ------------------------------------------------------------------------

	CbmLitFieldApproximationQa* fieldQa = new CbmLitFieldApproximationQa();

	std::vector<Double_t> zPos;
   zPos.push_back(50.);
   zPos.push_back(100.);
   zPos.push_back(150.);
   zPos.push_back(200.);
   fieldQa->SetSliceZPosition(zPos);

   std::vector<UInt_t> degrees;
// degrees.push_back(3);
//   degrees.push_back(5);
//   degrees.push_back(7);
//   degrees.push_back(9);
   fieldQa->SetPolynomDegrees(degrees);

	fieldQa->SetAcceptanceAngleX(35.);
	fieldQa->SetAcceptanceAngleY(35.);
	fieldQa->SetNofBinsX(30);
	fieldQa->SetNofBinsY(30);

	fieldQa->IsFixedBounds(true);
	fieldQa->SetUseEllipseAcc(false);
	fieldQa->SetPolynomDegreeIndex(0);
	fieldQa->SetOutputDir(std::string(resultDir));
	run->AddTask(fieldQa);

	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	// -----   Initialize and run   --------------------------------------------
	run->Init();
	run->Run(0, nEvents);
	// ------------------------------------------------------------------------

	// -----   Finish   -------------------------------------------------------
	timer.Stop();
   std::cout << "Macro finished successfully." << std::endl;
   std::cout << "Output file is " << fieldaprQaFile << std::endl;
   std::cout << "Parameter file is " << parFile << std::endl;
   std::cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << std::endl;
	// ------------------------------------------------------------------------
}
