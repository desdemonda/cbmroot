/** LitCudaField.h
 * @author Andrey Lebedev <andrey.lebedev@gsi.de>
 * @since 2009
 * @version 1.0
 **/

#ifndef LITCUDAFIELD_H_
#define LITCUDAFIELD_H_

struct LitCudaFieldValue {
   // components of the magnetic field
   float Bx, By, Bz;
};



struct LitCudaFieldSlice {
   LitCudaFieldSlice() {
      for(unsigned int i=0; i<10; i++) { cx[i] = cy[i] = cz[i] = 0; }
   }

   float cx[10], cy[10], cz[10]; // polinom coefficients
   float Z; // Z position of the slice

// friend std::ostream & operator<<(std::ostream &strm, const LitFieldSlice &slice){
//    strm << "LitFieldSlice: Z=" << slice.Z << ", cx=";
//    for(unsigned int i = 0; i < 10; i++) strm << slice.cx[i] << " ";
//    strm << std::endl;
//    strm << "LitFieldSlice: cy=";
//    for(unsigned int i = 0; i < 10; i++) strm << slice.cy[i] << " ";
//    strm << std::endl;
//    strm << "LitFieldSlice: cz=";
//    for(unsigned int i = 0; i < 10; i++) strm << slice.cz[i] << " ";
//    strm << std::endl;
//    return strm;
// }

};



__device__ void GetFieldValue(
   float x,
   float y,
   const LitCudaFieldSlice* slice,
   LitCudaFieldValue* B)
{
   float x2 = x*x;
   float y2 = y*y;
   float yx2 = y*x2;
   float xy2 = x*y2;
   float x3 = x2*x;
   float y3 = y2*y;
   const float* cx = slice->cx;
   const float* cy = slice->cy;
   const float* cz = slice->cz;
   B->Bx = cx[0] + cx[1]*x + cx[2]*y + cx[3]*x*y + cx[4]*x2 + cx[5]*y2 + cx[6]*xy2 + cx[7]*yx2 + cx[8]*x3 + cx[9]*y3;
   B->By = cy[0] + cy[1]*x + cy[2]*y + cy[3]*x*y + cy[4]*x2 + cy[5]*y2 + cy[6]*xy2 + cy[7]*yx2 + cy[8]*x3 + cy[9]*y3;
   B->Bz = cz[0] + cz[1]*x + cz[2]*y + cz[3]*x*y + cz[4]*x2 + cz[5]*y2 + cz[6]*xy2 + cz[7]*yx2 + cz[8]*x3 + cz[9]*y3;
}



struct LitCudaFieldRegion {
   void Set(
      const LitCudaFieldValue* B0,
      const float B0z,
      const LitCudaFieldValue* B1,
      const float B1z,
      const LitCudaFieldValue* B2,
      const float B2z ) {
      z0 = B0z;
      float dz1 = B1z - B0z, dz2 = B2z - B0z;
      float det = 1./(dz1 * dz2 * (dz2 - dz1));//rcp(float(dz1 * dz2 * (dz2 - dz1)));
      float w21 = -dz2 * det;
      float w22 = dz1 * det;
      float w11 = -dz2 * w21;
      float w12 = -dz1 * w22;

      float dB1 = B1->Bx - B0->Bx;
      float dB2 = B2->Bx - B0->Bx;
      cx0 = B0->Bx;
      cx1 = dB1 * w11 + dB2 * w12;
      cx2 = dB1 * w21 + dB2 * w22;

      dB1 = B1->By - B0->By;
      dB2 = B2->By - B0->By;
      cy0 = B0->By;
      cy1 = dB1 * w11 + dB2 * w12;
      cy2 = dB1 * w21 + dB2 * w22;

      dB1 = B1->Bz - B0->Bz;
      dB2 = B2->Bz - B0->Bz;
      cz0 = B0->Bz;
      cz1 = dB1 * w11 + dB2 * w12;
      cz2 = dB1 * w21 + dB2 * w22;
   }

   void Shift(
      float z) {
      float dz = z-z0;
      float cx2dz = cx2*dz;
      float cy2dz = cy2*dz;
      float cz2dz = cz2*dz;
      z0 = z;
      cx0+= ( cx1 + cx2dz )*dz;
      cy0+= ( cy1 + cy2dz )*dz;
      cz0+= ( cz1 + cz2dz )*dz;
      cx1+= cx2dz + cx2dz;
      cy1+= cy2dz + cy2dz;
      cz1+= cz2dz + cz2dz;
   }

   float cx0, cx1, cx2 ; // Bx(z) = cx0 + cx1*(z-z0) + cx2*(z-z0)^2
   float cy0, cy1, cy2 ; // By(z) = cy0 + cy1*(z-z0) + cy2*(z-z0)^2
   float cz0, cz1, cz2 ; // Bz(z) = cz0 + cz1*(z-z0) + cz2*(z-z0)^2
   float z0;
};


__device__ void GetFieldValue(
   float z,
   const LitCudaFieldRegion* region,
   LitCudaFieldValue* B)
{
   float dz = z - region->z0;
   float dzdz = dz * dz;
   B->Bx = region->cx0 + region->cx1 * dz + region->cx2 * dzdz;
   B->By = region->cy0 + region->cy1 * dz + region->cy2 * dzdz;
   B->Bz = region->cz0 + region->cz1 * dz + region->cz2 * dzdz;
}

__device__ void Set(
   const LitCudaFieldValue* B0,
   const float B0z,
   const LitCudaFieldValue* B1,
   const float B1z,
   LitCudaFieldRegion* region)
{
   region->z0 = B0z;
   float dzi = 1./(B1z - B0z);
   region->cx0 = B0->Bx;
   region->cy0 = B0->By;
   region->cz0 = B0->Bz;
   region->cx1 = (B1->Bx - B0->Bx) * dzi;
   region->cy1 = (B1->By - B0->By) * dzi;
   region->cz1 = (B1->Bz - B0->Bz) * dzi;
   region->cx2 = region->cy2 = region->cz2 = 0;
}

#endif /* LITCUDAFIELD_H_ */
