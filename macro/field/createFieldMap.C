// --------------------------------------------------------------------------
//
// This macro allows to create a field map out of an arbitrary number
// of instances of CbmField. The field values of all fields are added
// at each space point on the grid.
// 
// Typical applications are:
// -- adding two field maps,
// -- manipulating the grid of an existing field map.
//
// V. Friese   15/01/2008
//
// --------------------------------------------------------------------------


{

  // ----------   Load libraries   ------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  // ------------------------------------------------------------------------



  // ==========   Define the parameters of the map to be created   ==========

  // Extension of the map in coordinate space [cm]
  Double_t xmin = -100.;
  Double_t xmax =  100.;
  Double_t ymin = -100.;
  Double_t ymax =  100.;
  Double_t zmin = -100.;
  Double_t zmax =  450.;

  // Number of grid points
  Int_t nx = 101;
  Int_t ny = 101;
  Int_t nz = 276;

  // Name of the map to be created
  TString mapName = "TestField";

  // ========================================================================




  // -----------   Instantiate CbmFieldMapCreator   -------------------------
  CbmFieldMapCreator* creator = new CbmFieldMapCreator(mapName.Data());
  creator->SetGridParameters(nx, xmin, xmax, ny, ymin, ymax, nz, zmin, zmax);
  // ------------------------------------------------------------------------



  // ==========   Now instantiate the input fields and add them   ===========
  //
  // In this example, a compensating constant field with By = 1 T around
  // the origin ( +- 20 cm ) is added to the field map FieldmuonMagnet.
  //
  // Note that the new field map must not be shifted by 50 cm w.r.t.
  // the target since this is already done when using the input field map.
  //
  CbmFieldMap* field1 = new CbmFieldMapSym3("FieldMuonMagnet");
  field1->SetPosition(0., 0., 50.);
  field1->Init();
  creator->AddField(field1);

  CbmField* field2 = new CbmFieldConst("field2", 
				       -20., 20., 
				       -20., 20.,
				       -20., 20., 
				       0., 10., 0.);
  creator->AddField(field2);
  // ========================================================================



  // -----------   Create the new field map   -------------------------------
  creator->CreateMap();
  // ------------------------------------------------------------------------

}
