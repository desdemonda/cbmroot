/**
 * \file CbmLitFieldFitter.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 **/
#include "base/CbmLitFieldFitter.h"
#include "CbmUtils.h"

#include "FairField.h"
#include "FairRunAna.h"

//#include "TFitterMinuit.h"
#include "Minuit2/Minuit2Minimizer.h"
#include "Math/IFunction.h"

#include "TMatrixD.h"
#include "TVectorD.h"

#include <cmath>

/**
 * \class CbmLitPolynom0
 * \brief 0 degree polynomial with 1 coefficient.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitPolynom0 : public CbmLitPolynom
{
public:
	/**
	 * \brief Constructor.
	 */
   CbmLitPolynom0() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPolynom0() {}

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   double Calculate(
      double x,
      double y,
      double c[]) const {
      return c[0];
   }

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   unsigned int GetNofCoefficients() const {return 1;}
};

/**
 * \class CbmLitPolynom1
 * \brief 1st degree polynomial with 3 coefficients.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitPolynom1 : public CbmLitPolynom
{
public:
	/**
	 * \brief Constructor.
	 */
   CbmLitPolynom1() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPolynom1() {}

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   double Calculate(
      double x,
      double y,
      double c[]) const {
      return c[0] + c[1]*x + c[2]*y;
   }

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   unsigned int GetNofCoefficients() const {return 3;}
};

/**
 * \class CbmLitPolynom2
 * \brief 2nd degree polynomial with 6 coefficients.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitPolynom2 : public CbmLitPolynom
{
public:
	/**
	 * \brief Constructor.
	 */
   CbmLitPolynom2() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPolynom2() {}

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   double Calculate(
      double x,
      double y,
      double c[]) const {
      return c[0]
             + c[1]*x + c[2]*y
             + c[3]*x*x + c[4]*x*y + c[5]*y*y;
   }

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   unsigned int GetNofCoefficients() const {return 6;}
};

/**
 * \class CbmLitPolynom3
 * \brief 3rd degree polynomial with 10 coefficients.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitPolynom3 : public CbmLitPolynom
{
public:
	/**
	 * \brief Constructor.
	 */
   CbmLitPolynom3() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPolynom3() {}

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   double Calculate(
      double x,
      double y,
      double c[]) const {
      double x2 = x*x;
      double y2 = y*y;
      double x2y = x2*y;
      double xy2 = x*y2;
      double x3 = x2*x;
      double y3 = y2*y;

      return c[0]
             + c[1]*x + c[2]*y
             + c[3]*x2 + c[4]*x*y + c[5]*y2
             + c[6]*x3 + c[7]*x2y + c[8]*xy2 + c[9]*y3;
   }

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   unsigned int GetNofCoefficients() const {return 10;}
};

/**
 * \class CbmLitPolynom4
 * \brief 4th degree polynomial with 15 coefficients.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitPolynom4 : public CbmLitPolynom
{
public:
	/**
	 * \brief Constructor.
	 */
   CbmLitPolynom4() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPolynom4() {}

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   double Calculate(
      double x,
      double y,
      double c[]) const {

      double x2 = x*x;
      double y2 = y*y;
      double xy = x*y;

      double x3 = x2*x;
      double y3 = y2*y;
      double xy2 = x*y2;
      double x2y = x2*y;

      double x4 = x3*x;
      double y4 = y3*y;
      double xy3 = x*y3;
      double x2y2 = x2*y2;
      double x3y = x3*y;

      return c[0]
             +c[1]*x +c[2]*y
             +c[3]*x2 +c[4]*xy +c[5]*y2
             +c[6]*x3 +c[7]*x2y +c[8]*xy2 +c[9]*y3
             +c[10]*x4 +c[11]*x3y +c[12]*x2y2 +c[13]*xy3 +c[14]*y4;
   }

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   unsigned int GetNofCoefficients() const {return 15;}
};

