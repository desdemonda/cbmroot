
void draw_analysis_jpsi()
{
	TStopwatch timer;
	timer.Start();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();

	std::string outputDir = "results_analysis/";
	std::string fileName = "/Users/slebedev/Development/cbm/data/simulations/rich/richreco/analysis.0001.root";

	CbmSimulationReport* jpsiReport = new CbmAnaJpsiReport();
	jpsiReport->Create(fileName, outputDir);
}

