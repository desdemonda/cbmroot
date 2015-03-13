// --------------------------------------------------------------------------
//
// Macro to create a geometry file for the TRD
// This file is needed to create the digi parameters as
// next step
//
// F. Uhlig 07.05.2010
//
// D. Emschermann - 26.05.2010 - fix TRD geo input file
//
// expecting the trd_v13q.root file in directory "geometry"
//
// --------------------------------------------------------------------------

void create_geometry_file_root(TString geoName = "trd_v13q")
{
   TString outDir  = "data";
   TString outFile = outDir + "/test.mc."+ geoName + ".root";
   TString parFile = outDir + "/params.root";

   TString caveGeom   = "cave.geo";
//   TString trdGeom    = "trd/" + geoName + ".root";
//   TString trdGeom    = "../macro/trd/geometry/trd.v13/" + geoName + ".root";
   TString trdGeom    = "../macro/trd/geometry/" + geoName + ".geo.root";
   TString trdGeomOut = "geofile_" + geoName + ".root";

   FairRunSim* run = new FairRunSim();
   run->SetName("TGeant3");
   run->SetOutputFile(outFile);
   run->SetMaterials("media.geo");

   if (caveGeom != "") {
      FairModule* cave = new CbmCave("CAVE");
      cave->SetGeometryFileName(caveGeom);
      run->AddModule(cave);
   }
   if (trdGeom != "") {
      FairDetector* trd = new CbmTrd("TRD", kTRUE);
      trd->SetGeometryFileName(trdGeom);
      run->AddModule(trd);
   }

   FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
   run->SetGenerator(primGen);
   FairBoxGenerator* boxGen1 = new FairBoxGenerator(11, 20);
   boxGen1->SetPRange(2.,2.);
   boxGen1->SetPhiRange(0.,0.);
   boxGen1->SetThetaRange(0.,0.);
   boxGen1->SetCosTheta();
   boxGen1->Init();
   primGen->AddGenerator(boxGen1);

   // Constant Field
   CbmFieldConst* magField = new CbmFieldConst();
   magField->SetField(0, 14 ,0 ); // values are in kG
   magField->SetFieldRegion(-74, -39 ,-22 , 74, 39 , 160);
   run->SetField(magField);

   run->Init();
   run->Run(1);

   // Create separate file with the geomanager only
   run->CreateGeometryFile(trdGeomOut);
}

