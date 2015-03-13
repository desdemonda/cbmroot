#include "CbmEcalNN.h"

#include "TString.h"
#include "TRandom.h"
#include "TMath.h"
#include "TMatrixD.h"

#include <iostream>
#include <fstream>

using namespace std;
  
CbmEcalNN::CbmEcalNN(const char* str)
  : fN(-1111), fStr(NULL), fNeurons(NULL), fWNeurons(NULL), fWSynapses(NULL),
    fNNeurons(-1111), fNSynapses(-1111), fMS(-1111), fCurrent(NULL), fDNeurons(NULL),
    fDeDNeurons(NULL), fDeDSynapses(NULL), fDEDNeurons(NULL), fDEDSynapses(NULL),
    fOld(NULL),
    fReset(50), fEta(0.1), fEpsilon(0.0), fDelta(0.0), fEtaDecay(1.0),
    fLastAlpha(0.0), fTau(3.0)
{
  Int_t i;
  Int_t n=1;
  TString st="";

  for(i=0;str[i]!=0;i++)
  {
    if (str[i]==':') { n++; continue; }
    if (str[i]<48||str[i]>57)
    {
      cerr << "Wrong symbol in " << str << " at " << i+1 << endl; 
      return;
    }
  }

  fStr=new Int_t[n];

  n=0; 
  for(i=0;str[i]!=0;i++)
  {
    if (str[i]==':')
    {
      fStr[n++]=st.Atoi();
      st=""; continue;
    }
    st+=str[i];
  }
  fStr[n++]=st.Atoi();
  fN=n;

  //Counting neurons and synapses
  n=0;
  for(i=0;i<fN;i++) n+=fStr[i];
  fWNeurons=new Double_t[n];
  fDeDNeurons=new Double_t[n];
  fDEDNeurons=new Double_t[n];
  fNeurons=new Double_t[n];
  fDNeurons=new Double_t[n];
  fNNeurons=n;
  for(i=0;i<fNNeurons;i++) fDEDNeurons[i]=0.0;
  n=0;
  for(i=1;i<fN;i++) n+=fStr[i]*fStr[i-1];
  fWSynapses=new Double_t[n];
  fDeDSynapses=new Double_t[n];
  fDEDSynapses=new Double_t[n];
  fNSynapses=n;
  for(i=0;i<fNSynapses;i++) fDEDSynapses[i]=0.0;

  fMS=fNSynapses+fNNeurons;

  fCurrent=new Double_t[fStr[0]+fStr[fN-1]];
  fOld=new Double_t[fMS];
}

void CbmEcalNN::LoadWeights(const char* filename)
{
  char* buff=new char[100];
  ifstream f(filename);
  Int_t i;
  Double_t n1;
  Double_t n2;
  
  if (!f)
  {
    Error("LoadWeights", "Can't open file %s", filename);
    return;
  }
  f.getline(buff, 100); //input normalization header
  for(i=0;i<fStr[0];i++)
    f >> n1 >> n2; //Skip the normalization. We do not use it!
  f.getline(buff, 100);

  f.getline(buff, 100); //output normalization header
  for(i=0;i<fStr[fN-1];i++)
    f >> n1 >> n2; //Skip the normalization. We do not use it!
  f.getline(buff, 100);


  f.getline(buff, 100); //neuron weights header
  for(i=0;i<fNNeurons;i++)
  {
    f >> fWNeurons[i];
  }
  f.getline(buff, 100);

  f.getline(buff, 100); //synapse weights header
  for(i=0;i<fNSynapses;i++)
  {
    f >> fWSynapses[i];
  }
  f.getline(buff, 100);

  f.close();
  delete[] buff;
}


