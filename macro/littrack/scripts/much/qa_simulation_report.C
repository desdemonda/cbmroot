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

   std::string outputDir = "results/";
   std::string fileName = "/hera/cbm/users/andrey/mc/much/v12b/omega/auau/25gev/centr/qa.auau.25gev.centr.all.root";


   CbmSimulationReport* trackingQaReport = new CbmLitTrackingQaReport();
   trackingQaReport->Create(fileName, outputDir);

   CbmSimulationReport* fitQaReport = new CbmLitFitQaReport();
   fitQaReport->Create(fileName, outputDir);

//   CbmSimulationReport* clusteringQaReport = new CbmLitClusteringQaReport();
//   clusteringQaReport->Create(fileName, outputDir);

//   CbmLitRadLengthQaReport* radLengthQaReport = new CbmLitRadLengthQaReport();
//   radLengthQaReport->Create(fileName, outputDir);

   CbmSimulationReport* tofQaReport = new CbmLitTofQaReport();
   tofQaReport->Create(fileName, outputDir);

}