/**
 * \class CbmLitPolynom5
 * \brief 5th degree polynomial with 21 coefficients.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitPolynom5 : public CbmLitPolynom
{
public:
	/**
	 * \brief Constructor.
	 */
   CbmLitPolynom5() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPolynom5() {}

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   double Calculate(
      double x,
      double y,
      double c[]) const {

      double x2 = x*x;
      double y2 = y*y;
      double xy = x*y;

      double x3 = x2*x;
      double y3 = y2*y;
      double xy2 = x*y2;
      double x2y = x2*y;

      double x4 = x3*x;
      double y4 = y3*y;
      double xy3 = x*y3;
      double x2y2 = x2*y2;
      double x3y = x3*y;

      double x5 = x4*x;
      double y5 = y4*y;
      double xy4 = x*y4;
      double x2y3 = x2*y3;
      double x3y2 = x3*y2;
      double x4y = x4*y;

      return c[0]
             +c[1]*x +c[2]*y
             +c[3]*x2 +c[4]*xy +c[5]*y2
             +c[6]*x3 +c[7]*x2y +c[8]*xy2 +c[9]*y3
             +c[10]*x4 +c[11]*x3y +c[12]*x2y2 +c[13]*xy3 +c[14]*y4
             +c[15]*x5 +c[16]*x4y +c[17]*x3y2 +c[18]*x2y3 +c[19]*xy4 +c[20]*y5;
   }

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   unsigned int GetNofCoefficients() const {return 21;}
};

/**
 * \class CbmLitPolynom6
 * \brief 6th degree polynomial with 28 coefficients.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitPolynom6 : public CbmLitPolynom
{
public:
	/**
	 * \brief Constructor.
	 */
   CbmLitPolynom6() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPolynom6() {}

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   double Calculate(
      double x,
      double y,
      double c[]) const {

      double x2 = x*x;
      double y2 = y*y;
      double xy = x*y;

      double x3 = x2*x;
      double y3 = y2*y;
      double xy2 = x*y2;
      double x2y = x2*y;

      double x4 = x3*x;
      double y4 = y3*y;
      double xy3 = x*y3;
      double x2y2 = x2*y2;
      double x3y = x3*y;

      double x5 = x4*x;
      double y5 = y4*y;
      double xy4 = x*y4;
      double x2y3 = x2*y3;
      double x3y2 = x3*y2;
      double x4y = x4*y;

      double x6 = x5*x;
      double y6 = y5*y;
      double xy5 = x*y5;
      double x2y4 = x2*y4;
      double x3y3 = x3*y3;
      double x4y2 = x4*y2;
      double x5y = x5*y;

      return c[0]
             +c[1]*x +c[2]*y
             +c[3]*x2 +c[4]*xy +c[5]*y2
             +c[6]*x3 +c[7]*x2y +c[8]*xy2 +c[9]*y3
             +c[10]*x4 +c[11]*x3y +c[12]*x2y2 +c[13]*xy3 +c[14]*y4
             +c[15]*x5 +c[16]*x4y +c[17]*x3y2 +c[18]*x2y3 +c[19]*xy4 +c[20]*y5
             +c[21]*x6 +c[22]*x5y +c[23]*x4y2 +c[24]*x3y3 +c[25]*x2y4 +c[26]*xy5 +c[27]*y6;
   }

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   unsigned int GetNofCoefficients() const {return 28;}
};

