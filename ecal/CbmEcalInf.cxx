// -------------------------------------------------------------------------
// -----                  CbmEcalStructure source file                 -----
// -----                  Created  27/01/06  by M.Prokudin             -----
// -------------------------------------------------------------------------

/**  CbmEcalStructure.cxx
 *@author Mikhail Prokudin
 **
 ** Container of ECAL geometry parameters
 **/

/* $Id: CbmEcalInf.cxx,v 1.12 2006/08/30 13:16:45 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalInf.cxx,v $
 * Revision 1.12  2006/08/30 13:16:45  prokudin
 * Bug huntong
 *
 * Revision 1.11  2006/07/29 00:33:53  prokudin
 * Commenting. Bringing code to coding conventions. New way of parameter file processing
 *
 * Revision 1.10  2006/07/24 21:45:02  prokudin
 * Now we write information to parameter file
 *
 * Revision 1.9  2006/07/15 18:31:10  kharlov
 * Compilation warnings and coding conventions are corrected
 *
 * Revision 1.8  2006/07/12 12:04:59  prokudin
 * CbmEcalInf now singleton.
 *
 * Revision 1.7  2006/07/10 07:43:25  kharlov
 * Conversion of cell type from string to a number
 *
 * Revision 1.6  2006/06/30 06:17:17  kharlov
 * Optimization, commenting
 *
 * Revision 1.5  2006/06/22 06:52:13  kharlov
 * Compilation warnings removed
 *
 * Revision 1.4  2006/04/02 16:55:23  kharlov
 * Merging full and fast MC
 *
 * Revision 1.2  2006/01/31 17:07:17  kharlov
 * Correction for CbmEcalv1
 *
 * Revision 1.1  2006/01/27 17:26:30  kharlov
 * First adaptation of ECAL detailed geometry to cbmroot2
 *
 */
#include "CbmEcalInf.h"

#include "CbmGeoEcalPar.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TSystem.h"
#include "TMap.h"

#include <iostream>
#include <fstream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

CbmEcalInf* CbmEcalInf::fInf=NULL;
Int_t CbmEcalInf::fRefCount=0;


CbmEcalInf::~CbmEcalInf()
{
	for(Int_t i=0;i<fEcalStr.GetSize();i++)
		delete (TObjString*)(fEcalStr.At(i));
	fEcalStr.Clear();
}

CbmEcalInf* CbmEcalInf::GetInstance(const char* filename)
{
	if (filename==NULL)
	{
		if (fInf!=NULL)
			fRefCount++;
		return fInf;
	}
	TString newname=gSystem->Getenv("VMCWORKDIR");
	newname+="/geometry/";
	newname+=filename;
	if (fInf!=NULL)
		if (fInf->fFileName==newname)
		{
			fRefCount++;
			return fInf;
		}
		else
		{
		  cerr << "CbmEcalInf: Trying create ";
		  cerr << "instance of CbmEcalInf with";
		  cerr << " name " << filename;
		  cerr << ", which is different from ";
		  cerr << fInf->fFileName << "." << endl;
		  return NULL;
		}
	fInf=new CbmEcalInf(newname);
	//Is something wrong?
	if (fInf->fSuccess==0)
	{
		delete fInf;
		return NULL;
	}
	fRefCount++;
	return fInf;
}

int cmp_nocase(const string &s, const string &s2 )
{
  string::const_iterator p=s.begin();
  string::const_iterator p2=s2.begin();
  while(p!=s.end()&&p2!=s2.end()) {
    if (toupper(*p)!=toupper(*p2)) return (toupper(*p)<toupper(*p2))?-1:1;
    ++p;
    ++p2;
  }
  return(s2.size()==s.size())?0:(s.size()<s2.size())?-1:1; // size is unsigned
}

Double_t CbmEcalInf::GetVariableStrict(const char* key)
{
  TObjString* value=(TObjString*)fVariables->GetValue(key);
  if (value==NULL)
  {
    cerr << "Can't find variable named \"" << key << "\"";
    cerr << ". Exiting..." << endl;
    Fatal("","");
  }
  Double_t val;
  char* err=NULL;
  val=strtod(value->GetString(),&err);
  if (err[0]!='\0')
  {
    cerr << "Can't convert variable named \"" << key ;
    cerr << "\" to floating point. Value is \"";
    cerr << value->GetString() << "\"." << endl;
    cerr << "Exiting..." << endl;
    Fatal("","");
  }
  return val;
}

