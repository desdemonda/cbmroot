// -------------------------------------------------------------------------
// -----                    CbmTrdFindHits source file                 -----
// -----                  Created 16/12/09  by F. Uhlig                -----
// -----               Wrapper for different Trd Hit finder            -----
// -----      Depending on the input data level choose the correct     -----
// -----               version of the hitproducer/hitfinder            -----
// -------------------------------------------------------------------------


#include "CbmTrdFindHits.h"
#include "CbmTrdHitProducerDigi.h"
#include "CbmTrdHitProducerSmearing.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTrdFindHits::CbmTrdFindHits() :
  FairTask("Cbm Trd hitfinding task")
{
  this->Add(new CbmTrdHitProducerDigi("CbmTrdHitProducerDig"));
  this->Add(new CbmTrdHitProducerSmearing("CbmTrdHitProducerSmearing"));
 
  TList* thistasks = this->GetListOfTasks();
  for(Int_t i=0;i<thistasks->GetEntries();i++)
  {
    ((FairTask*)thistasks->At(i))->SetVerbose(fVerbose);
    ((FairTask*)thistasks->At(i))->SetActive(kFALSE);
  }
}
// -------------------------------------------------------------------------

// -----   Default constructor   -------------------------------------------
CbmTrdFindHits::CbmTrdFindHits(const char *name, const char *title, Int_t iVerbose) :
  FairTask(name,iVerbose)
{
  this->Add(new CbmTrdHitProducerDigi("CbmTrdHitProducerDigi"));
  this->Add(new CbmTrdHitProducerSmearing("CbmTrdHitProducerSmearing"));
 
  TList* thistasks = this->GetListOfTasks();
  for(Int_t i=0;i<thistasks->GetEntries();i++)
  {
    ((FairTask*)thistasks->At(i))->SetVerbose(iVerbose);
    ((FairTask*)thistasks->At(i))->SetActive(kFALSE);
  }
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmTrdFindHits::~CbmTrdFindHits()
{
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void CbmTrdFindHits::SetParContainers()
{
}

InitStatus CbmTrdFindHits::ReInit()
{
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus CbmTrdFindHits::Init()
{

  cout<<" *  CbmTrdFindHits::Init() "<<endl;

  InitStatus ReturnValue=kERROR;

    FairRootManager *ioman = FairRootManager::Instance();

    TClonesArray *TrdCluster = (TClonesArray*) ioman->GetObject("TRDCluster");
    if ( TrdCluster ) {
      cout << "--I-- CbmTrdFindHits::Init " << endl;
      cout << "--I-- activate CbmTrdFindHitsFromCluster " << endl;
      TList* thistasks = this->GetListOfTasks();
      for(Int_t i=0;i<thistasks->GetEntries();i++)
      {
        TString name = ((FairTask*)thistasks->At(i))->GetName();
        if (name.CompareTo("CbmTrdHitProducerCluster")) {
          cout<<"Name of Task: "<<name<<endl;
          ((FairTask*)thistasks->At(i))->SetActive(kTRUE);
        }
      }

      return kSUCCESS;
    }   
    //TrdDigi
    TClonesArray *TrdDigi = (TClonesArray*) ioman->GetObject("TRDDigi");
    if ( TrdDigi ) {
      cout << "--I-- CbmTrdFindHits::Init " << endl;
      cout << "--I-- activate CbmTrdHitProducerDigi" << endl;
      TList* thistasks = this->GetListOfTasks();
      for(Int_t i=0;i<thistasks->GetEntries();i++)
      {
        TString name = ((FairTask*)thistasks->At(i))->GetName();
        if (name.CompareTo("CbmTrdHitProducerDigi")) {
          cout<<"Name of Task: "<<name<<endl;
          ((FairTask*)thistasks->At(i))->SetActive(kTRUE);
        }
      }

      return kSUCCESS;
    }   

    TClonesArray *TrdPoint = (TClonesArray*) ioman->GetObject("TRDPoint");
    if ( TrdPoint ) {
      cout << "--I-- CbmTrdFindHits::Init " << endl;
      cout << "--I-- activate CbmTrdHitProducerSmearing" << endl;
      TList* thistasks = this->GetListOfTasks();
      for(Int_t i=0;i<thistasks->GetEntries();i++)
      {
        FairTask *task = ((FairTask*)thistasks->At(i));
        TString name = task->GetName();
        task->SetActive(kFALSE);
        Int_t comp=name.CompareTo("CbmTrdHitProducerSmearing");
        cout<<"1)Name of Task: "<<name<<" is "<<task->IsActive()<<" , "<<comp<<endl;
  


        if ( 0 == name.CompareTo("CbmTrdHitProducerSmearing")) {
          task->SetActive(kTRUE);
          cout<<"2)Name of Task: "<<name<<" is "<<task->IsActive()<<endl;
        }
      }

      return kSUCCESS;
    } else {

      cout << "--W-- CbmTrdFindHits::Init" << endl;
      cout << "--W-- No of the required data levels" << endl;
      cout << "--W-- (Cluster, Digi or Point) found" << endl;
      cout << "--W-- Task will be inactive" << endl;
      return kERROR;

    }

    return ReturnValue;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmTrdFindHits::Exec(Option_t* opt)
{
  return;
}


ClassImp(CbmTrdFindHits);
