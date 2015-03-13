/**
* \file CbmRichRingFitterEllipseMinuit.h
*
* \brief This is the implementation of ellipse fitting using MINUIT.
* \author Semen Lebedev <s.lebedev@gsi.de>
* \date 2011
**/

#ifndef CBM_RICH_RING_FITTER_ELLIPSE_MINUIT
#define CBM_RICH_RING_FITTER_ELLIPSE_MINUIT

#include "CbmRichRingFitterEllipseBase.h"

#include "Math/IFunction.h"
#include "Rtypes.h"

#include <vector>

using std::vector;

class FCNEllipse2: public ROOT::Math::IBaseFunctionMultiDim 
{
private:
   vector<Double_t> fX; // vector of X coordinates
   vector<Double_t> fY; // vector of Y coordinates
public:

 FCNEllipse2(vector<Double_t> x, vector<Double_t> y) 
     : ROOT::Math::IBaseFunctionMultiDim(), 
     fX(x),
     fY(y)
     {}
     
   double DoEval(const double* x) const 
   {
     Double_t r = 0.;
     for(UInt_t i = 0; i < fX.size(); i++) {
       Double_t ri = calcE(i, x);
       r +=   ri * ri;
     }
     return r;
   }
   
   Double_t calcE(
		  Int_t i,
		  const double* par) const
   {
     Double_t d1 = sqrt( (fX[i] - par[0])*(fX[i] - par[0]) +
			 (fY[i] - par[1])*(fY[i] - par[1])  );
     Double_t d2 = sqrt( (fX[i] - par[2])*(fX[i] - par[2]) +
			 (fY[i] - par[3])*(fY[i] - par[3])  );
     Double_t ri = d1 + d2 - 2 * par[4];
     return ri;
   }
   
   unsigned int NDim() const
   {
      return 5;
   }
 
   ROOT::Math::IBaseFunctionMultiDim* Clone() const
   {
     return new FCNEllipse2(fX, fY);
   }
};


/**
* \class CbmRichRingFitterEllipseMinuit
*
* \brief This is the implementation of ellipse fitting using MINUIT.
* \author Semen Lebedev <s.lebedev@gsi.de>
* \date 2011
**/
class CbmRichRingFitterEllipseMinuit : public CbmRichRingFitterEllipseBase
{
public:

   /**
    * \brief Default constructor.
    */
   CbmRichRingFitterEllipseMinuit();

   /**
    * \brief Standard destructor.
    */
   virtual ~CbmRichRingFitterEllipseMinuit();

   /**
    * \brief Inherited from CbmRichRingFitterBase.
    */
   virtual void DoFit(
         CbmRichRingLight *ring);

private:
   /**
    * \brief Execute ring fitting algorithm.
    * \param[in] x X coordinates of hits.
    * \param[in] y Y coordinates of hit.
    * \return Vector of fitted parameters.
    */
   vector<double> DoFit(
         const vector<double>& x,
         const vector<double>& y);

   /**
    * \brief Transform obtained parameters from MINUIT to CbmRichRingLight.
    * \param[out] ring Pointer to the RICH ring.
    * \param[in] par Parameters obtained from MINUIT.
    */
   void TransformToRichRing(
         CbmRichRingLight* ring,
         const vector<double>& par);

};

#endif