/**
 * \class CbmLitPolynom7
 * \brief 7th degree polynomial with 36 coefficients.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitPolynom7 : public CbmLitPolynom
{
public:
	/**
	 * \brief Constructor.
	 */
   CbmLitPolynom7() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPolynom7() {}

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   double Calculate(
      double x,
      double y,
      double c[]) const {

      double x2 = x*x;
      double y2 = y*y;
      double xy = x*y;

      double x3 = x2*x;
      double y3 = y2*y;
      double xy2 = x*y2;
      double x2y = x2*y;

      double x4 = x3*x;
      double y4 = y3*y;
      double xy3 = x*y3;
      double x2y2 = x2*y2;
      double x3y = x3*y;

      double x5 = x4*x;
      double y5 = y4*y;
      double xy4 = x*y4;
      double x2y3 = x2*y3;
      double x3y2 = x3*y2;
      double x4y = x4*y;

      double x6 = x5*x;
      double y6 = y5*y;
      double xy5 = x*y5;
      double x2y4 = x2*y4;
      double x3y3 = x3*y3;
      double x4y2 = x4*y2;
      double x5y = x5*y;

      double x7 = x6*x;
      double y7 = y6*y;
      double xy6 = x*y6;
      double x2y5 = x2*y5;
      double x3y4 = x3*y4;
      double x4y3 = x4*y3;
      double x5y2 = x5*y2;
      double x6y = x6*y;

      return c[0]
             +c[1]*x +c[2]*y
             +c[3]*x2 +c[4]*xy +c[5]*y2
             +c[6]*x3 +c[7]*x2y +c[8]*xy2 +c[9]*y3
             +c[10]*x4 +c[11]*x3y +c[12]*x2y2 +c[13]*xy3 +c[14]*y4
             +c[15]*x5 +c[16]*x4y +c[17]*x3y2 +c[18]*x2y3 +c[19]*xy4 +c[20]*y5
             +c[21]*x6 +c[22]*x5y +c[23]*x4y2 +c[24]*x3y3 +c[25]*x2y4 +c[26]*xy5 +c[27]*y6
             +c[28]*x7 +c[29]*x6y +c[30]*x5y2 +c[31]*x4y3 +c[32]*x3y4 +c[33]*x2y5 +c[34]*xy6 +c[35]*y7;
   }

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   unsigned int GetNofCoefficients() const {return 36;}
};

/**
 * \class CbmLitPolynom8
 * \brief 8th degree polynomial with 45 coefficients.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitPolynom8 : public CbmLitPolynom
{
public:
	/**
	 * \brief Constructor.
	 */
   CbmLitPolynom8() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPolynom8() {}

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   double Calculate(
      double x,
      double y,
      double c[]) const {

      double x2 = x*x;
      double y2 = y*y;
      double xy = x*y;

      double x3 = x2*x;
      double y3 = y2*y;
      double xy2 = x*y2;
      double x2y = x2*y;

      double x4 = x3*x;
      double y4 = y3*y;
      double xy3 = x*y3;
      double x2y2 = x2*y2;
      double x3y = x3*y;

      double x5 = x4*x;
      double y5 = y4*y;
      double xy4 = x*y4;
      double x2y3 = x2*y3;
      double x3y2 = x3*y2;
      double x4y = x4*y;

      double x6 = x5*x;
      double y6 = y5*y;
      double xy5 = x*y5;
      double x2y4 = x2*y4;
      double x3y3 = x3*y3;
      double x4y2 = x4*y2;
      double x5y = x5*y;

      double x7 = x6*x;
      double y7 = y6*y;
      double xy6 = x*y6;
      double x2y5 = x2*y5;
      double x3y4 = x3*y4;
      double x4y3 = x4*y3;
      double x5y2 = x5*y2;
      double x6y = x6*y;

      double x8 = x7*x;
      double y8 = y7*y;
      double xy7 = x*y7;
      double x2y6 = x2*y6;
      double x3y5 = x3*y5;
      double x4y4 = x4*y4;
      double x5y3 = x5*y3;
      double x6y2 = x6*y2;
      double x7y = x7*y;

      return c[0]
             +c[1]*x +c[2]*y
             +c[3]*x2 +c[4]*xy +c[5]*y2
             +c[6]*x3 +c[7]*x2y +c[8]*xy2 +c[9]*y3
             +c[10]*x4 +c[11]*x3y +c[12]*x2y2 +c[13]*xy3 +c[14]*y4
             +c[15]*x5 +c[16]*x4y +c[17]*x3y2 +c[18]*x2y3 +c[19]*xy4 +c[20]*y5
             +c[21]*x6 +c[22]*x5y +c[23]*x4y2 +c[24]*x3y3 +c[25]*x2y4 +c[26]*xy5 +c[27]*y6
             +c[28]*x7 +c[29]*x6y +c[30]*x5y2 +c[31]*x4y3 +c[32]*x3y4 +c[33]*x2y5 +c[34]*xy6 +c[35]*y7
      +c[36]*x8 +c[37]*x7y +c[38]*x6y2 +c[39]*x5y3 +c[40]*x4y4 +c[41]*x3y5 +c[42]*x2y6 +c[43]*xy7 +c[44]*y8;
   }

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   unsigned int GetNofCoefficients() const {return 45;}
};

