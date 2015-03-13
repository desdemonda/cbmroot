{
    gSystem->Load("libcbmnetcntlclientroot.so");
    gSystem->Load("libCbmFlibReader.so");
    gInterpreter->AddIncludePath("$VMCWORKDIR");
    gROOT->ProcessLine(".L config_get4v1x.cxx");
}
