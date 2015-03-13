/**
 * \file qa_simulation_report.C
 * \brief Macro for generation of reports from file with histograms.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */

void qa_simulation_report()
{
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   std::string outputDir = "test/";
   std::string fileName = "events/qa.auau.4gev.mbias.all.5M_Un.root";

//   CbmSimulationReport* trackingQaReport = new CbmLitTrackingQaReport();
//   trackingQaReport->Create(fileName, outputDir);

//   CbmSimulationReport* fitQaReport = new CbmLitFitQaReport();
//   fitQaReport->Create(fileName, outputDir);

//   CbmSimulationReport* clusteringQaReport = new CbmLitClusteringQaReport();
//   clusteringQaReport->Create(fileName, outputDir);

//   CbmLitRadLengthQaReport* radLengthQaReport = new CbmLitRadLengthQaReport();
//   radLengthQaReport->Create(fileName, outputDir);

   CbmSimulationReport* tofQaReport = new CbmLitTofQaReport();
   tofQaReport->Create(fileName, outputDir);
}