void CbmEcalNN::DumpWeights(const char* filename)	// In ROOT compatible format
{
  ofstream f(filename);
  Int_t i;
  f << "#input normalization" << endl;
  for(i=0;i<fStr[0];i++)
    f << "1 0" << endl; //Write unity normalization.

  f << "#output normalization" << endl;
  for(i=0;i<fStr[fN-1];i++)
    f << "1 0" << endl; //Write unity normalization.

  f << "#neurons weights" << endl;
  for(i=0;i<fNNeurons;i++)
    f << fWNeurons[i] << endl;

  f << "#synapses weights" << endl;
  for(i=0;i<fNSynapses;i++)
    f << fWSynapses[i] << endl;

  f.close();
}

void CbmEcalNN::Randomize()
{
  Int_t i;

  for(i=0;i<fNNeurons;i++)  fWNeurons[i]=gRandom->Uniform(-0.5, 0.5);
  for(i=0;i<fNSynapses;i++) fWSynapses[i]=gRandom->Uniform(-0.5, 0.5);
}

Double_t CbmEcalNN::Eval(Int_t idx, Double_t* input)
{
  Int_t i;
  Int_t j;
  Int_t k;
  Int_t dn=0;
  Int_t dn1;
  Int_t dn2;
  Int_t ds=0;
  Int_t ds1;
  Int_t ds2;
  Int_t tmp;
  Double_t t;

  for(i=0;i<fStr[0];i++)
    fNeurons[i]=input[i];

  dn=i; dn2=0;
  //calculate values in all layers
  for(i=1;i<fN-1;i++)
  {
    dn1=dn+fStr[i];
    ds2=fStr[i-1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      t=fWNeurons[j];
      ds1=ds+ds2;
      tmp=dn2-ds;
      //Loop over synapses
      for(k=ds;k<ds1;k++)
	t+=fNeurons[k+tmp]*fWSynapses[k];
      fNeurons[j]=TMath::Exp(-t*t);
      ds=ds1;
    }
    dn2=dn;
    dn=dn1;
  }

  //Last layer threated separately
  {
    dn1=dn+fStr[i];
    ds2=fStr[i-1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      t=fWNeurons[j];
      ds1=ds+ds2;
      tmp=dn2-ds;
      //Loop over synapses
      for(k=ds;k<ds1;k++)
	t+=fNeurons[k+tmp]*fWSynapses[k];
      fNeurons[j]=t;
      ds=ds1;
    }
  }

  return fNeurons[fNNeurons+idx-fStr[fN-1]];
}


Double_t CbmEcalNN::EvalD(Int_t idx, Double_t* input)
//Compute output and derivaties of neural network
{
  Int_t i;
  Int_t j;
  Int_t k;
  Int_t dn=0;
  Int_t dn1;
  Int_t dn2;
  Int_t ds=0;
  Int_t ds1;
  Int_t ds2;
  Int_t tmp;
  Double_t t;
  Double_t t1;

  for(i=0;i<fStr[0];i++)
    fNeurons[i]=input[i];

  dn=i; dn2=0;
  //calculate values in all layers
  for(i=1;i<fN-1;i++)
  {
    dn1=dn+fStr[i];
    ds2=fStr[i-1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      t=fWNeurons[j];
      ds1=ds+ds2;
      tmp=dn2-ds;
      //Loop over synapses
      for(k=ds;k<ds1;k++)
	t+=fNeurons[k+tmp]*fWSynapses[k];
      t1=TMath::Exp(-t*t);
      fNeurons[j]=t1;
      fDNeurons[j]=(-2.0)*t*t1;
      ds=ds1;
    }
    dn2=dn;
    dn=dn1;
  }

  //Last layer threated separately
  {
    dn1=dn+fStr[i];
    ds2=fStr[i-1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      t=fWNeurons[j];
      ds1=ds+ds2;
      tmp=dn2-ds;
      //Loop over synapses
      for(k=ds;k<ds1;k++)
	t+=fNeurons[k+tmp]*fWSynapses[k];
      fNeurons[j]=t;
      fDNeurons[j]=1.0;
      ds=ds1;
    }
  }

  return fNeurons[fNNeurons+idx-fStr[fN-1]];
}

