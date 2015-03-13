#ifndef LITCUDAMATERIALINFO_H_
#define LITCUDAMATERIALINFO_H_

#include <iostream>

struct LitCudaMaterialInfo {
   float Thickness; // Thickness of the material [cm]
   float X0; // Radiation length [cm]
   float Rho; // Density [g/cm^3]
   float Z; //
   float A; //
   float Zpos; // Z position of the material
   float RadThick; // Length/X0
   float SqrtRadThick; // std::sqrt(Length/X0)
   float LogRadThick; // std::log(Length/X0)
   // approximation of the mean excitation energy in GeV
   // I = (Z > 16)? 10 * Z * 1e-9 : 16 * std::pow(Z, 0.9) * 1e-9;
   float I;

   friend std::ostream& operator<<(std::ostream& strm, const LitCudaMaterialInfo& mat) {
      strm << "LitCudaMaterialinfo: Thickness=" << mat.Thickness << ", X0=" << mat.X0
           << ", Rho=" << mat.Rho << ", Z=" << mat.Z << ", A=" << mat.A
           << ", Zpos=" << mat.Zpos << ", I=" << mat.I << std::endl;
      return strm;
   }
};

#endif /* LITCUDAMATERIALINFO_H_ */
