#include "TTree.h"
void test(){
  TObjString* b1 = new TObjString();
  TObjString* b2 = new TObjString();
  
  TTree* t = new TTree("cbmsim","cbmsim",99);
  t->Branch("b1",b1);
  t->Branch("b2",b2);
  b1->SetString("a1");
  b2->SetString("a2");
  t->Fill();
  b1->SetString("c1");
  b2->SetString("c2");
  t->Fill();
  
  t->GetEntry(0);
  printf(" Branch1: %s",b1->GetString().Data());
  printf(" Branch2: %s",b2->GetString().Data());
  printf("\n");

  t->GetBranch("b1")->GetEntry(1);
  printf(" Branch1: %s",b1->GetString().Data());
  printf(" Branch2: %s",b2->GetString().Data());
  printf("\n");
}
