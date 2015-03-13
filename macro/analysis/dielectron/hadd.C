using namespace std;

void hadd() {
    gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
    loadlibs();
    gSystem->Load("libAnalysis");

    string dir = "/hera/cbm/users/slebedev/mc/dielectron/dec14/8gev/stsv14_4cm/richv14a/trd4/tofv13/1.0field/nomvd/";
    string fileArray = ".auau.8gev.centr.";
    string addString = "litqa"; //analysis or litqa
    int nofFiles = 250;

    TString script = TString(gSystem->Getenv("SCRIPT"));
    if (script == "yes") {
    	addString = string(gSystem->Getenv("LMVM_ADD_STRING"));
    	fileArray = string(gSystem->Getenv("LMVM_FILE_ARRAY"));
    	dir = string(gSystem->Getenv("LMVM_MAIN_DIR"));
        nofFiles = TString(gSystem->Getenv("LMVM_NOF_FILES")).Atof();
    }

    CbmLmvmHadd* hadd = new CbmLmvmHadd();
    hadd->AddFiles(dir, fileArray, addString, nofFiles);
}
