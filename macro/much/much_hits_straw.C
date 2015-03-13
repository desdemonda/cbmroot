/**
 * Performs reconstruction of simulated events
 * in the CBM MuCh setup: STS + MUCH.
 *
 * @author M.Ryzhinskiy m.ryzhinskiy@gsi.de
 * @param inFile         Input transport file
 * @param digiFile       Input file containing segmentation parameters
 * @param outFile        Output file for reconstruction data
 * @param nEvents        Number of events to process
 */
void much_hits_straw(TString inFile = "",
      TString digiFile = "",
      TString outFile = "",
      Int_t nEvents = 5)
{
   TString inputdir = gSystem->Getenv("VMCWORKDIR");
   if (inFile == "") {
      inFile = inputdir + "/macro/much/data/Jpsi.auau.25gev.centr.mc.root";
   }
   if (digiFile == "") {
      digiFile = inputdir + "/parameters/much/much_v11d.digi.root";
   }
   if (outFile == "") {
      outFile = inputdir + "/macro/much/data/Jpsi.auau.25gev.centr.muchhits.root";
   }

   // ========================================================================
   //          Adjust this part according to your requirements

   // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
   Int_t iVerbose = 1;

   // ----  Load libraries   -------------------------------------------------
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/much/muchlibs.C");
   muchlibs();
   // ------------------------------------------------------------------------

   // -----   Reconstruction run   -------------------------------------------
   FairRunAna *fRun= new FairRunAna();
   fRun->SetInputFile(inFile);
   fRun->SetOutputFile(outFile);
   // ------------------------------------------------------------------------


   // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
   FairParRootFileIo*  parIo1 = new FairParRootFileIo();
   parIo1->open(gFile);
   rtdb->setFirstInput(parIo1);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();
   // ------------------------------------------------------------------------

   // ---  MuCh digitizer ----------------------------------------------------
   CbmMuchDigitizeGem* digitize = new CbmMuchDigitizeGem(digiFile);
   fRun->AddTask(digitize);

   // ---  MuCh straw digitizer -----------------------------------------------
   CbmMuchDigitizeStraws* strawDigitize = new CbmMuchDigitizeStraws("MuchDigitizeStraws", digiFile, iVerbose);
   fRun->AddTask(strawDigitize);

   // ------------------------------------------------------------------------

   // ---  MuCh hit finder ---------------------------------------------------
   CbmMuchFindHitsGem* findHits = new CbmMuchFindHitsGem(digiFile);
   fRun->AddTask(findHits);

   // ---  MuCh straw hit finder ----------------------------------------------
   CbmMuchFindHitsStraws* strawFindHits = new CbmMuchFindHitsStraws("MuchFindHitsStraws", digiFile, iVerbose);
   fRun->AddTask(strawFindHits);

   // ------------------------------------------------------------------------

   // -----   Intialise and run   --------------------------------------------
   fRun->Init();
   fRun->Run(0,nEvents);
   // ------------------------------------------------------------------------

   cout << " Test passed" << endl;
   cout << " All ok " << endl;
   exit(0);
}
