/*  Description: This macro train and test different electron/pion identification methods in the TRD detector.

 Author : Semen Lebedev
 E-mail : S.Lebedev@gsi.de
 */
#include <vector>
using namespace std;

void trd_elid_train()
{

   // ----  Load libraries   -------------------------------------------------
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();
   gSystem->Load("libTMVA");


   int nofTrdLayers = 12;
   string outputDir = "results/temp/";
   int idMethod = 4; //kANN = 0, kBDT = 1, kMEDIANA = 2, kLIKELIHOOD = 3, kMeanCut = 4
   string radiatorName = "4mm_foam";

   TString script = TString(gSystem->Getenv("SCRIPT"));
   if (script == "yes"){
      nofTrdLayers = TString(gSystem->Getenv("NOF_TRD_LAYERS")).Atoi();
      outputDir = string(gSystem->Getenv("OUTPUT_DIR"));
      idMethod = TString(gSystem->Getenv("ID_METHOD")).Atoi();
      radiatorName = string(gSystem->Getenv("RADIATOR_NAME"));
   }

	CbmTrdElectronsTrainAnn* trainer = new CbmTrdElectronsTrainAnn(nofTrdLayers);

 /*	trainer->SetBeamDataFile("cern_oct_11_munster/"+radiatorName);
  trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2210010_Rad_G10_Sus02.root");
   trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2310008_Rad_A_Sus02.root");
   trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2310003_Rad_B_Sus02.root");
   trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2410003_Rad_H++_Sus02.root");
   //Spectra_Be_run2210012_Rad_G10_Sus02.root
   //Spectra_Be_run2310007_Rad_A_Sus02.root

   trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2310013_Rad_G30_Sus06.root");
   trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2210010_Rad_G10_Sus06.root");
   trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2310010_Rad_A_Sus06.root");
   //Spectra_Be_run2210011_Rad_G10_Sus06.root
   //Spectra_Be_run2210012_Rad_G10_Sus06.root
   //Spectra_Be_run2310015_Rad_G30_Sus06.root
   ///Spectra_Be_run2310014_Rad_G30_Sus06.root
   //Spectra_Be_run2310011_Rad_A_Sus06.root
   //Spectra_Be_run2410003_Rad_G30_Sus06.root
   //Spectra_Be_run2310026_Rad_G30_Sus06.root

   trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2210010_Rad_G10_Sus12.root");
   trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2310007_Rad_G20_Sus12.root");
   //Spectra_Be_run2210011_Rad_G10_Sus12.root
   //Spectra_Be_run2310008_Rad_G20_Sus12.root
   //Spectra_Be_run2210012_Rad_G10_Sus12.root

   trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2310004_Rad_F_Sus18.root");
   trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2210010_Rad_G10_Sus18.root");
   trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2310014_Rad_B_Sus18.root");
   trainer->SetBeamDataFile("cern_oct_11_munster/Spectra_Be_run2310010_Rad_G20_Sus18.root");
   //Spectra_Be_run2210011_Rad_G10_Sus18.root
   //Spectra_Be_run2310015_Rad_B_Sus18.root
   //Spectra_Be_run2210012_Rad_G10_Sus18.root
   //Spectra_Be_run2310026_Rad_B_Sus18.root
   //Spectra_Be_run2410003_Rad_B_Sus18.root
   //Spectra_Be_run2310011_Rad_G20_Sus18.root
   //Spectra_Be_run2310013_Rad_B_Sus18.root

   trainer->SetBeamDataPiHist("pionSpectrum");
   trainer->SetBeamDataElHist("electronSpectrum");
*/

	trainer->SetBeamDataFile("cern_oct_11_fra/qplots_andrey.root");
   trainer->SetBeamDataPiHist("pi_" + radiatorName);
   trainer->SetBeamDataElHist("el_" + radiatorName);
	//trainer->SetBeamDataPiHist("pi_4mm_foam");
	//trainer->SetBeamDataElHist("el_4mm_foam");
   //trainer->SetBeamDataPiHist("pi_5mm_fibre");
   //trainer->SetBeamDataElHist("el_5mm_fibre");
   //trainer->SetBeamDataPiHist("pi_4mm_f350");
   //trainer->SetBeamDataElHist("el_4mm_f350");
	trainer->SetOutputDir(outputDir);
   //kANN = 0, kBDT = 1, kMEDIANA = 2, kLIKELIHOOD = 3, kMeanCut = 4
   trainer->SetIdMethod(idMethod);
   if (idMethod == 0){
      trainer->SetTransformType(1);
      trainer->SetNofTrainSamples(2500);
   }
   if (idMethod == 1){
      trainer->SetTransformType(2);
      trainer->SetNofTrainSamples(150000);
   }
   trainer->SetIsDoTrain(true);
   trainer->SetSigmaError(0.0);

	trainer->RunBeamData();
}
