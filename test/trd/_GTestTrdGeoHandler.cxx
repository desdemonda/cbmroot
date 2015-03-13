#include "CbmTrdGeoHandler.h"

#include "TFile.h"
#include "TGeoManager.h"
#include "TString.h"
#include "TSystem.h"
#include "TList.h"
#include "TKey.h"

#include "FairMockVirtualMC.h"

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>

using std::cout;
using std::endl;

// Structure to pass filenames together with expected response into the
// parametrized test
struct InOutStructure {
  TString name;
  int result;
} ;

struct ParamStructure2 {
  TString name;
  int result;
  Int_t stationIdLength;
  Int_t* stationId;
  Int_t* stationIdVal;
  Int_t moduleIdLength;
  Int_t* moduleId;
  Int_t* moduleIdVal;
} ;

// Base class to use the same basic setup for parameterized and
// non-parameterized tests
// Here one defines everything which is common for all the different
// test cases
template <class T> class _TestTrdGeoHandlerBase : public T
{
  protected:

  TFile* fInputFile;
  TString geomToTest;
  TString fFileName;
  CbmTrdGeoHandler* fGeoHandler;
  int result;
  FairMockVirtualMC* fMockVMC;

  virtual void SetUp() {

    geomToTest="squared_segmented";
    CreateFileName(geomToTest);
    fInputFile = new TFile(fFileName,"READ");

    GetGeoManager();
    fGeoHandler = new CbmTrdGeoHandler();
  }

  virtual void TearDown() {
    gGeoManager=NULL;
    delete fGeoHandler;
    fInputFile->Close();
    delete fInputFile;
    //    delete fMockVMC;
    
  }

  virtual void CreateFileName(TString name) {
    fFileName = gSystem->Getenv("VMCWORKDIR");
    fFileName = fFileName + "/input/qa/trd_" + name + "_geometry.root";
  }

  virtual void GetGeoManager() {
    gGeoManager->SetVerboseLevel(0);
    TList* l= fInputFile->GetListOfKeys();  
    TKey* key;
    TIter next( l);
    while ((key = (TKey*)next())) {
      /**loop inside the delivered root file and try to fine a TGeoManager object
       * the first TGeoManager found will be read
       */  
      if (strcmp(key->GetClassName(),"TGeoManager") != 0) { continue; }
      gGeoManager = (TGeoManager*)key->ReadObj();
    }
    fMockVMC = new FairMockVirtualMC("Mock implementation of VMC interface");
  }
};

// This is the derived class for the non-parameterized test cases.
class  TrdGeoHandlerTest : public _TestTrdGeoHandlerBase<testing::Test> {};

TEST_F(TrdGeoHandlerTest, CheckUniqueIdCreation)
{

  // check for usage of gMC
  fGeoHandler->Init(kTRUE);

  TString TopNode = gGeoManager->GetTopNode()->GetName();
  //  cout <<"TopNode: "<<TopNode<<endl;
  const char* path = "/cave_1/trd1_0/trd1mod1_1001/trd1mod1gas_0";
  gGeoManager->cd(path);
  const char* path1 = gGeoManager->GetPath();

  //  cout << "Path: "<<path1<<endl;

  TGeoNode *node = gGeoManager->GetCurrentNode();
  Int_t copy = node->GetNumber();
  Int_t id = node->GetVolume()->GetNumber();
  //  cout << "Id: "<<id<<" , "<<copy<<endl;
  
  Int_t uniqueId = fGeoHandler->GetUniqueDetectorId();
  
  //  cout <<"UniqueId: "<<uniqueId<<endl;
  EXPECT_EQ(135717, uniqueId);

  // check for usage of gMC
  fGeoHandler->Init(kFALSE);

  TopNode = gGeoManager->GetTopNode()->GetName();
  path = "/cave_1/trd1_0/trd1mod1_1001/trd1mod1gas_0";
  gGeoManager->cd(path);
  path1 = gGeoManager->GetPath();

  node = gGeoManager->GetCurrentNode();
  copy = node->GetNumber();
  id = node->GetVolume()->GetNumber();

  uniqueId = fGeoHandler->GetUniqueDetectorId();
  
  EXPECT_EQ(135717, uniqueId);

}

