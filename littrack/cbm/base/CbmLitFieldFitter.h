/**
 * \file CbmLitFieldFitter.h
 * \brief Implementation of the polynomial field approximation.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/

#ifndef CBMLITFIELDFITTER_H_
#define CBMLITFIELDFITTER_H_

#include <vector>
#include "../../parallel/LitFieldSlice.h"
class FairField;

/**
 * \class CbmLitPolynom
 * \brief Abstract class for polynomial function.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitPolynom
{
public:
	/**
	 * \brief Constructor.
	 */
	CbmLitPolynom() {}

	/**
	 * \brief Destructor.
	 */
	virtual ~CbmLitPolynom() {}

   /**
    * \brief Returns calculated value.
    * \param[in] x X coordinate.
    * \param[in] y Y coordinate.
    * \param[in] c Array of polynomial coefficients.
    * \return Calculated field value.
    */
   virtual double Calculate(
      double x,
      double y,
      double c[]) const = 0;

   /**
    * \brief Return number of coefficients for this polynomial function.
    * \return Number of coefficients for this polynomial function.
    */
   virtual unsigned int GetNofCoefficients() const = 0;
};

/**
 * \class CbmLitFieldFitter
 * \brief Implementation of the polynomial field approximation.
 *
 * Class performs approximation of magnetic field using
 * 0-9 degree polynomial. MINUIT is used for minimization.
 * Also custom LSM fit is implemented for comparison.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitFieldFitter
{
public:
   /**
    * \brief Constructor.
    * \param[in] polynomDegree Polynomial degree to be used for field approximation.
    */
   CbmLitFieldFitter(unsigned int polynomDegree);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitFieldFitter();

   /**
    * \brief Fits (X, Y) slice of the magnetic field at Z position.
    * \param[in] Z Z position of the slice.
    * \param[out] slice Output approximated field slice.
    */
   template <class T>
   void FitSlice(
      float Z,
      lit::parallel::LitFieldSlice<T>& slice);

   /**
    * \brief FitSlice implementation using fscal data type.
    */
   void FitSliceScal(
      float Z,
      lit::parallel::LitFieldSlice<fscal>& slice);

   /**
    * \brief FitSlice implementation using fvec data type.
    */
   void FitSliceVec(
      float Z,
      lit::parallel::LitFieldSlice<fvec>& slice);

   /* Setters */
   void SetXangle(double xangle) {fXangle = xangle;}
   void SetYangle(double yangle) {fYangle = yangle;}
   void SetNofBinsX(int nofBinsX) {fNofBinsX = nofBinsX;}
   void SetNofBinsY(int nofBinsY) {fNofBinsY = nofBinsY;}
   void SetUseEllipseAcc(bool useEllipseAcc) {fUseEllipseAcc = useEllipseAcc;}

private:

   /**
    * \brief Fit (X, Y) slice of magnetic field at Z position.
    * \param[in] Z Z position of the slice.
    * \param[out] parBx Output vector with polynomial coefficients for Bx.
    * \param[out] parBy Output vector with polynomial coefficients for By.
    * \param[out] parBz Output vector with polynomial coefficients for Bz.
    */
   void FitSlice(
      double Z,
      std::vector<double>& parBx,
      std::vector<double>& parBy,
      std::vector<double>& parBz);

   /**
    * \brief Fit (X, Y) slice of magnetic field.
    * \param[in] x Vector with X coordinates.
    * \param[in] y Vector with y coordinates.
    * \param[in] z Vector with field values.
    * \param[out] par Output vector with polynomial coefficients.
    */
   void FitSlice(
      const std::vector<double>& x,
      const std::vector<double>& y,
      const std::vector<double>& z,
      std::vector<double>& par);

   /**
    * \brief Fit (X, Y) slice of the magnetic field at Z position.
    * \param[in] Z Z position of the slice.
    * \param[out] parBx Output vector with polynomial coefficients for Bx.
    * \param[out] parBy Output vector with polynomial coefficients for By.
    * \param[out] parBz Output vector with polynomial coefficients for Bz.
    */
   void FitSliceMy(
      double Z,
      std::vector<double>& parBx,
      std::vector<double>& parBy,
      std::vector<double>& parBz);

   /**
    * \brief Return polynomial which is used for approximation.
    * \return Polynomial which is used for approximation.
    */
   const CbmLitPolynom* GetPolynom() const {return fPolynom;}

private:
   FairField* fField; // Magnetic field

   double fXangle; // Acceptance angle for X [grad]
   double fYangle; // Acceptance angle for Y [grad]

   int fNofBinsX; // Number of bins for X [cm]
   int fNofBinsY; // Number of bins for Y [cm]

   bool fUseEllipseAcc; // If true than only values inside an ellipse will be fitted

   unsigned int fPolynomDegree; // Degree of the polynomial to be used for the field approximation
   CbmLitPolynom* fPolynom; // Polynomial to be used for field approximation
};

#endif /* CBMLITFIELDFITTER_H_ */
