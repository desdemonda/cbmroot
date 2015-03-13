/**
 * Prints out some details about segmentation.
 *
 * @author M.Ryzhinskiy m.ryzhinskiy@gsi.de
 * @param digiFile  Input file containing segmentation parameters
 */
void much_seg_info(TString digiFile = ""){

  TString inputdir = gSystem->Getenv("VMCWORKDIR");
  if (digiFile == "") {
    digiFile = inputdir + "/macro/much/data/much_digi.root";
  }

  // ----  Load libraries   -------------------------------------------------
  cout << endl << "=== much_seg_info.C : Loading libraries ..." << endl;
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libMuch");
  // -----------------------------------------------------------------------

  TFile* f = new TFile(digiFile.Data());
  f->ls();
  Int_t nPadsTotal = 0;
  Int_t nSectorsTotal = 0;
  TObjArray* stations = (TObjArray*) f->Get("stations");

  printf("\n\n\n---------------------------------------------------------------------\n");
  for (Int_t st=0;st<stations->GetEntriesFast();st++){
    Int_t nSectors = 0;
    Int_t nPads = 0;
    TObjArray* sectors = new TObjArray();
    CbmMuchStation* station = (CbmMuchStation*) stations->At(st);
    for(Int_t iLayer=0; iLayer < station->GetNLayers(); ++iLayer){
      CbmMuchLayer* layer = station->GetLayer(iLayer);
      for (Int_t s=1;s>=0;s--){
        CbmMuchLayerSide* side = layer->GetSide(s);
        for (Int_t m=0;m<side->GetNModules();m++) {
          CbmMuchModuleGem* module = (CbmMuchModuleGem*)side->GetModule(m);
          nSectors += module->GetNSectors();
          for (Int_t j=0;j<module->GetNSectors();j++){
            CbmMuchSector* sector = module->GetSector(j);
            nPads += sector->GetNChannels();

            Int_t sec_nX = sector->GetPosition()[0] < 0 ? -1 : 1;
            Int_t sec_nY = sector->GetPosition()[1] < 0 ? -1 : 1;
            Double_t secLx = sector->GetSize()[0];
            Double_t secLy = sector->GetSize()[1];

            // Position of the corner closest to the center of the layer
            Double_t secX = sector->GetPosition()[0] - sec_nX*secLx/2.;
            Double_t secY = sector->GetPosition()[1] - sec_nY*secLy/2.;

            // Reject incomplete sectors by size
            Double_t less = TMath::Min(secLx, secLy);
            Double_t more = TMath::Max(secLx, secLy);
            Int_t nReal = Int_t((more+1e-5)/less);
            Int_t nPow = Int_t(TMath::Log2(nReal) + 1e-2);
            Double_t more1 = less*TMath::Power(2,nPow);
            if(TMath::Abs(more1-more ) > 1e-5) continue;

            // Reject incomplete sectors by number of channels
            if(sector->GetNChannels() < module->GetNSectorChannels()) continue;

            // Find if the sector of such size already in the list
            Bool_t exists = false;
            Int_t i;
            CbmMuchSector* sec = NULL;
            for(i=0; i<sectors->GetEntries(); ++i){
              sec = (CbmMuchSector*) sectors->At(i);
              Double_t lx = sec->GetSize()[0];
              Double_t ly = sec->GetSize()[1];
              if(TMath::Abs(lx-secLx) < 1e-5 && TMath::Abs(ly-secLy) < 1e-5){
                exists = true;
                break;
              }
            }
            if(exists){
              Int_t nX = sec->GetPosition()[0] < 0 ? -1 : 1;
              Int_t nY = sec->GetPosition()[1] < 0 ? -1 : 1;
              Double_t x = sec->GetPosition()[0] - nX*sec->GetSize()[0]/2.;
              Double_t y = sec->GetPosition()[1] - nY*sec->GetSize()[1]/2.;
              Double_t rad = TMath::Sqrt(x*x + y*y);
              Double_t secRad = TMath::Sqrt(secX*secX + secY*secY);
              if(rad < secRad) sectors->AddAt(sector, i);
            }
            else {
              sectors->Add(sector);
            }
          } // sectors
        } // modules
      } // sides
    } // layers

    nPadsTotal += nPads;
    nSectorsTotal += nSectors;

    // Sort sectors by their radii
    for(Int_t i=0; i<sectors->GetEntries(); ++i){
      for(Int_t j=0; j<sectors->GetEntries() - 1; ++j){
        CbmMuchSector* sec1 = (CbmMuchSector*)sectors->At(j);
        Int_t nX1 = sec1->GetPosition()[0] < 0 ? -1 : 1;
        Int_t nY1 = sec1->GetPosition()[1] < 0 ? -1 : 1;
        Double_t x1 = sec1->GetPosition()[0] - nX1*sec1->GetSize()[0]/2.;
        Double_t y1 = sec1->GetPosition()[1] - nY1*sec1->GetSize()[1]/2.;
        Double_t rad1 = TMath::Sqrt(x1*x1 + y1*y1);
        CbmMuchSector* sec2 = (CbmMuchSector*)sectors->At(j+1);
        Int_t nX2 = sec2->GetPosition()[0] < 0 ? -1 : 1;
        Int_t nY2 = sec2->GetPosition()[1] < 0 ? -1 : 1;
        Double_t x2 = sec2->GetPosition()[0] - nX2*sec2->GetSize()[0]/2.;
        Double_t y2 = sec2->GetPosition()[1] - nY2*sec2->GetSize()[1]/2.;
        Double_t rad2 = TMath::Sqrt(x2*x2 + y2*y2);
        if(rad1 > rad2) {
          sectors->AddAt(sec1, j+1);
          sectors->AddAt(sec2, j);
        }
      }
    }

    // Print output information
    printf("Station %i:\n", st);
    for(Int_t i=0; i<sectors->GetEntries(); ++i){
      CbmMuchSector* sec = (CbmMuchSector*) sectors->At(i);
      Int_t nX = sec->GetPosition()[0] < 0 ? -1 : 1;
      Int_t nY = sec->GetPosition()[1] < 0 ? -1 : 1;
      Double_t x = sec->GetPosition()[0] - nX*sec->GetSize()[0]/2.;
      Double_t y = sec->GetPosition()[1] - nY*sec->GetSize()[1]/2.;
      Double_t rad = TMath::Sqrt(x*x + y*y);
      printf("   Region %i:\n", i);
      printf("      radius: %6.3f [cm]:\n", rad);
      printf("      sector size: %4.2fx%4.2f [cm^2]\n", sec->GetSize()[0], sec->GetSize()[1]);
      printf("      pad size: %4.2fx%4.2f [cm^2]\n", sec->GetDx(), sec->GetDy());
    }
    printf("   Number of sectors: %i\n", nSectors);
    printf("   Number of channels: %i\n", nPads);
    printf("---------------------------------------------------------------------\n");
  }//stations
  printf("Total number of sectors: %i\n", nSectorsTotal);
  printf("Total number of channels: %i\n", nPadsTotal);
}
