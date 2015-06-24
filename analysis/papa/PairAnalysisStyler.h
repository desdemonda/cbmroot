#ifndef PAIRANALYSISSTYLER_H
#define PAIRANALYSISSTYLER_H
///////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       //
// PairAnalysis stylers                                                                    //
//                                                                                       //
//                                                                                       //
// Authors:                                                                              //
//   Julian Book <Julian.Book@cern.ch>                                                   //
//                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////


#include <TStyle.h>
#include <TAttMarker.h>
#include <Rtypes.h>

namespace PairAnalysisStyler
{
  enum Estyle { kNMaxMarker=8, kNMaxLine=6, kNMaxColor=10 };
  static const Int_t Marker[]= {kOpenCircle,
				kOpenDiamond,
				kOpenSquare,
				kOpenCross,
				kOpenStar,
				kMultiply,
				kPlus,
				kStar }; // kNMaxMarker

  static const Int_t Line[]= {kSolid,
			      kDashed,
			      kDotted,
			      9,
			      kDashDotted}; // kNMaxLine

  static const Int_t Color[]= {kGray+1,
			       kBlue-4,
			       kRed-4,
			       kGreen+1,
			       kAzure+1,
			       kOrange+1,
			       kSpring+2,
			       kViolet+1,
			       kCyan+2,
			       kGray+1  }; // kNMaxColor

  extern TStyle *fUserDielStyle;   // user defined style
  extern void SetStyle(TStyle *userStyle);

  void LoadStyle();
  void Style(TObject *obj, Int_t idx=0);


}

#endif
