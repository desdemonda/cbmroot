#include "CbmUtils.h"

#include "TCanvas.h"
#include "TSystem.h"
#include "TH1D.h"
#include "TH2D.h"

namespace Cbm
{

void SaveCanvasAsImage(
   TCanvas* c,
   const std::string& dir,
   const std::string& option)
{
   if (dir == "") return;
   gSystem->mkdir(dir.c_str(), true); // create directory if it does not exist
   if (option.find("eps") != std::string::npos) c->SaveAs(std::string(dir + std::string(c->GetTitle()) + ".eps").c_str());
   if (option.find("png") != std::string::npos) c->SaveAs(std::string(dir + std::string(c->GetTitle()) + ".png").c_str());
   if (option.find("gif") != std::string::npos) c->SaveAs(std::string(dir + std::string(c->GetTitle()) + ".gif").c_str());
}

string FindAndReplace(
		const string& name,
		const string& oldSubstr,
		const string& newSubstr)
{
	string newName = name;
	Int_t startPos = name.find(oldSubstr);
	newName.replace(startPos, oldSubstr.size(), newSubstr);
	return newName;
}

vector<string> Split(
		const string& name,
		char delimiter)
{
	vector<string> result;
	Int_t begin = 0;
	Int_t end = name.find_first_of(delimiter);
	while (end != string::npos) {
		string str = name.substr(begin, end - begin);
		if (str[0] == delimiter) str.erase(0, 1);
		result.push_back(str);
		begin = end;
		end = name.find_first_of(delimiter, end + 1);
	}
	result.push_back(name.substr(begin + 1));
	return result;
}


TH1D* DivideH1(
      TH1* h1,
      TH1* h2,
	  const string& histName,
	  double scale,
	  const string& titleYaxis)
{
    int nBins = h1->GetNbinsX();
    double min = h1->GetXaxis()->GetXmin();
    double max = h1->GetXaxis()->GetXmax();
    string hname = string(h1->GetName()) + "_divide";
    if (histName != "") hname = histName;

    TH1D* h3 = new TH1D(histName.c_str(), hname.c_str(), nBins, min, max);
    h3->GetXaxis()->SetTitle(h1->GetXaxis()->GetTitle());
    h3->GetYaxis()->SetTitle(titleYaxis.c_str());
    h1->Sumw2();
    h2->Sumw2();
    h3->Sumw2();
    h3->Divide(h1, h2, 1., 1., "B");
    h3->Scale(scale);
    return h3;
}

TH2D* DivideH2(
      TH2* h1,
      TH2* h2,
	  const string& histName,
	  double scale,
	  const string& titleZaxis)
{
    int nBinsX = h1->GetNbinsX();
    double minX = h1->GetXaxis()->GetXmin();
    double maxX = h1->GetXaxis()->GetXmax();
    int nBinsY = h1->GetNbinsY();
    double minY = h1->GetYaxis()->GetXmin();
    double maxY = h1->GetYaxis()->GetXmax();
    string hname = string(h1->GetName()) + "_divide";
    if (histName != "") hname = histName;

    TH2D* h3 = new TH2D(hname.c_str(), hname.c_str(), nBinsX, minX, maxX, nBinsY, minY, maxY);
    h3->GetXaxis()->SetTitle(h1->GetXaxis()->GetTitle());
    h3->GetYaxis()->SetTitle(h1->GetYaxis()->GetTitle());
    h3->GetZaxis()->SetTitle(titleZaxis.c_str());
    h1->Sumw2();
    h2->Sumw2();
    h3->Sumw2();
    h3->Divide(h1, h2, 1., 1., "B");
    h3->Scale(scale);
    return h3;
}

}