/**
 * \class CbmLitPolynom9
 * \brief 9th degree polynomial with 55 coefficients.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitPolynom9 : public CbmLitPolynom
{
public:
	/**
	 * \brief Constructor.
	 */
   CbmLitPolynom9() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPolynom9() {}

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   double Calculate(
      double x,
      double y,
      double c[]) const {

      double x2 = x*x;
      double y2 = y*y;
      double xy = x*y;

      double x3 = x2*x;
      double y3 = y2*y;
      double xy2 = x*y2;
      double x2y = x2*y;

      double x4 = x3*x;
      double y4 = y3*y;
      double xy3 = x*y3;
      double x2y2 = x2*y2;
      double x3y = x3*y;

      double x5 = x4*x;
      double y5 = y4*y;
      double xy4 = x*y4;
      double x2y3 = x2*y3;
      double x3y2 = x3*y2;
      double x4y = x4*y;

      double x6 = x5*x;
      double y6 = y5*y;
      double xy5 = x*y5;
      double x2y4 = x2*y4;
      double x3y3 = x3*y3;
      double x4y2 = x4*y2;
      double x5y = x5*y;

      double x7 = x6*x;
      double y7 = y6*y;
      double xy6 = x*y6;
      double x2y5 = x2*y5;
      double x3y4 = x3*y4;
      double x4y3 = x4*y3;
      double x5y2 = x5*y2;
      double x6y = x6*y;

      double x8 = x7*x;
      double y8 = y7*y;
      double xy7 = x*y7;
      double x2y6 = x2*y6;
      double x3y5 = x3*y5;
      double x4y4 = x4*y4;
      double x5y3 = x5*y3;
      double x6y2 = x6*y2;
      double x7y = x7*y;

      double x9 = x8*x;
      double y9 = y8*y;
      double xy8 = x*y8;
      double x2y7 = x2*y7;
      double x3y6 = x3*y6;
      double x4y5 = x4*y5;
      double x5y4 = x5*y4;
      double x6y3 = x6*y3;
      double x7y2 = x7*y2;
      double x8y = x8*y;

      return c[0]
             +c[1]*x +c[2]*y
             +c[3]*x2 +c[4]*xy +c[5]*y2
             +c[6]*x3 +c[7]*x2y +c[8]*xy2 +c[9]*y3
             +c[10]*x4 +c[11]*x3y +c[12]*x2y2 +c[13]*xy3 +c[14]*y4
             +c[15]*x5 +c[16]*x4y +c[17]*x3y2 +c[18]*x2y3 +c[19]*xy4 +c[20]*y5
             +c[21]*x6 +c[22]*x5y +c[23]*x4y2 +c[24]*x3y3 +c[25]*x2y4 +c[26]*xy5 +c[27]*y6
             +c[28]*x7 +c[29]*x6y +c[30]*x5y2 +c[31]*x4y3 +c[32]*x3y4 +c[33]*x2y5 +c[34]*xy6 +c[35]*y7
      +c[36]*x8 +c[37]*x7y +c[38]*x6y2 +c[39]*x5y3 +c[40]*x4y4 +c[41]*x3y5 +c[42]*x2y6 +c[43]*xy7 +c[44]*y8
      +c[45]*x9 +c[46]*x8y +c[47]*x7y2 +c[48]*x6y3 +c[49]*x5y4 +c[50]*x4y5 +c[51]*x3y6 +c[52]*x2y7 +c[53]*xy8 +c[54]*y9;
   }

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   unsigned int GetNofCoefficients() const {return 55;}
};

