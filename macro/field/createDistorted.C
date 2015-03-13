
{

  // ----------   Load libraries   ------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  // ------------------------------------------------------------------------


  // ========================================================================

  // Name of the map to be distorted
  TString parentMapName = "FieldMuonMagnet";


  // construct new field with the name Tst1, distortion information in the file DistortedFormulas_001.root from the parent map:
  CbmFieldMapDistorted* dist_field = new CbmFieldMapDistorted("Tst1","DistortedFormulas_001",parentMapName.Data());

  // initialize the parent map and the distorted map
  dist_field->Init();

  
 // ========================================================================
  // Create the distortion information and save it into the file:
  // (The artificial example to illustrate how you can create the distortion of one or all field components)

// Bx_new =Bx_old [z>100 && z<200]; =0 for other z
  dist_field->SetDistortionFormula("0. +1.0*(z>100 && z<200)","x","m"); // =0 for z<100 and z>200

// By_new =By_old*2 [z>100 && z<200]; =By_old+0.5 [z>250]; =By_old for other z
  dist_field->SetDistortionFormula("1.0+2.0*(z>100 && z<200)","y","m"); // =1 for z<100 and z>200 
  dist_field->SetDistortionFormula("0.5*(z>250)","y","a");              // =0.5 for z>250

// Bz_new =TMath::Landau(z)*sin(x)
  dist_field->SetDistortionFormula("0","z","m");                        // =0
  dist_field->SetDistortionFormula("TMath::Landau(z)*sin(x)","z","a");  // Landau oscillations along z

  dist_field->WriteDistortionInformation();
 // ========================================================================

  // Information about the original field and the distortion:
  dist_field->Print();

  //  Check the resulting field:
  cout << " x     y      z       Bx_new  By_new  Bz_new    Bx_old  By_old  Bz_old" << endl;
  Double_t po[3],b[3], bp[3];
  po[0]=-10.; po[1]=10; po[2]=10;
  for (int i=0; i<20; i++)
    {
  dist_field->GetFieldValue(po,b);
  dist_field->GetParent()->GetFieldValue(po,bp);
  cout << po[0] << " " << po[1] << " " << po[2] << "    "  << b[0] << " " << b[1] << " " << b[2] << "    " << bp[0] << " " << bp[1] << " " << bp[2] << endl;
    po[2]=po[2]+20.;
    }

  // Plot the original and distorted field
  dist_field->PlotBy();

  //  dist_field can be used in simulations and analysis
  //  Use in the simulation:
  // CbmFieldMapDistorted* field = new CbmFieldMapDistorted("Tst1","DistortedFormulas_001","FieldMuonMagnet");
  // Use in the analysis: works since SVN Revision 2096
  // 
}
