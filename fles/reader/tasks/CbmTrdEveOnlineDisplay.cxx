#include "CbmTrdEveOnlineDisplay.h"

#include "CbmHistManager.h"
#include "CbmTrdRawBeamProfile.h"

#include "FairLogger.h"

#include "TCanvas.h"
#include "TPad.h"
#include "TStyle.h"
#include "TH1.h"
#include "TH2.h"
#include "TString.h"
#include "TROOT.h"
#include "TSeqCollection.h"
#include "TTask.h"
#include "TClass.h"
#include "TDataMember.h"
#include "TMethodCall.h"
#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TGFileBrowser.h"
#include "TMacro.h"
#include "TFolder.h"

TCanvas* g_Canvas = NULL;
TCanvas* g_Spadic1 = NULL;
TGFileBrowser* g_Browser =NULL;

// ---- Default constructor -------------------------------------------
CbmTrdEveOnlineDisplay::CbmTrdEveOnlineDisplay()
  :FairTask("CbmTrdEveOnlineDisplay"),
   //   fCanvas(NULL),
   fSpadic1(NULL),
   fSpadic1a(NULL),
   //   fBrowser(NULL),
   fUpdateInterval(10),
   fEventCounter(0)
{
}

// ---- Destructor ----------------------------------------------------
CbmTrdEveOnlineDisplay::~CbmTrdEveOnlineDisplay()
{
  //  delete fCanvas;
  delete fSpadic1;
  delete fSpadic1a;
  //  delete fBrowser;
}