/**
 * \class CbmLitPolynom10
 * \brief 10th degree polynomial with 66 coefficients.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class CbmLitPolynom10 : public CbmLitPolynom
{
public:
	/**
	 * \brief Constructor.
	 */
   CbmLitPolynom10() {}

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPolynom10() {}

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   double Calculate(
      double x,
      double y,
      double c[]) const {
      double x2 = x*x;
      double y2 = y*y;
      double xy = x*y;

      double x3 = x2*x;
      double y3 = y2*y;
      double xy2 = x*y2;
      double x2y = x2*y;

      double x4 = x3*x;
      double y4 = y3*y;
      double xy3 = x*y3;
      double x2y2 = x2*y2;
      double x3y = x3*y;

      double x5 = x4*x;
      double y5 = y4*y;
      double xy4 = x*y4;
      double x2y3 = x2*y3;
      double x3y2 = x3*y2;
      double x4y = x4*y;

      double x6 = x5*x;
      double y6 = y5*y;
      double xy5 = x*y5;
      double x2y4 = x2*y4;
      double x3y3 = x3*y3;
      double x4y2 = x4*y2;
      double x5y = x5*y;

      double x7 = x6*x;
      double y7 = y6*y;
      double xy6 = x*y6;
      double x2y5 = x2*y5;
      double x3y4 = x3*y4;
      double x4y3 = x4*y3;
      double x5y2 = x5*y2;
      double x6y = x6*y;

      double x8 = x7*x;
      double y8 = y7*y;
      double xy7 = x*y7;
      double x2y6 = x2*y6;
      double x3y5 = x3*y5;
      double x4y4 = x4*y4;
      double x5y3 = x5*y3;
      double x6y2 = x6*y2;
      double x7y = x7*y;

      double x9 = x8*x;
      double y9 = y8*y;
      double xy8 = x*y8;
      double x2y7 = x2*y7;
      double x3y6 = x3*y6;
      double x4y5 = x4*y5;
      double x5y4 = x5*y4;
      double x6y3 = x6*y3;
      double x7y2 = x7*y2;
      double x8y = x8*y;

      double x10 = x9*x;
      double y10 = y9*y;
      double xy9 = x*y9;
      double x2y8 = x2*y8;
      double x3y7 = x3*y7;
      double x4y6 = x4*y6;
      double x5y5 = x5*y5;
      double x6y4 = x6*y4;
      double x7y3 = x7*y3;
      double x8y2 = x8*y2;
      double x9y = x9*y;

      return c[0]
             +c[1]*x +c[2]*y
             +c[3]*x2 +c[4]*xy +c[5]*y2
             +c[6]*x3 +c[7]*x2y +c[8]*xy2 +c[9]*y3
             +c[10]*x4 +c[11]*x3y +c[12]*x2y2 +c[13]*xy3 +c[14]*y4
             +c[15]*x5 +c[16]*x4y +c[17]*x3y2 +c[18]*x2y3 +c[19]*xy4 +c[20]*y5
             +c[21]*x6 +c[22]*x5y +c[23]*x4y2 +c[24]*x3y3 +c[25]*x2y4 +c[26]*xy5 +c[27]*y6
             +c[28]*x7 +c[29]*x6y +c[30]*x5y2 +c[31]*x4y3 +c[32]*x3y4 +c[33]*x2y5 +c[34]*xy6 +c[35]*y7
      +c[36]*x8 +c[37]*x7y +c[38]*x6y2 +c[39]*x5y3 +c[40]*x4y4 +c[41]*x3y5 +c[42]*x2y6 +c[43]*xy7 +c[44]*y8
      +c[45]*x9 +c[46]*x8y +c[47]*x7y2 +c[48]*x6y3 +c[49]*x5y4 +c[50]*x4y5 +c[51]*x3y6 +c[52]*x2y7 +c[53]*xy8 +c[54]*y9
      +c[55]*x10 +c[56]*x9y +c[57]*x8y2 +c[58]*x7y3 +c[59]*x6y4 +c[60]*x5y5 +c[61]*x4y6 +c[62]*x3y7 +c[63]*x2y8 +c[64]*xy9 +c[65]*y10;
   }

   /**
    * \brief Inherited from CbmLitPolynom.
    */
   unsigned int GetNofCoefficients() const {return 66;}
};



