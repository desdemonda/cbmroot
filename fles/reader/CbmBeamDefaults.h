#ifndef CBMBEAMDEFAULTS_H
#define CBMBEAMDEFAULTS_H 1

#include <map>

/**  DetectorID enumerator  **/
enum Exp_Group {kRich=0xE000,
                kMuenster=0xE001,
                kFrankfurt=0xE002,
                kBucarest=0xE003,
		kHodo1=0xE004,
		kHodo2=0xE005};

/** Maximum number of Syscores which could be in the system **/
static const Int_t NrOfSyscores = 3;

/** Maximum number of Spadics which could attached to one Syscore **/
static const Int_t NrOfSpadics = 3;
static const Int_t NrOfHalfSpadics = NrOfSpadics*2;

/** Base address which will be added to each half spadic number 
    the first halfspadic which is connected to the Syscore 
    get SpadicBaseAddress+0, the second SpadicBaseAddress +1 ... */
static const Int_t SpadicBaseAddress = 0;

/*
static const std::map<Int_t, Int_t> groupToExpMap 
  = { std::make_pair(kMuenster,0),
      std::make_pair(kFrankfurt,1),
      std::make_pair(Bucarest,2)
};
*/
#ifndef __CINT__
static const std::map<Int_t, Int_t> groupToExpMap = {
  {kMuenster,0},
  {kFrankfurt,1},
  {kBucarest,2}
};
#endif

#endif
