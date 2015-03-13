#ifndef LITCUDATRACKPARAM_H_
#define LITCUDATRACKPARAM_H_ 1

#include <iostream>

struct LitCudaTrackParam {
   float X, Y, Z, Tx, Ty, Qp;

   float
   C0, C1, C2,  C3,  C4,
     C5, C6,  C7,  C8,
     C9, C10, C11,
     C12, C13,
     C14;

   friend std::ostream& operator<<(std::ostream& strm, const LitCudaTrackParam& par) {
      strm << "LitCudaTrackParam: " << "X=" << par.X << " Y=" << par.Y
           << " Z=" << par.Z << " Tx=" << par.Tx << " Ty=" << par.Ty
           << " Qp=" << par.Qp << std::endl;
      return strm;
   }
};

#endif
