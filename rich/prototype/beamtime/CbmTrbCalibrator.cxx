#include "CbmTrbCalibrator.h"

#include "FairLogger.h"
#include "TFile.h"
#include "TCanvas.h"
#include "CbmDrawHist.h"

CbmTrbCalibrator* CbmTrbCalibrator::fInstance = 0;

// By default the calibration tables are imported from the file
CbmTrbCalibrator::CbmTrbCalibrator()
  : TObject(),
    fCalibMode(etn_NOCALIB),
    fTablesCreated(kFALSE),
    fCalibrationPeriod(10000),
    fTRBroot(NULL),
    fInputFilename(""),
    fCalibrationDoneHisto(),
    fhLeadingFineBuffer(),
    fhLeadingFine(),
    fhCalcBinWidth(),
    fhCalBinTime(),
    fFTcounter()
{
}

CbmTrbCalibrator::~CbmTrbCalibrator()
{
}

CbmTrbCalibrator* CbmTrbCalibrator::Instance()
{
   if (!fInstance) fInstance = new CbmTrbCalibrator();
   return fInstance;
}

void CbmTrbCalibrator::Export(const char* filename)
{
/*
   TH1I* calibrationEntries = new TH1I("abc", "abc", TRB_TDC3_CHANNELS*TRB_TDC3_NUMTDC*TRB_TDC3_NUMBOARDS, 0, TRB_TDC3_CHANNELS*TRB_TDC3_NUMTDC*TRB_TDC3_NUMBOARDS);
   for (Int_t b=0; b<TRB_TDC3_NUMBOARDS; ++b) {
      for (Int_t t=0; t<TRB_TDC3_NUMTDC; ++t) {
         for (Int_t i=0; i<TRB_TDC3_CHANNELS; ++i) {
            calibrationEntries->SetBinContent(b*TRB_TDC3_NUMTDC*TRB_TDC3_CHANNELS + t*TRB_TDC3_CHANNELS + i, fFTcounter[b][t][i]);
         }
      }
   }
   DrawH1(calibrationEntries);
*/ 

   TDirectory* current = gDirectory;
   TFile* old = gFile;
   TFile* file = new TFile(filename, "RECREATE");
   
   if (file == NULL || !(file->IsOpen()))
   {
      // error
   } else {
      file->cd();

      if (fTRBroot) {
         //gROOT->GetRootFolder()->ls();
         fTRBroot->Write();
      }

      file->Close();
      current->cd();   
   }
   
   delete file;
   
   gFile = old;
   gDirectory = current;
}

// This method will overwrite the existing tables
void CbmTrbCalibrator::Import(/*const char* filename*/)
{
   LOG(INFO) << "Importing calibration tables from " << fInputFilename << FairLogger::endl;

   TDirectory* current = gDirectory;
   TFile* old = gFile;
   TFile* file = new TFile(fInputFilename, "READ");

   if (file == NULL || !(file->IsOpen()))
   {
      // error
   } else {

      TString obname;
      TFolder* TRBfolder = (TFolder*)file->Get("TRB");
      
      for (Int_t b=0; b<TRB_TDC3_NUMBOARDS; ++b) {
         obname.Form("TRB%02d", b+1);
         TFolder* perTRBfolder = (TFolder*)(TRBfolder->FindObject(obname));

         for (Int_t t=0; t<TRB_TDC3_NUMTDC; ++t) {
            obname.Form("TDC%d", t);
            TFolder* perTDCfolder = (TFolder*)(perTRBfolder->FindObject(obname));

            for (Int_t i=0; i<TRB_TDC3_CHANNELS; ++i) {
               obname.Form("FineTimeBuffer_%02d_%d_%02d", b+1, t, i);
               TH1D* curHisto = (TH1D*)(perTDCfolder->FindObject(obname));
               fhLeadingFineBuffer[b][t][i] = curHisto;

               obname.Form("FineTime_%02d_%d_%02d", b+1, t, i);
               curHisto = (TH1D*)(perTDCfolder->FindObject(obname));
               fhLeadingFine[b][t][i] = curHisto;

               obname.Form("BinWidth_%02d_%d_%02d", b+1, t, i);
               curHisto = (TH1D*)(perTDCfolder->FindObject(obname));
               fhCalcBinWidth[b][t][i] = curHisto;

               obname.Form("CalTableBinTime_%02d_%d_%02d", b+1, t, i);
               curHisto = (TH1D*)(perTDCfolder->FindObject(obname));
               fhCalBinTime[b][t][i] = curHisto;
            }

            obname.Form("CalibrationDone_%02d_%d", b+1, t);
            TH1C* curHisto2 = (TH1C*)(perTRBfolder->FindObject(obname));
            fCalibrationDoneHisto[b][t] = curHisto2;
         }
      }

   }
   
   delete file;
   
   gFile = old;
   gDirectory = current;

   fTablesCreated = kTRUE;
}

