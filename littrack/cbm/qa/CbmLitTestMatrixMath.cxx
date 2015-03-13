#include "qa/CbmLitTestMatrixMath.h"
#include "utils/CbmLitMatrixMath.h"

#include "TRandom.h"
#include "TMatrixTSym.h"
#include "TMatrixT.h"

#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>


CbmLitTestMatrixMath::CbmLitTestMatrixMath():
   fEpsilon(1e-6)
{

}

CbmLitTestMatrixMath::~CbmLitTestMatrixMath()
{

}

void CbmLitTestMatrixMath::RunTest()
{
   std::cout << "Start matrix math test..." << std::endl;

   TestInvSym15(1);
   TestInvSym15(2);
   TestInvSym15(3);

   TestMult15On5(1);
   TestMult15On5(2);
   TestMult15On5(3);

   std::cout << "Finish matrix math test..." << std:: endl;
}

void CbmLitTestMatrixMath::TestInvSym15(
   int testId)
{
   std::cout << "Test InvSym15 #" << testId << std::endl;
   std::vector<litfloat> input1(15);
   for (int i = 0; i < 15; i++) { input1[i] = gRandom->Rndm(); }

   std::cout << "Input array: " << VectorToString(input1);

   std::vector<litfloat> a1(input1);
   InvSym15(a1);
   std:: cout << "Output InvSym15: " << VectorToString(a1);

   litfloat rd1[25], rd2[15];
   Convert15To25(&input1[0], rd1);
   TMatrixTSym<litfloat> r1(5, rd1);
   r1.InvertFast();
   Convert25To15(r1.GetMatrixArray(), rd2);
   std:: cout << "Output ROOT: " << ArrayToString(rd2, 15);

   // Compare output arrays
   bool testPassed = true;
   for (int i = 0; i < 15; i++) {
      if (std::abs(a1[i] - rd2[i]) > fEpsilon) {
         testPassed = false;
         break;
      }
   }
   if (testPassed) { std::cout << "Test InvSym15 #" << testId << " PASSED" << std::endl; }
   else { std::cout << "Test InvSym15 #" << testId << " !!!NOT PASSED!!!" << std::endl; }
}

void CbmLitTestMatrixMath::TestMult15On5(
   int testId)
{
   std::cout << "Test Mult15On5 #" << testId << std::endl;
   std::vector<litfloat> input1(15);
   for (int i = 0; i < 15; i++) { input1[i] = gRandom->Rndm(); }
   std::vector<litfloat> input2(5);
   for (int i = 0; i < 5; i++) { input2[i] = gRandom->Rndm(); }

   std::cout << "Input array1: " << VectorToString(input1);
   std::cout << "Input array2: " << VectorToString(input2);

   std::vector<litfloat> a1(5);
   Mult15On5(input1, input2, a1);
   std:: cout << "Output Mult15On5: " << VectorToString(a1);

   litfloat rd1[25], rd2[15];
   Convert15To25(&input1[0], rd1);
   TMatrixTSym<litfloat> r1(5, rd1);
   TMatrixT<litfloat> v1(5,1, &input2[0]);
   TMatrixT<litfloat> u1(5,1);
   u1 = r1 * v1;

   litfloat* ro = u1.GetMatrixArray();

   std::cout << "Output ROOT: " << ArrayToString(ro, 5);

   // Compare output arrays
   bool testPassed = true;
   for (int i = 0; i < 5; i++) {
      if (std::abs(a1[i] - ro[i]) > fEpsilon) {
         testPassed = false;
         break;
      }
   }
   if (testPassed) { std::cout << "Test Mult15On5 #" << testId << " PASSED" << std::endl; }
   else { std::cout << "Test Mult15On5 #" << testId << " !!!NOT PASSED!!!" << std::endl; }
}

void CbmLitTestMatrixMath::Convert15To25(
   const litfloat* a15,
   litfloat* a25)
{
   a25[0] = a15[0];
   a25[1] = a15[1];
   a25[2] = a15[2];
   a25[3] = a15[3];
   a25[4] = a15[4];
   a25[5] = a15[1];
   a25[6] = a15[5];
   a25[7] = a15[6];
   a25[8] = a15[7];
   a25[9] = a15[8];
   a25[10] = a15[2];
   a25[11] = a15[6];
   a25[12] = a15[9];
   a25[13] = a15[10];
   a25[14] = a15[11];
   a25[15] = a15[3];
   a25[16] = a15[7];
   a25[17] = a15[10];
   a25[18] = a15[12];
   a25[19] = a15[13];
   a25[20] = a15[4];
   a25[21] = a15[8];
   a25[22] = a15[11];
   a25[23] = a15[13];
   a25[24] = a15[14];
}

void CbmLitTestMatrixMath::Convert25To15(
   const litfloat* a25,
   litfloat* a15)
{
   a15[0] = a25[0];
   a15[1] = a25[1];
   a15[2] = a25[2];
   a15[3] = a25[3];
   a15[4] = a25[4];
   a15[5] = a25[6];
   a15[6] = a25[7];
   a15[7] = a25[8];
   a15[8] = a25[9];
   a15[9] = a25[12];
   a15[10] = a25[13];
   a15[11] = a25[14];
   a15[12] = a25[18];
   a15[13] = a25[19];
   a15[14] = a25[24];
}

std::string CbmLitTestMatrixMath::VectorToString(
   const std::vector<litfloat>& a)
{
   std::stringstream out;
   for (int i = 0; i < a.size(); i++) { out << a[i] << " "; }
   out << std::endl;
   return out.str();
}

std::string CbmLitTestMatrixMath::ArrayToString(
   const litfloat* a, int n)
{
   std::stringstream out;
   for (int i = 0; i < n; i++) { out << a[i] << " "; }
   out << std::endl;
   return out.str();
}

