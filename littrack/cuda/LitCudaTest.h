#ifndef LITCUDATEST_H_
#define LITCUDATEST_H_

class LitCudaTest
{
public:
   LitCudaTest();
   virtual ~LitCudaTest();

   void MyDeviceInfo() const;

   void MyAddVec() const;
};

#endif /* LITCUDATEST_H_ */
