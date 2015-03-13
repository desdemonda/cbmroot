#include "CbmTofCreateDigiPar.h"

#include "CbmTofCell.h"
#include "CbmTofGeoHandler.h"
#include "CbmTofDigiPar.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairLogger.h"

#include <iostream>
using std::cout;
using std::endl;

#include <algorithm>

// ---- Default constructor -------------------------------------------
CbmTofCreateDigiPar::CbmTofCreateDigiPar()
  :FairTask("TofCreateDigiPar"),
   fSMType(-1),
   fSModule(-1),
   fCounter(-1),
   fGap(-1),
   fCell(-1),
   fRegion(-1),
   fSizex(-1.),
   fSizey(-1.),
   fX(-1.),
   fY(-1.),
   fZ(-1.),
   fDetID(-1),
   fCellID(-1),
   fCellMap(),
   fCellMapIt(),
   fDigiPar(NULL),
   fGeoHandler(new CbmTofGeoHandler())
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTofCreateDigiPar::CbmTofCreateDigiPar(const char *name, const char *title)
  :FairTask(name),
   fSMType(-1),
   fSModule(-1),
   fCounter(-1),
   fGap(-1),
   fCell(-1),
   fRegion(-1),
   fSizex(-1.),
   fSizey(-1.),
   fX(-1.),
   fY(-1.),
   fZ(-1.),
   fDetID(),
   fCellID(-1),
   fCellMap(),
   fCellMapIt(),
   fDigiPar(NULL),
   fGeoHandler(new CbmTofGeoHandler())
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTofCreateDigiPar::~CbmTofCreateDigiPar()
{
}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTofCreateDigiPar::SetParContainers(){

  LOG(INFO)<<" * CbmTofCreateDigiPar:: SetParContainers() "<<FairLogger::endl;

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTofDigiPar*)
             (rtdb->getContainer("CbmTofDigiPar"));

  LOG(INFO)<<" * CbmTofCreateDigiPar:: fDigiPar "<<fDigiPar<<FairLogger::endl;

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTofCreateDigiPar::ReInit(){

  LOG(INFO)<<" * CbmTofCreateDigiPar * :: ReInit() "<<FairLogger::endl;

  // Get Base Container
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTofDigiPar*)
             (rtdb->getContainer("CbmTofDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTofCreateDigiPar::Init(){

  Int_t geoVersion = fGeoHandler->Init();

//  fModInfoMap = GetModuleInfoMap();

  LOG(INFO)<<" * CbmTofCreateDigiPar * :: Init() "<<FairLogger::endl;

  FairRootManager *ioman = FairRootManager::Instance();
  if ( ! ioman ) LOG(FATAL)<<"No FairRootManager found"<<FairLogger::endl;
  

  if (k14a == geoVersion) {
    LOG(INFO)<<"Will now create digitization parameters for root geometry."<<FairLogger::endl;
    FillCellMapRootGeometry();
  }
  if (k12b == geoVersion) {
    LOG(INFO)<<"Will now create digitization parameters for root geometry."<<FairLogger::endl;
    FillCellMapRootGeometry();
  }
  if (k07a == geoVersion) {
	LOG(INFO)<<"Will now create digitization parameters for ascii geometry."<<FairLogger::endl;
	FillCellMapAsciiGeometry();
  }

  return kSUCCESS;
}
// --------------------------------------------------------------------
void CbmTofCreateDigiPar::FinishTask(){

  LOG(INFO)<<" * CbmTofCreateDigiPar * :: FinishTask() "<<FairLogger::endl;

  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTofDigiPar*)
             (rtdb->getContainer("CbmTofDigiPar"));

  fDigiPar->print();
}

// ---- Exec ----------------------------------------------------------
void CbmTofCreateDigiPar::Exec(Option_t * option)
{
}

