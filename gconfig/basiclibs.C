// $Id: basiclibs.C,v 1.1.1.1 2005/06/23 07:14:09 dbertini Exp $
//
// Macro for loading basic libraries used with both Geant3 and Geant4

void basiclibs()
{
  gSystem->Load("libGeom");
  gSystem->Load("libGeomPainter");
 
  gSystem->Load("libVMC");
  gSystem->Load("libPhysics");
  gSystem->Load("libEG"); 
  gSystem->Load("libEGPythia6");
  gSystem->Load("libPythia6");  
  gSystem->Load("libPluto");  
  gSystem->Load("libMinuit");
  gSystem->Load("libMLP");

  gSystem->Load("libProof");
  gSystem->Load("libProofPlayer");

  gSystem->Load("libFairTools");
  gSystem->Load("libFairDB");
}  
