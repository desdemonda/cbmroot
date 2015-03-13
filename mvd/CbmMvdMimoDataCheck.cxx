// -------------------------------------------------------------------------
// -----                        CbmMvdMimoDataCheck source file                   -----
// -----           Created 23.08.2011  by Q. Li, M. Deveaux      -----
// -------------------------------------------------------------------------

#include "CbmMvdMimoDataCheck.h"
#include "FairLogger.h"

#include <iostream>
#include <vector>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmMvdMimoDataCheck::CbmMvdMimoDataCheck(): FairTask() {
    
    
    fVerbose=0;
    

}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMvdMimoDataCheck::CbmMvdMimoDataCheck(const char* name, Int_t iVerbose)
  : FairTask(name){
    fVerbose=iVerbose;
    fInputPersistance=-1;
    


}
// -------------------------------------------------------------------------

InitStatus CbmMvdMimoDataCheck::Init(){
  
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
	cout << "-E- " << GetName() << "::Init: No FairRootManager!" << endl;
	return kFATAL;
    }
    
  fDigiArray= (TClonesArray*) ioman->GetObject("MvdDigi");
  if (!fDigiArray){
  Fatal (GetName(), "Could not find Mvddigis");}  

  fMimoDataStateMatchArray = (TClonesArray*) ioman->GetObject("MimoDataStateMatch");
  if (!fMimoDataStateMatchArray) {
  Fatal (GetName(), "Could not find StateMatch!");}
  
  fMimoDataArray = (TClonesArray*) ioman->GetObject("MimoData");
  if (!fMimoDataArray) {
      Fatal (GetName(), "Could not find MimoData!");
  }
   
  
 
  
}


// -------------------------------------------------------------------------


InitStatus CbmMvdMimoDataCheck::ReInit(){cout << "Hello World Init the ClusterFinder again " << endl;}