TString CbmEcalInf::GetStringVariable(const char* key)
{
  TObjString* value=(TObjString*)fVariables->GetValue(key);
  if (value==NULL)
  {
    Fatal("GetStringVariable","Can't find variable named %s.", key);
  }
  return value->GetString();
}



Double_t CbmEcalInf::GetVariable(const char* key)
{
  TObjString* value=(TObjString*)fVariables->GetValue(key);
  if (value==NULL)
    return -1111;
  Double_t val;
  char* err=NULL;
  val=strtod(value->GetString(),&err);
  if (err[0]!='\0')
    return -1111;
  return val;
}

void CbmEcalInf::AddVariable(const char* key, const char* value)
{
  TObjString* skey=(TObjString*)fVariables->FindObject(key);
  //Value for this key already exists!!!
  if (skey!=NULL) return;
  skey=new TObjString(key);
  skey->String().ToLower();
  TObjString* svalue=new TObjString(value);
  fVariables->Add(skey, svalue);
}
//=============================================================================
CbmEcalInf::CbmEcalInf(const char* filename)
  : TObject(), 
    fVariables(new TMap(200)), 
    fEcalStr(), 
    fXPos(0.), 
    fYPos(0.),
    fZPos(0.), 
    fNLayers(0), 
    fXSize(0), 
    fYSize(0), 
    fModuleSize(0.), 
    fPSLead(0.),
    fPSScin(0.), 
    fPSGap(0.), 
    fLead(0.), 
    fScin(0.), 
    fTyveec(0.), 
    fThicknessLayer(0.),
    fCellSize(0.), 
    fEcalSize(), 
    fEcalHole(), 
    fECut(0.), 
    fHCut(0.), 
    fFastMC(-1),
    fSuccess(1), 
    fFileName(filename) 
{
  /**
   ** Constructor reads and parses the ascii file, and fill 
   ** the ECAL geometry container
   **/
  FairRunAna* ana = FairRunAna::Instance();
  if (ana!=NULL)
  {
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();
    rtdb->getContainer("CbmGeoEcalPar");
  }

  std::ifstream file(filename);
  Int_t linenum;
  Double_t val;
  string buffer;
  string message;
  string variable;
  string value;
  TObjString* str=NULL;
  char** err=NULL;
  char winend[2]={13, 0};
  int ssize=-1;

  if (!file) {
    cerr << "CbmEcalInf: Can't open information file " << filename << "!" << endl;
    cerr << "CbmEcalInf: Ecal will not be constructed correctly." << endl;
    fSuccess=0;
    return;
  }

  linenum=0;
  while(getline(file,buffer)) {
    linenum++;
    message=buffer.substr(buffer.find_first_not_of(" 	"));	//Skiping initial spaces
    message=message.substr(0,message.find("#"));	//Removing comments
    // Threat windows end of strings correctly
    message=message.substr(0,message.find(winend));
    if (message.empty()) continue;		//This was just a comment
    variable=message.substr(0,message.find("="));
    if (variable=="structure") {
      while(getline(file,buffer)) {
	linenum++;
	if (buffer.empty()) break;
	message=buffer.substr(buffer.find_first_not_of(" 	"));	//Skiping initial spaces
	message=message.substr(0,message.find("#"));	//Removing comments
	message=message.substr(0,message.find_last_not_of(" 	")+1);	//Skiping ending spaces

        // Threat windows end of strings correctly
        message=message.substr(0,message.find(winend));

	if (!message.empty()) {
	  if (-1==ssize)
	    ssize=message.size();
	  else
	    if (ssize!=(Int_t)message.size()) {
	      cerr << "Error in ECAL structure at line " << linenum;
	      cerr << "." << endl;
	      cerr << "Line length differs from previous one" << endl;
              fSuccess=0;
	      file.close();
	      return;
	      
	    }
	  
	  str=new TObjString(message.c_str());
	  fEcalStr.Add(str);
	}
      }
      break;
    }
    if (variable==message) {
      cerr << "Syntax error: File " << filename << ".Line " << linenum << "." << endl;
      fSuccess=0;
      file.close();
      return;
    }
    variable=variable.substr(0,variable.find_first_of(" 	"));
    value=message.substr(message.find("=")+1);
    value=value.substr(value.find_first_not_of(" 	"));	//Skiping initial spaces
    value=value.substr(0,value.find_first_of(" 	"));
/*
    value=value.substr(0,value.find_first_not_of("1234567890-+e."));
    val=strtod(value.c_str(),err);
    if (err) {
      cerr << "Syntax error after =: File " << filename << ".Line " << linenum << "." << endl;
      fSuccess=0;
      file.close();
      return;
    }
*/
    AddVariable(variable.c_str(), value.c_str());
  }
  file.close();
  InitVariables();
}

