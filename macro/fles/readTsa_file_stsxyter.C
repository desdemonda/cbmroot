
void readTsa_file_stsxyter(TString inFile = "stsxdata.tsa")
{
    Int_t nEvents = 1815;
    TString outFile = "data/test_online.root";

    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
    gDebug = 0;

    std::cout << std::endl;
    std::cout << ">>> readTsa:  input file is " << inFile  << std::endl;
    std::cout << ">>> readTsa: output file is " << outFile << std::endl;


    std::cout << std::endl;
    std::cout << ">>> readTsa: Initialising..." << std::endl;

    CbmFlibFileSourceNew* source = new CbmFlibFileSourceNew();
    //source->SetHostName("cbmflib01");
    source->SetFileName(inFile);

    CbmTSUnpackStsxyter * stsxyter_unpacker = new CbmTSUnpackStsxyter();
    source->AddUnpacker( stsxyter_unpacker, 0x10 );

    FairRunOnline *run = new FairRunOnline(source);
    run->SetOutputFile(outFile);

    run->Init();

    TStopwatch timer;
    timer.Start();
    std::cout << ">>> readTsa: Starting run..." << std::endl;
    run->Run(nEvents, 0);
    timer.Stop();

    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    std::cout << std::endl << std::endl;
    std::cout << ">>> readTsa: Macro finished successfully." << std::endl;
    std::cout << ">>> readTsa: Output file is " << outFile << std::endl;
    std::cout << ">>> readTsa: Real time " << rtime << " s, CPU time "
            << ctime << " s" << std::endl;
    std::cout << std::endl;

    std::cout << " Test passed" << std::endl;
    std::cout << " All ok " << std::endl;
}