/**
 * \class FCNPolynom
 * \brief Implements FCNBase which is used for MINUIT minimization.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2009
 */
class FCNPolynom : public ROOT::Math::IBaseFunctionMultiDim {
public:
   /**
    * \brief Constructor.
    */
   FCNPolynom(
      const std::vector<double>& x,
      const std::vector<double>& y,
      const std::vector<double>& z,
      CbmLitPolynom* polynom) :
      fX(x),
      fY(y),
      fZ(z),
      fPolynom(polynom) {}

   /**
    * \brief Destructor.
    */
   ~FCNPolynom() {}

   /**
    * \brief Inherited from FCNBase.
    */
   virtual double Up() const {return 1.;}

   /**
    * \brief Inherited from FCNBase.
    */

  //   virtual double operator()(
  //    const std::vector<double>& par) const 
  Double_t DoEval(const Double_t* x) const
  {
    //       double* p = const_cast<double*>(&par[0]);
    double* p = const_cast<double*>(x);
      double r = 0.;
      for(unsigned int i = 0; i < fX.size(); i++) {
         // calculate weight
//       double rad = std::sqrt(std::fabs(fX[i]*fX[i] + fY[i]*fY[i]));
//       if( rad == 0. ) continue;
//       Double_t w = 1./rad;//(r*r+1);
//       Double_t T = 0.;
//       Double_t w = (1 + T) / (1 + T * std::exp(rad));
         double w = 1.;
         double ri = w * (fZ[i] - fPolynom->Calculate(fX[i], fY[i], p));
         r +=   ri * ri;
      }
      return r;
   }

   unsigned int NDim() const
   {
      return fPolynom->GetNofCoefficients();
   }
 
   ROOT::Math::IBaseFunctionMultiDim* Clone() const
   {
     return new    FCNPolynom(fX, fY, fZ, fPolynom);
   }

   /**
    * \brief Return polynomial which is used for minimization.
    * \return Polynomial which is used for minimization.
    */
   const CbmLitPolynom* GetPolynom() const {return fPolynom;}

private:
   std::vector<double> fX;
   std::vector<double> fY;
   std::vector<double> fZ;
   CbmLitPolynom* fPolynom;
};


CbmLitFieldFitter::CbmLitFieldFitter(
   unsigned int polynomDegree):
   fXangle(27.),
   fYangle(27.),
   fNofBinsX(100),
   fNofBinsY(100),
   fUseEllipseAcc(true),
   fPolynomDegree(polynomDegree)
{
   fField = FairRunAna::Instance()->GetField();

   switch (polynomDegree) {
   case 0:
      fPolynom = new CbmLitPolynom0();
      break;
   case 1:
      fPolynom = new CbmLitPolynom1();
      break;
   case 2:
      fPolynom = new CbmLitPolynom2();
      break;
   case 3:
      fPolynom = new CbmLitPolynom3();
      break;
   case 4:
      fPolynom = new CbmLitPolynom4();
      break;
   case 5:
      fPolynom = new CbmLitPolynom5();
      break;
   case 6:
      fPolynom = new CbmLitPolynom6();
      break;
   case 7:
      fPolynom = new CbmLitPolynom7();
      break;
   case 8:
      fPolynom = new CbmLitPolynom8();
      break;
   case 9:
      fPolynom = new CbmLitPolynom9();
      break;
   default:
      fPolynom = NULL;
      break;
   }
}

