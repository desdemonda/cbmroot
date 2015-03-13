
void draw_analysis()
{
	TStopwatch timer;
	timer.Start();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();

	TString outputDir = "recoA/";
	TString runTitle = "run_title_a";
	TString histFileName = "te14324200946.hld.root";

	TString script = TString(gSystem->Getenv("SCRIPT"));

	if (script == "yes") {
	   histFileName= TString(gSystem->Getenv("OUTPUT_ROOT_FILE_ALL"));
	   outputDir = TString(gSystem->Getenv("OUTPUT_DIR"));
	   runTitle = TString(gSystem->Getenv("RUN_TITLE"));
	}

	CbmRichTrbRecoQa* qaReco = new CbmRichTrbRecoQa();
	qaReco->SetOutputDir(outputDir);
	qaReco->SetRunTitle(runTitle);
	qaReco->SetDrawHist(true);
	qaReco->DrawHistFromFile(histFileName);
}

