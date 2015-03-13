// --------------------------------------------------------------------------
// -----                 Class CbmTofHitProducer                       ------
// -----           Created  by E. Cordier 14/09/05                     ------
// -----           Modified by D. Gonzalez-Diaz 07/09/06               ------
// -----           Modified by D. Gonzalez-Diaz 02/02/07               ------
// --------------------------------------------------------------------------

#include "CbmTofHitProducer.h"

#include "CbmTofPoint.h"
#include "CbmTofHit.h"
#include "CbmMCTrack.h"
#include "CbmTofGeoHandler.h"
#include "CbmTofCell.h"
#include "CbmTofDigiPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TRandom.h"
#include "TString.h"
#include "TVector3.h"
#include "TSystem.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------

CbmTofHitProducer::CbmTofHitProducer() :
      FairTask("CbmTofHitProducer"),
      fVerbose(1),
      fTofPoints(NULL),
      fMCTracks(NULL),
      fTofHits(NULL),
      X(),
      Dx(),
      Y(),
      Dy(),
      type(),
      tl(),
      tr(),
      trackID_left(),
      trackID_right(),
      point_left(),
      point_right(),
      fSigmaT(0.),
      fSigmaEl(0.),
      fSigmaXY(0.),
      fSigmaY(0.),
      fSigmaZ(0.),
      fVersion(""),
      fNHits(-1),
      fTofZPosition(0.),
      fGeoHandler(new CbmTofGeoHandler()),
      fDigiPar(NULL),
      fCellInfo(NULL),
      fParInitFromAscii(kTRUE)
{
}

// ---- Constructor ----------------------------------------------------

CbmTofHitProducer::CbmTofHitProducer(const char *name, Int_t verbose) :
      FairTask(name, verbose),
      fVerbose(verbose),
      fTofPoints(NULL),
      fMCTracks(NULL),
      fTofHits(NULL),
      X(),
      Dx(),
      Y(),
      Dy(),
      type(),
      tl(),
      tr(),
      trackID_left(),
      trackID_right(),
      point_left(),
      point_right(),
      fSigmaT(0.),
      fSigmaEl(0.),
      fSigmaXY(0.),
      fSigmaY(0.),
      fSigmaZ(0.),
      fVersion(""),
      fNHits(-1),
      fTofZPosition(0.),
      fGeoHandler(new CbmTofGeoHandler()),
      fDigiPar(NULL),
      fCellInfo(NULL),
      fParInitFromAscii(kTRUE)
{
}

// ---- Destructor ----------------------------------------------------

CbmTofHitProducer::~CbmTofHitProducer()
{
  if (fGeoHandler) {
    delete fGeoHandler;
  }
}

// --------------------------------------------------
void CbmTofHitProducer::SetParContainers()
{
  if (!fParInitFromAscii) {
    LOG(INFO)<<" Get the digi parameters for tof"<<FairLogger::endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTofDigiPar*)
               (rtdb->getContainer("CbmTofDigiPar"));

  }
}
// --------------------------------------------------------------------
InitStatus CbmTofHitProducer::ReInit()
{
  if (!fParInitFromAscii) {
    LOG(INFO)<<"Reinitialize the digi parameters for tof"<<FairLogger::endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTofDigiPar*)
               (rtdb->getContainer("CbmTofDigiPar"));

  }
}

// ---- Init ----------------------------------------------------------

InitStatus CbmTofHitProducer::Init() {
   FairRootManager* ioman = FairRootManager::Instance();

   fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
   fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");

   // Initialize the TOF GeoHandler
   Bool_t isSimulation=kFALSE;
   Int_t bla = fGeoHandler->Init(isSimulation);

   if (fParInitFromAscii) {
     ReadTofZPosition();
     InitParametersFromAscii();
   } else {
     InitParametersFromContainer();
   }


   fTofHits = new TClonesArray("CbmTofHit");
   ioman->Register("TofHit", "Tof", fTofHits, kTRUE);

   LOG(INFO) << "-I- CbmTofHitProducer: Intialization successfull!" << FairLogger::endl;

   return kSUCCESS;
}

// ---- Exec ----------------------------------------------------------