// --------------------------------------------------------------------
void CbmTofCreateDigiPar::FillCellMapAsciiGeometry(){
 
  // The geometry structure is treelike with cave as
  // the top node. For the TOF there is a keeping volume
  // tof1. Inside there is a region (supermodule) with cells
  // which are constructed out of glass and the active gaps.
  // To extract the gap information one has to navigate there.
  // Loop over all nodes below the top node (cave). If one of
  // the nodes contains a string tof it must be the tof keeping
  // volume. Now loop over all modules. For each module loop over
  // all cells and for each cell loop now over the parts of this cell.
  // Each cell is build out of inactive glass plates and the active gaps.
  // since we are not interested in the inactive parts store only
  // the relevant information about the gaps.
  // Example for full path to gap
  //   /cave/tof1_0/t1reg1mod_1/t1reg1cel_1/t1reg1gap_1

  Int_t nrCells = 0;
  std::vector<CbmTofCell*> cellVector;
  CbmTofCell *tofCell;


  TString TopNode = gGeoManager->GetTopNode()->GetName();
  LOG(DEBUG2)<<TopNode<<FairLogger::endl;

  // Loop over all detector systems to find tof part
  TObjArray* detSystems = gGeoManager->GetTopNode()->GetNodes();
  for (Int_t iSys = 0; iSys < detSystems->GetEntriesFast(); ++iSys) {
    TGeoNode* node = (TGeoNode*) detSystems->At(iSys);
    LOG(DEBUG2)<<node->GetName()<<FairLogger::endl;

    // Only do something useful for tof part of geometry
    // The node name contains a string "tof"
    // e.g. tof_vXXy or tof_v12b
    if (TString(node->GetName()).Contains("tof")) {
      TString TofNode = node->GetName();
      TGeoNode* keep = node;
      TObjArray* keeping = keep->GetNodes();

      cout << "Building Tof Digi Par database ... "<< endl;

      // Loop over tof keeping volume. There should be only one.
      for (Int_t ikeep = 0; ikeep < keeping->GetEntriesFast(); ikeep++) {
        TGeoNode* keepvol = (TGeoNode*) keeping->At(ikeep);
        TString KeepNode = keepvol->GetName();
        TObjArray* modarray = keepvol->GetNodes();

        // Loop over the different found modules
        for (Int_t imodule = 0; imodule < modarray->GetEntriesFast(); imodule++) {
      	  TGeoNode* module = (TGeoNode*) modarray->At(imodule);
      	  TString ModuleNode = module->GetName();
      	  TObjArray* cellarray =module->GetNodes();

      	  // Loop over all cells of each module
      	  for (Int_t icell = 0; icell < cellarray->GetEntriesFast(); icell++) {
      	    TGeoNode* cell = (TGeoNode*) cellarray->At(icell);
      	    TString CellNode = cell->GetName();
      	    TObjArray* gaparray = cell->GetNodes();

      	    // Loop over all elements belonging to one cell
      	    // Relevant are only the gaps which are the active parts of the detector
      	    for (Int_t igap = 0; igap < gaparray->GetEntriesFast(); igap++) {
      	      TGeoNode* gap = (TGeoNode*) gaparray->At(igap);
      	      TString GapNode = gap->GetName();

      	      if (GapNode.Contains("gap")) {

      	        // Construct full path name for the gap
                // Extract the necessary geometrical information and store
      	        // this information in member variables
      	        TString FullPath = "/" + TopNode + "/" + TofNode + "/" +
      	            KeepNode + "/" + ModuleNode + "/" + CellNode + "/" + GapNode;
      	        LOG(DEBUG2) <<"Path: "<<FullPath<<FairLogger::endl;
      	        FillCellInfoFromGeoHandler(FullPath);

                // Since there are 8 gaps per cell, the information for all these
      	        // gaps are stored. After all the information is available the
      	        // position of the cell is calculated as mean position of the
      	        // 8 individual gaps. The size of each cell should be the same for
      	        // all 8 gaps.

      	        fCellMapIt =  fCellMap.find(fCellID);
      	        if ( fCellMapIt == fCellMap.end() ) {
      	          // new tof cell
                  tofCell = new CbmTofCell(fCellID, fX, fY, fZ, fSizex, fSizey);
      	          cellVector.clear();
      	          cellVector.push_back(tofCell);
      	          fCellMap.insert( std::pair<Int_t, std::vector<CbmTofCell*> >(fCellID, cellVector));
      	        }
      	        else {
      	          // already existing cell
                  tofCell = new CbmTofCell(fCellID, fX, fY, fZ, fSizex, fSizey);
                  fCellMap[fCellID].push_back(tofCell);
      	        }

      	      }
      	    }
      	  }
        }
      }
    }
  }
  // Calculate the mean position for each cell and fill the tof digi parameters
  FillDigiPar();
}