/** compare each state between MimoData and StateMatch**/
void CbmMvdMimoDataCheck::CompareDataWithStateMatch(){
  
  Int_t fNumOfEntriesDataArray = 0;
  Int_t fNumOfEntriesStateArray = 0;
  Int_t fFrameID1 = 0;
  Int_t fFrameID2 = 0;
  
  Int_t fSensorID1 = 0;
  Int_t fSensorID2 = 0;
  Int_t fStateNum = 0;
  Int_t fStateIndex = 0;
  Int_t fDigiIndex = 0;
  Int_t fPixel_Colum = 0;
  Int_t fPixel_Row = 0;
  Int_t fStateNumberInStateMatch = 0;
  Int_t fStateNumInMimoData = 0;
  Double_t fTotalNum = 0;
  Double_t fRightNum = 0;
  Double_t fBadNum = 0;
  
 
  fNumOfEntriesDataArray = fMimoDataArray->GetEntries();// to get the index of entry
  fNumOfEntriesStateArray = fMimoDataStateMatchArray->GetEntries(); 
  tcan = new TCanvas("Checker","StateChecker",10,10,1200,1200);
  tcan->cd();
  tcan->Divide(1,2);
  Float_t state;
  TH2I *fDigiHisto = new TH2I("MimoData","MimoDataHisto",1152,0,1152,576,0,576);
  TH2I *fStateHisto = new TH2I("StateMatch","StateMatchHisto",1152,0,1152,576,0,576);
  
  if(fNumOfEntriesDataArray == fNumOfEntriesStateArray)
  {
    while(fNumOfEntriesDataArray)
    {
      fMimoData = (CbmMvdMimoData*)fMimoDataArray->At(fNumOfEntriesDataArray-1);
      fMimoStateMatch = (CbmMvdMimoStateMatch*)fMimoDataStateMatchArray->At(fNumOfEntriesStateArray-1);
      fNumOfEntriesDataArray--;
      if (fMimoData == NULL) 
	cout << "ZERO POINTER in MimoData "<< endl;
      else
	if (fMimoStateMatch == NULL)
	  cout << "ZERO POINTER in MimoStateMatch"<< endl;
	else
	{
	  fFrameID1 = fMimoData->GetROCFrame();
	  fFrameID2 = fMimoStateMatch->GetROCFrame();
	  fSensorID1 = fMimoData->GetSensorOffset();
	  fSensorID2 = fMimoStateMatch->GetSensorOffset();
	   fStateNumInMimoData = fMimoData->GetStateNum();
	  fStateNumberInStateMatch = fMimoStateMatch->GetStateNums();
	 
	  if((fSensorID1 == fSensorID2)&&(fFrameID1 == fFrameID2)&&(fStateNumInMimoData == fStateNumberInStateMatch ))
	  {
	    fStateVector.clear();
	    fMimoData->GetState(fStateVector);
	    
	    for(Int_t i=0;i<fStateVector.size()/2;i++)
	    {
	      Int_t fStateLine = fStateVector[2*i+1]*256+fStateVector[2*i];
	      Int_t fStateNum = fStateLine/4096;
	      Int_t fRowAddr = fStateLine%4096/2;
	     
	      for(Int_t j = 0;j < fStateNum;j++ )
	      {
		fTotalNum++;
		Int_t fState = Int_t (fStateVector[2*(i+j+1)+1]*256+fStateVector[2*(i+j+1)]);
		//add for test
		for(Int_t nn=0;nn<fState/16384+1;nn++)
		{
		  fDigiHisto->SetBinContent(fRowAddr,fState/8%2048+nn,1);
		  fDigiHisto->SetBinContent(fRowAddr,fState/8%2048+nn+10,0);
		}
		stateDigis.clear();
		if(fStateNumberInStateMatch > (fStateIndex+j))
		{
		  fMimoStateMatch->GetStateDigis(fStateIndex+j,stateDigis);
		  fDigiIndex = stateDigis[0];
		  fDigi = (CbmMvdDigi*)fDigiArray->At(fDigiIndex);
		  fPixel_Colum = fDigi->GetPixelX()%1152;
		  fPixel_Row = fDigi->GetPixelY()%576;
		  Int_t fDigiNumInOneState = stateDigis.size();
		  //add for test
		  for(Int_t nn=0;nn<fDigiNumInOneState;nn++)
		    fStateHisto->SetBinContent(fPixel_Row,fPixel_Colum,1);
		
		  if(fPixel_Row!=fRowAddr)
		  {
		      cout<<"The "<< fStateIndex <<"th state is wrong because of fRowAddr Error"<<endl;
		  }
		  else
		    if(fPixel_Colum != fState/8%2048)
		    {
		      cout<<"The "<<fStateIndex+1<<"th state is wrong because of fColumAddr Error"<<endl;
		    }
		    else
		      if(fDigiNumInOneState != fState/16384+1)
		      {
			  cout<<"The "<<fStateIndex<<"th state is wrong because of number of pixel Error"<<endl;
		      }
		      else
		      {
			fRightNum ++ ;
		      }

		}
		else
		{
		  cout<<"Index error~"<<fStateIndex<<" VS "<<fStateNumberInStateMatch<<endl;
		}
		
	      }
	      fStateIndex = fStateIndex + fStateNum;     	      
	      i = i+fStateNum;
	    }
	    tcan->cd(1);
            fDigiHisto->DrawClone("colz"); 
	    tcan->cd(2);
            fStateHisto->DrawClone("colz"); 
	   	  }
	  else
	    cout<<"Board Info is wrong!"<<endl;
	}
    }
   
    cout <<"The checker accuracy is "<< fRightNum/fTotalNum*100<<"%"<<endl;
 
    
  }
  
  else cout<<"The total num of data Info is wrong!"<<endl;
}

  









void CbmMvdMimoDataCheck::Exec(Option_t* opt){
 
  cout << "Hello World Exec " << endl;
  fEventNum++;
  if(fEventNum == 1)
    CompareDataWithStateMatch();

  
//   // -----   Timer   --------------------------------------------------------
//   TStopwatch timer;
//   timer.Start();
// 
//   timer.Stop();
//   Double_t rtime = timer.RealTime();
//   Double_t ctime = timer.CpuTime();
  

}


void CbmMvdMimoDataCheck::Finish()
{
   cout<<"End..."<<endl;
}
  


// -----   Destructor   ----------------------------------------------------
CbmMvdMimoDataCheck::~CbmMvdMimoDataCheck() { }
// -------------------------------------------------------------------------

ClassImp(CbmMvdMimoDataCheck)