Double_t CbmEcalNN::GetError(Int_t n, Double_t* dta)
// Get error for given dataset
// n --- is number of entries
// dta --- array of input/output pairs
{
  Int_t ninp=fStr[0];
  Int_t nout=fNNeurons-fStr[fN-1];
  Int_t n1=fStr[fN-1];
  Int_t n2=ninp+n1;
  Int_t i;
  Int_t j;
  Double_t err=0;
  Double_t d;

  for(i=0;i<n;i++)
  {
    Eval(0, &(dta[i*n2])); //Calculate neuron network 
    for(j=0;j<n1;j++)
    {
      //chi^2 used for gaussian activation function
      d=fNeurons[nout+j]-dta[i*n2+ninp+j]; 
      d*=d; err+=d;
    }
  }

  return err/2.0;
}

void CbmEcalNN::ComputeDeDs()
{
  Int_t i=fN-1;
  Int_t j;
  Int_t k;
  Int_t dn=0;
  Int_t dn1;
  Int_t dn2;
  Int_t ss;
  Int_t ds=0;
  Int_t ds1;
  Int_t ds2;
  Int_t tmp;
  Double_t t;
  Double_t d;
  
  EvalD(0, fCurrent);

  dn=fNNeurons-fStr[fN-1];
  dn2=dn-fStr[fN-2];
  ss=ds=fNSynapses-fStr[fN-2]*fStr[fN-1];

  //Last layer threated separately
  {
    dn1=dn+fStr[i];
    ds2=fStr[i-1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      fDeDNeurons[j]=fNeurons[j]-fCurrent[j-dn+fStr[0]];
      ds1=ds+ds2;
      tmp=dn2-ds;
      //Loop over synapses
      for(k=dn2;k<dn;k++)
	fDeDSynapses[k-tmp]=fNeurons[k]*fDeDNeurons[j];
      ds=ds1;
    }
  }

  //Other layers
  for(i=fN-2;i>0;i--)
  {
    dn1=dn; dn-=fStr[i];
    ds=ss;
    ds2=fStr[i+1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      t=0;
      ds1=ds+ds2;
      tmp=dn1+ds-ds1;
      //Loop over synapses
      for(k=0;k<ds1-ds;k++)
      {
	t+=fWSynapses[k*fStr[i]+ds]*fDeDNeurons[k+dn1];
      }

      fDeDNeurons[j]=t*fDNeurons[j];
      ds++;
    }
    ss-=fStr[i]*fStr[i-1];
  }

  dn=fNNeurons-fStr[fN-1];
  ss=ds=fNSynapses-fStr[fN-2]*fStr[fN-1];
  for(i=fN-2;i>0;i--)
  {
    dn1=dn; dn-=fStr[i];
    dn2=dn-fStr[i-1];
    ds=ss=ss-fStr[i]*fStr[i-1];
    ds2=fStr[i-1];
    //For all neurons in layer
    for(j=dn;j<dn1;j++)
    {
      t=0;
      ds1=ds+ds2;
      tmp=dn2-ds;
      //Loop over synapses
      for(k=dn2;k<dn;k++)
	fDeDSynapses[k-tmp]=fNeurons[k]*fDeDNeurons[j];
      ds=ds1;
    }
  }

}

void CbmEcalNN::Train(Int_t epochs, Int_t n, Double_t* dta)
//Train a neural netrork. Use BFGS algo
{
  Int_t ie;
  TMatrixD bfgsh(fMS, fMS);
  TMatrixD gamma(fMS, 1);
  TMatrixD delta(fMS, 1);
  Double_t* buf;
  Double_t* dir;

  buf=new Double_t[fMS];
  dir=new Double_t[fMS];

  cout << "Train: Starting error is " <<  GetError(n, dta) << endl << flush;

  for(ie=0;ie<epochs;ie++)
  {
    SetGammaDelta(gamma, delta, buf, n, dta);
    if (ie%fReset==0)
    {
      SteepestDir(dir);
      bfgsh.UnitMatrix();
    }
    else
    {
      if (GetBFGSH(bfgsh, gamma, delta))
      {
	SteepestDir(dir);
	bfgsh.UnitMatrix();
      }
      else
      {
	BFGSDir(bfgsh, dir);
      }
    }
    if (DerivDir(dir)>0)
    {
      SteepestDir(dir);
      bfgsh.UnitMatrix();
    }
    if (LineSearch(dir, buf, n, dta))
    {
	SteepestDir(dir);
	bfgsh.UnitMatrix();
	if (LineSearch(dir, buf, n, dta))
	{
	  Error("Train()", "Line search failed :(");
	  ie=epochs;
	}
    }
    cout << "Epoch: "<< ie << " (Root Error: " << TMath::Sqrt(GetError(n, dta)/n) << ")" << endl << flush;
//    cout << "Epoch: " << ie << endl;
  }

  delete [] buf;
  delete [] dir;
  //TODO
}

