using namespace std;
void create_rich_geo_with_mirror_misalignment()
{
	string vmcDir = string(gSystem->Getenv("VMCWORKDIR"));

	string inGeoFileName = vmcDir + "/geometry/rich/rich_v14b.gdml";
	string outGeoFileName = vmcDir + "/geometry/rich/rich_v14b_misaligned.gdml";
	double sigmaGeg = 3; // sigma for the Gauss distribution of misaligned mirrors in deg.

	ifstream inGeoFile(inGeoFileName.c_str());
	ofstream outGeoFile(outGeoFileName.c_str());
	TRandom3* random = new TRandom3();
	string line;
	if (inGeoFile.is_open()) {
		while (getline (inGeoFile, line) ) {
			size_t found1 = line.find("<variable name=\"misalignment_");
			if (found1 != string::npos) { // select string in which misalignment values are set
				size_t found2 = line.find("value=");
				string newStr = line.substr(0, found2);
				stringstream ss;
				ss << newStr << "value=\"" << random->Gaus(0., sigmaGeg) << "\"/>";
				newStr = ss.str();
				outGeoFile << newStr << endl;

			} else {
				outGeoFile << line;
			}
		}
		inGeoFile.close();
		outGeoFile.close();
	}

	cout << "New geometry file is created: " << outGeoFileName << endl;
}