void CbmTofCreateDigiPar::FillCellMapRootGeometry(){
 
  // The geometry structure is treelike with cave as
  // the top node. For the TOF there is a keeping volume
  // tof1_v<version>. <version is the geometry version which is constructed
  // from the year when this geometry was developed and a running index starting
  // with a,b .. . So tof_v12b denotes the second tof geometry version
  // developed in 2012.
  // Inside the tof keeping volumes there is one or more (supermodules)
  // with a gas box inside which contain the different counters.
  // Each of this counters in now build of a stack of glass plates
  // and active gas gaps. Interesting for the parameters are only the
  // gas gaps. Each gap is then subdivided into several detector cells.
  // To extract the gap information one has to navigate there.
  // Loop over all nodes below the top node (cave). If one of
  // the nodes contains a string tof_v it must be the tof keeping
  // volume. Now loop over all super-modules. For each super-module
  // loop over all counters. For each counter loop over all gaps and
  // for each gap loop over all cells.
  // For each cell/gap store now the relevant information.
  // Example for full path to gap
  //   /cave_0/tof_v12b_0/module_0_0/gas_box_0/counter_0/Gap_0/Cell_1


  Int_t nrCells = 0;
  std::vector<CbmTofCell*> cellVector;
  CbmTofCell *tofCell;


  TString TopNode = gGeoManager->GetTopNode()->GetName();
  LOG(DEBUG2)<<"TopNode: "<<TopNode<<FairLogger::endl;

  // Loop over all detector systems to find tof part
  TObjArray* detSystems = gGeoManager->GetTopNode()->GetNodes();
  for (Int_t iSys = 0; iSys < detSystems->GetEntriesFast(); ++iSys) {
    TGeoNode* node = (TGeoNode*) detSystems->At(iSys);
    LOG(DEBUG2)<<"Det system: "<<node->GetName()<<FairLogger::endl;

    // Only do something useful for tof_v part of geometry
    // The node name contains a string "tof_v"
    // e.g. tof_v12b
    if (TString(node->GetName()).Contains("tof")) {
      TString TofNode = node->GetName();
      LOG(DEBUG2)<<"Found tof keeping volume: "<<TofNode<<FairLogger::endl;
      TGeoNode* keep = node;
      TObjArray* modarray = keep->GetNodes();

      // Loop over the different found modules
      for (Int_t imodule = 0; imodule < modarray->GetEntriesFast(); imodule++) {
        TGeoNode* module = (TGeoNode*) modarray->At(imodule);
        TString ModuleNode = module->GetName();
        TObjArray* modpartarray =module->GetNodes();

        // Loop over the different parts of a module
        for (Int_t imodpart = 0; imodpart < modpartarray->GetEntriesFast(); imodpart++) {
          TGeoNode* modpart = (TGeoNode*) modpartarray->At(imodpart);
          TString ModPartNode = modpart->GetName();

          if (ModPartNode.Contains("gas_box")) {
            TObjArray* counterarray = modpart->GetNodes();

            // Loop over the different counters
            for (Int_t icounter = 0; icounter < counterarray->GetEntriesFast(); icounter++) {
              TGeoNode* counter = (TGeoNode*) counterarray->At(icounter);
              TString CounterNode = counter->GetName();
              TObjArray* gaparray = counter->GetNodes();

              // Loop over the different gaps
              for (Int_t igap = 0; igap < gaparray->GetEntriesFast(); igap++) {
                TGeoNode* gap = (TGeoNode*) gaparray->At(igap);
                TString GapNode = gap->GetName();
                if (GapNode.Contains("Gap")) {
                  TObjArray* cellarray = gap->GetNodes();

                  // Loop over the different cells
                  for (Int_t icell = 0; icell < cellarray->GetEntriesFast(); icell++) {
                    TGeoNode* cell = (TGeoNode*) cellarray->At(icell);
                    TString CellNode = cell->GetName();

                    // Construct full path name for the gap
                    // Extract the necessary geometrical information and store
                    // this information in member variables
                    TString FullPath = "/" + TopNode + "/" + TofNode + "/" +
                        + ModuleNode + "/" + ModPartNode
                        + "/" + CounterNode + "/" + GapNode + "/" + CellNode;
                     LOG(DEBUG2) <<"Path: "<<FullPath<<FairLogger::endl;

                    FillCellInfoFromGeoHandler(FullPath);

                    // Since there are 8 gaps per cell, the information for all these
                    // gaps are stored. After all the information is available the
                    // position of the cell is calculated as mean position of the
                    // 8 individual gaps. The size of each cell should be the same for
                    // all 8 gaps.

                    fCellMapIt =  fCellMap.find(fCellID);
                    if ( fCellMapIt == fCellMap.end() ) {
                      // new tof cell
                      tofCell = new CbmTofCell(fCellID, fX, fY, fZ, fSizex, fSizey);
                      cellVector.clear();
                      cellVector.push_back(tofCell);
                      fCellMap.insert( std::pair<Int_t, std::vector<CbmTofCell*> >(fCellID, cellVector));
                    } else {
                      // already existing cell
                      tofCell = new CbmTofCell(fCellID, fX, fY, fZ, fSizex, fSizey);
                      fCellMap[fCellID].push_back(tofCell);
                    }

                  }
                }
              }
            }
          }
        }
      }
    }
  }
  // Calculate the mean position for each cell and fill the tof digi parameters
  FillDigiPar();
}

