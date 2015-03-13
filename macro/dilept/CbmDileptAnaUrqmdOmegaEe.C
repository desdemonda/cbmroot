// --------------------------------------------------------------------------
//
// Macro for di - electron analysis
// Not yet reconstructed tracks
// MC level, ideal simulation
//
// author: T. Galatyuk  29/05/2006
//
// --------------------------------------------------------------------------

{
    // Load libraries
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    gSystem->Load("libGeoBase");
    gSystem->Load("libParBase");
    gSystem->Load("libBase");
    gSystem->Load("libField");
    gSystem->Load("libGen");
    gSystem->Load("libPassive");
    gSystem->Load("libSts");
    gSystem->Load("libRich");
    gSystem->Load("libTrd");
    gSystem->Load("libTof");
    gSystem->Load("libGlobal");
    gSystem->Load("libKF");
    gSystem->Load("libL1");

    gSystem->Load("libDilept");

    TString inputFileNameMC  = "urqmd_omega_ee.auau.25gev.centr.1k.root";
    TString inputFileNameRec = "reco_urqmd_omega_ee.auau.25gev.centr.1k.root";
    TString outputFileName   = "urqmd_omega_ee.auau.25gev.centr.1k.spectra.root";

    FairRunAna* fRun = new FairRunAna();

    // Set the MC version used
    fRun->SetName("TGeant3");

    fRun->SetInputFile( inputFileNameRec.Data() );
    fRun->SetOutputFile( outputFileName.Data() );

    fRun->AddFriend( inputFileNameMC.Data() );

    // Init simulation parameters from root file
    FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
    FairParRootFileIo* input=new FairParRootFileIo();
    input->open("urqmd_omega_ee.auau.25gev.centr.1k.params.root");
    rtdb->setFirstInput(input);

    // Create and add task
    CbmDiLeptTask *task = new CbmDiLeptTask("name");

    // Weight omega = N * BR = 38 * 7.07x10-5 = 0.0026866
    task->SetWeight(0.0026866);

    task->SetGcut(0.025);      // Gamma conversion cut (GeV/c^2)
    task->SetDcut(0.15);       // Distance to the closest neigbour (cm)

    task->Set2Dpcut(2.);       // Invariant mass of closest neighbour (GeV/c)
    task->Set2Dtcut(2.);       // Opening angle of closest neighbour (deg)

    task->SetPtcut(0.2);       // Transverse momentum of the identified tracks (GeV/c)
    task->SetTcut(2.);         // Opening angle (deg)
    task->SetPi0cut(0.2);      // Invariant mass of the pi0 dalitz decay (GeV/c^2)

    task->SetCoutBunch(1);

    fRun->AddTask(task);

    fRun->Init();
    fRun->Run(0, 10);

    task->WriteOutput();
}
