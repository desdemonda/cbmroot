#include <string>

using namespace std;

void run_reco(Int_t nEvents = 100000)
{
   Int_t iVerbose = 0;
   gRandom->SetSeed(10);

   TString dir = "/Users/slebedev/Development/cbm/data/simulations/richprototype/";
   TString inFile = dir + "/mc.root";
   TString parFile = dir + "/params.root";
   TString outFile = dir + "/reco.root";

   TString script = TString(gSystem->Getenv("SCRIPT"));

   int nofNoiseHitsInRich = 5;
   double collectionEff = 1.0;
   double sigmaErrorRich = 0.06;
   double crosstalkRich = 0.02;
   int pmtType = 6;
   string ringFinder = "hough_prototype"; // ideal or hough_prototype

   if (script == "yes") {
      inFile = TString(gSystem->Getenv("MC_FILE"));
      parFile = TString(gSystem->Getenv("PAR_FILE"));
      outFile = TString(gSystem->Getenv("RECO_FILE"));

      nofNoiseHitsInRich = TString(gSystem->Getenv("NOF_NOISE_HITS_IN_RICH")).Atoi();
      collectionEff = TString(gSystem->Getenv("RICH_COLLECTION_EFF")).Atof();
      sigmaErrorRich = TString(gSystem->Getenv("SIGMA_ERROR_RICH")).Atof();
      crosstalkRich = TString(gSystem->Getenv("CROSSTALK_RICH")).Atof();
      pmtType = TString(gSystem->Getenv("PMT_TYPE")).Atoi();
      ringFinder = string(gSystem->Getenv("RING_FINDER"));
   }

   gDebug = 0;
   TStopwatch timer;
   timer.Start();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   FairRunAna *run = new FairRunAna();
   run->SetInputFile(inFile);
   run->SetOutputFile(outFile);

   CbmKF *kalman = new CbmKF();
   run->AddTask(kalman);

   // =========================================================================
   // ===                        RICH reconstruction                        ===
   // =========================================================================
   CbmRichProtHitProducer* richHitProd  = new CbmRichProtHitProducer();
   richHitProd->SetDetectorType(pmtType);
   richHitProd->SetNofNoiseHits(nofNoiseHitsInRich);
   richHitProd->SetCollectionEfficiency(collectionEff);
   richHitProd->SetSigmaMirror(sigmaErrorRich);
   richHitProd->SetCrossTalkHitProb(crosstalkRich);
   run->AddTask(richHitProd);


   CbmRichProtPrepareExtrapolation *richPrepare = new CbmRichProtPrepareExtrapolation(iVerbose);
   richPrepare->SetReso(1./10., 1./1000.);
   run->AddTask(richPrepare);

   CbmRichReconstruction* richReco = new CbmRichReconstruction();
   richReco->SetZTrackExtrapolation(50.);
   richReco->SetMinNofStsHits(0);
   richReco->SetFinderName(ringFinder);
  // richReco->SetFinderName("ideal");
   richReco->SetProjectionName("prototype");
   richReco->SetRunTrackAssign(false);
   run->AddTask(richReco);

   CbmRichMatchRings* matchRings = new CbmRichMatchRings();
   run->AddTask(matchRings);

   CbmRichPrototypeQa* geoQa = new CbmRichPrototypeQa();
  // geoTest->SetOutputDir(resultDir);
   run->AddTask(geoQa);

   // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   parIo1->open(parFile.Data());
   rtdb->setFirstInput(parIo1);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();
   // ------------------------------------------------------------------------


   // -----   Intialise and run   --------------------------------------------
   //run->LoadGeometry();
   run->Init();
   cout << "Starting run" << endl;
   run->Run(0, nEvents);
   // ------------------------------------------------------------------------


   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   cout << endl << endl;
   cout << "Macro finished succesfully." << endl;
   cout << "Output file is " << outFile << endl;
   cout << "Parameter file is " << parFile << endl;
   cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
   cout << endl;
   // ------------------------------------------------------------------------


   cout << " Test passed" << endl;
   cout << " All ok " << endl;
}


