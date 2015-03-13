/**
 * \file radlength_ana.C
 * \brief Macro runs radiathin length QA task.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
void radlength_ana(Int_t nEvents = 1210000)
{
   TString script = TString(gSystem->Getenv("LIT_SCRIPT"));

   TString dir = "events/radlength_rich_v13c/"; // Output directory
   TString mcFile = dir + "radlength.mc.0000.root"; // MC transport file
   TString parFile = dir + "radlength.param.0000.root"; // Parameter file
   TString radLengthQaFile = dir + "radlength.qa.root"; // MC transport file
   TString resultDir = "events/results_radlength_rich_v13c/";

   if (script == "yes") {
      mcFile = TString(gSystem->Getenv("LIT_MC_FILE"));
      parFile = TString(gSystem->Getenv("LIT_PAR_FILE"));
      radLengthQaFile= TString(gSystem->Getenv("LIT_RAD_LENGTH_QA_FILE"));
      resultDir = TString(gSystem->Getenv("LIT_RESULT_DIR"));
   }

   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   // -----   Reconstruction run   -------------------------------------------
   FairRunAna *run= new FairRunAna();
   run->SetInputFile(mcFile);
   run->SetOutputFile(radLengthQaFile);
   // ------------------------------------------------------------------------

   CbmLitRadLengthQa* radLengthQa = new CbmLitRadLengthQa();
   radLengthQa->SetOutputDir(std::string(resultDir));
   run->AddTask(radLengthQa);

   // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   parIo1->open(parFile.Data());
   rtdb->setFirstInput(parIo1);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();
   // ------------------------------------------------------------------------

   // -----   Initialize and run   --------------------------------------------
   run->Init();
   run->Run(0, nEvents);
   // ------------------------------------------------------------------------

   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   std::cout << "Macro finished successfully." << std::endl;
   std::cout << "Output file is " << radLengthQaFile << std::endl;
   std::cout << "Parameter file is " << parFile << std::endl;
   std::cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << std::endl;
   // ------------------------------------------------------------------------
}