// inTRBid - raw id, looks like 0x8025    TRB - from 0 to 16 incl.
// inTDCid - raw id, looks like 0x****    TDC - from 0 to 3 incl.
// inCHid - raw id of the channel - the same as CH - number of the channel from 0 to 32 incl.
void CbmTrbCalibrator::AddFineTime(UInt_t inTRBid, UShort_t inTDCid, UShort_t inCHid, UShort_t fineTime)
{
   // for etn_NOCALIB and etn_IDEAL this method is useless


   if (fCalibMode == etn_ONLINE)
   {
      if (!fTablesCreated) GenHistos();

      UInt_t TRB = ((inTRBid >> 4) & 0x00FF) - 1;
      UShort_t TDC = (inTDCid & 0x000F);
      UShort_t CH = inCHid;

      LOG(DEBUG3) << "AddFineTime: TRB" << TRB << " TDC" << TDC << " CH" << CH << FairLogger::endl;

      fhLeadingFineBuffer[TRB][TDC][CH]->Fill(fineTime);
      fhLeadingFine[TRB][TDC][CH]->Fill(fineTime);

      fFTcounter[TRB][TDC][CH]++;
      
      if (fFTcounter[TRB][TDC][CH] >= fCalibrationPeriod
         && fCalibrationDoneHisto[TRB][TDC]->GetBinContent(CH+1) == 0)
      {
         DoCalibrate(TRB, TDC, CH);
         fFTcounter[TRB][TDC][CH] = 0;
      }
   }
}

void CbmTrbCalibrator::DoCalibrate(UShort_t TRB, UShort_t TDC, UShort_t CH)
{
   LOG(DEBUG) << "DoCalibrate: TRB"  << TRB << " TDC" << TDC << " CH" << CH << FairLogger::endl;

   TH1* hfinecal = 0;
   UInt_t sum;
   Double_t binwidth;
   Double_t binhits;
         
   hfinecal = fhLeadingFineBuffer[TRB][TDC][CH];
   sum = hfinecal->GetEntries();
   
   for (Int_t fb=0; fb<TRB_TDC3_FINEBINS; ++fb)
   {
      binwidth = 1.;
      binhits = hfinecal->GetBinContent(fb + 1);
      if (sum)
         binwidth = (TRB_TDC3_COARSEUNIT * binhits) / (Double_t)sum;
      fhCalcBinWidth[TRB][TDC][CH]->SetBinContent(fb + 1, binwidth);
      
      Double_t calbintime = 1.;
      if (fb == 0)
         calbintime = fhCalcBinWidth[TRB][TDC][CH]->GetBinContent(fb + 1) / 2;
      else
         calbintime = fhCalBinTime[TRB][TDC][CH]->GetBinContent(fb) +
                     (fhCalcBinWidth[TRB][TDC][CH]->GetBinContent(fb) + fhCalcBinWidth[TRB][TDC][CH]->GetBinContent(fb + 1)) / 2;

      fhCalBinTime[TRB][TDC][CH]->SetBinContent(fb + 1, calbintime);
   }

   fhLeadingFineBuffer[TRB][TDC][CH]->Reset();

   fCalibrationDoneHisto[TRB][TDC]->SetBinContent(CH+1, 1);
}

