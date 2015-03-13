/**
 * \file LitMaterialGrid.h
 * \brief Class stores a grid of material thickness in silicon equivalent.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */

#ifndef LITMATERIALGRID_H_
#define LITMATERIALGRID_H_

#include "LitTypes.h"
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

using std::vector;
using std::string;
using std::stringstream;
using std::ostream;
using std::right;
using std::setfill;
using std::setw;

namespace lit {
namespace parallel {

/**
 * \class LitMaterialGrid
 * \brief Class stores a grid of material thickness in silicon equivalent.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 *
 * Each object of this class stores a slice in XY for approximated material
 * thickness in silicon equivalent.
 */
class LitMaterialGrid
{
public:

   /**
    * \brief Returns Z position of the grid.
    * \return Z position of the grid.
    */
 //  fscal GetZ() const { return fZ; }

   /**
    * \brief Sets Z position of the grid.
    * \param[in] Z position of the grid.
    */
 //  void SetZ(fscal Z) { fZ = Z; }

   /**
    * \brief Set material thicknesses for the grid.
    *
    * Grid is a rectangle with
    * (xmin, ymax) as a top left corner and
    * (xmax, ymin) as a bottom right corner and
    * has a total number of bins nofBinsX*nofBinsY.
    *
    * \param[in] material 2D vector of silicon thickness.
    * \param[in] xmax Maximum X position.
    * \param[in] xmin Minimum X position.
    * \param[in] ymax Maximum Y position.
    * \param[in] ymin Minimum Y position.
    * \param[in] nofBinsX Number of bins in X.
    * \param[in] nofBinsY Number of bins in Y.
    */
   void SetMaterial(
         const vector<vector<fscal> >& material,
         fscal xmin,
         fscal xmax,
         fscal ymin,
         fscal ymax,
         int nofBinsX,
         int nofBinsY) {
      fMaterial = material;
      fXMin = xmin;
      fXMax = xmax;
      fYMin = ymin;
      fYMax = ymax;
      fNofBinsX = nofBinsX;
      fNofBinsY = nofBinsY;
      fBinSizeX = ((xmax - xmin) / nofBinsX);
      fBinSizeY = ((ymax - ymin) / nofBinsY);
   }

   /**
    * \brief Return material thickness in silicon equivalent for (X, Y) position (scalar version).
    * \param[in] x X position.
    * \param[in] y Y position.
    * \return Material thickness in silicon equivalent.
    */
   fscal GetMaterial(
      fscal x,
      fscal y) const {
      // Check bound conditions and if out of bounds return zero.
      // Can be removed considering performance!
      if (x < fXMin || x > fXMax || y < fYMin || y > fXMax) {
      //   std::cout << "LitMaterialGrid::GetMaterial: out of bounds x=" << x << " y=" << y << std::endl;
         return 0.;
      }
      // Calculate bin indices for X and Y
      short ix = short((x - fXMin) / fBinSizeX);
      short iy = short((y - fYMin) / fBinSizeY);

      // Check bound conditions and if out of bounds return zero.
      // Can be removed considering performance!
      if (ix < 0 || iy < 0 || ix >= fNofBinsX - 1 || iy >= fNofBinsY - 1) {
       //  std::cout << "LitMaterialGrid::GetMaterial: out of bounds ix=" << ix << " iy=" << iy << std::endl;
         return 0.;
      }

      return fMaterial[ix][iy];


      // Implementation based on the weighted mean of material thicknesses.


//      // Material thickness on the bin nodes
//      const fscal v1 = fMaterial[ix  ][iy];
//      const fscal v2 = fMaterial[ix+1][iy];
//      const fscal v3 = fMaterial[ix  ][iy+1];
//      const fscal v4 = fMaterial[ix+1][iy+1];
//      // Calculate weights depending on the distance to the bin nodes
//      fscal dx1 = (x - ix * fBinSizeX - fXMin);
//      fscal dx2 = (x - (ix + 1) * fBinSizeX - fXMin);
//      fscal dy1 = (y - iy * fBinSizeY - fYMin);
//      fscal dy2 = (y - (iy + 1) * fBinSizeY - fYMin);
//      fscal w1 = 1./(dx1 * dx1 + dy1 * dy1);
//      fscal w2 = 1./(dx2 * dx2 + dy1 * dy1);
//      fscal w3 = 1./(dx1 * dx1 + dy2 * dy2);
//      fscal w4 = 1./(dx2 * dx2 + dy2 * dy2);
//      fscal wsum = w1 + w2 + w3 + w4;
////      cout << "distances: " << 1./w1 << " " << 1./w2 << " "
////            << 1./w3 << " " << 1./w4 << " " << endl;
////      cout << "distances sqrt: " << sqrt(1./w1) << " " << sqrt(1./w2) << " "
////            << sqrt(1./w3) << " " << sqrt(1./w4) << " " << endl;
////      cout << "weights: " << w1/wsum << " " << w2/wsum << " "
////            << w3/wsum << " " << w4/wsum << " " << endl;
//      if (wsum == 0.) { // Can be removed considering performance!
//         return 0.;
//      }
//      // Calculate output weighted mean material thickness
//      return (w1 * v1 + w2 * v2 + w3 * v3 + w4 * v4) / wsum;
   }

