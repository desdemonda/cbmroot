#ifndef LITCUDATRACK_H_
#define LITCUDATRACK_H_

//#include "LitTypes.h"
#include "LitCudaTrackParam.h"
#include "LitCudaHit.h"

#include <iostream>

const unsigned char LIT_CUDA_MAX_NOF_HITS_IN_TRACK = 30;

struct LitCudaTrack {

   LitCudaTrack():
      chiSq(0.),
      NDF(1),
      nofHits(0),
      nofMissingHits(0),
      previouseTrackId(0) {
   }

   void AddHit(LitCudaPixelHit* hit) {
      hits[nofHits++] = hit;
   }

   LitCudaTrackParam paramFirst;
   LitCudaTrackParam paramLast;
   float chiSq;
   unsigned short NDF;
   LitCudaPixelHit* hits[LIT_CUDA_MAX_NOF_HITS_IN_TRACK];
   unsigned short nofHits;
   unsigned short nofMissingHits;
   unsigned short previouseTrackId;

   friend std::ostream& operator<<(std::ostream& strm, const LitCudaTrack& track) {
      strm << "LitCudaTrack: " << "nofHits=" << track.nofHits << " chiSq=" << track.chiSq
           << " NDF=" << track.NDF << " nofMissingHits=" << track.nofMissingHits
           << " previouseTrackId=" << track.previouseTrackId
           << " paramFirst=" << track.paramFirst << " paramLast=" << track.paramLast
           << std::endl;
      return strm;
   }
};


#endif /* LITCUDATRACK_H_ */