void CbmTofHitProducer::Exec(Option_t * option) {
   fTofHits->Clear();
   fNHits = -1; // Must start in -1

   // Some numbers on TOF distributions
   Int_t tof_tracks = 0, tof_tracks_vert = 0, tof_tracks_local = 0;
   Int_t nofMCTracks = fMCTracks->GetEntries();
   for (Int_t p = 0; p < nofMCTracks; p++) {
      const CbmMCTrack* mcTrack = static_cast<const CbmMCTrack*>(fMCTracks->At(p));
      if (mcTrack->GetNPoints(kTOF) > 0) tof_tracks++;
 //     if (mcTrack->GetNPoints(kTOF) > 0 && mcTrack->GetStartZ() > (fTofZPosition - 4)) tof_tracks_local++;
      if (mcTrack->GetNPoints(kTOF) > 0 && mcTrack->GetMotherId() == -1) tof_tracks_vert++;
   }
   cout << "-I- CbmTofHitProducer : " << tof_tracks << " tracks in Tof\n";
   cout << "-I- CbmTofHitProducer : " << tof_tracks_vert << " tracks in Tof from vertex\n";
 //  cout << "-I- CbmTofHitProducer : " << tof_tracks_local << " tracks in Tof produced locally (Z > " << fTofZPosition - 4 << " cm)\n";
 //  cout << "-I- CbmTofHitProducer : " << tof_tracks - tof_tracks_local << " tracks in Tof able to produce a hit\n";
   cout << "-I- CbmTofHitProducer : " << fTofPoints->GetEntriesFast() << " MC points in this event\n";

   Int_t nregions = 10, nmodules = 500, ncells = 500, ngaps = 8;
   // FIXME: these parameters must be provided externally
   //Double_t Z = 1000;
   Double_t Dz = 2.04; //FIXME: Introduce also Dz and Z as (constant) parameters
   Float_t sigma_T = 0.08, sigma_Y = 0.7, sigma_t_gap, t_o, sigma_el, vprop = 20, Pgap = 0.75;
   //time[ns], position[cm], velocity[cm/ns]
   // FIXME: these parameters must be provided externally

   // Here check for the validity of the parameters
   if (fSigmaY > 1)
      cout << "IRREALISTIC TOF POSITION RESOLUTION!! (HitProducer may crash)" << endl;
   if ((fSigmaT < 0.07 && fSigmaT > 0) || fSigmaT > 0.2)
      cout << "IRREALISTIC TOF RESOLUTION!! (HitProducer may crash)" << endl;

   //Parameterizations. Now they depend on the geometry/algorithm. FIXME

   if (fSigmaY != 0) sigma_el = sqrt(2.) * fSigmaY / vprop * 1.3;
   else sigma_el = sqrt(2.) * sigma_Y / vprop * 1.3;

   if (fSigmaT != 0) {
      sigma_t_gap = sqrt(
            pow(fSigmaT / (0.5 * pow(ngaps, -0.361) + 0.40), 2)
                  - 1.4 / 1.3 * pow(sigma_el, 2));
      t_o = 1.93 * fSigmaT;
   } else {
      sigma_t_gap = sqrt(
            pow(sigma_T / (0.5 * pow(ngaps, -0.361) + 0.40), 2)
                  - 1.4 / 1.3 * pow(sigma_el, 2));
      t_o = 1.93 * sigma_T;
   }

   //Initialization of cell times
   for (Int_t i = 0; i < nregions; i++) {
      for (Int_t j = 0; j < nmodules; j++) {
         for (Int_t k = 0; k < ncells; k++) {
            tl[i][j][k] = 1e+6;
            tr[i][j][k] = 1e+6;
         }
      }
   }

   // Loop over the TOF points
   Int_t nofTofPoints = fTofPoints->GetEntries();
   for (Int_t j = 0; j < nofTofPoints; j++) {
      // Probability that the avalanche is detected
      if (gRandom->Uniform(1) > Pgap) continue;

      CbmTofPoint* mcPoint = (CbmTofPoint*) fTofPoints->At(j);
      if (mcPoint == NULL) {
         cout << "Be careful: hole in the CbmTofPoint TClonesArray!" << endl;
         continue;
      }

      if (mcPoint->GetTrackID() < 0) continue;
      CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(mcPoint->GetTrackID());

      //Get relevant information from the point
      Int_t trackID = mcPoint->GetTrackID();
      Int_t detID   = mcPoint->GetDetectorID();
      Int_t cell    = fGeoHandler->GetCell(detID)-1;
      Int_t module  = fGeoHandler->GetCounter(detID)-1;
      Int_t region  = fGeoHandler->GetRegion(detID)-1;

      // Reject particles produced in the last 4 cm. Better job must be done here. For example:
      // it could be better to go up to the parent particle and get its trackID, then the
      // secondary is processed. FIXME.
      //if ((mcTrack->GetStartZ()) > 996) continue;
      if ((mcTrack->GetStartZ()) > (Z[region][module][cell] - 4.)) continue;

      LOG(DEBUG2)<<"Det System: "<<fGeoHandler->GetDetSystemId(detID)<<FairLogger::endl;
      LOG(DEBUG2)<<"SMtype: "<<fGeoHandler->GetSMType(detID)<<FairLogger::endl;
      LOG(DEBUG2)<<"SModule: "<<fGeoHandler->GetSModule(detID)<<FairLogger::endl;
      LOG(DEBUG2)<<"Counter: "<<fGeoHandler->GetCounter(detID)<<FairLogger::endl;
      LOG(DEBUG2)<<"Gap: "<<fGeoHandler->GetGap(detID)<<FairLogger::endl;
      LOG(DEBUG2)<<"Cell: "<<fGeoHandler->GetCell(detID)<<FairLogger::endl;
      LOG(DEBUG2)<<"Region: "<<fGeoHandler->GetRegion(detID)<<FairLogger::endl;

      TVector3 pos;
      mcPoint->Position(pos);

      Double_t T_smearing = gRandom->Gaus(t_o, sigma_t_gap);
      Double_t tl_new = mcPoint->GetTime() + T_smearing - pos.Y() / vprop + gRandom->Gaus(0, sigma_el);
      Double_t tr_new = mcPoint->GetTime() + T_smearing + pos.Y() / vprop + gRandom->Gaus(0, sigma_el);

      Int_t flag = 1;

      //Increase the counter for the TofHit TClonesArray if the first time a hit is attached to this cell
      if (tl[region][module][cell] == 1e+6 && tr[region][module][cell] == 1e+6) {
         fNHits++;
      } else {
         if (trackID_left[region][module][cell] != trackID
               || trackID_right[region][module][cell] != trackID) {
            flag = 2;
            //Worsening of the time response in the presence of 2 (or more) hits is not possible to implement
            //within the present algorithm (taking the fastest hit). FIXME
         }
      }

      // Take the fastest time from all the points/gaps in this cell
      if (tl_new < tl[region][module][cell]) {
         tl[region][module][cell] = tl_new;
         trackID_left[region][module][cell] = trackID;
         point_left[region][module][cell] = j;
      }

      if (tr_new < tr[region][module][cell]) {
         tr[region][module][cell] = tr_new;
         trackID_right[region][module][cell] = trackID;
         point_right[region][module][cell] = j;
      }

      // X and Y depend on the orientation of the cell. FIXME
      Double_t xHit = 0., yHit = 0., zHit = 0., tHit = 0.;
      Double_t xHitErr = 0., yHitErr = 0., zHitErr = 0.;
      // Parameters of the Hit. Different for pad or strip read-out.
      if (type[region][module][cell] == "s") {
         yHit = (tr[region][module][cell] - tl[region][module][cell]) / 2 * vprop;
         yHitErr = sigma_el / sqrt(2.) * vprop / 1.3; //This must be better done.
         // 1.3 comes from a relation lines above. FIXME
         tHit = (tl[region][module][cell] + tr[region][module][cell]) / 2;
      } else if (type[region][module][cell] == "p") {
         yHit = Dy[region][module][cell] / 2 / 10 + Y[region][module][cell] / 10;
         yHitErr = Dy[region][module][cell] / 10 / sqrt(12.);
         tHit = tl[region][module][cell] + pos.Y() / vprop;
      }

      xHit = Dx[region][module][cell] / 2 / 10 + X[region][module][cell] / 10;
      xHitErr = Dx[region][module][cell] / 10 / sqrt(12.);
      zHit = Z[region][module][cell] + Dz / 2.; //Dz / 2 + Z;
      zHitErr = Dz / sqrt(12.);

      // Reference to the point that contributes to the left side.
      Int_t ref = point_left[region][module][cell];

      // Reference to the point that contributes to the right side.
      if (type[region][module][cell] == "s"
            && (trackID_left[region][module][cell]
                  != trackID_right[region][module][cell])) {
         flag = 10 + point_right[region][module][cell];
         // More than one TRACK contributed to this Hit: store the reference as flag + an offset of 10.
      }

      TVector3 hitPos(xHit, yHit, zHit);
      TVector3 hitPosErr(xHitErr, yHitErr, zHitErr);
      AddHit(mcPoint->GetDetectorID(), hitPos, hitPosErr, j, tHit, flag);
   }

   cout << "-I- CbmTofHitProducer : " << fNHits << " hits in Tof created" << endl;
}

