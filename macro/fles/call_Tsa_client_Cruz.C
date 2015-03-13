void call_Tsa_client_Cruz(Int_t nMin = 1){
  Int_t nEvents = nMin*60*10; //minutes * seconds * 10Hz TimeSlices
  FairRunOnline* run = new FairRunOnline();
  gROOT->ProcessLine(".x readTsa_server_Cruz.C");
  run->Run(nEvents);
  //Int_t c;
  //std::cin >> c >> endl;
  //printf("to read additional n envents call\n  run->Run(n);\n");
}