TEST_F(TrdGeoHandlerTest, CheckDefaultSettings)
{

  std::map<Int_t, Int_t>  expStationMap;
  expStationMap.insert( std::pair<Int_t, Int_t>(218,1) );
  expStationMap.insert( std::pair<Int_t, Int_t>(243,2) );
  expStationMap.insert( std::pair<Int_t, Int_t>(268,3) );

  std::map<Int_t, Int_t>  expModuleMap;
  expModuleMap.insert( std::pair<Int_t, Int_t>(219,1) );
  expModuleMap.insert( std::pair<Int_t, Int_t>(227,2) );
  expModuleMap.insert( std::pair<Int_t, Int_t>(235,3) );
  expModuleMap.insert( std::pair<Int_t, Int_t>(244,1) );
  expModuleMap.insert( std::pair<Int_t, Int_t>(252,2) );
  expModuleMap.insert( std::pair<Int_t, Int_t>(260,3) );
  expModuleMap.insert( std::pair<Int_t, Int_t>(269,3) );

  // check for usage of gMC
  Int_t retVal = fGeoHandler->Init(kTRUE);
  EXPECT_EQ(4, retVal);
  EXPECT_EQ(4, fGeoHandler->GetGeoVersion());

  std::map<Int_t, Int_t> stationMap = fGeoHandler->GetStationMap();
  EXPECT_TRUE( stationMap == expStationMap) <<"Maps differ.";

  std::map<Int_t, Int_t> moduleMap = fGeoHandler->GetModuleMap();
  EXPECT_TRUE( moduleMap == expModuleMap) <<"Maps differ.";

  // check for usage of gGeoManager
  retVal = fGeoHandler->Init(kFALSE);
  EXPECT_EQ(4, retVal);
  EXPECT_EQ(4, fGeoHandler->GetGeoVersion());

  stationMap = fGeoHandler->GetStationMap();
  EXPECT_TRUE( stationMap == expStationMap) <<"Maps differ.";

  moduleMap = fGeoHandler->GetModuleMap();
  EXPECT_TRUE( moduleMap == expModuleMap) <<"Maps differ.";

}

#// This is the derived class for the parameterized test cases.
class TrdGeoHandlerParamTest2 : public _TestTrdGeoHandlerBase<
  testing::TestWithParam<ParamStructure2> >
{
 protected:

  std::map<Int_t, Int_t> expStationMap; 
  std::map<Int_t, Int_t> expModuleMap; 
  
  virtual void SetUp() {
    ParamStructure2 const& p = GetParam();

    geomToTest = p.name;
    result = p.result;
    for (Int_t i=0; i< p.stationIdLength; i++) {
      if ( 0 != p.stationId[i] ) {
	expStationMap.insert(std::pair<Int_t, Int_t> 
			     (p.stationId[i], p.stationIdVal[i]));
      }
    }

    for (Int_t i=0; i< p.moduleIdLength; i++) {
      if ( 0 != p.moduleId[i] ) {
	expModuleMap.insert(std::pair<Int_t, Int_t>
			    (p.moduleId[i], p.moduleIdVal[i]));
      }
    }

    CreateFileName(geomToTest);
    fInputFile = new TFile(fFileName,"READ");
    
    GetGeoManager();
    fGeoHandler = new CbmTrdGeoHandler();
  }
};