// ---- Add Hit to HitCollection --------------------------------------

void CbmTofHitProducer::AddHit(Int_t detID, TVector3 &posHit,
      TVector3 &posHitErr, Int_t ref, Double_t tHit, Int_t flag) {
   new ((*fTofHits)[fNHits]) CbmTofHit(detID, posHit, posHitErr, ref, tHit, flag);
   if (fVerbose > 1) {
      CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(fNHits);
      tofHit->Print();
      cout << endl;
   }
}

// ---- Finish --------------------------------------------------------

void CbmTofHitProducer::Finish() {
}

void CbmTofHitProducer::ReadTofZPosition()
{
   TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
   for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
      TGeoNode* tof = (TGeoNode*) nodes->At(iNode);
      if (TString(tof->GetName()).Contains("tof")) {
         const Double_t* tofPos = tof->GetMatrix()->GetTranslation();
         TGeoBBox* shape = (TGeoBBox*) tof->GetVolume()->GetShape();
         fTofZPosition = tofPos[2] - shape->GetDZ();
         LOG(INFO)<<tofPos[2]<<" , "<< shape->GetDZ()<<FairLogger::endl;
      }
   }
   LOG(INFO)<< "Set TOF z position from geometry manager= " << fTofZPosition << FairLogger::endl;
}

