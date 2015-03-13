/*
 * CbmTrdDigiWriteoutBuffer.cxx
 *
 *  Created on: Mar 7, 2012
 *      Author: uhlig
 */

#include "CbmTrdDigiWriteoutBuffer.h"
#include "CbmTrdDigi.h"

#include "FairLogger.h"

CbmTrdDigiWriteoutBuffer::CbmTrdDigiWriteoutBuffer()
  : FairWriteoutBuffer(),
    fData_map()
{
}


CbmTrdDigiWriteoutBuffer::CbmTrdDigiWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance)
  : FairWriteoutBuffer(branchName, "CbmTrdDigi", folderName, persistance),
    fData_map()
{
}

CbmTrdDigiWriteoutBuffer::~CbmTrdDigiWriteoutBuffer()
{
}

//std::vector<std::pair<double, FairTimeStamp*> > Modify(std::pair<double, FairTimeStamp*> oldData, std::pair<double, FairTimeStamp*> newData) 
//std::vector<std::pair<double, CbmTrdDigi*> > CbmTrdDigiWriteoutBuffer::Modify(std::pair<double, CbmTrdDigi*> oldData, std::pair<double, CbmTrdDigi*> newData)
//{
//  FairLogger* gLogger = FairLogger::GetLogger();
//  gLogger->Debug(MESSAGE_ORIGIN,"Inside Modify:");
//  std::vector<std::pair<double, CbmTrdDigi*> > result;
//  result.push_back(oldData);
//  return result;
//  /*
//  std::vector<std::pair<double, FairTimeStamp*> > result;
//  return result;
//  std::pair<double, CbmTrdDigi*> singleResult;
//  if (newData.first > 0) {
//    singleResult.first = oldData.first + newData.first;
//  }
//  singleResult.second = oldData.second;
//  singleResult.second->AddCharge(newData.second->GetCharge());
//  // if (fVerbose > 0) {
//    std::cout << "Modify hit" << std::endl;
//    std::cout << "OldData: " << oldData.first << " : " << oldData.second << " NewData: " << newData.first << " : " << newData.second << std::endl;
//    std::cout << "Resulting Data: " << singleResult.first << " : " << singleResult.second << std::endl;
//    //  }
//
//  result.push_back(singleResult);
//  return result;
//  */
//}


//std::vector<std::pair<double, FairTimeStamp*> > Modify(std::pair<double, FairTimeStamp*> oldData, std::pair<double, FairTimeStamp*> newData) 
//{
//  FairLogger* gLogger = FairLogger::GetLogger();
//  gLogger->Debug(MESSAGE_ORIGIN,"Inside Modify:");
//  std::vector<std::pair<double, FairTimeStamp*> > result;
//  result.push_back(oldData);
//  return result;
//}

void CbmTrdDigiWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp* data)
{
  FairRootManager* ioman = FairRootManager::Instance();
  TClonesArray* myArray = ioman->GetTClonesArray(fBranchName);
  gLogger->Debug(MESSAGE_ORIGIN,"Data Inserted: ");
  CbmTrdDigi* bla =(CbmTrdDigi*)(data);
  bla->Print();
  new ((*myArray)[myArray->GetEntries()]) CbmTrdDigi(*(CbmTrdDigi*)(data));
}

double CbmTrdDigiWriteoutBuffer::FindTimeForData(FairTimeStamp* data)
{
  std::map<CbmTrdDigi, double>::iterator it;
  CbmTrdDigi myData = *(CbmTrdDigi*)data;
  it = fData_map.find(myData);
  if (it == fData_map.end()) {
    return -1;
  } else {
    return it->second;
  }
}

void CbmTrdDigiWriteoutBuffer::FillDataMap(FairTimeStamp* data, double activeTime)
{
  CbmTrdDigi myData = *(CbmTrdDigi*)data;
  fData_map[myData] = activeTime;
}

void CbmTrdDigiWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp* data)
{
  CbmTrdDigi myData = *(CbmTrdDigi*)data;
  if (fData_map.find(myData) != fData_map.end()) {
    fData_map.erase(fData_map.find(myData));
  }
}

ClassImp(CbmTrdDigiWriteoutBuffer);
