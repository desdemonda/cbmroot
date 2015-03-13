// --------------------------------------------------------------------------
// $Id $
// Macro for elctron identification from already reconstructed events
//
// author: Supriya Das (S.Das@gsi.de) 
//
// --------------------------------------------------------------------------

void CbmDileptonPid( const char* inputFileNameMC, const char* inputFileNameRec, const char* outputFileName, const char* parFile)
{

    // Load libraries
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libGeoBase");
    gSystem->Load("libParBase");
    gSystem->Load("libBase");
    gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
    gSystem->Load("libField");
    gSystem->Load("libGen");
    gSystem->Load("libPassive");
    gSystem->Load("libMvd");
    gSystem->Load("libSts");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libTof");
    gSystem->Load("libEcal");
    gSystem->Load("libGlobal");
    gSystem->Load("libKF");
    gSystem->Load("libL1");

    gSystem->Load("libDilept");
    
    // STS digitisation file
    TString stsDigiFile = "sts_standard.digi.par";

    // Create CBM Run
    FairRunAna* fRun = new FairRunAna();

    // Set the MC version used
    //    fRun->SetName("TGeant3");

    // Add input/output files to run
    fRun->SetInputFile( inputFileNameRec );
    fRun->SetOutputFile( outputFileName );
    fRun->AddFriend( inputFileNameMC );
     
    // Init simulation parameters from root file

    TString stsDigi = gSystem->Getenv("VMCWORKDIR");
    stsDigi += "/parameters/sts/";
    stsDigi += stsDigiFile;
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(parFile);
    parIo2->open(stsDigi.Data(),"in");
    rtdb->setFirstInput(parIo1);
    rtdb->setSecondInput(parIo2);

    // Create and add users tasks

    // CbmKF is needed for Extrapolation
    CbmKF* kf = new CbmKF();
    fRun->AddTask(kf);
           
    // Electron id assignment 
    CbmDileptonAssignPid* taskPid = new CbmDileptonAssignPid(1);

    Float_t aRichCuts[] = {1., 0.4, 0., 130, 21.85, 4.35, 6.17, 0.14}; // Distance, SelectionNN, selection2D, Radial, NHitMean, NHitSigma, RMean, RSigma
    taskPid->SetRichCuts(true, true, aRichCuts); //sMomentum, sSelection (true=NN, false=2D)
    
    Float_t aTrdCuts[] = {1.5, 0.95, 1.1, 21., 0.9}; // Mom, PidLikeLow, PidLikeHigh, PidWkn, PidAnn
    taskPid->SetTrdCuts(false, false, true, false, false, aTrdCuts); //sMom, sAccept, sLike, sWkn, sAnn
    
    taskPid->SetTofCuts(true, false, 0.01); //sMom, sAccept, Mass2
    
    fRun->AddTask(taskPid);
   
    // MC Association, needed for simulated data stream
    CbmDileptonAssignMCid* taskMCid = new CbmDileptonAssignMCid();
    taskMCid->SetCuts(3.,130.,4); //chi2primary, radial, sts
    fRun->AddTask(taskMCid);

    // Init and Run
    fRun->Init();
    fRun->Run(0,10);

}
