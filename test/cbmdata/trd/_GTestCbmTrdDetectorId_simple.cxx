#include "CbmTrdDetectorId.h"
#include "CbmDetectorList.h"

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"

TEST(CbmTrdDetectorIdTest, CheckDefaultSettings)
{
  CbmTrdDetectorId fTrdId;

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

