// --------------------------------------------------------------------------
//
// Macro for automatic check of a magnetic field map.
// The macro compare field values extracted from the field map
// with a older version of this values. If there is a change in
// the field map itself or in the routines the test should fail.
//
// D.Emschermann  29.08.2014 - switch to field_v12b from v09e
// F. Uhlig       05.10.2010
//
// --------------------------------------------------------------------------

void FieldMapTest(TString fieldMap = "field_v12b",
		  Bool_t create_file = kFALSE)
{
  
  // -----   Load libraries   ---------------------------------------------
  // Load libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  // ----------------------------------------------------------------------
  
  
  // -------  Get magnetic field  -----------------------------------------
  // Somehow this information should be extracted from the map itself
  CbmFieldMap* magField = NULL;

  if (fieldMap == "field_v12b" )
    CbmFieldMap* magField = new CbmFieldMapSym3(fieldMap);
  // old magnetic field maps
  else if (fieldMap == "field_v09e" )
    CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  else if (fieldMap == "field_v09m" )
    CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  else if (fieldMap == "field_v10e" )
    CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  else if (fieldMap == "field_electron_standard" ) 
    CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  else if (fieldMap == "field_muon_standard" )
    CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  else if (fieldMap == "FieldMuonMagnet" )
  else {
    cout << "===> ERROR: Unknown field map " << fieldMap << endl;
    exit;
  }
  
  magField->Init();
  magField->Print();
  
  if (create_file) {

    // Number of test positions for which magnetic field values are
    // compared
    const Int_t numPoints=10000;

    Double_t targX = magField->GetPositionX();
    Double_t targY = magField->GetPositionY();
    Double_t targZ = magField->GetPositionZ();
    
    Double_t fxmin = magField->GetXmin();
    Double_t fxmax = magField->GetXmax();
    Double_t fymin = magField->GetYmin();
    Double_t fymax = magField->GetYmax();
    Double_t fzmin = magField->GetZmin();
    Double_t fzmax = magField->GetZmax();
    Double_t stepX = magField->GetXstep();
    Double_t stepY = magField->GetYstep();
    Double_t stepZ = magField->GetZstep();
    Int_t nX = magField->GetNx();
    Int_t nY = magField->GetNy();
    Int_t nZ = magField->GetNz();
    
    Double_t posX;
    Double_t posY;
    Double_t posZ;
    Double_t bX;
    Double_t bY;
    Double_t bZ;
    
    // ----------------------------------------------------------------------
    
    TString inputFile = "Qa_"+fieldMap+".root";

    TFile* f = new TFile(inputFile,"RECREATE");
    TTree *tree = new TTree("T","Store field values and positions");
    tree->Branch("posX",&posX,"posX/D");
    tree->Branch("posY",&posY,"posY/D");
    tree->Branch("posZ",&posZ,"posZ/D");
    tree->Branch("bX",&bX,"bX/D");
    tree->Branch("bY",&bY,"bY/D");
    tree->Branch("bZ",&bZ,"bZ/D");
    
    TTree *tree1 = new TTree("fieldInfo","Store field info from fieldmap");
    tree1->Branch("fxmin",&fxmin,"fxmin/D");
    tree1->Branch("fxmax",&fxmax,"fxmax/D");
    tree1->Branch("fymin",&fymin,"fymin/D");
    tree1->Branch("fymax",&fymax,"fymax/D");
    tree1->Branch("fzmin",&fzmin,"fzmin/D");
    tree1->Branch("fzmax",&fzmax,"fzmax/D");
    tree1->Branch("stepX",&stepX,"stepX/D");
    tree1->Branch("stepY",&stepY,"stepY/D");
    tree1->Branch("stepZ",&stepZ,"stepZ/D");
    tree1->Branch("targX",&targX,"targX/D");
    tree1->Branch("targY",&targY,"targY/D");
    tree1->Branch("targZ",&targZ,"targZ/D");
    tree1->Branch("nX",&nX,"nX/I");
    tree1->Branch("nY",&nY,"nY/I");
    tree1->Branch("nZ",&nZ,"nZ/I");
    
    // ------------ Get Field value for numPoints random positions ---------- 
    
    tree1->Fill();
    
    cout << "=====>  Filling Arrays..." << endl;
    for (Int_t i=0; i<numPoints; i++){
      posX=gRandom->Uniform(fxmin, fxmax);
      posY=gRandom->Uniform(fymin, fymax);
      posZ=gRandom->Uniform(fzmin, fzmax);
      bX = magField->GetBx(posX,posY,posZ) / 10.;
      bY = magField->GetBy(posX,posY,posZ) / 10.;
      bZ = magField->GetBz(posX,posY,posZ) / 10.;
      
      tree->Fill();
    }
    
    f->Write();
    f->Close();

  } else {

    Double_t targX_map = magField->GetPositionX();
    Double_t targY_map = magField->GetPositionY();
    Double_t targZ_map = magField->GetPositionZ();
    
    Double_t fxmin_map = magField->GetXmin();
    Double_t fxmax_map = magField->GetXmax();
    Double_t fymin_map = magField->GetYmin();
    Double_t fymax_map = magField->GetYmax();
    Double_t fzmin_map = magField->GetZmin();
    Double_t fzmax_map = magField->GetZmax();
    Double_t stepX_map = magField->GetXstep();
    Double_t stepY_map = magField->GetYstep();
    Double_t stepZ_map = magField->GetZstep();
    Int_t nX_map = magField->GetNx();
    Int_t nY_map = magField->GetNy();
    Int_t nZ_map = magField->GetNz();
    
    Double_t posX_map;
    Double_t posY_map;
    Double_t posZ_map;
    Double_t bX_map;
    Double_t bY_map;
    Double_t bZ_map;
    
    Double_t targX;
    Double_t targY;
    Double_t targZ;
    
    Double_t fxmin;
    Double_t fxmax;
    Double_t fymin;
    Double_t fymax;
    Double_t fzmin;
    Double_t fzmax;
    Double_t stepX;
    Double_t stepY;
    Double_t stepZ;
    Int_t nX;
    Int_t nY;
    Int_t nZ;
    
    Double_t posX;
    Double_t posY;
    Double_t posZ;
    Double_t bX;
    Double_t bY;
    Double_t bZ;
    
    Int_t compare_equal = 0;
    Int_t compare_in_range = 0;
    Int_t compare_wrong = 0;
    Bool_t test_ok = kTRUE;
    Float_t compare_range = 0.00001;
      
    TString inDir   = gSystem->Getenv("VMCWORKDIR");
    TString inputFile = inDir + "/input/qa/Qa_"+fieldMap+".root";


    TFile* f = new TFile(inputFile,"READ");
    TTree *tree = (TTree*)f->Get("T");
    tree->SetBranchAddress("posX",&posX);
    tree->SetBranchAddress("posY",&posY);
    tree->SetBranchAddress("posZ",&posZ);
    tree->SetBranchAddress("bX",&bX);
    tree->SetBranchAddress("bY",&bY);
    tree->SetBranchAddress("bZ",&bZ);
    
    TTree *tree1 = (TTree*)f->Get("fieldInfo");
    tree1->SetBranchAddress("fxmin",&fxmin);
    tree1->SetBranchAddress("fxmax",&fxmax);
    tree1->SetBranchAddress("fymin",&fymin);
    tree1->SetBranchAddress("fymax",&fymax);
    tree1->SetBranchAddress("fzmin",&fzmin);
    tree1->SetBranchAddress("fzmax",&fzmax);
    tree1->SetBranchAddress("stepX",&stepX);
    tree1->SetBranchAddress("stepY",&stepY);
    tree1->SetBranchAddress("stepZ",&stepZ);
    tree1->SetBranchAddress("targX",&targX);
    tree1->SetBranchAddress("targY",&targY);
    tree1->SetBranchAddress("targZ",&targZ);
    tree1->SetBranchAddress("nX",&nX);
    tree1->SetBranchAddress("nY",&nY);
    tree1->SetBranchAddress("nZ",&nZ);
    
    // ------------ Get Field value for numPoints random positions ---------- 
    
    //    tree->Print();
    //    tree1->Print();

    // compare field information
    tree1->GetEntry(0);
    
    if ( fxmin != fxmin_map ) test_ok = kFALSE; 
    if ( fxmax != fxmax_map ) test_ok = kFALSE; 
    if ( fymin != fymin_map ) test_ok = kFALSE; 
    if ( fymax != fymax_map ) test_ok = kFALSE; 
    if ( fzmin != fzmin_map ) test_ok = kFALSE; 
    if ( fzmax != fzmax_map ) test_ok = kFALSE; 
    
    if ( stepX != stepX_map ) test_ok = kFALSE; 
    if ( stepY != stepY_map ) test_ok = kFALSE; 
    if ( stepZ != stepZ_map ) test_ok = kFALSE; 
    if ( targX != targX_map ) test_ok = kFALSE; 
    if ( targY != targY_map ) test_ok = kFALSE; 
    if ( targZ != targZ_map ) test_ok = kFALSE; 
    if ( nX != nX_map ) test_ok = kFALSE; 
    if ( nY != nY_map ) test_ok = kFALSE; 
    if ( nZ != nZ_map ) test_ok = kFALSE; 

    if (test_ok) {
      cout<<"Compared field information is ok."<<endl;
    } else {
      cout<<"Compared field information differ."<<endl;
    }
    
    Int_t nEntries = tree->GetEntriesFast();
    
    for (Int_t i=0; i<nEntries; i++){
      //    for (Int_t i=0; i<10; i++){
      tree->GetEntry(i);
      bX_map = magField->GetBx(posX,posY,posZ) / 10.;
      bY_map = magField->GetBy(posX,posY,posZ) / 10.;
      bZ_map = magField->GetBz(posX,posY,posZ) / 10.;
      if ( bX == bX_map &&  bY == bY_map &&  bZ == bZ_map ) {
	compare_equal++;
      } 
      else if ( (bX-bX_map)/bX_map < compare_range &&
		(bY-bY_map)/bY_map < compare_range &&
		(bZ-bZ_map)/bZ_map < compare_range ) {
	compare_in_range++;
      }
      else {
	compare_wrong++;
      }
    }
    
    
    f->Close();
    
    if ( compare_equal > 0 ) {
      cout<<compare_equal<<" checked field values are equal."<<endl;
    }
    if ( compare_in_range > 0 ) {
      cout<<compare_in_range<<" checked field values are of by less then "
	  <<compare_range*100<<" %."<<endl;
    }
    if ( compare_wrong > 0 ) {
      cout<<compare_wrong<<" checked field values are of by more then "
	  <<compare_range*100<<" %."<<endl;
      test_ok = kFALSE;
    }
    
    if ( test_ok) {
      cout<<"Test Passed"<<endl;
      cout<<"All ok"<<endl;
    } else {
      cout<<"Test failed"<<endl;
    }
    
  }    
}


