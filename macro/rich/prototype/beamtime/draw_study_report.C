
void draw_study_report()
{
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();


	std::vector<std::string> fileNames, studyNames;
	std::string outputDir = "results/report_wls_on_offset100/";

	fileNames.push_back("results/ringB1.root");
	fileNames.push_back("results/ringH1.root");
	fileNames.push_back("results/ringG1.root");
	fileNames.push_back("results/ringH2.root");
	fileNames.push_back("results/ringB2.root");
	fileNames.push_back("results/ringA1.root");
	fileNames.push_back("results/ringB3.root");


	studyNames.push_back("ringB1");
	studyNames.push_back("ringH1");
	studyNames.push_back("ringG1");
	studyNames.push_back("ringH2");
	studyNames.push_back("ringB2");
	studyNames.push_back("ringA1");
	studyNames.push_back("ringB3");


	if (outputDir != ""){
		gSystem->mkdir(outputDir.c_str(), true); // create directory if it does not exist
	}

	CbmRichTrbRecoQaStudyReport* reportQa = new CbmRichTrbRecoQaStudyReport();
	reportQa->SetFitHist(true);
	reportQa->Create(fileNames, studyNames, outputDir);
}