Bool_t CbmEcalNN::GetBFGSH(TMatrixD& bfgsh, TMatrixD& gamma, TMatrixD& delta)
//From ROOT:
//Computes the hessian matrix using the BFGS update algorithm.
//from gamma (g_{(t+1)}-g_{(t)}) and delta (w_{(t+1)}-w_{(t)}).
//Returns true if such a direction could not be found
//(gamma and delta are orthogonal).
{
  TMatrixD gd(gamma, TMatrixD::kTransposeMult, delta);
  if (gd[0][0]==0.0) return kTRUE;
  TMatrixD aHg(bfgsh, TMatrixD::kMult, gamma);
  TMatrixD tmp(gamma, TMatrixD::kTransposeMult, bfgsh);
  TMatrixD gHg(gamma, TMatrixD::kTransposeMult, aHg);
  Double_t a=1.0/gd[0][0];
  Double_t f=1.0+(gHg[0][0]*a);
//  TMatrixD res(TMatrixD(delta, TMatrixD::kMult, TMatrixD(TMatrixD::kTransposed, delta)));
  TMatrixD res(delta, TMatrixD::kMult, TMatrixD(TMatrixD::kTransposed, delta));
  res*=f;
  res-=(TMatrixD(delta,TMatrixD::kMult,tmp)+TMatrixD(aHg,TMatrixD::kMult,TMatrixD(TMatrixD::kTransposed, delta)));
  res*=a;
  bfgsh+=res;
/*
   Int_t i;
   Int_t j;
   for(i=0;i<bfgsh.GetNrows();i++)
   {
     for(j=0;j<bfgsh.GetNrows();j++)
       cout << bfgsh[i][j] << " ";
     cout << endl << flush;
   }
*/  
  return kFALSE;
}

void CbmEcalNN::BFGSDir(TMatrixD& bfgsh, Double_t* dir)	//direction=Hessian estimate x dir
{
  Int_t i;
  Int_t j;
  TMatrixD dedw(fMS, 1);

  for(i=0;i<fNNeurons;i++)
    dedw[i][0]=fDEDNeurons[i];
  j=i;
  for(i=0;i<fNSynapses;i++)
    dedw[j++][0]=fDEDSynapses[i];
  TMatrixD direction(bfgsh, TMatrixD::kMult, dedw);
   for (i=0;i<fMS;i++)
      dir[i]=-direction[i][0];
}

void CbmEcalNN::SetGammaDelta(TMatrixD& gamma, TMatrixD& delta, Double_t* buf, Int_t n, Double_t* dta)
{
  Int_t i;
  Int_t j;

  for(i=0;i<fMS;i++) delta[i]=buf[i];
  for(i=0;i<fNNeurons;i++)
    gamma[i][0]=-fDEDNeurons[i];
  j=i;
  for(i=0;i<fNSynapses;i++)
    gamma[j++][0]=-fDEDSynapses[i];

  ComputeDEDs(n, dta);
  for(i=0;i<fNNeurons;i++)
    gamma[i][0]+=fDEDNeurons[i];
  j=i;
  for(i=0;i<fNSynapses;i++)
    gamma[j++][0]+=fDEDSynapses[i];

//  for(i=0;i<fNNeurons+fNSynapses;i++) cout << "gamma[" << i << "][0]" << gamma[i][0] << endl;
}

