
void fillm2(Float_t beamMomentum,
	    Float_t timeResolution,
	    Int_t level)
{
    if(level<1 || level>2) return;
    Int_t nEvents = 1000;

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
    gSystem->Load("libHadron");


    char strMCFile[1000];
    char strParamFile[1000];
    char strHadronFile[1000];
    char strOutputFile[1000];

    char *ver = getenv("CBMVER");

    sprintf(strOutputFile, "/home/kresan/data/hadron/%s/urqmd/\
auau/%1.0fgev/centr", ver, beamMomentum);
    sprintf(strOutputFile, "%s/urqmd.auau.%1.0fgev.centr.%1.0fps.m2mom",
	    strOutputFile, beamMomentum, timeResolution);
    if(2 == level) {
	sprintf(strOutputFile, "%s.reco", strOutputFile);
    }
    sprintf(strOutputFile, "%s.root", strOutputFile);

    sprintf(strMCFile, "/home/kresan/data/mc/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.0000.mc.root", ver, beamMomentum, beamMomentum);
    sprintf(strParamFile, "/home/kresan/data/mc/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.0000.mc.param.root", ver, beamMomentum, beamMomentum);
    if(2 == level) {
	sprintf(strHadronFile, "/home/kresan/data/dst/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.0000.dst.root",
		ver, beamMomentum, beamMomentum);
    }


    FairRunAna* fRun = new FairRunAna();
    if(1 == level) {
	fRun->SetInputFile(strMCFile);
    } else {
	fRun->SetInputFile(strHadronFile);
    }
/*
    for(Int_t iFile = 1; iFile < 200; iFile++) {
        sprintf(strMCFile, "/lustre/cbm/user/kresan/mc/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.%4d.mc.root", ver, beamMomentum, beamMomentum, iFile);
	if(2 == level) {
	    sprintf(strHadronFile, "/lustre/cbm/user/kresan/dstIdeal/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.%4d.dstIdeal.root",
		    ver, beamMomentum, beamMomentum, iFile);
	} else if(3 == level) {
	    sprintf(strHadronFile, "/lustre/cbm/user/kresan/dst/%s/urqmd/\
auau/%1.0fgev/centr/urqmd.auau.%1.0fgev.centr.%4d.dst.root",
		    ver, beamMomentum, beamMomentum, iFile);
	}
	for(Int_t i = 0; i < 1000; i++) {
            if(' ' == strMCFile[i]) strMCFile[i] = '0';
	    if(' ' == strHadronFile[i]) strHadronFile[i] = '0';
	}
	if(1 == level) {
	    fRun->AddFile(strMCFile);
	} else {
	    fRun->AddFile(strHadronFile);
	}
    }
*/

    fRun->SetOutputFile(strOutputFile);


    // Init Simulation Parameters from Root File
    FairRuntimeDb* rtdb=fRun->GetRuntimeDb();
    FairParRootFileIo* input=new FairParRootFileIo();
    input->open(strParamFile);
    rtdb->setFirstInput(input);



    // --------- Hadron Identification -----------------------
    // create and add hadronTask
    CbmFillM2* hadronTask = new CbmFillM2("FillM2", 1,
					  450, -1.5, 3.0,
					  100, 0., 10.);
    hadronTask->SetCoutBunch(10);
    hadronTask->SetTimeResolution(timeResolution * 1e-3);
    hadronTask->SetLevel(level);
    fRun->AddTask(hadronTask);
    // -------------------------------------------------------



    fRun->Init();


    fRun->Run(0, nEvents);


    hadronTask->WriteHistogramms();
}