CbmLitFieldFitter::~CbmLitFieldFitter()
{

}

template <class T>
void CbmLitFieldFitter::FitSlice(
   float Z,
   lit::parallel::LitFieldSlice<T>& slice)
{
   std::vector<double> aparBx, aparBy, aparBz;
   FitSlice(Z, aparBx, aparBy, aparBz);

   std::vector<T> aparBxT, aparByT, aparBzT;
   aparBxT.assign(aparBx.begin(), aparBx.end());
   aparByT.assign(aparBy.begin(), aparBy.end());
   aparBzT.assign(aparBz.begin(), aparBz.end());

   slice.SetZ(Z);
   slice.SetCoefficients(aparBxT, aparByT, aparBzT);
}

void CbmLitFieldFitter::FitSliceScal(
   float Z,
   lit::parallel::LitFieldSlice<fscal>& slice)
{
   FitSlice<fscal>(Z, slice);
}

void CbmLitFieldFitter::FitSliceVec(
   float Z,
   lit::parallel::LitFieldSlice<fvec>& slice)
{
   FitSlice<fvec>(Z, slice);
}

void CbmLitFieldFitter::FitSlice(
   double Z,
   std::vector<double>& parBx,
   std::vector<double>& parBy,
   std::vector<double>& parBz)
{
   double tanXangle = std::tan(fXangle*3.14159265/180.); //
   double tanYangle = std::tan(fYangle*3.14159265/180.); //

   double Xmax = Z * tanXangle;
   double Ymax = Z * tanYangle;

   std::cout << "Fit slice. Xmax=" << Xmax << " Ymax=" << Ymax
             << " Z=" << Z << " Xanlge=" << fXangle << " Yangle=" << fYangle << std::endl;

   double HX = 2 * Xmax / fNofBinsX; // step size for X position
   double HY = 2 * Ymax / fNofBinsY; // step size for Y position
   std::vector<double> x;
   std::vector<double> y;
   std::vector<double> Bx;
   std::vector<double> By;
   std::vector<double> Bz;
   for (int j = 0; j < fNofBinsX; j++) { // loop over x position
      double X = -Xmax + j * HX;
      for (int k = 0; k < fNofBinsY; k++) { // loop over y position
         double Y = -Ymax + k * HY;

         //check the acceptance
         if (fUseEllipseAcc) {
            double el = (X*X)/(Xmax*Xmax) + (Y*Y)/(Ymax*Ymax);
            if (el > 1.) { continue; }
         }

         // get field value
         double pos[3] = {X, Y, Z};
         double B[3];
         fField->GetFieldValue(pos, B);

         x.push_back(X);
         y.push_back(Y);
         Bx.push_back(B[0]);
         By.push_back(B[1]);
         Bz.push_back(B[2]);
      }
   }
   FitSlice(x, y, Bx, parBx);
   FitSlice(x, y, By, parBy);
   FitSlice(x, y, Bz, parBz);
}

