#include "CbmEcalParam.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <TSystem.h>

#include "TMap.h"

using namespace std;

CbmEcalParam::~CbmEcalParam()
{
  ;
}

int cmp_nocase(const string &s, const string &s2);
/*
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
*/
// ---------------------------------------------------------------------------

TString CbmEcalParam::GetString(const char* key) const
{
  TObjString* value=(TObjString*)fVariables.GetValue(key);
  if (value==NULL)
    Fatal("GetString","Can't find variable named %s.", key);
  return value->GetString();
}

Int_t CbmEcalParam::GetInteger(const char* key) const
{
  TObjString* value=(TObjString*)fVariables.GetValue(key);
  if (value==NULL)
    Fatal("GetInteger","Can't find variable named %s.", key);
  Int_t val;
  char* err=NULL;
  val=strtol(value->GetString(),&err, 10);
  if (err[0]!='\0')
    Fatal("GetInteger","Can't convert variable named %s==\"%s\" to floating point number.", key, value->GetString().Data());
  return val;
}

Double_t CbmEcalParam::GetDouble(const char* key) const
{
  TObjString* value=(TObjString*)fVariables.GetValue(key);
  if (value==NULL)
    Fatal("GetDouble","Can't find variable named %s.", key);
  Double_t val;
  char* err=NULL;
  val=strtod(value->GetString(),&err);
  if (err[0]!='\0')
    Fatal("GetDouble","Can't convert variable named %s==\"%s\" to floating point number.", key, value->GetString().Data());
  return val;
}

TString CbmEcalParam::GetStringSlack(const char* key) const 
{
  TObjString* value=(TObjString*)fVariables.GetValue(key);
  if (value==NULL)
  {
    Warning("GetStringSlack","Can't find variable named %s.", key);
    return "";
  }
  return value->GetString();
}

Int_t CbmEcalParam::GetIntegerSlack(const char* key) const
{
  TObjString* value=(TObjString*)fVariables.GetValue(key);
  if (value==NULL)
  {
    Warning("GetIntegerSlack","Can't find variable named %s.", key);
    return -1111;
  }
  Int_t val;
  char* err=NULL;
  val=strtol(value->GetString(),&err, 10);
  if (err[0]!='\0')
  {
    Warning("GetIntegerSlack","Can't convert variable named %s==\"%s\" to floating point number.", key, value->GetString().Data());
    return -1111;
  }
  return val;
}

Double_t CbmEcalParam::GetDoubleSlack(const char* key) const
{
  TObjString* value=(TObjString*)fVariables.GetValue(key);
  if (value==NULL)
  {
    Warning("GetDoubleSlack","Can't find variable named %s.", key);
    return -1111;
  }
  Double_t val;
  char* err=NULL;
  val=strtod(value->GetString(),&err);
  if (err[0]!='\0')
  {
    Warning("GetDoubleSlack","Can't convert variable named %s==\"%s\" to floating point number.", key, value->GetString().Data());
    return -1111;
  }
  return val;
}

void CbmEcalParam::AddVariable(const char* key, const char* value)
{
  TObjString* skey=(TObjString*)fVariables.FindObject(key);
  if (skey!=NULL)
  {
    /** Value for this key already exists **/
    skey->GetString()=value;
    return;
  }
  skey=new TObjString(key);
  skey->String().ToLower();
  TObjString* svalue=new TObjString(value);
  fVariables.Add(skey, svalue);
}

//=============================================================================
CbmEcalParam::CbmEcalParam(const char* name, const char* filename)
  : TNamed(name, filename), 
    fVariables(200),
    fSuccess(0), 
    fFileName(filename)
{
  TString fname=filename;
  gSystem->ExpandPathName(fname);
  ifstream file(fname);
  Int_t linenum;
  string buffer;
  string message;
  string variable;
  string value;

  fSuccess=1;
  if (!file) {
    Error("CbmEcalParam", "Can't open file %s. ", filename);
    fSuccess=0;
    return;
  }

  linenum=0;
  file.exceptions(ifstream::goodbit);
  while(getline(file,buffer))
  {
    linenum++;
    if (buffer.empty()) continue;
    if (buffer.find_first_not_of(" 	")==string::npos) continue;
    message=buffer.substr(buffer.find_first_not_of(" 	"));	//Skipping initial spaces
    if (message.empty()) continue;
    message=message.substr(0,message.find("#"));	//Removing comments
    if (message.empty()) continue;		//This was just a comment
    variable=message.substr(0,message.find("="));
    if (variable==message)
    {
      Error("CbmEcalParam","Syntax error: File %s. Line %d. ", fname.Data(), linenum);
      fSuccess=0;
      file.close();
      return;
    }
    variable=variable.substr(0,variable.find_first_of(" 	"));
    if (message.find("=")==string::npos)
      Error("CbmEcalParam","Can't find value for varible %s at line %d.", variable.c_str(), linenum);
    value=message.substr(message.find("=")+1);
    if (value.find_first_not_of(" 	")==string::npos)
    {
      Info("CbmEcalParam","Value of varible %s at empty.", variable.c_str());
      value="";
    }
    else
      value=value.substr(value.find_first_not_of(" 	"));	//Skipping initial spaces
    AddVariable(variable.c_str(), value.c_str());
    if (file.eof()) break;
  }
  //file.close();
}

//-----------------------------------------------------------------------------

void CbmEcalParam::DumpContainer(ostream& stream) const
{
  TObjString* key;
  TIterator* iter=fVariables.MakeIterator();
  while((key=(TObjString*)iter->Next())!=NULL)
  {
    TObjString* str=(TObjString*)fVariables.GetValue(key);
    stream << key->String() << "=" << str->String() << endl;
  }
}

void CbmEcalParam::DumpContainer() const
{
  DumpContainer(cout);
}

//-----------------------------------------------------------------------------

Bool_t CbmEcalParam:: Differs(const CbmEcalParam* info) const
{
  TObjString* key;
  TIterator* iter=fVariables.MakeIterator();
  while((key=(TObjString*)iter->Next())!=NULL)
  {
    TObjString* str=(TObjString*)fVariables.GetValue(key);
    TObjString* other=(TObjString*)info->fVariables.GetValue(key);
    if (!other||str->String()!=other->String()) break;
  }
  if (!key) return kTRUE;

  iter=info->fVariables.MakeIterator();
  while((key=(TObjString*)iter->Next())!=NULL)
  {
    TObjString* str=(TObjString*)fVariables.GetValue(key);
    TObjString* other=(TObjString*)info->fVariables.GetValue(key);
    if (!other||str->String()!=other->String()) break;
  }
  if (!key) return kTRUE;
  return kFALSE;
}