void CbmEcalNN::ComputeDEDs(Int_t n, Double_t* dta)
//Compute summ of derivatives of the error for whole data set
{
  Int_t i;
  Int_t j;
  Int_t n2=fStr[0]+fStr[fN-1];

  for(i=0;i<fNNeurons;i++)  fDEDNeurons[i]=0.0;
  for(i=0;i<fNSynapses;i++) fDEDSynapses[i]=0.0;

  for(j=0;j<n;j++)
  {
    fCurrent=&(dta[j*n2]);
    ComputeDeDs();
    for(i=0;i<fNNeurons;i++)  fDEDNeurons[i]+=fDeDNeurons[i];
    for(i=0;i<fNSynapses;i++) fDEDSynapses[i]+=fDeDSynapses[i];
  }

  for(i=0;i<fNNeurons;i++)  fDEDNeurons[i]/=n;
  for(i=0;i<fNSynapses;i++) fDEDSynapses[i]/=n;
/*
  for(i=0;i<fNSynapses;i++) cout << "fDEDSynapses[" << i << "]=" << fDEDSynapses[i] << endl;
  for(i=0;i<fNNeurons;i++)  cout << "fDEDNeurons[" << i << "]=" << fDEDNeurons[i] << endl;
*/
}

void CbmEcalNN::SteepestDir(Double_t * dir)
//Steepest descent search direction
{
  Int_t i;
  Int_t j;

  for(i=0;i<fNNeurons;i++)
    dir[i]=-fDEDNeurons[i];
  j=i;
  for(i=0;i<fNSynapses;i++)
    dir[j++]=-fDEDSynapses[i];

//  for(i=0;i<fNNeurons+fNSynapses;i++) cout << "dir[" << i << "]=" << dir[i] << endl;
}

void CbmEcalNN::TrainStochastic(Int_t epochs, Int_t n, Double_t* dta)
//Train a neural netrork. Use BFGS algo
{
  Double_t* buf;
  Int_t* idx;
  Int_t ie;
  Int_t i;
  Int_t j;
  Int_t k;
  Int_t ninp=fStr[0];
  Int_t nout=fNNeurons-fStr[fN-1];
  Int_t n1=fStr[fN-1];
  Int_t n2=ninp+n1;

  buf=new Double_t[fMS];
  idx=new Int_t[n];
  for(i=0;i<fMS;i++) buf[i]=0.0;
  for(i=0;i<n;i++) idx[i]=i;

  cout << "TrainStochastic: Starting error is " <<  GetError(n, dta) << endl << flush;
  for(ie=0;ie<epochs;ie++)
  {
    Shuffle(n, idx);
    for(i=0;i<n;i++)
    {
      fCurrent=&(dta[idx[i]*n2]);
      ComputeDeDs();
      for(j=0;j<fNNeurons;j++)
      {
	buf[j]=(-fEta)*(fDelta+fDeDNeurons[j])+fEpsilon*buf[j];
	fWNeurons[j]+=buf[j];
      }
      for(k=0;k<fNSynapses;k++)
      {
	buf[j]=(-fEta)*(fDelta+fDeDSynapses[k])+fEpsilon*buf[j];
	fWSynapses[k]+=buf[j++];
      }
    }
    fEta*=fEtaDecay;

//    cout << "Epoch: "<< ie << " (Error: " <<  GetError(n, dta) << ")" << endl << flush;
    cout << "Epoch: " << ie << endl;
  }

  delete [] buf;
  delete [] idx;
}

void CbmEcalNN::MLP_Line(Double_t* old, Double_t* dir, Double_t dst)
{
  Int_t i;
  Int_t j;

  for(i=0;i<fNNeurons;i++)
//  {
    fWNeurons[i]=old[i]+dir[i]*dst;
//    cout << "fWNeurons[" << i << "]=" << fWNeurons[i] << endl;
//  }
  for(j=0;j<fNSynapses;j++)
  {
    fWSynapses[j]=old[i]+dir[i]*dst;
//    cout << "fWSynapses[" << i << "]=" << fWSynapses[j] << endl;
    i++;
  }
}

