//
//   comments
//
// 2014-07-03 - DE - use PIPE v14h together with RICH v14a
// 2014-06-30 - DE - make RICH v14a the new default
// 2014-06-30 - DE - make TRD v14a the new default
// 2014-06-25 - DE - define digi files through tags for STS, TRD and TOF
// 2013-11-05 - DE - switch to trd_v13p geometry
// 2013-10-11 - DE - add empty string defining the platform
// 2013-10-07 - DE - pipe_v13d.geo fixes overlap with TOF
// 2013-10-07 - DE - rich_v13a is not at nominal position 1800mm, but at 1600mm
//

static TString caveGeom;
static TString pipeGeom;
static TString magnetGeom;
static TString mvdGeom;
static TString stsGeom;
static TString richGeom;
static TString muchGeom;
static TString shieldGeom;
static TString trdGeom;
static TString tofGeom;
static TString ecalGeom;
static TString platformGeom;
static TString psdGeom;
static Double_t psdZpos;
static Double_t psdXpos;

static TString stsTag;
static TString trdTag;
static TString tofTag;

static TString stsDigi;
static TString trdDigi;
static TString tofDigi;

static TString  fieldMap;
static Double_t fieldZ;
static Double_t fieldScale;
static Int_t    fieldSymType;

void sis300_electron_setup()
{
  FairLogger* logger = FairLogger::GetLogger();

  // -----   Magnetic field   -----------------------------------------------
  fieldMap     = "field_v12b";   // name of field map
  fieldZ       = 40.;            // field centre z position
  fieldScale   =  1.;            // field scaling factor
  fieldSymType =  3;

  // -----  Tags  -----------------------------------------------------------
  stsTag       = "v13d";
  trdTag       = "v14a_3e";
  tofTag       = "v13-5d";

  // -----  DigiFiles  ------------------------------------------------------
  stsDigi      = "sts/sts_" + stsTag + "_std.digi.par";
  trdDigi      = "trd/trd_" + trdTag + ".digi.par";
  //  tofDigi      = "tof/tof_" + tofTag + ".digi.par";
  tofDigi      = "tof/tof_v13b.digi.par";

  // -----  Geometries  -----------------------------------------------------
  caveGeom     = "cave.geo";
  pipeGeom     = "pipe/pipe_v13b.geo.root";
  magnetGeom   = "magnet/magnet_v12b.geo.root";
  mvdGeom      = "";
  stsGeom      = "sts/sts_" + stsTag + ".geo.root";
  richGeom     = "rich/rich_v08a.geo";
  muchGeom     = "";
  shieldGeom   = "";
  platformGeom = ""; // "passive/platform_v13b.geo";
  trdGeom      = "trd/trd_" + trdTag + ".geo.root";
  tofGeom      = "tof/tof_" + tofTag + ".geo.root";
  ecalGeom     = "";
  psdGeom      = "psd_geo_xy.txt";
  psdZpos = 1500.;
  psdXpos = 10.;

  cout<<"Reading geometry setup and magnetic field settings from "<< gSystem->UnixPathName(gInterpreter->GetCurrentMacroName())<<endl;
}