TEST_P(TrdGeoHandlerParamTest2, checkAllDifferentGeometries)
{
  // check for usage of gMC 
  Int_t retVal = fGeoHandler->Init(kTRUE);
  EXPECT_EQ(result, retVal);
  EXPECT_EQ(result, fGeoHandler->GetGeoVersion());

  std::map<Int_t, Int_t> stationMap = fGeoHandler->GetStationMap();
  EXPECT_TRUE( stationMap == expStationMap) <<"Maps differ.";

  std::map<Int_t, Int_t> moduleMap = fGeoHandler->GetModuleMap();
  EXPECT_TRUE( moduleMap == expModuleMap) <<"Maps differ.";

  // check for usage of gGeoManager
  retVal = fGeoHandler->Init(kFALSE);
  EXPECT_EQ(result, retVal);
  EXPECT_EQ(result, fGeoHandler->GetGeoVersion());

  stationMap = fGeoHandler->GetStationMap();
  EXPECT_TRUE( stationMap == expStationMap) <<"Maps differ.";

  moduleMap = fGeoHandler->GetModuleMap();
  EXPECT_TRUE( moduleMap == expModuleMap) <<"Maps differ.";

}

// Fill vector with expected volume ids for this geometry.
Int_t stationIdLength=3;
Int_t moduleIdLength=9;

// The momolithic geometry has no modules and so only the stationId is filled
Int_t stationId1[3]={221, 228, 235};
Int_t stationIdVal1[3]={1, 2, 3};
Int_t moduleId1[9]={0, 0, 0, 0, 0, 0, 0, 0, 0};
Int_t moduleIdVal1[9]={-1, -1, -1, -1, -1, -1, -1, -1, -1};
ParamStructure2 val1= {"monolithic", 1, stationIdLength, 
		       stationId1, stationIdVal1, 
		       moduleIdLength, moduleId1, moduleIdVal1};

Int_t stationId2[3]={218, 238, 258};
Int_t stationIdVal2[3]={1, 2, 3};
Int_t moduleId2[9]={220, 226, 232, 240, 246, 252, 0, 0, 260};
Int_t moduleIdVal2[9]={1, 2, 3, 1, 2, 3, 0, 0, 3};
ParamStructure2 val2= {"quasi_monolithic", 2, stationIdLength, 
		       stationId2, stationIdVal2, 
		       moduleIdLength, moduleId2, moduleIdVal2};

Int_t stationId3[3]={218, 244, 270};
Int_t stationIdVal3[3]={1, 2, 3};
Int_t moduleId3[9]={220, 228, 236, 246, 254, 262, 0, 0, 272};
Int_t moduleIdVal3[9]={1, 2, 3, 1, 2, 3, 0, 0, 3};
ParamStructure2 val3= {"rectangular_segmented", 3, stationIdLength, 
		       stationId3, stationIdVal3, 
		       moduleIdLength, moduleId3, moduleIdVal3};

Int_t stationId4[3]={218, 243, 268};
Int_t stationIdVal4[3]={1, 2, 3};
Int_t moduleId4[9]={219, 227, 235, 244, 252, 260, 0, 0, 269};
Int_t moduleIdVal4[9]={1, 2, 3, 1, 2, 3, 0, 0, 3};
ParamStructure2 val4= {"squared_segmented", 4, stationIdLength, 
		       stationId4, stationIdVal4, 
		       moduleIdLength, moduleId4, moduleIdVal4};

Int_t stationId5[3]={218, 0, 0};
Int_t stationIdVal5[3]={1, 0, 0};
Int_t moduleId5[9]={219, 227, 235, 243, 275, 267, 251, 259, 0};
Int_t moduleIdVal5[9]={1, 2, 3, 4, 5, 6, 7, 8, 0};
ParamStructure2 val5= {"one_keeping_volume", 5, stationIdLength, 
		       stationId5, stationIdVal5, 
		       moduleIdLength, moduleId5, moduleIdVal5};


INSTANTIATE_TEST_CASE_P(TestAllGeometries,
                        TrdGeoHandlerParamTest2,
			::testing::Values(val1, val2, val3, val4, val5));		

