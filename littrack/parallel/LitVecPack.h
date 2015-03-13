/**
 * \file LitVecPack.h
 *
 * \brief Functions to pack and unpack vector data classes.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 */

#ifndef LITVECPACK_H_
#define LITVECPACK_H_

#include "LitTypes.h"
#include "LitTrackParam.h"
#include "LitHit.h"

namespace lit {
namespace parallel {

/**
 * \fn void inline void PackFieldValue(const LitFieldValue<fscal> value[], LitFieldValue<fvec>& lvalue)
 * \brief Packs \c LitFieldValue.
 * \param[in] value Array of fvecLen length of LitFieldValue<fscal> objects to be packed into a vector.
 * \param[out] lvalue Packed LitFieldValue.
 */
inline void PackFieldValue(
   const LitFieldValue<fscal> value[],
   LitFieldValue<fvec>& lvalue)
{
   for(unsigned char i = 0; i < fvecLen; i++) {
      lvalue.Bx[i] = value[i].Bx;
      lvalue.By[i] = value[i].By;
      lvalue.Bz[i] = value[i].Bz;
   }
}



/**
 * \fn inline void PackTrackParam(const LitTrackParam<fscal> par[], LitTrackParam<fvec>& lpar)
 * \brief Packs \c LitTrackParam.
 * \param[in] par Array of fvecLen length of LitTrackParam<fscal> objects to be packed into a vector.
 * \param[out] lpar Packed LitTrackParam.
 */
inline void PackTrackParam(
   const LitTrackParam<fscal> par[],
   LitTrackParam<fvec>& lpar)
{
   for(unsigned char i = 0; i < fvecLen; i++) {
      lpar.X[i] = par[i].X;
      lpar.Y[i] = par[i].Y;
      lpar.Tx[i] = par[i].Tx;
      lpar.Ty[i] = par[i].Ty;
      lpar.Qp[i] = par[i].Qp;
      lpar.Z[i] = par[i].Z;
      lpar.C0[i] = par[i].C0;
      lpar.C1[i] = par[i].C1;
      lpar.C2[i] = par[i].C2;
      lpar.C3[i] = par[i].C3;
      lpar.C4[i] = par[i].C4;
      lpar.C5[i] = par[i].C5;
      lpar.C6[i] = par[i].C6;
      lpar.C7[i] = par[i].C7;
      lpar.C8[i] = par[i].C8;
      lpar.C9[i] = par[i].C9;
      lpar.C10[i] = par[i].C10;
      lpar.C11[i] = par[i].C11;
      lpar.C12[i] = par[i].C12;
      lpar.C13[i] = par[i].C13;
      lpar.C14[i] = par[i].C14;
   }
}



/**
 * \fn inline void UnpackTrackParam(const LitTrackParam<fvec>& lpar, LitTrackParam<fscal> par[])
 * \brief Unpacks \c LitTrackParam.
 * \param[in] lpar Packed LitTrackParam to be unpacked.
 * \param[out] par Array of fvecLen length to store unpacked scalar objects.
 */
inline void UnpackTrackParam(
   const LitTrackParam<fvec>& lpar,
   LitTrackParam<fscal> par[])
{
   for(unsigned char i = 0; i < fvecLen; i++) {
      par[i].X = lpar.X[i];
      par[i].Y = lpar.Y[i];
      par[i].Tx = lpar.Tx[i];
      par[i].Ty = lpar.Ty[i];
      par[i].Qp = lpar.Qp[i];
      par[i].Z = lpar.Z[i];
      par[i].C0 = lpar.C0[i];
      par[i].C1 = lpar.C1[i];
      par[i].C2 = lpar.C2[i];
      par[i].C3 = lpar.C3[i];
      par[i].C4 = lpar.C4[i];
      par[i].C5 = lpar.C5[i];
      par[i].C6 = lpar.C6[i];
      par[i].C7 = lpar.C7[i];
      par[i].C8 = lpar.C8[i];
      par[i].C9 = lpar.C9[i];
      par[i].C10 = lpar.C10[i];
      par[i].C11 = lpar.C11[i];
      par[i].C12 = lpar.C12[i];
      par[i].C13 = lpar.C13[i];
      par[i].C14 = lpar.C14[i];
   }
}



/**
 * \fn inline void UnpackTrackParam(unsigned int i, const LitTrackParam<fvec>& lpar, LitTrackParam<fscal>& par)
 * \brief Unpacks \c LitTrackParam.
 * \param[in] i Index of vector LitTrackParam to be unpacked to scalar.
 * \param[in] lpar Packed LitTrackParam to be unpacked.
 * \param[out] par Array of fvecLen length to store unpacked scalar objects.
 */
inline void UnpackTrackParam(
   unsigned int i,
   const LitTrackParam<fvec>& lpar,
   LitTrackParam<fscal>& par)
{
   par.X = lpar.X[i];
   par.Y = lpar.Y[i];
   par.Tx = lpar.Tx[i];
   par.Ty = lpar.Ty[i];
   par.Qp = lpar.Qp[i];
   par.Z = lpar.Z[i];
   par.C0 = lpar.C0[i];
   par.C1 = lpar.C1[i];
   par.C2 = lpar.C2[i];
   par.C3 = lpar.C3[i];
   par.C4 = lpar.C4[i];
   par.C5 = lpar.C5[i];
   par.C6 = lpar.C6[i];
   par.C7 = lpar.C7[i];
   par.C8 = lpar.C8[i];
   par.C9 = lpar.C9[i];
   par.C10 = lpar.C10[i];
   par.C11 = lpar.C11[i];
   par.C12 = lpar.C12[i];
   par.C13 = lpar.C13[i];
   par.C14 = lpar.C14[i];
}



/**
 * \fn inline void PackPixelHit(const LitScalPixelHit hit[], LitPixelHit<fvec>& lhit)
 * \brief Packs \c LitPixelHit.
 *
 * \param[in] par Array of fvecLen length of LitScalPixelHit objects to be packed into a vector.
 * \param[out] lpar Packed LitPixelHit.
 */
inline void PackPixelHit(
   const LitScalPixelHit hit[],
   LitPixelHit<fvec>& lhit)
{
   for(unsigned char i = 0; i < fvecLen; i++) {
      lhit.X[i] = hit[i].X;
      lhit.Y[i] = hit[i].Y;
      lhit.Dx[i] = hit[i].Dx;
      lhit.Dy[i] = hit[i].Dy;
      lhit.Dxy[i] = hit[i].Dxy;
   }
}



/**
 * \fn inline void UnpackPixelHit(const LitPixelHit<fvec>& lhit, LitScalPixelHit hit[])
 * \brief Unpacks \c LitPixelHit.
 *
 * \param[in] lpar Packed LitPixelHit to be unpacked.
 * \param[out] par Array of fvecLen length to store unpacked scalar objects.
 */
inline void UnpackPixelHit(
   const LitPixelHit<fvec>& lhit,
   LitScalPixelHit hit[])
{
   for(unsigned char i = 0; i < fvecLen; i++) {
      hit[i].X = lhit.X[i];
      hit[i].Y = lhit.Y[i];
      hit[i].Dx = lhit.Dx[i];
      hit[i].Dy = lhit.Dy[i];
      hit[i].Dxy = lhit.Dxy[i];
   }
}



/**
 * \fn inline void PackTrack(const LitTrack<fscal>* track[], LitTrack<fvec>& ltrack)
 * \brief Packs \c LitTrack.
 *
 * \param[in] par Array of fvecLen length of LitTrack objects to be packed into a vector.
 * \param[out] lpar Packed LitTrack.
 */
inline void PackTrack(
   const LitTrack<fscal>* track[],
   LitTrack<fvec>& ltrack)
{
//    LitTrackParam<fvec>& lparFirst = ltrack.paramFirst;
   LitTrackParam<fvec>& lparLast = ltrack.paramLast;
   //std::cout << "pack track parameters" << std::endl;
   for(unsigned char i = 0; i < fvecLen; i++) {
//        const LitTrackParam<fscal>* par = &track[i]->paramFirst;

// lparFirst.X[i] = par->X;
// lparFirst.Y[i] = par->Y;
// lparFirst.Tx[i] = par->Tx;
// lparFirst.Ty[i] = par->Ty;
// lparFirst.Qp[i] = par->Qp;
// lparFirst.Z[i] = par->Z;
// lparFirst.C0[i] = par->C0;
// lparFirst.C1[i] = par->C1;
// lparFirst.C2[i] = par->C2;
// lparFirst.C3[i] = par->C3;
// lparFirst.C4[i] = par->C4;
// lparFirst.C5[i] = par->C5;
// lparFirst.C6[i] = par->C6;
// lparFirst.C7[i] = par->C7;
// lparFirst.C8[i] = par->C8;
// lparFirst.C9[i] = par->C9;
// lparFirst.C10[i] = par->C10;
// lparFirst.C11[i] = par->C11;
// lparFirst.C12[i] = par->C12;
// lparFirst.C13[i] = par->C13;
// lparFirst.C14[i] = par->C14;

      const LitTrackParam<fscal>* par = &track[i]->paramLast;
      lparLast.X[i] = par->X;
      lparLast.Y[i] = par->Y;
      lparLast.Tx[i] = par->Tx;
      lparLast.Ty[i] = par->Ty;
      lparLast.Qp[i] = par->Qp;
      lparLast.Z[i] = par->Z;
      lparLast.C0[i] = par->C0;
      lparLast.C1[i] = par->C1;
      lparLast.C2[i] = par->C2;
      lparLast.C3[i] = par->C3;
      lparLast.C4[i] = par->C4;
      lparLast.C5[i] = par->C5;
      lparLast.C6[i] = par->C6;
      lparLast.C7[i] = par->C7;
      lparLast.C8[i] = par->C8;
      lparLast.C9[i] = par->C9;
      lparLast.C10[i] = par->C10;
      lparLast.C11[i] = par->C11;
      lparLast.C12[i] = par->C12;
      lparLast.C13[i] = par->C13;
      lparLast.C14[i] = par->C14;
   }
   //  std::cout << "pack hits" << std::endl;
   unsigned short nofHits = track[0]->GetNofHits();
   for(unsigned short iHit = 0; iHit < nofHits; iHit++) {
      LitPixelHit<fvec>* const lhit = ltrack.hits[iHit];
      for(unsigned char i = 0; i < fvecLen; i++) {
         LitPixelHit<fscal>* const hit = track[i]->hits[iHit];
         lhit->X[i] = hit->X;
         lhit->Y[i] = hit->Y;
         lhit->Dx[i] = hit->Dx;
         lhit->Dy[i] = hit->Dy;
         lhit->Dxy[i] = hit->Dxy;
      }
   }
   //  std::cout << "pack hits finished" << std::endl;
// FIXME: check if we need to set nofHits
//   ltrack.nofHits = nofHits;
}



/**
 * \fn inline void UnpackTrack(const LitTrack<fvec>& ltrack, LitTrack<fscal> track[])
 * \brief Unpacks \c LitTrack.
 *
 * NOTE: Currently not implemented!
 *
 * \param[in] lpar Packed LitTrack to be unpacked.
 * \param[out] par Array of fvecLen length to store unpacked scalar objects.
 */
inline void UnpackTrack(
   const LitTrack<fvec>& ltrack,
   LitTrack<fscal> track[])
{
   std::cout << "UnpackTrack: not implemented!" << std::endl;
}

} // namespace parallel
} // namespace lit
#endif /* LITVECPACK_H_ */
