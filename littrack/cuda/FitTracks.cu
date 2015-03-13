#include "LitCudaTrackFitter.cu"
#include "LitCudaDetectorGeometry.h"

__global__ void LitCudaFitTrack(
   LitCudaTrack* tracks[],
   const LitCudaDetectorLayout* layout)
{
   int i = blockDim.x * blockIdx.x + threadIdx.x;
   LitCudaTrack* track = tracks[i];
   LitCudaTrackFitter(track, layout);
}

//Host code
extern "C" void LitCudaFitTracks(
   LitCudaTrack* h_tracks[],
   const LitCudaDetectorLayout* h_layout,
   int nofTracks)
{
//    printf("Executing FitTracks...\n");

   // Calculate required memory size
   size_t size = nofTracks * sizeof(LitCudaTrack);
   size_t layoutSize = sizeof(LitCudaDetectorLayout);

//    printf("Number of elements %d. Size %d \n", nofTracks, size);

   // Allocate device memory
//    printf("Allocate device memory...\n");
   LitCudaTrack** d_tracks;
   cudaMalloc((void**)&d_tracks, size);

   LitCudaDetectorLayout* d_layout;
   cudaMalloc((void**)&d_layout, layoutSize);

   //Copy vectors from host to device memory
//    printf("Copy vectors from host to device memory...\n");
   cudaMemcpy(d_tracks, h_tracks, size, cudaMemcpyHostToDevice);
   // Copy detector layout to device
   cudaMemcpy(d_layout, h_layout, layoutSize, cudaMemcpyHostToDevice);

   //Invoke kernel
   int threadsPerBlock = 256;
   int blocksPerGrid = (nofTracks + threadsPerBlock - 1) / threadsPerBlock;
   LitCudaFitTrack<<<blocksPerGrid, threadsPerBlock>>>(d_tracks, d_layout);

   cudaMemcpy(h_tracks, d_tracks, size, cudaMemcpyDeviceToHost);

   //Free device memory
   cudaFree(d_tracks);

//    printf("Finish...");
}
