/* CbmTrdDigiWriteoutBuffer.h
 *
 *  Created on: Mar 7, 2012
 *      Author: uhlig
 */

#ifndef CBMTRDDIGIWRITEOUTBUFFER_H_
#define CBMTRDDIGIWRITEOUTBUFFER_H_

#include "FairWriteoutBuffer.h"
#include "CbmTrdDigi.h"

class FairTimeStamp;

class CbmTrdDigiWriteoutBuffer : public FairWriteoutBuffer
{

  public:
    CbmTrdDigiWriteoutBuffer();
    CbmTrdDigiWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);
    void AddNewDataToTClonesArray(FairTimeStamp*);

    virtual ~CbmTrdDigiWriteoutBuffer();

    //    std::vector<std::pair<double, CbmTrdDigi*> > Modify(std::pair<double, CbmTrdDigi*> oldData, std::pair<double, CbmTrdDigi*> newData)
    std::vector<std::pair<double, FairTimeStamp*> > Modify(std::pair<double, FairTimeStamp*> oldData, std::pair<double, FairTimeStamp*> newData)
      {
	fLogger->Debug(MESSAGE_ORIGIN,"Inside Modify:");
	fLogger->Debug(MESSAGE_ORIGIN,"oldData.first %f", oldData.first);
	fLogger->Debug(MESSAGE_ORIGIN,"newData.first: %f", newData.first);
	
	std::vector<std::pair<double, FairTimeStamp*> > result;

	std::pair<double, FairTimeStamp*> singleResult;
//
	CbmTrdDigi* newDigiData = (CbmTrdDigi*) newData.second;
	CbmTrdDigi* oldDigiData = (CbmTrdDigi*) oldData.second;
	CbmTrdDigi* combinedDigiData = oldDigiData;
        fLogger->Debug(MESSAGE_ORIGIN,"Old Charge: %f", oldDigiData->GetCharge());
        fLogger->Debug(MESSAGE_ORIGIN,"Combined Charge: %f", combinedDigiData->GetCharge());

//	if (newData.first > 0) {
//	  singleResult.first = oldData.first + newData.first;
	  singleResult.first = oldData.first;
//	}
	combinedDigiData->AddCharge(newDigiData->GetCharge());
	combinedDigiData->AddLink(newDigiData->GetLink(0));
        fLogger->Debug(MESSAGE_ORIGIN,"New Charge: %f", newDigiData->GetCharge());
        fLogger->Debug(MESSAGE_ORIGIN,"Combined Charge: %f", combinedDigiData->GetCharge());
	singleResult.second = (FairTimeStamp*)combinedDigiData;
	//        singleResult.second->Print();
	CbmTrdDigi* bla = (CbmTrdDigi*) singleResult.second;
        bla->Print();
	//        fLogger->Debug(MESSAGE_ORIGIN,"Combined Charge: %f", combinedDigiData->GetCharge());
//  if (fVerbose > 0) {
//    std::cout << "Modify hit" << std::endl;
//    std::cout << "OldData: " << oldData.first << " : " << oldData.second << " NewData: " << newData.first << " : " << newData.second << std::endl;
//    std::cout << "Resulting Data: " << singleResult.first << " : " << singleResult.second << std::endl;
//  }

	result.push_back(singleResult);
  return result;

  
  //	std::vector<std::pair<double, FairTimeStamp*> > result;
  //        //std::vector<std::pair<double, CbmTrdDigi*> > result;
  //  	result.push_back(oldData);
  //	return result;
      }; 

    virtual double FindTimeForData(FairTimeStamp* data) ;
    virtual void FillDataMap(FairTimeStamp* data, double activeTime) ;
    virtual void EraseDataFromDataMap(FairTimeStamp* data);
  protected:

    std::map<CbmTrdDigi, double> fData_map;

    ClassDef(CbmTrdDigiWriteoutBuffer, 1);
};

#endif /* CBMTRDDIGIWRITEOUTBUFFER_H_ */

