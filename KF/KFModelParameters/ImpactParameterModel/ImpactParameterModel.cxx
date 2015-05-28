#include "ImpactParameterModel.h"
#include <iostream>
#include <fstream>


ImpactParameterModel::ImpactParameterModel(std::string filename):
	bfromN(), bfromNerror()
 {
	std::fstream fin(filename.c_str(), std::fstream::in);
	int tot = 0;
	fin >> tot;
	bfromN.resize(tot+1);
	bfromNerror.resize(tot+1);
	bfromN[0] = 1000.;
	bfromNerror[0] = 1000.;
	int N = 0;
	for(int i=0;i<tot;++i) {
		fin >> N >> bfromN[i+1] >> bfromNerror[i+1];
		bfromNerror[i+1] = TMath::Sqrt(bfromNerror[i+1]);
	}
	fin.close();
}