   /**
    * \brief Return material thickness in silicon equivalent for (X, Y) position (SIMD version).
    * \param[in] x X position.
    * \param[in] y Y position.
    * \return Material thickness in silicon equivalent.
    */
   fvec GetMaterialValue(
      fvec x,
      fvec y) const {
      fvec v;
      // Get material thickness for each packed value
      for (unsigned int i = 0; i < fvecLen; i++) {
         v[i] = GetMaterial(x[i], y[i]);
      }
      return v;
   }

   /**
    * \brief Check if material was set.
    */
   bool IsEmpty() const {
      return fMaterial.empty();
   }

   /**
    * \brief Return string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitMaterialGrid:" /*Z=" << fZ*/ << " Xmin=" << fXMin << " Xmax=" << fXMax
         << " Ymin=" << fYMin << " Ymax=" << fYMax << " nofBinsX=" << fNofBinsX
         << " nofBinsY=" << fNofBinsY << " binSizeX=" << fBinSizeX
         << " binSizeY=" << fBinSizeY << " material.size=" << fMaterial.size();
      if (fNofBinsX > 0 && fNofBinsY > 0) {
         unsigned int stepX = fNofBinsX / 10;
         unsigned int stepY = fNofBinsY / 10;
         ss << "\nGrid: stepX=" << stepX << " stepY=" << stepY << "\n";
         for (unsigned int i = 0; i < 11; i++) {
            for (unsigned int j = 0; j < 11; j++) {
               if (i < 10 && j < 10) ss << right << setfill(' ') << setw(10) << fMaterial[i * stepX][j * stepY] << " ";
               if (j == 10 && i != 10) ss << right << setfill(' ') << setw(10) << fMaterial[i * stepX][fNofBinsY - 1] << " ";
               if (i == 10 && j != 10) ss << right << setfill(' ') << setw(10) << fMaterial[fNofBinsX - 1][j * stepY] << " ";
               if (i == 10 && j == 10) ss << right << setfill(' ') << setw(10) << fMaterial[fNofBinsX - 1][fNofBinsY - 1] << " ";
            }
            ss << "\n";
         }
      }
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitMaterialGrid& grid) {
      strm << grid.ToString();
      return strm;
   }

private:
   fscal fXMin, fXMax; // Maximum and minimum grid size in X [cm]
   fscal fYMin, fYMax; // Maximum and minimum grid size in Y [cm]
//   fscal fZ; // Z position of grid slice
   unsigned short fNofBinsX; // Number of bins along X
   unsigned short fNofBinsY; // Number of bins along Y
   fscal fBinSizeX; // Bin size along X [cm]
   fscal fBinSizeY; // Bin size along Y [cm]
   // Material thickness in bin nodes.
   // Total number of values is
   // (fNofBinsX + 1) * (fNofBinsY + 1)
   vector<vector<fscal> > fMaterial;
} _fvecalignment;

} // namespace parallel
} // namespace lit


#endif /* LITMATERIALGRID_H_ */