Bool_t CbmEcalNN::LineSearch(Double_t* dir, Double_t* buf, Int_t n, Double_t* dta)
//Search along the line defined by direction
{
  Int_t i;
  Int_t j;
  Double_t err1;
  Double_t err2;
  Double_t err3;
  Double_t a1=0.0;
  Double_t a2=fLastAlpha;
  Double_t a3;
  Bool_t done=kFALSE;

  //Store weights
  for(i=0;i<fNNeurons;i++)
    fOld[i]=fWNeurons[i];
  for(j=0;j<fNSynapses;j++)
    fOld[i++]=fWSynapses[j];
  err1=GetError(n, dta);

  //Add some magic!!!
  if (a2>2.0) a2=2.0;
  if (a2<0.01) a2=0.01;
  a3=a2;
  MLP_Line(fOld, dir, a2);
  err3=err2=GetError(n, dta);
  if (err1>err2)
  {
    for(i=0;i<100;i++)
    {
      a3*=fTau;
      MLP_Line(fOld, dir, a3);
      err3=GetError(n, dta);
      if (err3>err2)
      {
	done=kTRUE;
        break;
      }
      a1=a2; a2=a3;
      err1=err2; err2=err3;
    }
    if (done==kFALSE)
    {
      //Set weights back
      MLP_Line(fOld, dir, 0.0);
      return kTRUE;
    }
  }
  else
  {
    for(i=0;i<100;i++)
    {
      a2/=fTau;
      MLP_Line(fOld, dir, a2);
      err2=GetError(n, dta);
      if (err1>err2)
      {
	done=kTRUE;
        break;
      }
      a3=a2; err3=err2;
    }
    if (done==kFALSE)
    {
      MLP_Line(fOld, dir, 0.);
      fLastAlpha = 0.05;
      return kTRUE;
    }
  }
  fLastAlpha=0.5*(a1+a3-(err3-err1)/((err3-err2)/(a3-a2)-(err2-err1)/(a2-a1)));
  if (fLastAlpha>10000) fLastAlpha=10000;
  MLP_Line(fOld, dir, fLastAlpha);
  GetError(n, dta);

  for(i=0;i<fNNeurons;i++)
//  {
    buf[i]=fWNeurons[i]-fOld[i];
//    cout << "buf[" << i << "]=" << buf[i] << endl;
//  }
  for(j=0;j<fNSynapses;j++)
  {
    buf[i]=fWSynapses[j]-fOld[i];
//    cout << "buf[" << i << "]=" << buf[i] << endl;
    i++;
  }

  return kFALSE;
}

Double_t CbmEcalNN::DerivDir(Double_t* dir)
//Scalar product between gradient and direction
{
  Int_t i;
  Int_t j;
  Double_t res=0.0;

  for(i=0;i<fNNeurons;i++)
    res+=fDEDNeurons[i]*dir[i];
  j=i;
  for(i=0;i<fNSynapses;i++)
  {
    res+=fDEDSynapses[i]*dir[j];
    j++;
  }

//  cout << "DerivDir: " << res << endl;
  return res;
}

void CbmEcalNN::Shuffle(Int_t n, Int_t* idx)
//Shuffle indexes
{
  Int_t i;
  Int_t j;
  Int_t k;

  for(i=0;i<n;i++)
  {
    j=gRandom->Integer(n);
    k=idx[j];
    idx[j]=idx[i];
    idx[i]=k;
  }
}

CbmEcalNN::~CbmEcalNN()
{
  delete [] fStr;
  delete [] fWNeurons;
  delete [] fNeurons;
  delete [] fWSynapses;

  delete [] fCurrent;
  delete [] fDNeurons;
  delete [] fDeDNeurons;
  delete [] fDeDSynapses;
  delete [] fDEDNeurons;
  delete [] fDEDSynapses;
  delete [] fOld;
}

ClassImp(CbmEcalNN)
