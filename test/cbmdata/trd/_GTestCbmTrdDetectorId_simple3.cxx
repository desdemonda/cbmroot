#include "CbmTrdDetectorId.h"
#include "CbmDetectorList.h"

#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"

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

// This is the derived class for the parameterized test cases.
class CbmTrdDetectorIdParamTest : public ::testing::TestWithParam<InOutStructure> 
{
 protected:
  CbmTrdDetectorId fTrdId;
  Int_t detInfo_array[6];
  Int_t result;
  Int_t uniqueId;

  virtual void SetUp() {
    InOutStructure const& p = GetParam();
    
    detInfo_array[0] = p.system;
    detInfo_array[1] = p.station;
    detInfo_array[2] = p.layer;
    detInfo_array[3] = p.type;
    detInfo_array[4] = p.copynr;
    detInfo_array[5] = p.sector; 
    result=p.result;
  }
};


TEST_P(CbmTrdDetectorIdParamTest, checkUniqueIdCreation)
{
  uniqueId = fTrdId.SetDetectorInfo(detInfo_array);
  EXPECT_EQ(result, uniqueId);
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
