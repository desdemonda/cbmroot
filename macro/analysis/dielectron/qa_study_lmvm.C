/**
 * \file qa_study_lmvm.C
 * \brief Macro for generation of summary reports for LMVM analysis.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2012
 */

void qa_study_lmvm()
{
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();
   gSystem->Load("libAnalysis");

   std::vector<std::string> results, names;
   std::string outputDir;

   std::string dir = "/Users/slebedev/Development/cbm/data/lmvm/";
   results.push_back(dir + "mar15_rtcut/8gev/stsv13d/richv14a_1e/trd4/tofv13/1.0field/nomvd/lmvm_results_all/draw_all_hist.root");
   results.push_back(dir + "mar15_trdclustering/8gev/stsv13d/richv14a_1e/trd4/tofv13/1.0field/nomvd/lmvm_results_all/draw_all_hist.root");
   results.push_back(dir + "mar15_rtcut/8gev/stsv13d/richv14a_1e/notrd/tofv13/1.0field/nomvd/lmvm_results_all/draw_all_hist.root");

   names.push_back("TRD smearing");
   names.push_back("TRD clustering");
   names.push_back("NO TRD");

   outputDir = "/Users/slebedev/Development/cbm/data/lmvm/lmvm_study_report_8gev_trd/";

   CbmAnaDielectronReports reports;
   reports.CreateStudyReport("LMVM", results, names, outputDir);
}
