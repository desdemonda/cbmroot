/**
 * \file determine setup.C
 * \brief Macro determines detector presence in CBM setup.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 */



/**
 * \function CheckDetectorPresence
 * \brief Determines detector presence in TGeo by its name.
 * \param[in] parFile Name of the parameter file with TGeo.
 * \param[in] name Detector name.
 * \return True if detector presents in the TGeo.
 */
Bool_t CheckDetectorPresence(
			     const TString& parFile,
			     const char* name)
{
  cout << "In CheckDetectorPresence for detector " << name << endl; 
  
  TFile *currentfile = gFile;
  TFile* f = new TFile(parFile);
  
  // In fairbase trunk the geometry is moved into a separate container, 
  // so we have to check which version of paramtere file we have
  
  if (!gGeoManager) {
    f->Get("FairBaseParSet"); 
    TGeoManager *geoMan = gGeoManager;
    if(!geoMan) {
      f->Get("FairGeoParSet"); 
      geoMan = gGeoManager;
      if(!geoMan) {
	std::cout<<"Could not find valid GeoManager. Abort now!"<<std::endl;
	exit(1);
      }
    }
  }

  cout << "After getting the geomanager " << name << endl; 
  
  TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
  for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
    TGeoNode* node = (TGeoNode*) nodes->At(iNode);
    cout << "Node: "<< node->GetName() <<endl;
    if (TString(node->GetName()).Contains(name)) {
      f->Close();
      delete f;
      gFile=currentfile;
      return true;
    }
  }
  
  if (std::string(name) == "mvd") {
    TGeoNode* node1 = gGeoManager->GetTopVolume()->FindNode("pipevac1_0");
    if (node1) { // old ascii geometry for pipe
      nodes = node1->GetNodes();
      for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
	TGeoNode* node = (TGeoNode*) nodes->At(iNode);
	cout << "Node: "<< node->GetName() <<endl;
	if (TString(node->GetName()).Contains(name)) {
	  f->Close();
	  delete f;
	  gFile=currentfile;
	  return true;
	}
      }
    } else {
      // Find Pipe top node
      TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
      for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
	TGeoNode* node = (TGeoNode*) nodes->At(iNode);
	TString nodeName = node->GetName();
	nodeName.ToLower();
	if (nodeName.Contains("pipe")) {
	  cout << "Found node: " << node->GetName() << endl;

          // find pipevac1
	  TObjArray* nodes2 = node->GetNodes();
	  for (Int_t iiNode = 0; iiNode < nodes2->GetEntriesFast(); iiNode++) {
	    TGeoNode* node2 = (TGeoNode*) nodes2->At(iiNode);
	    TString nodeName2 = node2->GetName();
	    nodeName2.ToLower();
	    if (nodeName2.Contains("pipevac1")) {
	      cout << "Node: "<< nodeName2 <<endl;
              // check if there is a mvd in the pipevac
	      TObjArray* nodes3 = node2->GetNodes();
	      for (Int_t iiiNode = 0; iiiNode < nodes3->GetEntriesFast(); iiiNode++) {
		TGeoNode* node3 = (TGeoNode*) nodes3->At(iiiNode);
		TString nodeName3 = node3->GetName();
		nodeName3.ToLower();
		cout << "Node: "<< nodeName3 <<endl;
		if (nodeName3.Contains("mvd")) {
		  cout << "Node: "<< nodeName3 <<endl;
		  f->Close();
		  delete f;
		  gFile=currentfile;
		  return true;
		}
	      }
	    }
	  }
	}
      }
    }
  }
  f->Close();
  gFile=currentfile;
  delete f;
  return false;
}

/**
 * \function IsTrd
 * \brief Determines TRD detector presence in TGeo.
 * \param[in] parFile Name of parameter file with TGeo.
 * \return True if TRD exists in TGeo.
 */
Bool_t IsTrd(
		const TString& parFile)
{
	return CheckDetectorPresence(parFile, "trd");
}

/**
 * \function IsMuch
 * \brief Determines MUCH detector presence in TGeo.
 * \param[in] parFile Name of parameter file with TGeo.
 * \return True if MUCH exists in TGeo.
 */
Bool_t IsMuch(
		const TString& parFile)
{
	return CheckDetectorPresence(parFile, "much");
}

/**
 * \function IsTof
 * \brief Determines TOF detector presence in TGeo.
 * \param[in] parFile Name of parameter file with TGeo.
 * \return True if TOF exists in TGeo.
 */
Bool_t IsTof(
		const TString& parFile)
{
	return CheckDetectorPresence(parFile, "tof");
}

/**
 * \function IsRich
 * \brief Determines RICH detector presence in TGeo.
 * \param[in] parFile Name of parameter file with TGeo.
 * \return True if RICH exists in TGeo.
 */
Bool_t IsRich(
		const TString& parFile)
{
	return CheckDetectorPresence(parFile, "rich");
}

/**
 * \function IsMvd
 * \brief Determines MVD detector presence in TGeo.
 * \param[in] parFile Name of the parameter file with TGeo.
 * \return True if MVD exists in TGeo.
 */
Bool_t IsMvd(
		const TString& parFile)
{
	return CheckDetectorPresence(parFile, "mvd");
}