// ---- SetSigmaT -----------------------------------------------------

void CbmTofHitProducer::SetSigmaT(Double_t sigma) {
   fSigmaT = sigma;
}

// ---- SetSigmaXY -----------------------------------------------------

void CbmTofHitProducer::SetSigmaXY(Double_t sigma) {
   fSigmaXY = sigma;
}

// ---- SetSigmaY -----------------------------------------------------

void CbmTofHitProducer::SetSigmaY(Double_t sigma) {
   fSigmaY = sigma;
}

// ---- SetSigmaZ -----------------------------------------------------

void CbmTofHitProducer::SetSigmaZ(Double_t sigma) {
   fSigmaZ = sigma;
}

// ---- GetSigmaXY -----------------------------------------------------

Double_t CbmTofHitProducer::GetSigmaT() {
   return fSigmaT;
}

// ---- GetSigmaXY -----------------------------------------------------

Double_t CbmTofHitProducer::GetSigmaXY() {
   return fSigmaXY;
}

// ---- GetSigmaY -----------------------------------------------------

Double_t CbmTofHitProducer::GetSigmaY() {
   return fSigmaY;
}

// ---- GetSigmaZ -----------------------------------------------------

Double_t CbmTofHitProducer::GetSigmaZ() {
   return fSigmaZ;
}


