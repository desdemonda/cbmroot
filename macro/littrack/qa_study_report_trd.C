/**
 * \file qa_study_report_trd.C
 * \brief Macro for generation of summary reports.
 * \author Jonathan Pieper <j.pieper@gsi.de>
 * \date Dec, 2014
 * \original_author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \original_date 2011
 */

void qa_study_report_trd()
{
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   TString script = TString(gSystem->Getenv("LIT_SCRIPT"));

   std::vector<std::string> fileNames, studyNames;
   std::string outputDir;
   if (script == "yes") {
      Int_t nofStudies = TString(gSystem->Getenv("LIT_NOF_STUDIES")).Atoi();
      for (Int_t i = 0; i < nofStudies; i++) {
         std::ostringstream ssresult, ssname;
         ssresult << "LIT_FILE_NAME" << i+1;
         ssname << "LIT_STUDY_NAME" << i+1;
         fileNames.push_back(gSystem->Getenv(ssresult.str().c_str()));
         studyNames.push_back(gSystem->Getenv(ssname.str().c_str()));
      }
      outputDir = std::string(gSystem->Getenv("LIT_STUDY_OUTPUT_DIR"));
   } else {
      fileNames.push_back("scripts/events_trd_v13p_3e/qa.digi.prf.0000.root");
      fileNames.push_back("scripts/events_trd_v13p_3e/qa.digi.simple.0000.root");
      fileNames.push_back("scripts/events_trd_v13p_3e/qa.smearing.0000.root");
      fileNames.push_back("scripts/events_trd_v13p_3e/qa.clustering.0000.root");

      studyNames.push_back("digi prf");
      studyNames.push_back("digi simple");
      studyNames.push_back("smearing");
      studyNames.push_back("clustering");

   //   fileNames.push_back("scripts/events_trd_v13d/qa.smearing.branch.0000.root");
    //  fileNames.push_back("scripts/events_trd_v13e/qa.smearing.branch.0000.root");
    //  fileNames.push_back("scripts/events_trd_v13f/qa.smearing.branch.0000.root");

   //   studyNames.push_back("v13d");
   //   studyNames.push_back("v13e");
   //   studyNames.push_back("v13f");

 //      fileNames.push_back("scripts/events_much_v11a/qa.0000.root");
 //      fileNames.push_back("scripts/events_much_v12a/qa.0000.root");
  //     fileNames.push_back("scripts/events_much_v12b/qa.0000.root");
 //      fileNames.push_back("scripts/events_much_v12c/qa.0000.root");

  //     studyNames.push_back("v11a");
  //     studyNames.push_back("v12a");
  //     studyNames.push_back("v12b");
  //     studyNames.push_back("v12c");

      outputDir = "./test/";
   }

//   CbmStudyReport* trackingQa = new CbmLitTrackingQaStudyReport();
//   trackingQa->Create(fileNames, studyNames, outputDir);

   CbmStudyReport* clusteringQa = new CbmLitClusteringQaStudyReport();
   clusteringQa->Create(fileNames, studyNames, outputDir);
}