void CbmTofCreateDigiPar::FillCellInfoFromGeoHandler(TString FullPath)
{
  // Calculate the unique detector ID including the gap information.
  // Since all gaps for a given cell are added up to one channel the
  // id and corresponding information stored in the parameter container
  // should be calculated without the gap information.

  fDetID = fGeoHandler->GetUniqueDetectorId(FullPath);

  fSMType=fGeoHandler->GetSMType(fDetID);
  fSModule=fGeoHandler->GetSModule(fDetID);
  fCounter=fGeoHandler->GetCounter(fDetID);
  fGap=fGeoHandler->GetGap(fDetID);
  fCell=fGeoHandler->GetCell(fDetID);
  fRegion=fGeoHandler->GetRegion(fDetID);

  fSizex = fGeoHandler->GetSizeX(FullPath);
  fSizey = fGeoHandler->GetSizeY(FullPath);

  fX = fGeoHandler->GetX(FullPath);
  fY = fGeoHandler->GetY(FullPath);
  fZ = fGeoHandler->GetZ(FullPath);

  LOG(DEBUG2)<<"FCI: X: "<< fX;
  LOG(DEBUG2)<<" Y: "<< fY;
  LOG(DEBUG2)<<" Z: "<< fZ;
  LOG(DEBUG2)<<" SizeX: "<< fSizex;
  LOG(DEBUG2)<<" SizeY: "<< fSizey;
  LOG(DEBUG2)<<" Region: "<< fRegion;
  LOG(DEBUG2)<<" Module: "<< fCounter;
  LOG(DEBUG2)<<" Gap: "<< fGap;
  LOG(DEBUG2)<<" Cell: "<< fCell <<FairLogger::endl;

  fCellID = fGeoHandler->GetCellId(fDetID);

  fSMType=fGeoHandler->GetSMType(fCellID);
  fSModule=fGeoHandler->GetSModule(fCellID);
  fCounter=fGeoHandler->GetCounter(fCellID);
  fGap=fGeoHandler->GetGap(fCellID);
  fCell=fGeoHandler->GetCell(fCellID);
  fRegion=fGeoHandler->GetRegion(fCellID);

  LOG(DEBUG2)<<"FCI: Cell ID: "<< Form("0x%08x",fCellID) << " detId "<< Form("0x%08x",fDetID);
  LOG(DEBUG2)<<" Region:  "<< fGeoHandler->GetRegion(fCellID); 
  LOG(DEBUG2)<<" SMTYP:   "<< fGeoHandler->GetSMType(fCellID);
  LOG(DEBUG2)<<" SModule: "<< fGeoHandler->GetSModule(fCellID);
  LOG(DEBUG2)<<" Module:  "<< fGeoHandler->GetCounter(fCellID);
  LOG(DEBUG2)<<" Gap:     "<< fGeoHandler->GetGap(fCellID);
  LOG(DEBUG2)<<" Cell: "<< fGeoHandler->GetCell(fCellID) <<FairLogger::endl;

}


