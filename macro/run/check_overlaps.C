void check_overlaps(const char* setup = "sis300_electron")
{
  // 2014-07-04 - DE - test CBM setups for collisions in nightly tests
  // 2014-07-04 - DE - currently there are 2 overlaps between the PIPE and STS layer 8
  // 2014-07-04 - DE - set the default to 0 overlaps, anyway
  //
  // Number of known and unresolved overlaps. Most are in the Magnet.
  // Some are in the Sts.
  // Until they can be fixed we will not mark the test as error if the
  // number of overlaps is eqaul the known number of overlaps.
  // Today there are also some overlaps in the rich i don't remember.
  // FU 12.11.10
  // Remove overlaps of pipe with sts keeping volumes by increasing the
  // inner radius of all sts keeping volumes by 0.4 mm. This reduces the
  // number of known overlaps by 8 from 94 to 86.
  // FU 12.11.10

  Int_t benchmarkNumber = 0;

  TString outDir  = "data/";
  TString geoFile = outDir + setup + "_geofile_full.root";
  TFile* f = new TFile(geoFile);

  gGeoManager = (TGeoManager*) f->Get("FAIRGeom"); 
  gGeoManager->CheckOverlaps(0.001);

  TObjArray *overlapArray = gGeoManager->GetListOfOverlaps();
  Int_t numOverlaps = overlapArray->GetEntries();
  gGeoManager->PrintOverlaps();

  if ( numOverlaps != benchmarkNumber ) {
    cout << " Test failed" << endl;
    cout << " We have in total "<<numOverlaps<<" overlaps."<<endl;
  } else {
    cout << " Test passed" << endl;
    cout << " All ok " << endl;
  }
}