Double_t CbmTrbCalibrator::GetFineTimeCalibrated(UShort_t TRB, UShort_t TDC, UShort_t CH, UShort_t fineCnt)
{
   // hard-code for special channel number 0 which is a reference channel.
   // for this channel the ideal calibration is used
   if (CH == 0) return GetAlmostLinearCalibratedFT(fineCnt);

   switch (fCalibMode) {
      case etn_IMPORT:     // yes, this is correct - no break - fall through
         if (!fTablesCreated) this->Import();
      case etn_ONLINE:
         if (fCalibrationDoneHisto[TRB][TDC]->GetBinContent(CH+1) == 1)
            return GetRealCalibratedFT(TRB, TDC, CH, fineCnt);
         else
            return GetAlmostLinearCalibratedFT(fineCnt); // if the channel is not calibrated return time as if it was deally calibrated
         break;
      case etn_NOCALIB:
         return GetLinearCalibratedFT(fineCnt);
         break;
      case etn_IDEAL:
         return GetAlmostLinearCalibratedFT(fineCnt);
         break;
      case etn_NOFINE:
         return 0.;
         break;
   }
}

// etn_NOCALIB
Double_t CbmTrbCalibrator::GetLinearCalibratedFT(UShort_t fineCnt)
{
   return (Double_t)fineCnt * 5. / 512.; // counter * 5 ns / 512.  512 because there are 10 bits for fine time counter.
}

// etn_IDEAL
Double_t CbmTrbCalibrator::GetAlmostLinearCalibratedFT(UShort_t fineCnt)
{
   UShort_t tailSize = 30;

   if (fineCnt < tailSize) {    // Left tail - 0 ns
      return 0.;
   } else if (fineCnt > 512-tailSize) {     // Right tail - 5 ns
      return 5.;
   } else {                   // Middle - linear function
      return (Double_t)(fineCnt-tailSize) * 5. / (512. - 2.*tailSize);
   }
}

// etn_IMPORT || etn_ONLINE
Double_t CbmTrbCalibrator::GetRealCalibratedFT(UShort_t TRB, UShort_t TDC, UShort_t CH, UShort_t fineCnt)
{
   return fhCalBinTime[TRB][TDC][CH]->GetBinContent(fineCnt+1);
}