Bool_t CbmEcalInf::ExcludeParameter(TString parname)
{
  if (parname.CompareTo("ecalversion")==0) return kTRUE;
  return kFALSE;
}

void CbmEcalInf::CheckVariables()
{
  FairRunAna* ana = FairRunAna::Instance();
  if (ana==NULL)
  {
    return;
  }
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  CbmGeoEcalPar* par=(CbmGeoEcalPar*)(rtdb->findContainer("CbmGeoEcalPar"));
  if (par==NULL)
  {
    Info("CheckVariables","No parameters container is found.");
    return;
  }
  TMap* parVariables=par->GetVariables();
  if (parVariables)
  {
    TObjString* key;
    TIterator* iter=parVariables->MakeIterator();
    while((key=(TObjString*)iter->Next())!=NULL)
    {
      TObjString* first=(TObjString*)parVariables->GetValue(key->String());
      TObjString* second=(TObjString*)fVariables->GetValue(key->String());
      if (ExcludeParameter(key->String())==kFALSE)
      if (second==NULL)
      {
	Info("CheckVariables", "Parameter %s not found in .geo file, but found in parameter file.", key->String().Data());
      } else
      if (first->String()!=second->String())
      {
	Info("CheckVariables", "Parameter %s differs in .geo file and parameter file!", key->String().Data());
	Info("CheckVariables", "%s=%s in parameter file.", key->String().Data(), first->String().Data());
	Info("CheckVariables", "%s=%s in .geo file.", key->String().Data(), second->String().Data());
      }
      if (ExcludeParameter(key->String())==kTRUE)
	AddVariable(key->String().Data(), first->String().Data());
    }
  }

  TObjArray* parEcalStr=par->GetEcalStr();
  if (parEcalStr)
  {
    TObjString* key;
    for(Int_t i=0;i<parEcalStr->GetEntriesFast();i++)
    {
      TObjString* first=(TObjString*)parEcalStr->At(i);
      TObjString* second=(TObjString*)fEcalStr.At(i);
      if (second&&first->String()!=second->String())
      {
	Info("CheckVariables", "String %d in calorimeter structure differs in .geo file and in parameter file.", i);
	Info("CheckVariables", "%s=%s in parameter file", key->String().Data(), first->String().Data());
	Info("CheckVariables", "%s=%s in .geo file", key->String().Data(), second->String().Data());
      }
    }
  }
}

void CbmEcalInf::InitVariables()
{
  TString stri;
  TObjString* str=(TObjString*)fEcalStr.At(0);

  fXPos=GetVariableStrict("xpos");
  fYPos=GetVariableStrict("ypos");
  fZPos=GetVariableStrict("zpos");
  fNLayers=(Int_t)GetVariableStrict("nlayers");
  fModuleSize=GetVariableStrict("modulesize");
  fLead=GetVariableStrict("lead");
  fScin=GetVariableStrict("scin");
  fTyveec=GetVariableStrict("tyveec");
  fPSLead=GetVariableStrict("pslead");
  fPSScin=GetVariableStrict("psscin");
  fPSGap=GetVariableStrict("psgap");
  fCellSize=GetVariableStrict("cellsize");
  fEcalSize[2]=GetVariableStrict("ecalzsize");
  fECut=GetVariableStrict("ecut");
  fHCut=GetVariableStrict("hcut");
  fFastMC=(Int_t)GetVariableStrict("fastmc");

  stri=""; stri+=str->GetString().Length();
  AddVariable("xsize", stri);
  stri=""; stri+=fEcalStr.GetLast()+1;
  AddVariable("ysize", stri);

  fXSize=(Int_t)GetVariableStrict("xsize");
  fYSize=(Int_t)GetVariableStrict("ysize");
  fThicknessLayer = 2.0*GetTyveec()+GetScin()+GetLead();
  fEcalSize[0]    = GetXSize()*GetModuleSize();
  fEcalSize[1]    = GetYSize()*GetModuleSize();
  stri=""; stri+=fEcalSize[0];
  AddVariable("xecalsize",stri);
  stri=""; stri+=fEcalSize[1];
  AddVariable("yecalsize",stri);

  CalculateHoleSize();

  fEcalHole[0]=GetVariableStrict("xholesize");
  fEcalHole[1]=GetVariableStrict("yholesize");

  fEcalHole[2]=fEcalSize[2];

}

