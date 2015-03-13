#include "CbmTrdDetectorId.h"
#include "CbmDetectorList.h"

#include "TString.h"

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"

#include <iostream>
using std::cout;
using std::endl;

// Structure to pass filenames together with expected response into the
// parametrized test
struct InOutStructure {
  int system;
  int station;
  int layer;
  int type;
  int copynr;
  int sector;
  int result;
} ;

// Base class to use the same basic setup for parameterized and
// non-parameterized tests
// Here one defines everything which is common for all the different
// test cases
template <class T> class _TestCbmTrdDetectorIdBase : public T
{
 protected:
  CbmTrdDetectorId fTrdId;
  
  
  virtual void SetUp() {
    
  }
  
  virtual void TearDown() {
  }

};

// This is the derived class for the non-parameterized test cases.
class  CbmTrdDetectorIdTest : public _TestCbmTrdDetectorIdBase<testing::Test> {};

TEST_F(CbmTrdDetectorIdTest, CheckDefaultSettings)
{
  Int_t system = 0;
  Int_t station = 0;
  Int_t layer = 0;
  Int_t moduleType = 0;
  Int_t moduleNr = 0;
  Int_t sector = 0;
  Int_t detInfo_array[6]={system, station, layer, moduleType, 
			  moduleNr, sector}; 
  
  Int_t retVal = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(0, retVal);
}

// This is the derived class for the parameterized test cases.
class CbmTrdDetectorIdParamTest : public _TestCbmTrdDetectorIdBase<
  testing::TestWithParam<InOutStructure> >
{
 protected:
  
  Int_t detInfo_array[6];
  Int_t modInfo_array[6]; 
  Int_t result;

  virtual void SetUp() {
    InOutStructure const& p = GetParam();
    
    detInfo_array[0] = p.system;
    detInfo_array[1] = p.station;
    detInfo_array[2] = p.layer;
    detInfo_array[3] = p.type;
    detInfo_array[4] = p.copynr;
    detInfo_array[5] = p.sector; 
    result=p.result;

    modInfo_array[0] = detInfo_array[0];
    modInfo_array[1] = detInfo_array[1];
    modInfo_array[2] = detInfo_array[2];
    modInfo_array[3] = detInfo_array[3];
    modInfo_array[4] = detInfo_array[4];
    modInfo_array[5] = 0;

  }
};


TEST_P(CbmTrdDetectorIdParamTest, checkUniqueIdCreation)
{
  Int_t uniqueId = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(result, uniqueId);

  Int_t systemId = fTrdId.GetSystemId(uniqueId);
  EXPECT_EQ(detInfo_array[0], systemId);

  Int_t sectorNr = fTrdId.GetSector(uniqueId);
  EXPECT_EQ(detInfo_array[5], sectorNr);

  Int_t modId = fTrdId.SetDetectorInfo(modInfo_array);
  Int_t newUniqueId = fTrdId.SetSector(modId, detInfo_array[5]);
  EXPECT_EQ(result, newUniqueId);

  Int_t newModId = fTrdId.GetModuleId(newUniqueId);
  EXPECT_EQ(modId, newModId);
}

InOutStructure val1 = {0, 0, 0, 0, 0, 0, 0};
InOutStructure val2 = {kTRD, 0, 0, 0, 0, 0, 5};
InOutStructure val3 = {0, 1, 0, 0, 0, 0, 32};
InOutStructure val4 = {0, 0, 1, 0, 0, 0, 512};
InOutStructure val5 = {0, 0, 0, 1, 0, 0, 4096};
InOutStructure val6 = {0, 0, 0, 0, 1, 0, 131072};
InOutStructure val7 = {0, 0, 0, 0, 0, 1, 33554432};
InOutStructure val8 = {kTRD, 1, 1, 1, 1, 1, 33690149};
InOutStructure val9 = {kTRD, 3, 3, 2, 34, 3, 105129573};
InOutStructure val10 = {kTRD, 2, 3, 3, 17, 2, 69350981};
InOutStructure val11 = {0, 0, 0, 8, 0, 0, 32768};
InOutStructure val12 = {kTRD, 2, 3, 5, 17, 2, 69359173};


INSTANTIATE_TEST_CASE_P(TestAllParameters,
                        CbmTrdDetectorIdParamTest,
			::testing::Values(val1, val2, val3, val4, val5,
					  val6, val7, val8, val9, val10,
					  val11, val12));

// This is the derived class for the parameterized test cases.
class CbmTrdDetectorIdParamTest1 : public _TestCbmTrdDetectorIdBase<
  testing::TestWithParam<InOutStructure> >
{
 protected:  
  Int_t detInfo_array[6];
  Int_t result_array[6];
  Int_t uniqueId;

  virtual void SetUp() {
    InOutStructure const& p = GetParam();
    uniqueId=p.result;
    result_array[0] = p.system;
    result_array[1] = p.station;
    result_array[2] = p.layer;
    result_array[3] = p.type;
    result_array[4] = p.copynr;
    result_array[5] = p.sector; 
  }
};


TEST_P(CbmTrdDetectorIdParamTest1, checkExtractInfoFromUniqueId)
{
  Int_t* retVal = fTrdId.GetDetectorInfo(uniqueId);
  for ( int i=0;i<6;i++) {
    EXPECT_EQ(result_array[i], retVal[i]);
  }
}


INSTANTIATE_TEST_CASE_P(TestAllParameters,
                        CbmTrdDetectorIdParamTest1,
			::testing::Values(val1, val2, val3, val4, val5,
					  val6, val7, val8, val9, val10,
					  val11, val12));