void CbmTrbCalibrator::GenHistos()
{
   TString obname;
   TString obtitle;
   TString dirname;

   UInt_t tbins = TRB_TDC3_FINEBINS;
   Int_t trange = TRB_TDC3_FINEBINS;

   fTRBroot = gROOT->GetRootFolder()->AddFolder("TRB", "TRB root folder");
   gROOT->GetListOfBrowsables()->Add(fTRBroot, "TRB");

   TFolder* TRBfolders[TRB_TDC3_NUMBOARDS];
   TFolder* TDCfolders[TRB_TDC3_NUMBOARDS][TRB_TDC3_NUMTDC];

   // TRBs are numerated starting from 1, but the arrays of histos are from 0 (as in any C++ code)
   // TDCs are numerated from 0. One TRB has TDCs {0,1,2,3}.
   // Channels are numerated from 0 and there are total 33 channels for each TDC. 0-th channel is the reference channel.

   for (Int_t b=0; b<TRB_TDC3_NUMBOARDS; ++b) {
   
      obname.Form("TRB%02d", b+1);
      obtitle.Form("TRB%02d", b+1);
      TRBfolders[b] = fTRBroot->AddFolder(obname, obtitle);
      
      for (Int_t t=0; t<TRB_TDC3_NUMTDC; ++t) {
      
         obname.Form("TDC%d", t);
         obtitle.Form("TDC%d", t);
         TDCfolders[b][t] = TRBfolders[b]->AddFolder(obname, obtitle);
      
         for (Int_t i=0; i<TRB_TDC3_CHANNELS; ++i) {

            obname.Form("FineTimeBuffer_%02d_%d_%02d", b+1, t, i);
            obtitle.Form("Fine time Leading edge TRB %d TDC %d Channel %02d (calibration buffer)", b+1, t, i);
            fhLeadingFineBuffer[b][t][i] = new TH1D(obname.Data(), obtitle.Data(), tbins, 0, trange);
            TDCfolders[b][t]->Add(fhLeadingFineBuffer[b][t][i]);

            obname.Form("FineTime_%02d_%d_%02d", b+1, t, i);
            obtitle.Form("Fine time Leading edge TRB %02d TDC %02d Channel %02d", b+1, t, i);
            fhLeadingFine[b][t][i] = new TH1D(obname.Data(), obtitle.Data(), tbins, 0, trange);
            TDCfolders[b][t]->Add(fhLeadingFine[b][t][i]);

            obname.Form("BinWidth_%02d_%d_%02d", b+1, t, i);
            obtitle.Form("Binwidth Histogram of TRB %02d TDC %d Channel %02d", b+1, t, i);
            fhCalcBinWidth[b][t][i] = new TH1D(obname.Data(), obtitle.Data(), TRB_TDC3_FINEBINS, 0, TRB_TDC3_FINEBINS);

            //! Reset time bin correction arrays
            for (int fb=0; fb<TRB_TDC3_FINEBINS; ++fb)
               fhCalcBinWidth[b][t][i]->SetBinContent(fb + 1, 1.0);

            TDCfolders[b][t]->Add(fhCalcBinWidth[b][t][i]);

            obname.Form("CalTableBinTime_%02d_%d_%02d", b+1, t, i);
            obtitle.Form("Calibration Table of TRB %02d TDC %d Channel %02d", b+1, t, i);
            fhCalBinTime[b][t][i] = new TH1D(obname.Data(), obtitle.Data(), TRB_TDC3_FINEBINS, 0, TRB_TDC3_FINEBINS);
            
            //! Reset time bin correction arrays
            for (int fb=0; fb<TRB_TDC3_FINEBINS; ++fb)
               fhCalBinTime[b][t][i]->SetBinContent(fb + 1, (Double_t)(TRB_TDC3_COARSEUNIT * fb) / (Double_t)TRB_TDC3_FINEBINS);

            TDCfolders[b][t]->Add(fhCalBinTime[b][t][i]);

         } // Channels
         
         obname.Form("CalibrationDone_%02d_%d", b+1, t);
         obtitle.Form("Flag indicating that calibration is done for TRB %02d TDC %d", b+1, t);
         fCalibrationDoneHisto[b][t] = new TH1C(obname.Data(), obtitle.Data(), TRB_TDC3_CHANNELS, 0, TRB_TDC3_CHANNELS);
         
         TRBfolders[b]->Add(fCalibrationDoneHisto[b][t]);
         
      } // TDCs
   } // TRBs

   LOG(DEBUG) << "Histos created." << FairLogger::endl;
   fTablesCreated = kTRUE;

} // end of method

void CbmTrbCalibrator::Draw()
{
   TCanvas* c[4];
   for (int t=0; t<4; t++)
   {
      TString obname;
      obname.Form("Channels calibration flags of TDC%d of all TRBs", t);
      c[t] = new TCanvas(obname, obname, 1000, 1000);
      c[t]->Divide(4, 4);
      for (int b=0; b<16; b++) {
         c[t]->cd(b+1);
         DrawH1(fCalibrationDoneHisto[b][t]);
      }
   }
}

ClassImp(CbmTrbCalibrator)
