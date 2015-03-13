#ifndef LITCUDAHIT_H_
#define LITCUDAHIT_H_

#include <iostream>

struct LitCudaStripHit {
public:
   float phiCos;
   float phiSin;
   float U;
   float Du;
   unsigned char planeId;
   unsigned short refId;
   float Z;

   friend std::ostream& operator<<(std::ostream& strm, const LitCudaStripHit& hit) {
      strm << "LitCudaStripHit: " << "phiCos=" << hit.phiCos << " phiSin=" << hit.phiSin
           << " U=" << hit.U << " Du=" << hit.Du << " planeId=" << (int)hit.planeId
           << " refId=" << hit.refId << " Z=" << hit.Z << std::endl;
      return strm;
   }
};



struct LitCudaPixelHit {
public:
   float X, Y;
   float Dx, Dy;
   float Dxy;
   unsigned char planeId;
   unsigned short refId;
   float Z;

   friend std::ostream& operator<<(std::ostream& strm, const LitCudaPixelHit& hit) {
      strm << "LitCudaPixelHit: " << "X=" << hit.X << " Y=" << hit.Y
           << " Dx=" << hit.Dx << " Dy=" << hit.Dy << " Dxy=" << hit.Dxy
           << " planeId=" << (int)hit.planeId << " refId=" << hit.refId << " Z=" << hit.Z << std::endl;
      return strm;
   }
};


#endif /* LITCUDAHIT_H_ */
