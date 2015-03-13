void draw_litqa()
{
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();


   //std::string dir = "/Users/slebedev/Development/cbm/data/lmvm/nov13/25gev/trd/1.0field/nomvd/rho0/";
   std::string outputDir = "results_litqa/";
   std::string fileName = "/Users/slebedev/Development/cbm/data/simulations/rich/richreco/25gev.reco.0001.root";

   CbmSimulationReport* trackingQaReport = new CbmLitTrackingQaReport();
   trackingQaReport->Create(fileName, outputDir + "/tracking/");

  // CbmSimulationReport* fitQaReport = new CbmLitFitQaReport();
  // fitQaReport->Create(fileName, outputDir + "/fit/");

//   CbmSimulationReport* clusteringQaReport = new CbmLitClusteringQaReport();
//   clusteringQaReport->Create(fileName, outputDir);

//   CbmLitRadLengthQaReport* radLengthQaReport = new CbmLitRadLengthQaReport();
//   radLengthQaReport->Create(fileName, outputDir);

//   CbmSimulationReport* tofQaReport = new CbmLitTofQaReport();
//   tofQaReport->Create(fileName, outputDir);
}
