#ifndef LITCUDAMATH_H_
#define LITCUDAMATH_H_

#include "LitCudaTrackParam.h"
#include "LitCudaHit.h"
#include "LitCudaTrack.h"

__device__ float ChiSq(
   const LitCudaTrackParam* par,
   const LitCudaStripHit* hit)
{
   const float TWO = 2.;

   float duu = hit->Du * hit->Du;
   float phiCosSq = hit->phiCos * hit->phiCos;
   float phiSinSq = hit->phiSin * hit->phiSin;
   float phi2SinCos = TWO * hit->phiCos * hit->phiSin;

   float r = hit->U - par->X * hit->phiCos - par->Y * hit->phiSin;
   float rr = r * r;
   float norm = duu + par->C0 * phiCosSq + phi2SinCos * par->C1 + par->C5 * phiSinSq;
// float norm = duu + par->C0 * hit->phiCos + par->C5 * hitphiSin;

   return rr / norm;
}

__device__ float ChiSq(
   const LitCudaTrackParam* par,
   const LitCudaPixelHit* hit)
{
   const float TWO = 2.;

   float dxx = hit->Dx * hit->Dx;
   float dyy = hit->Dy * hit->Dy;
   float dx = hit->X - par->X;
   float dy = hit->Y - par->Y;

   float norm = -dxx * dyy + dxx * par->C5 + dyy * par->C0 - par->C0 * par->C5 + hit->Dxy * hit->Dxy - TWO * hit->Dxy * par->C1 + par->C1 * par->C1;
// if (norm == 0->) norm = 1e-10;

   return (-dx * dx * (dyy - par->C5) - dy * dy * (dxx - par->C0) + TWO * dx * dy * (hit->Dxy - par->C1)) / norm;
}

__device__ unsigned short NDF(
   const LitCudaTrack* track)
{
   // TODO check NDF

   unsigned short ndf =  2 * track->nofHits - 5;
   if (ndf > 0) { return ndf; }
   else { return 1; }

// int ndf = 0;
// for (int i = 0; i < track->GetNofHits(); i++){
//    if (track->GetHit(i)->GetType() == kLITPIXELHIT) ndf += 2;
//    else if (track->GetHit(i)->GetType() == kLITSTRIPHIT) ndf++;
// }
// ndf -= 5;
// if (ndf > 0) return ndf; else return 1;
}
//#undef cnst
#endif /* LITCUDAMATH_H_ */
