void eloss_ana(Int_t nEvents = 20000)
{
  TString dir = "/home/d/andrey/eloss/";
  TString inFile = dir + "eloss.mc.root";
  TString outFile = dir + "eloss.ana.root";

   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
   cbmrootlibs();

   FairRunAna *run= new FairRunAna();
   run->SetInputFile(inFile);
   run->SetOutputFile(outFile);

   CbmLitCheckMultipleScattering* msCheck = new CbmLitCheckMultipleScattering();
   msCheck->SetOutputDir("/home/d/andrey/eloss/");
   run->AddTask(msCheck);

   run->Init();
   run->Run(0,nEvents);


   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   cout << endl << endl;
   cout << "Macro finished succesfully." << endl;
   cout << "Output file is "    << outFile << endl;
   cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << endl;
   cout << endl;
   // ------------------------------------------------------------------------
}
