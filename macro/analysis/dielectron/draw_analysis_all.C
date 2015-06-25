/** draw_analysis.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 3.0
 **/

void draw_analysis_all() {
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
	loadlibs();
    gSystem->Load("libAnalysis");

    Bool_t useMvd = false;
    std::string dir = "/Users/slebedev/Development/cbm/data/lmvm/mar15/25gev/stsv13d/richv14a_3e/trd10/tofv13/1.0field/nomvd/";
    std::string fileName = "analysis.auau.25gev.centr.all.root";
    //std::string fileName = "analysis.pimisid.0.0.auau.8gev.centr.all.root";


    std::string script = std::string(TString(gSystem->Getenv("SCRIPT")).Data());
    if (script == "yes"){
       dir = std::string(TString(gSystem->Getenv("LMVM_MAIN_DIR")).Data());
       fileName = std::string(TString(gSystem->Getenv("LMVM_ANA_FILE_NAME")).Data());
    }

    std::string outputDir = dir + "lmvm_results_all/";
    std::string fnRho0 = dir + "rho0/" + fileName;
    std::string fnOmega = dir +"omegaepem/" + fileName;
    std::string fnPhi = dir + "phi/" + fileName;
    std::string fnOmegaD = dir +"omegadalitz/" + fileName;

    CbmAnaDielectronTaskDrawAll *draw = new CbmAnaDielectronTaskDrawAll();
    draw->DrawHistosFromFile(fnRho0, fnOmega, fnPhi, fnOmegaD, outputDir, useMvd);
}