void CbmLitFieldFitter::FitSlice(
   const std::vector<double>& x,
   const std::vector<double>& y,
   const std::vector<double>& z,
   std::vector<double>& par)
{
   FCNPolynom* theFCN = new FCNPolynom(x, y, z, fPolynom);

   //   TFitterMinuit theMinuit;
   ROOT::Minuit2::Minuit2Minimizer theMinuit;

   theMinuit.SetPrintLevel(-1);
   theMinuit.SetFunction(*theFCN);
   int nofParams = theFCN->GetPolynom()->GetNofCoefficients();

   for (int i = 0; i < nofParams; i++) {
      std::string ss = "c" + Cbm::ToString<int>(i);
      theMinuit.SetVariable(i, ss.c_str(), 0., 0.1);
      theMinuit.SetVariableLimits(i, 1., -1.);
   }
   //   theMinuit.CreateMinimizer();
   theMinuit.Minimize();

   const double *fitResults = theMinuit.X();

   par.clear();
   for(int i = 0; i < nofParams; i++) {
      par.push_back(fitResults[i]);
   }
// delete theFCN;
}


void CbmLitFieldFitter::FitSliceMy(
   double Z,
   std::vector<double>& parBx,
   std::vector<double>& parBy,
   std::vector<double>& parBz)
{
   const int M=fPolynomDegree;//5; // polinom order
   const int N=(M+1)*(M+2)/2;

   double tanXangle = std::tan(fXangle*3.14159265/180); // AL
   double tanYangle = std::tan(fYangle*3.14159265/180); // AL

   double Xmax = Z * tanXangle; // AL
   double Ymax = Z * tanYangle; // AL

   double dx = 1.; // step for the field approximation
   double dy = 1.;

   if( dx > Xmax/N/2 ) { dx = Xmax/N/4.; }
   if( dy > Ymax/N/2 ) { dy = Ymax/N/4.; }

//    double C[3][N];
//    for( int i=0; i<3; i++) {
//    for( int k=0; k<N; k++) {
//       C[i][k] = 0.;
//    }
//    }

   TMatrixD A(N,N);
//    A.Zero();
   TVectorD b0(N), b1(N), b2(N);
//    b0.Zero();
//    b1.Zero();
//    b2.Zero();
   for( int i=0; i<N; i++) {
      for( int j=0; j<N; j++) {
         A(i,j)=0.; // AL was ==
      }
      b0(i) = b1(i) = b2(i) = 0.;
   }
   for( double x=-Xmax; x<=Xmax; x+=dx ) {
      for( double y=-Ymax; y<=Ymax; y+=dy )  {
         double r = std::sqrt(std::fabs(x*x/Xmax/Xmax+y/Ymax*y/Ymax));
         if( r>1. ) { continue; }
         Double_t w = 1./(r*r+1);
         Double_t p[3] = { x, y, Z};
         Double_t B[3] = {0., 0., 0.};
         FairRunAna::Instance()->GetField()->GetFieldValue(p, B);
         TVectorD m(N);
         m(0)=1;
         for( int i=1; i<=M; i++) {
            int k = (i-1)*(i)/2;
            int l = i*(i+1)/2;
            for( int j=0; j<i; j++ ) { m(l+j) = x*m(k+j); }
            m(l+i) = y*m(k+i-1);
         }

         TVectorD mt = m;
         for( int i=0; i<N; i++) {
            for( int j=0; j<N; j++) {
               A(i,j)+=w*m(i)*m(j);
            }
            b0(i)+=w*B[0]*m(i);
            b1(i)+=w*B[1]*m(i);
            b2(i)+=w*B[2]*m(i);
         }
      }
   }
   double det;
   A.Invert(&det);
   TVectorD c0 = A*b0, c1 = A*b1, c2 = A*b2;
//    for(int i=0; i<N; i++){
//    C[0][i] = c0(i);
//    C[1][i] = c1(i);
//    C[2][i] = c2(i);
//    }
   for(int i=0; i<N; i++) {
      parBx.push_back(c0(i));
      parBy.push_back(c1(i));
      parBz.push_back(c2(i));
   }
   //    geo[ind++] = N;
   //    for( int k=0; k<3; k++ ){
   //      for( int j=0; j<N; j++) geo[ind++] = C[k][j];
   //    }
}
