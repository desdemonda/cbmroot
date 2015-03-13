/*
  File: CbmMvdStateMatch.h

*/

#ifndef CbmMvdStateMatch_H
#define CbmMvdStateMatchs_H 1

#include "TObject.h"
#include <vector>
#include <iostream>
//#include "FairTimeStamp.h"

using std::cout;
using std::endl;
using namespace std;

class CbmMvdStateMatch : public TObject
{
  public:
    
    /** Default constructor**/
    CbmMvdStateMatch();

    /** Constructor with all variables **/
    CbmMvdStateMatch(unsigned char* mimodata);
  
  // destructor
   ~CbmMvdStateMatch();
 // vector< Int_t > vectorPoint;

  /** accesors**/
  Int_t GetHeader()		{return (Int_t)( fMimoData[fWidthOfData-1]*16777216 + fMimoData[fWidthOfData-2]*65536 + fMimoData[fWidthOfData-3]*256 + fMimoData[fWidthOfData-4]);};
  Int_t GetFrameCounter()	{return (Int_t)( fMimoData[2*fWidthOfData-1]*16777216 + fMimoData[2*fWidthOfData-2]*65536 + fMimoData[2*fWidthOfData-3]*256 + fMimoData[2*fWidthOfData-4]);};
  Int_t GetDatalength()		{return (Int_t)( fMimoData[3*fWidthOfData-1]*16777216 + fMimoData[3*fWidthOfData-2]*65536 + fMimoData[3*fWidthOfData-3]*256 + fMimoData[3*fWidthOfData-4]);};
  Int_t GetTrailer()		{return (Int_t)( fMimoData[fByteNumberOfData-1]*16777216 + fMimoData[fByteNumberOfData-2]*65536 + fMimoData[fByteNumberOfData-3]*256 + fMimoData[fByteNumberOfData-4]);};
 
  void GetState( vector< unsigned char >& state);//only read the useful data from fMimoData into the vector state[]
 
   /** modifier **/
  //header and framecounter are 32-bits value
 
  
  private:
    
  vector< vector< Int_t > > fStateInformation
    

      
ClassDef(CbmMvdStateMatch,1);
};
#endif