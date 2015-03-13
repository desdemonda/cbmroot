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
  DetectorId detIdFromEnum;
  int detId;
  TString detName;
} ;

// Base class to use the same basic setup for parameterized and
// non-parameterized tests
// Here one defines everything which is common for all the different
// test cases
template <class T> class _TestCbmDetectorListBase : public T
{
 protected:
  CbmDetectorList fList;
  
  
  virtual void SetUp() {
    
  }
  
  virtual void TearDown() {
  }

};

// This is the derived class for the non-parameterized test cases.
class  CbmDetectorListTest : public _TestCbmDetectorListBase<testing::Test> {};

// This is the derived class for the parameterized test cases.
class CbmDetectorListParamTest : public _TestCbmDetectorListBase<
  testing::TestWithParam<InOutStructure> >
{
 protected:
  
  DetectorId detIdFromEnum;
  Int_t detId;
  TString detName;

  virtual void SetUp() {
    InOutStructure const& p = GetParam();
    
    detIdFromEnum = p.detIdFromEnum;
    detId = p.detId;
    detName = p.detName;
  }
};


TEST_P(CbmDetectorListParamTest, checkUniqueIdCreation)
{
  TString retName; 
  fList.GetSystemName(detIdFromEnum, retName);
  EXPECT_EQ(detName, retName);
//  fList.GetSystemName(detId, retName);
//  EXPECT_EQ(detName, retName);
}

InOutStructure val1 = {kREF, 0, "ref"};
InOutStructure val2 = {kMVD, 1, "mvd"};
InOutStructure val3 = {kSTS, 2, "sts"};
InOutStructure val4 = {kRICH, 3, "rich"};
InOutStructure val5 = {kMUCH, 4, "much"};
InOutStructure val6 = {kTRD, 5, "trd"};
InOutStructure val7 = {kTOF, 6, "tof"};
InOutStructure val8 = {kECAL, 7, "ecal"};
InOutStructure val9 = {kZDC, 8, "unknown"};
InOutStructure val10 = {kSTT, 9, "unknown"};
InOutStructure val11 = {kTutDet, 10, "unknown"};
//InOutStructure val12 = {kBLA, 11, "unknown"};


INSTANTIATE_TEST_CASE_P(TestAllParameters,
                        CbmDetectorListParamTest,
			::testing::Values(val1, val2, val3, val4, val5,
                                          val6, val7, val8, val9, val10,
                                          val11));

