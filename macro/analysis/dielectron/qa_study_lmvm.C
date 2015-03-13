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

 /*  std::string dir = "/Users/slebedev/Development/cbm/data/lmvm/8gev/1.0field/nomvd/";

   results.push_back(dir + "100field/nomvd/all/real_pid/");
   results.push_back(dir + "100field/mvd/all/real_pid/");
   results.push_back(dir + "70field/nomvd/all/real_pid/");
   results.push_back(dir + "70field/mvd/all/real_pid/");

   names.push_back("100field/nomvd");
   names.push_back("100field/mvd");
   names.push_back("70field/nomvd");
   names.push_back("70field/mvd");*/

   std::string dir = "/Users/slebedev/Development/cbm/data/lmvm/8gev/1.0field/nomvd/results_all/";
   results.push_back(dir + "real_pid/");
   results.push_back(dir + "pimisid.0.01/");
   results.push_back(dir + "pimisid.0.002/");
   results.push_back(dir + "pimisid.0.001/");
   results.push_back(dir + "pimisid.0.0002/");
   results.push_back(dir + "pimisid.0.0001/");
   results.push_back(dir + "pimisid.0.0/");

   names.push_back("real pid");
   names.push_back("0.01");
   names.push_back("0.002");
   names.push_back("0.001");
   names.push_back("0.0002");
   names.push_back("0.0001");
   names.push_back("ideal pid");


   outputDir = "/Users/slebedev/Development/cbm/data/lmvm/8gev/1.0field/nomvd/study_report_pimisid/";

   CbmAnaDielectronReports reports;
   reports.CreateStudyReport("LMVM", results, names, outputDir);
}
