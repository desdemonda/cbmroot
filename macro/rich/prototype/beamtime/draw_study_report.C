
void draw_study_report()
{
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();


	std::vector<std::string> fileNames, studyNames;
	std::string outputDir = "results_reports/report_wls_off_offset250_posH/";

	fileNames.push_back("results_wlsoff_offset250/ringH1.root");
	fileNames.push_back("results_wlsoff_offset250/ringH2.root");
	//fileNames.push_back("results_wlson_offset250/ringB4.root");
	//fileNames.push_back("results_wlsoff_offset250/ringG1.root");

	studyNames.push_back("ringH1");
	studyNames.push_back("ringH2");
	//studyNames.push_back("ringB4");
	//studyNames.push_back("ringH1");
	//studyNames.push_back("ringG1");

	if (outputDir != ""){
		gSystem->mkdir(outputDir.c_str(), true); // create directory if it does not exist
	}

	CbmRichTrbRecoQaStudyReport* reportQa = new CbmRichTrbRecoQaStudyReport();
	reportQa->SetFitHist(true);
	reportQa->Create(fileNames, studyNames, outputDir);
}
