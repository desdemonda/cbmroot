#include "CbmTrdDetectorId.h"
#include "CbmDetectorList.h"

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"

class CbmTrdDetectorIdTest : public ::testing::Test 
{
 protected:

  CbmTrdDetectorId fTrdId;
  Int_t system;
  Int_t station;
  Int_t layer;
  Int_t moduleType;
  Int_t moduleNr;
  Int_t sector;
  Int_t detInfo_array[6];
  Int_t retVal;  

  virtual void SetUp() 
  {  
  }
  
  virtual void TearDown() 
  {
  }

  void FillDetArray(Int_t sys, Int_t stat, Int_t lay, Int_t type, 
		    Int_t copy, Int_t sect)
  {
    detInfo_array[0]=sys;
    detInfo_array[1]=stat;
    detInfo_array[2]=lay;
    detInfo_array[3]=type;
    detInfo_array[4]=copy;
    detInfo_array[5]=sect;
  } 
};

TEST_F(CbmTrdDetectorIdTest, CheckDefaultSettings)
{
  FillDetArray(0, 0, 0, 0, 0, 0);
  retVal = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(0, retVal);

  FillDetArray(kTRD, 0, 0, 0, 0, 0);
  retVal = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(5, retVal);

  FillDetArray(0, 1, 0, 0, 0, 0);
  retVal = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(32, retVal);

  FillDetArray(0, 0, 1, 0, 0, 0);
  retVal = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(512, retVal);

  FillDetArray(0, 0, 0, 1, 0, 0);
  retVal = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(4096, retVal);

  FillDetArray(0, 0, 0, 0, 1, 0);
  retVal = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(131072, retVal);

  FillDetArray(0, 0, 0, 0, 0, 1);
  retVal = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(33554432, retVal);

  FillDetArray(kTRD, 1, 1, 1, 1, 1);
  retVal = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(33690149, retVal);

  FillDetArray(kTRD, 3, 3, 2, 34, 3);
  retVal = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(105129573, retVal);

  FillDetArray(0, 0, 0, 8, 0, 0);
  retVal = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(32768, retVal);

  FillDetArray(kTRD, 2, 3, 5, 17, 2);
  retVal = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(69359173, retVal);
}