// ----  Initialisation  ----------------------------------------------
void CbmTrdEveOnlineDisplay::SetParContainers()
{
}

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdEveOnlineDisplay::Init()
{

  TEveManager::Create();

  // --- Create special browser

  gEve->GetBrowser()->StartEmbedding(0);
  g_Browser = gEve->GetBrowser()->MakeFileBrowser();
  gEve->GetBrowser()->StopEmbedding("TRD Canvas");

  TMacro* m;

  m = new TMacro;
  m->AddLine("{ g_Browser->Clear();"
	     "  g_Browser->cd();"
	     "  g_Browser->Update(); }");
  m->SetName("Clear Canvas");
  g_Browser->Add(m);
  
  TH1* h1 = NULL;
  TH1* h2 = NULL;
  gROOT->cd();
  Float_t lsize=0.07;
  gStyle->SetPalette(1);
  gStyle->SetLabelSize(lsize);


  // --- Create an embedded canvas
  
  gEve->GetBrowser()->StartEmbedding(1);
  g_Canvas = new TCanvas;
  gEve->GetBrowser()->StopEmbedding("TRD Canvas");

  gEve->GetBrowser()->StartEmbedding(1);
  g_Spadic1 = new TCanvas;//("fSpadic1", "Spadic1", 0, 0, 1010, 810);  
  gEve->GetBrowser()->StopEmbedding("TRD Canvas");
  g_Spadic1->Divide(3,2);

  // Should be set for each pad of the Canvas
  gPad->SetFillColor(0);
  
  g_Spadic1->cd(1);
  h1=static_cast<TH1*>(gROOT->FindObjectAny("CountRate_SysCore1_Spadic1"));
  if (h1!=NULL) {
    h1->Draw("");
  }
  g_Browser->Add(h1);

  g_Spadic1->cd(2);
  h2=static_cast<TH2*>(gROOT->FindObjectAny("BaseLine_SysCore1_Spadic1"));
  if (h2!=NULL) {
    h2->Draw("COLZ");
  }
  g_Browser->Add(h2);

  g_Spadic1->cd(3);
  h2=static_cast<TH2*>(gROOT->FindObjectAny("Integrated_ADC_Spectrum_SysCore1_Spadic1"));
  if (h2!=NULL) {
    h2->Draw("COLZ");
  }
  g_Browser->Add(h2);

  g_Spadic1->cd(4);
  h2=static_cast<TH2*>(gROOT->FindObjectAny("Trigger_Heatmap_SysCore1_Spadic1"));
  if (h2!=NULL) {
    h2->Draw("COLZ");
  }
  g_Browser->Add(h2);

  //TFolder* Syscore1_Spadic1 = new TFolder("Syscore1_Spadic1","Syscore1_Spadic1");
  //  Syscore1_Spadic1->Add(fSpadic1);
  //g_Browser->Add(Syscore1_Spadic1);

  
  /*

  TCanvas* fSpadic1a = new TCanvas("fSpadic1a", "Spadic1_Signal_Shape", 50, 50, 1510, 810);
  fSpadic1a->Divide(8,4);	
  gPad->SetFillColor(0);

  for (Int_t i=0; i<32; i++) {
    fSpadic1a->cd(i+1);

    TString temphistname = TString::Format("Signal_Shape_SysCore1_Spadic1_Ch%02d", i);
    h2=static_cast<TH2*>(gROOT->FindObjectAny(temphistname.Data()));
  */
    /*
      if(i<10) {
      temphistname = temphistname + std::to_string(0) + std::to_string(i);
      h2=static_cast<TH2*>(gROOT->FindObjectAny(temphistname.Data()));
      if (h2!=NULL) {
        h2->Draw("");
	}
	} else {
	temphistname = temphistname + std::to_string(i);
	h2=static_cast<TH2*>(gROOT->FindObjectAny(temphistname.Data()));
      if (h2!=NULL) {
      h2->Draw("");

      }
      }
  }
    */

  /*

  // --- Fill and register some lists/folders/histos
  
  gDirectory = 0;
  */
  /*
  TList* l = new TList;
  l->SetName("Spadic 1");
  l->Add(fSpadic1);
  */
  //fBrowser->Add(fSpadic1);

  /*
    TFolder* f = new TFolder("Booboayes", "Statisticos");
    h = new TH1F("Fooes", "Baros", 51, 0, 1);
    for (Int_t i=0; i<2000; ++i) {
    h->Fill(gRandom->Gaus(.7, .1));
    h->Fill(gRandom->Gaus(.3, .1));
    }
    f->Add(h);
    g_hlt_browser->Add(f);
    
    h = new TH1F("Fooesoto", "Barosana", 51, 0, 1);
    for (Int_t i=0; i<4000; ++i) {
    h->Fill(gRandom->Gaus(.25, .02), 0.04);
    h->Fill(gRandom->Gaus(.5, .1));
    h->Fill(gRandom->Gaus(.75, .02), 0.04);
    }
    g_hlt_browser->Add(h);
  */
  
   // --- Add some macros.
  
  /*
  */

/*
  CbmTrdRawBeamProfile* c = static_cast<CbmTrdRawBeamProfile*>(gROOT->FindObjectAny("CbmTrdRawBeamProfile")).Data()); 
  LOG(INFO)<<"Found class "<< c->GetName() << FairLogger::endl;
  
  /*
    CbmTrdRawBeamProfile* c = static_cast<CbmTrdRawBeamProfile*>(gROOT->FindObjectAny("CbmTrdRawBeamProfile")).Data()); 
    LOG(INFO)<<"Found class "<< c->GetName() << FairLogger::endl;

    TClass *cl = c->IsA();
    TDataMember *dm = cl->GetDataMember("fHM");
    TMethodCall *getter = dm->GetterMethod(c); 

  
    CbmHistManager* HM_RawProfile;
    getter->Execute(c,"",HM_RawProfile); 

    if (HM_RawProfile)
    LOG(INFO) << "Hier bin ich" << HM_RawProfile->GetName() << FairLogger::endl;

    TList* list = gROOT->GetListOfBrowsables();
    TIter nextT(list);
    TObject *t = NULL;
    while ((t = (TObject *) nextT())) {
    LOG(INFO) << "Found obj " << t->GetName();
    }
  */

  return kSUCCESS;

}

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdEveOnlineDisplay::ReInit()
{
  return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------
void CbmTrdEveOnlineDisplay::Exec(Option_t* option)
{

  fEventCounter++;
  
  if ( 0 == fEventCounter%fUpdateInterval ) {
    LOG(DEBUG)<<"Update Canvas for Event "<< fEventCounter << FairLogger::endl;
    for(Int_t iCh=0; iCh<5; iCh++){
      fSpadic1->cd(iCh+1);
      gPad->Modified();
      gPad->Update();
    }
    fSpadic1->Modified();
    fSpadic1->Update();
  } 

}

// ---- Finish --------------------------------------------------------
void CbmTrdEveOnlineDisplay::Finish()
{
  fSpadic1->Update();
}

ClassImp(CbmTrdEveOnlineDisplay)
