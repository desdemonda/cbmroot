using namespace std;
void draw_analysis_study()
{
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();

   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();

   gSystem->Load("libAnalysis");

   //std::string fileName = "analysis.pimisid.0.01.auau.8gev.centr.all.root";
   std::string energy = "25gev";
   std::string fileName = "analysis."+energy+".centr.all.root";
   Bool_t useMvd = false;

   std::string outputDir = "/Users/slebedev/Development/cbm/data/lmvm/feb13_new/"+energy+"/results_field/";
  // std::string outputDir = "results/25gev/study/";

 /*  vector<string> files;
   string dir = "/Users/slebedev/Development/cbm/data/lmvm/feb13_new/"+energy+"/1.0field/nomvd/rho0/";
   files.push_back(dir + "analysis.pisupp0.01."+energy+".centr.all.root");
   files.push_back(dir + "analysis.pisupp0.002."+energy+".centr.all.root");
   files.push_back(dir + "analysis.pisupp0.001."+energy+".centr.all.root");
   files.push_back(dir + "analysis.pisupp0.0004."+energy+".centr.all.root");
   files.push_back(dir + "analysis.pisupp0.0002."+energy+".centr.all.root");
   files.push_back(dir + "analysis.pisupp0.0001."+energy+".centr.all.root");
   files.push_back(dir + "analysis.pisupp0.0."+energy+".centr.all.root");

   vector<string> filesMean;
   string dir = "/Users/slebedev/Development/cbm/data/lmvm/feb13_new/"+energy+"/1.0field/nomvd/results_all/";
   filesMean.push_back(string(dir + "pisupp0.01/mean_hist.root"));
   filesMean.push_back(string(dir + "pisupp0.002/mean_hist.root"));
   filesMean.push_back(string(dir + "pisupp0.001/mean_hist.root"));
   filesMean.push_back(string(dir + "pisupp0.0004/mean_hist.root"));
   filesMean.push_back(string(dir + "pisupp0.0002/mean_hist.root"));
   filesMean.push_back(string(dir + "pisupp0.0001/mean_hist.root"));
   filesMean.push_back(string(dir + "pisupp0.0/mean_hist.root"));

   vector<string> studyNames;
   studyNames.push_back("1/100");
   studyNames.push_back("1/500");
   studyNames.push_back("1/1000");
   studyNames.push_back("1/2500");
   studyNames.push_back("1/5000");
   studyNames.push_back("1/10000");
   studyNames.push_back("ideal");*/

   vector<string> files;
   string dir = "/Users/slebedev/Development/cbm/data/lmvm/feb13_new/"+energy;
   files.push_back(dir +"/0.7field/nomvd/rho0/" + "analysis."+energy+".centr.all.root");
   files.push_back(dir + "/1.0field/nomvd/rho0/" + "analysis."+energy+".centr.all.root");

   vector<string> filesMean;
   string dir = "/Users/slebedev/Development/cbm/data/lmvm/feb13_new/"+energy;
   filesMean.push_back(string(dir + "/0.7field/nomvd/results_all/" + "mean_hist.root"));
   filesMean.push_back(string(dir + "/1.0field/nomvd/results_all/realpid/" + "mean_hist.root"));

   vector<string> studyNames;
   studyNames.push_back("70% B-field");
   studyNames.push_back("100% B-field");

   CbmAnaLmvmDrawStudy *draw = new CbmAnaLmvmDrawStudy();
   draw->DrawFromFile(files, filesMean, studyNames, outputDir);
}
