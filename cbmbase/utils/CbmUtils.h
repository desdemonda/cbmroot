#ifndef CBMUTILS_H_
#define CBMUTILS_H_

class TCanvas;
class TH1;
class TH1D;
class TH2;
class TH2D;

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using std::string;
using std::vector;

namespace Cbm
{

template <class T>
string ToString(
   const T& value)
{
   std::stringstream ss;
   ss << (T)value;
   return ss.str();
}

template <class T>
string NumberToString(
   const T& value, int precision = 1)
{
   // First determine number of digits in float
   string digis = ToString<int>(value);
   int ndigis = digis.size();

   std::stringstream ss;
   ss.precision(ndigis + precision);
   ss << value;
   return ss.str();
}

/* Returns -1 if x<0, +1 if x>0, 0 if x==0 */
template <class T>
int Sign(
   const T& x)
{
   static const T ZERO = 0;
   return (x > ZERO) ? 1 : ((x < ZERO) ? -1 : 0);
}

void SaveCanvasAsImage(
   TCanvas* c,
   const std::string& dir,
   const std::string& option = "eps;png;gif");

string FindAndReplace(
		const string& name,
		const string& oldSubstr,
		const string& newSubstr);

vector<string> Split(
		const string& name,
		char delimiter);

/*
 * \brief Divide 1D histograms and return result histogram h = h1 / h2.
 * \param[in] h1 Pointer to the first histogram.
 * \param[in] h2 Pointer to the second histogram.
 * \param[in] histName Name of the result histogram. if histName = "" then histName = h1->GetName() + "_divide"
 * \param[in] scale Scale factor of result histogram.
 * \param[in] titleYaxis Y axis title of result histogram.
 */
TH1D* DivideH1(
		TH1* h1,
		TH1* h2,
		const string& histName = "",
		double scale = 100.,
		const string& titleYaxis = "Efficiency [%]");


/*
 * \brief Divide 2D histograms and return result histogram h = h1 / h2.
 * \param[in] h1 Pointer to the first histogram.
 * \param[in] h2 Pointer to the second histogram.
 * \param[in] histName Name of the result histogram. if histName = "" then histName = h1->GetName() + "_divide"
 * \param[in] scale Scale factor of result histogram.
 * \param[in] titleZaxis Z axis title of result histogram.
 */
TH2D* DivideH2(
      TH2* h1,
      TH2* h2,
	  const string& histName = "",
	  double scale = 100.,
	  const string& titleZaxis = "Efficiency [%]");

}

#endif /* CBMUTILS_H_ */
