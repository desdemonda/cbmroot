/**
 * \file event_display.C
 * \brief Macro runs event display.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/

void event_display()
{
	TString dir = "events/mvd/";
	TString mcFile = dir + "mc.0000.root"; // MC transport file
	TString parFile = dir + "param.0000.root"; // Parameter file

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	gSystem->Load("libEve");
	gSystem->Load("libEventDisplay");

	FairRunAna *run= new FairRunAna();

	run->SetInputFile(mcFile);
	run->SetOutputFile("test.root");

	FairEventManager* eventManager= new FairEventManager();
	FairMCTracks* mcTracks = new FairMCTracks("Monte-Carlo Tracks");
	FairMCPointDraw* richPoints = new FairMCPointDraw("RichPoint", kOrange, kFullSquare);
	FairMCPointDraw* tofPoints = new FairMCPointDraw("TofPoint", kBlue, kFullSquare);
	FairMCPointDraw* trdPoints = new FairMCPointDraw("TrdPoint", kTeal, kFullSquare);
	FairMCPointDraw* muchPoints = new FairMCPointDraw("MuchPoint", kRed, kFullSquare);
	FairMCPointDraw* ecalPoints = new FairMCPointDraw("EcalPoint", kYellow, kFullSquare);
	FairMCPointDraw* refPlanePoints = new FairMCPointDraw("RefPlanePoint", kPink, kFullSquare);
	FairMCPointDraw* stsPoints = new FairMCPointDraw("StsPoint", kCyan, kFullSquare);

	eventManager->AddTask(mcTracks);
	eventManager->AddTask(richPoints);
	eventManager->AddTask(ecalPoints);
	eventManager->AddTask(tofPoints);
	eventManager->AddTask(trdPoints);
	eventManager->AddTask(muchPoints);
	eventManager->AddTask(refPlanePoints);
	eventManager->AddTask(stsPoints);


	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	eventManager->Init();
}
