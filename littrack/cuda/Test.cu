#include <cuda.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "LitCudaHit.h"
#include "LitCudaMaterialInfo.h"

//Device code
__global__ void VecAdd(float* A, float* B, float* C)
{
   int i = blockDim.x * blockIdx.x + threadIdx.x;
//    if (i < N)
   C[i] = A[i] + B[i];
}

//Host code
extern "C" void AddVec(void)
{
   LitCudaPixelHit pixelHit;
   pixelHit.X = 10.;
//    LitCudaStripHit<float> stripHit;
//    stripHit.U = 20.;
   std::cout << "PLUS" << pixelHit << std::endl;


   printf("Executing AddVec...\n");
   const int N = 10000000;
   size_t size = N * sizeof(float);
   printf("Number of elements %d. Size %d \n", N, size);

   // Allocate vectors in host memory
   printf("Allocate arrays in host memory...\n");
   float* h_A = (float*) malloc(size);
   float* h_B = (float*) malloc(size);
   float* h_C = (float*) malloc(size);

   // Allocate vectors in device memory
   printf("Allocate arrays in device memory...\n");
   float* d_A;
   cudaMalloc((void**)&d_A, size);
   float* d_B;
   cudaMalloc((void**)&d_B, size);
   float* d_C;
   cudaMalloc((void**)&d_C, size);

   //Copy vectors from host to device memory
   printf("Copy vectors from host to device memory...\n");
   cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
   cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);

   //Invoke kernel
   int threadsPerBlock = 256;
   int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
   VecAdd<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C);

   cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);

   //Free device memory
   cudaFree(d_A);
   cudaFree(d_B);
   cudaFree(d_C);

   printf("Finish...");
}
