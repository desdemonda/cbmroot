void run_analysis(Int_t nEvents = 2, Int_t mode = 0, Int_t file_nr = 1)
	// mode 1 = tomography
	// mode 2 = urqmd
	// mode 3 = pluto
{
	if(mode == 0) {
		cout << "ERROR: No mode specified! Exiting..." << endl;
		exit();
	}
   TTree::SetMaxTreeSize(90000000000);

	Int_t iVerbose = 0;
	
	Char_t filenr[5];
	sprintf(filenr,"%05d",file_nr);
	printf("Filenr: %s\n", filenr);
	TString temp = filenr;
//	TString temp = "00003";

	TString script = TString(gSystem->Getenv("SCRIPT"));
	TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
	TString stsMatBudgetFileName = parDir + "/sts/sts_matbudget_v12b.root"; // Material budget file for L1 STS tracking

	gRandom->SetSeed(10);

	TString mcFile = 		"";
	TString parFile = 		"";
	TString recoFile =		"";
	TString analysisFile =	"";
	
	
	TString outName = "tomo_0to60deg";
	if(mode == 1) {	// tomography
		mcFile =		"/common/home/reinecke/CBM-Simulationen/outputs/tomography." + outName + ".mc.0001.root";
		parFile =		"/common/home/reinecke/CBM-Simulationen/outputs/tomography." + outName + ".param.0001.root";
		recoFile =		"/common/home/reinecke/CBM-Simulationen/outputs/tomography." + outName + ".reco.0001.root";
		analysisFile =	"/common/home/reinecke/CBM-Simulationen/outputs/tomography." + outName + ".analysis.0001.root";
	}
	if(mode == 2) {	// urqmd
		mcFile =		"/common/home/reinecke/CBM-Simulationen/outputs/urqmd/urqmd." + outName + ".mc." + temp + ".root";
		parFile =		"/common/home/reinecke/CBM-Simulationen/outputs/urqmd/urqmd." + outName + ".param." + temp + ".root";
		recoFile =		"/common/home/reinecke/CBM-Simulationen/outputs/urqmd/urqmd." + outName + ".reco." + temp + ".root";
		analysisFile =	"/common/home/reinecke/CBM-Simulationen/outputs/urqmd/urqmd." + outName + ".analysis." + temp + ".root";
	}
	if(mode == 3) {	// pluto
		mcFile =		"/common/home/reinecke/CBM-Simulationen/outputs/pluto." + outName + ".mc.0001.root";
		parFile =		"/common/home/reinecke/CBM-Simulationen/outputs/pluto." + outName + ".param.0001.root";
		recoFile =		"/common/home/reinecke/CBM-Simulationen/outputs/pluto." + outName + ".reco.0001.root";
		analysisFile =	"/common/home/reinecke/CBM-Simulationen/outputs/pluto." + outName + ".analysis.0001.root";
	}
	
	


	if (script == "yes") {
		mcFile = TString(gSystem->Getenv("MC_FILE"));
		recoFile = TString(gSystem->Getenv("RECO_FILE"));
		parFile = TString(gSystem->Getenv("PAR_FILE"));
	}

   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
   TList *parFileList = new TList();
   TObjString stsDigiFile = parDir + "/sts/sts_v13d_std.digi.par"; // STS digi file
   TObjString trdDigiFile = parDir + "/trd/trd_v13g.digi.par"; // TRD digi file
   TObjString tofDigiFile = parDir + "/tof/tof_v13b.digi.par"; // TOF digi file

   parFileList->Add(&stsDigiFile);
   parFileList->Add(&trdDigiFile);
   parFileList->Add(&tofDigiFile);
   gDebug = 0;

    TStopwatch timer;
    timer.Start();

	// ----  Load libraries   -------------------------------------------------
   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/determine_setup.C");

	// -----   Reconstruction run   -------------------------------------------
	FairRunAna *run= new FairRunAna();
	if (mcFile != "") run->SetInputFile(mcFile);
	if (recoFile != "") run->SetOutputFile(analysisFile);
	run->AddFriend(recoFile);




	CbmKF* kalman = new CbmKF();
	run->AddTask(kalman);
	CbmL1* l1 = new CbmL1();
    l1->SetMaterialBudgetFileName(stsMatBudgetFileName);
	run->AddTask(l1);


   	CbmAnaConversion* conversionAna = new CbmAnaConversion();
   	conversionAna->SetMode(mode);
   	run->AddTask(conversionAna);
   	
   	
// ----- PID for KF Particle Finder --------------------------------------------
   CbmKFParticleFinderPID* kfParticleFinderPID = new CbmKFParticleFinderPID();
	kfParticleFinderPID->SetPIDMode(1); //mc
//	kfParticleFinderPID->SetPIDMode(2); //tof
   run->AddTask(kfParticleFinderPID);
  
   // ----- KF Particle Finder --------------------------------------------
   CbmKFParticleFinder* kfParticleFinder = new CbmKFParticleFinder();
   kfParticleFinder->SetPIDInformation(kfParticleFinderPID);
   run->AddTask(kfParticleFinder);

   // ----- KF Particle Finder QA --------------------------------------------
   CbmKFParticleFinderQA* kfParticleFinderQA = new CbmKFParticleFinderQA("CbmKFParticleFinderQA", 0, kfParticleFinder->GetTopoReconstructor(),"");
   kfParticleFinderQA->SetPrintEffFrequency(1000);
   TString effFileName = analysisFile;
   effFileName += ".eff_mc.txt";
   kfParticleFinderQA->SetEffFileName(effFileName.Data());
   run->AddTask(kfParticleFinderQA);

    // =========================================================================
    // ===                        ECAL reconstruction                        ===
    // =========================================================================

//  // -----   ECAL hit producer  ----------------------------------------------
//  CbmEcalHitProducerFastMC* ecalHitProd
//    = new CbmEcalHitProducerFastMC("ECAL Hitproducer");
//  run->AddTask(ecalHitProd);

    // -----  Parameter database   --------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(parFile.Data());
    parIo2->open(parFileList, "in");
    rtdb->setFirstInput(parIo1);
    rtdb->setSecondInput(parIo2);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();

    run->Init();
    cout << "Starting run" << endl;
    run->Run(0,nEvents);
    
    cout << "\n\n" << endl;
    cout << "######## RESULTS #########" << endl;
    cout << "Test: " << conversionAna->GetTest() << endl;
    cout << "Nof Events: " << conversionAna->GetNofEvents() << endl;
    cout << "Fraction: " <<  1.0 * conversionAna->GetTest() / conversionAna->GetNofEvents() << endl;

    // -----   Finish   -------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "######## Finish ########" << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Output file is "    << analysisFile << endl;
    cout << "Parameter file is " << parFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;

    cout << " Test passed" << endl;
    cout << " All ok " << endl;
	
	Int_t analyseMode = mode;
	ofstream outputfile("log.txt", mode = ios_base::app);
	if(!outputfile) {
		cout << "Error!" << endl;
	}
	else {
		TTimeStamp testtime;
		outputfile << "########## run_analysis.C ##########" << endl;
		outputfile << "Date (of end): " << testtime.GetDate() << "\t Time (of end): " << testtime.GetTime() << " +2" << endl;
		outputfile << "Output file is "    << analysisFile << endl;
		outputfile << "Parameter file is " << parFile << endl;
		outputfile << "Number of events: " << nEvents << "\t mode: " << analyseMode << endl;
		outputfile << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
		outputfile.close();
	}
}
