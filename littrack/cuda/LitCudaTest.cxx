#include "LitCudaTest.h"

#include <cstdlib>

#include <cuda.h>

extern "C" void DeviceInfo(void);
extern "C" void AddVec(void);

LitCudaTest::LitCudaTest()
{

}

LitCudaTest::~LitCudaTest()
{

}

void LitCudaTest::MyDeviceInfo() const
{
   DeviceInfo();
}

void LitCudaTest::MyAddVec() const
{
   AddVec();
}