void CbmTofCreateDigiPar::FillDigiPar()
{

/*
  ofstream fout;
  fout.open("output.txt");
  fout << "#####################################################################################"<<"\n";
  fout << "# Geometry for the TOF detector"<<"\n";
  fout << "# Format:"<<"\n";
  fout << "#"<<"\n";
  fout << "# Region Module Cell   type       X[mm]           Y[mm]         Dx[mm]  Dy[mm]"<<"\n";
  fout << "#####################################################################################"<<"\n";
  fout << "[TofGeoPar]"<<"\n";
*/

  Int_t Nrcells = (Int_t)fCellMap.size();
  LOG(DEBUG) <<"FillDigiPar:: Nr. of tof cells: "<<Nrcells<<FairLogger::endl;

  fDigiPar->SetNrOfCells(Nrcells);        //transfer info to DigiPar

  TArrayI *CellId  = new TArrayI(Nrcells);
  TArrayD *CellX   = new TArrayD(Nrcells);
  TArrayD *CellY   = new TArrayD(Nrcells);
  TArrayD *CellZ   = new TArrayD(Nrcells);
  TArrayD *CellDx  = new TArrayD(Nrcells);
  TArrayD *CellDy  = new TArrayD(Nrcells);

  Int_t iDigi=0;

  
  std::map<Int_t, CbmTofCell* > singleCellMap;
  CbmTofCell* singlecell; 

  for ( fCellMapIt=fCellMap.begin(); fCellMapIt!=fCellMap.end(); fCellMapIt++) {

    CellId->AddAt(fCellMapIt->first,iDigi);

    std::vector<CbmTofCell*> vcell = fCellMapIt->second;
    Int_t cellId = fCellMapIt->first;
    // sanity check
    std::vector<CbmTofCell*>::iterator vcellIt;
    Int_t id;
    Int_t oldid;
    Bool_t first=kTRUE;
    CbmTofCell* tofcell;
    Double_t x=0.;
    Double_t y=0.;
    Double_t z=0.;
    Double_t sizex=0.;
    Double_t sizey=0.;
    for ( vcellIt=vcell.begin(); vcellIt!=vcell.end(); ++vcellIt) {
      tofcell = (*vcellIt);
      id = tofcell->GetDetectorId();
      if ( id != cellId) {
        LOG(INFO)<<"id, cellId: "<<id<<" , "<< cellId<<"\n";
        LOG(INFO)<<"id and CellId differ"<<FairLogger::endl;
      }
      x+=tofcell->GetX();
      y+=tofcell->GetY();
      z+=tofcell->GetZ();
      sizex+=2.*tofcell->GetSizex(); //nh: factor 2 
      sizey+=2.*tofcell->GetSizey();
    }

    CellX->AddAt(x/vcell.size(),iDigi);
    CellY->AddAt(y/vcell.size(),iDigi);
    CellZ->AddAt(z/vcell.size(),iDigi);
    CellDx->AddAt(sizex/vcell.size(),iDigi);
    CellDy->AddAt(sizey/vcell.size(),iDigi);

    /**/
    singlecell = new CbmTofCell(cellId, x/vcell.size(), y/vcell.size(), z/vcell.size(),
        sizex/vcell.size(),sizey/vcell.size());
    singleCellMap.insert( std::pair<Int_t, CbmTofCell*>(cellId, singlecell));
    /**/

    fRegion=fGeoHandler->GetRegion(cellId);
    fSMType=fGeoHandler->GetSMType(cellId);
    fSModule=fGeoHandler->GetSModule(cellId);
    fCounter=fGeoHandler->GetCounter(cellId);
    fCell=fGeoHandler->GetCell(cellId);

    if (0) {
    cout<<"FillDigiPar "<<iDigi<<", cellId = "<<cellId
        <<", t "<< fSMType <<"  m "<< fSModule <<"  c "<< fCounter <<"  s "<< fCell
        <<"   "<< x/vcell.size()*10 <<"   "<< y/vcell.size()*10 <<"   "<< z/vcell.size()*10
        <<"   "<<sizex/vcell.size()*10 <<"   "<< sizey/vcell.size()*10<<"\n";
    }
    iDigi++;
  }


  fDigiPar->SetNrOfCells(Nrcells);
  fDigiPar->SetCellIdArray(*CellId);
  fDigiPar->SetCellXArray(*CellX);
  fDigiPar->SetCellYArray(*CellY);
  fDigiPar->SetCellZArray(*CellZ);
  fDigiPar->SetCellDxArray(*CellDx);
  fDigiPar->SetCellDyArray(*CellDy);
  fDigiPar->SetCellMap(singleCellMap);

}


ClassImp(CbmTofCreateDigiPar)