void CbmTofHitProducer::InitParametersFromAscii()
{
  LOG(INFO)<<"Initializing the tof parameters from the old ascii file."<<FairLogger::endl;

  FILE *par;

  //Reading the parameter file. In the future this must be done in a different way.

  char header = '#', cell_type = '#';
  int region, module, cell;
  Int_t nregions = 10, nmodules = 500, ncells = 500;
  Float_t X_tmp, Y_tmp, Dx_tmp, Dy_tmp;

  //    Initialize the matrixes [make this index visible in all the macro]. FIXME

  for (int i = 0; i < nregions; i++) {
     for (int j = 0; j < nmodules; j++) {
        for (int k = 0; k < ncells; k++) {
           X[i][j][k] = -1;
           Y[i][j][k] = -1;
           Dx[i][j][k] = -1;
           Dy[i][j][k] = -1;
           Z[i][j][k] = fTofZPosition;
        }
     }
  }

  TString tofGeoFile = gSystem->Getenv("VMCWORKDIR");
  tofGeoFile += "/parameters/tof/tof_standard.geom.par";
  par = fopen(tofGeoFile, "r");

  if (par == NULL) {
     LOG(FATAL)<<"ERROR WHILE OPENING THE PARAMETER FILE IN TOF HIT PRODUCER!"<<FairLogger::endl;
  }

  //Skip the header. In the future the header structure must be defined. FIXME

  while (fscanf(par, "%c", &header) >= 0) {
     if ((int) (header - '0') == 0) break;
  }

  //Read the first line
  region = 0;
  fscanf(par, "%d%d%s%f%f%f%f", &module, &cell, &cell_type, &X_tmp, &Y_tmp, &Dx_tmp, &Dy_tmp);
  X[region][module][cell] = X_tmp;
  Y[region][module][cell] = Y_tmp;
  Dx[region][module][cell] = Dx_tmp;
  Dy[region][module][cell] = Dy_tmp;
  type[region][module][cell] = cell_type;

  //Read all the lines

  while (fscanf(par, "%d%d%d%s%f%f%f%f", &region, &module, &cell, &cell_type,
        &X_tmp, &Y_tmp, &Dx_tmp, &Dy_tmp) >= 0) {
     X[region][module][cell] = X_tmp;
     Y[region][module][cell] = Y_tmp;
     Dx[region][module][cell] = Dx_tmp;
     Dy[region][module][cell] = Dy_tmp;
     type[region][module][cell] = cell_type;
  }

  fclose(par);
}

void CbmTofHitProducer::InitParametersFromContainer()
{

   Int_t nregions = 10, nmodules = 500, ncells = 500;

   //    Initialize the matrixes [make this index visible in all the macro]. FIXME

   for (int i = 0; i < nregions; i++) {
      for (int j = 0; j < nmodules; j++) {
         for (int k = 0; k < ncells; k++) {
            X[i][j][k] = -1;
            Y[i][j][k] = -1;
            Z[i][j][k] = -1;
            Dx[i][j][k] = -1;
            Dy[i][j][k] = -1;
         }
      }
   }

   Int_t nrOfCells = fDigiPar->GetNrOfModules();
   LOG(INFO)<<"Parameter container contain "<<nrOfCells<<" cells."<<FairLogger::endl;

   for (Int_t icell = 0; icell < nrOfCells; ++icell) {
     Int_t cellId = fDigiPar->GetCellId(icell);
     fCellInfo =fDigiPar->GetCell(cellId);

     Int_t region = fGeoHandler->GetRegion(cellId)-1;
     Int_t module = fGeoHandler->GetCounter(cellId)-1;
     Int_t cell = fGeoHandler->GetCell(cellId)-1;
     Double_t x = fCellInfo->GetX();
     Double_t y = fCellInfo->GetY();
     Double_t z = fCellInfo->GetZ();
     Double_t dx = fCellInfo->GetSizex();
     Double_t dy = fCellInfo->GetSizey();

     // The conversions have to be done to have the same dimensions as for
     // the old parameters taken from the file.
     // X, Y, DX and DY are in mm in the code but in cm in the param container
     // Z is in cm in the code and in cm in the param container.
     // The magic value 1.42 which is subtracted is to calculate the front position of
     // the detector. Don't know exactely why this is needed. The position of the hit should be
     // in the middle of all gaps. To be fixed.
     // The dx and dy are the full size of the cell in the code. In the file the size is
     // stored as the half size of the box in cm.
     //
     X[region][module][cell] = x*10;
     Y[region][module][cell] = y*10;
     Z[region][module][cell] = (z-1.42);
     Dx[region][module][cell] = dx*20;
     Dy[region][module][cell] = dy*20;
     if (TMath::Abs(dx-dy) < 0.001) {
       type[region][module][cell] = "p";
     } else {
       type[region][module][cell] = "s";
     }
/*
      LOG(INFO)<<"X, Y, Z, Dx, Dy, type: "<<x<<" , "<<y<<" , "<<z<<" , "
         <<dx<<" , "<<dy<<" , "<<type<<" , "<<FairLogger::endl;
*/
   }
}


ClassImp(CbmTofHitProducer)