//-----------------------------------------------------------------------------
void CbmEcalInf::CalculateHoleSize()
{
  // Calculate the ECAL hole size [cm] from the number of supermodules
  // of the type 0

  Int_t i;
  Int_t j;
  Int_t hsx=-1;	//Coord of upper left hole angle
  Int_t hex=-1;
  Int_t hsy=-1;
  Int_t hey=-1;
  int cells; 
  TString str;

  for(i=0;i<fYSize;i++)
    for(j=0;j<fXSize;j++) {
      cells=GetType(j,i);
      if (cells<0) {
	cerr << "Warrning: Looks like an incorrect cells division: " << cells << "."<< endl;
	cerr << "At (" << j << "," << i << ")." << endl;
      }
      if (!cells) {
	if (-1==hsx) {
	  hsx=j;
	  hsy=i;
	}
	if (j<hsx) {
	  Info("CalculateHoleSize", "Caorimeter hole is not rectangular.");
	  AddVariable("xholesize","-1111");
	  AddVariable("yholesize","-1111");
//	  cout << "Error: At the moment we can describe only rect hole shapes." << endl;
	  return;
	}
	if (i==hsy)
	  hex=j;
	if (j>hex) {
	  Info("CalculateHoleSize", "Calorimeter hole is not rectangular.");
	  AddVariable("xholesize","-1111");
	  AddVariable("yholesize","-1111");
//	  cerr << "Error: At the moment we can describe only rect hole shapes." << endl;
	  return;
	}
	hey=i;
      }
    }
  str=""; str+=GetModuleSize()*(hex-hsx+1);
  AddVariable("xholesize",str);
  str=""; str+=GetModuleSize()*(hey-hsy+1);
  AddVariable("yholesize",str);
}

//-----------------------------------------------------------------------------
void CbmEcalInf::DumpContainer() const
{
  // Print out the ECAL geometry container

  if (fVariables)
  {
    TObjString* key;
    TIterator* iter=fVariables->MakeIterator();
    while((key=(TObjString*)iter->Next())!=NULL)
    {
      TObjString* str=(TObjString*)fVariables->GetValue(key);
      cout << key->String() << "=" << str->String() << endl;
    }
  }
  TObjString* key;
  TIterator* iter=fEcalStr.MakeIterator();

  Int_t modules=0;
  Int_t channels=0;
  Int_t i;
  Int_t j;
  Int_t m[10];
  char stri[2]={0, 0};
  TString st;
  for(i=0;i<10;i++) m[i]=0;

  while((key=(TObjString*)iter->Next())!=NULL)
  {
    st=key->String();
    cout << key->String() << endl;
    for(i=0;i<st.Length();i++)
    {
      stri[0]=st[i];
      j=atoi(stri);
      m[j]++;
      if (j) modules++;
      channels+=j*j;
    }
  }
  cout << "Total modules:  " << modules << endl;
  cout << "Total channels: " << channels << endl;
  for(i=1;i<10;i++)
  {
    if (m[i]==0) continue;
    cout << "	Type " << i << " : modules=" << m[i] << ", channels=" << m[i]*i*i << endl;
  }

}

void CbmEcalInf::FillGeoPar(CbmGeoEcalPar* par,Int_t write_str) const
{
  TObjString* key;
  TIterator* iter=fVariables->MakeIterator();
  par->setChanged();

  TMap* parVariables=new TMap();
  par->SetVariables(parVariables);

  while((key=(TObjString*)iter->Next())!=NULL)
    parVariables->Add(key, fVariables->GetValue(key));
 if (write_str==1)
 {
   iter=fEcalStr.MakeIterator();
   TObjArray* parEcalStr=new TObjArray();
   par->SetEcalStr(parEcalStr);
   while((key=(TObjString*)iter->Next())!=NULL)
     parEcalStr->Add(key);

 }
}
 
