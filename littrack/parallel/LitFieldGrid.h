/**
 * \file LitFieldGrid.h
 * \brief Class stores a grid of magnetic field values in XY slice at Z position.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
#ifndef LITFIELDGRID_H_
#define LITFIELDGRID_H_

#include "LitFieldValue.h"
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
 * \class LitFieldGrid
 * \brief Class stores a grid of magnetic field values in XY slice at Z position.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 *
 * This approach is used as an alternative to
 * parabolic field approximation. It is more
 * precise - the same grid is used as
 * in the full magnetic field map. However
 * the access to the grid can not be SIMDized.
 * And the values are accessed one by one
 * and packed later to the vector.
 * But it is still fast!
 * The value of the field is calculated as a weighted
 * mean of the four surrounding values.
 */
class LitFieldGrid
{
public:

   /**
    * \brief Constructor
    */
   LitFieldGrid() :
      fXMin(0.),
      fXMax(0.),
      fYMin(0.),
      fYMax(0.),
      fZ(0.),
      fNofBinsX(0),
      fNofBinsY(0),
      fBinSizeX(0.),
      fBinSizeY(0.),
      fField() { }

   /**
    * \brief Returns Z position of the grid.
    * \return Z position of the grid.
    */
   fscal GetZ() const { return fZ; }

   /**
    * \brief Sets Z position of the grid.
    * \param[in] Z position of the grid.
    */
   void SetZ(fscal Z) { fZ = Z; }

   /**
    * \brief Set field values for the grid.
    *
    * Grid is a rectangle with
    * (xmin, ymax) as a top left corner and
    * (xmax, ymin) as a bottom right corner and
    * has a total number of bins nofBinsX*nofBinsY.
    *
    * \param[in] field 2D vector of field values.
    * \param[in] xmax Maximum X position.
    * \param[in] xmin Minimum X position.
    * \param[in] ymax Maximum Y position.
    * \param[in] ymin Minimum Y position.
    * \param[in] nofBinsX Number of bins in X.
    * \param[in] nofBinsY Number of bins in Y.
    */
   void SetField(
         const vector<vector<LitFieldValue<fscal> > >& field,
         fscal xmin,
         fscal xmax,
         fscal ymin,
         fscal ymax,
         int nofBinsX,
         int nofBinsY) {
      fField = field;
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
    * \brief Return field value for (X, Y) position (scalar version).
    * \param[in] x X position.
    * \param[in] y Y position.
    * \param[out] B Field value.
    */
   void GetFieldValue(
      fscal x,
      fscal y,
      LitFieldValue<fscal> &B) const {
      // Check bound conditions and if out of bounds return zero field values.
      // Can be removed considering performance!
      if (x < fXMin || x > fXMax || y < fYMin || y > fXMax) {
         B.Bx = 0.;
         B.By = 0.;
         B.Bz = 0.;
         return;
      }
      // Calculate bin indices for X and Y
      unsigned short ix = short((x - fXMin) / fBinSizeX);
      unsigned short iy = short((y - fYMin) / fBinSizeY);

      // Check bound conditions and if out of bounds return zero field values.
      // Can be removed considering performance!
      if (ix < 0 || iy < 0 || ix >= fNofBinsX - 1 || iy >= fNofBinsY - 1) {
    	  B.Bx = 0.;
    	  B.By = 0.;
    	  B.Bz = 0.;
    	  return;
      }

      B = fField[ix][iy];

//      // Field values on the bin nodes
//      const LitFieldValue<fscal>& v1 = fField[ix  ][iy];
//      const LitFieldValue<fscal>& v2 = fField[ix+1][iy];
//      const LitFieldValue<fscal>& v3 = fField[ix  ][iy+1];
//      const LitFieldValue<fscal>& v4 = fField[ix+1][iy+1];
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
//         B.Bx = 0.;
//         B.By = 0.;
//         B.Bz = 0.;
//         cout << "LitFieldGrid::GetFieldValue: zero wsum=" << wsum << endl;
//         return;
//      }
//      // Calculate output weighted mean B value
//      B.Bx = (w1 * v1.Bx + w2 * v2.Bx + w3 * v3.Bx + w4 * v4.Bx) / wsum;
//      B.By = (w1 * v1.By + w2 * v2.By + w3 * v3.By + w4 * v4.By) / wsum;
//      B.Bz = (w1 * v1.Bz + w2 * v2.Bz + w3 * v3.Bz + w4 * v4.Bz) / wsum;
   }

   /**
    * \brief Returns field value for (X, Y) position (SIMD version).
    * \param[in] x X position.
    * \param[in] y Y position.
    * \param[out] B Field value.
    */
   void GetFieldValue(
      fvec x,
      fvec y,
      LitFieldValue<fvec> &B) const {
      LitFieldValue<fscal> v;
      // Get field value for each packed value
      for (unsigned int i = 0; i < fvecLen; i++) {
         // Get field value in scalar format
         GetFieldValue(x[i], y[i], v);
         // Store field value in vector format
         B.Bx[i] = v.Bx;
         B.By[i] = v.By;
         B.Bz[i] = v.Bz;
      }
   }

   /**
    * \brief Check if field was set.
    */
   bool IsEmpty() const {
      return fField.empty();
   }

   /**
    * \brief Returns string representation of the class.
    * \return String representation of the class.
    */
   string ToString() const {
      stringstream ss;
      ss << "LitFieldGrid: Z=" << fZ << " Xmin=" << fXMin << " Xmax=" << fXMax
         << " Ymin=" << fYMin << " Ymax=" << fYMax << " nofBinsX=" << fNofBinsX
         << " nofBinsY=" << fNofBinsY << " binSizeX=" << fBinSizeX
         << " binSizeY=" << fBinSizeY << " field.size=" << fField.size();
//      if (fNofBinsX > 0 && fNofBinsY > 0) {
//         ss << "\nGrid:\n";
//         unsigned int stepX = fNofBinsX / 10;
//         unsigned int stepY = fNofBinsY / 10;
//         for (unsigned int i = 0; i < 10; i++) {
//            for (unsigned int j = 0; j < 10; j++) {
//               const LitFieldValue<fscal>& v = fField[i * stepX][j * stepY];
//               ss << right << setfill(' ') << setw(15) << v.Bx;
//            }
//            ss << "\n";
//         }
//      }
      return ss.str();
   }

   /**
    * \brief Operator << for convenient output to ostream.
    * \return Insertion stream in order to be able to call a succession of insertion operations.
    */
   friend ostream& operator<<(ostream& strm, const LitFieldGrid& grid) {
      strm << grid.ToString();
      return strm;
   }
private:
   fscal fXMin, fXMax; // Maximum and minimum grid size in X [cm]
   fscal fYMin, fYMax; // Maximum and minimum grid size in Y [cm]
   fscal fZ; // Z position of grid slice
   unsigned short fNofBinsX; // Number of bins along X
   unsigned short fNofBinsY; // Number of bins along Y
   fscal fBinSizeX; // Bin size along X [cm]
   fscal fBinSizeY; // Bin size along Y [cm]
   // Field values in bin nodes.
   // Total number of field values is
   // (fNofBinsX + 1) * (fNofBinsY + 1)
   vector<vector<LitFieldValue<fscal> > > fField;
} _fvecalignment;

} // namespace parallel
} // namespace lit
#endif /* LITFIELDGRID_H_ */
