int checkFields (const char *field_basename, double field_Z_origin=0, 
		 double fixed_coord=170, int flag_yx_zx_z=0, double xyz_shift=0)
{
  // -------------------------------------------------------------------------------
  //
  // Macro to plot selected parts of magnetic field (map or B-splined) and auto save to pdf 
  //
  // E.Litvinenko   12/05/2006    -  file "checkFields.C"
  // last update - litvinen  08/07/2009      
  //
  // usage examples:
  //    .x checkFields.C ("FieldSC_16x13",   50,145,0)  - map     field, centre in Z=50 sm, fixed z=145, 6 pads B(y),B(x)
  //    .x checkFields.C ("FieldActive_Bs",50,0,1)      - splined field, centre in Z=50 sm, fixed y=0,   6 pads B(z),B(x)
  //    .x checkFields.C ("FieldSCmuon_16x13",0,20,2)   - map     field, centre in Z=0 sm,  fixed y=20,  6 pads B(z),|B|(z)
  // ----------------------------------------------------------------------------------


  TString fieldName = field_basename;

  // ---> Regions of interest for the field
  Double_t fxmin =  -160.;    // along x axis
  Double_t fxmax =   160.;
  Double_t fymin =  -160.;    // along y axis
  Double_t fymax =   160.;
  Double_t fzmin =  -50.;    // along z axis
  Double_t fzmax =   300.;

  char big_title[200],stored_pdf_filename[200],*ext_add[3]={"xy","xz","z"};

  
  // -----   Load libraries   ---------------------------------------------
  // Load libraries
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libPostscript.so");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
 


  // -------  Get magnetic field  -----------------------------------------

  CbmFieldMap* field = NULL;

  //  Possible magnetic fields:

  //  FieldActive                     - old default magnetic field map               
  //  FieldIron                       - not good    (May2006 -  Mar2007)        
  //      FieldAlligator_v04          - former  FIELD.v04_pavel
  //      FieldAlligator              - former  FIELD.v05_pavel                
  //  FieldActive_Bs                  - B-splined default magnetic field         
  //      FieldDipole                 - field map with angular acceptance 17 deg   
  //  FieldMuonMagnet              - magnetic field map for muon mode (default)                            (Sep2006)   
  //      FieldHera                    - field map for HERA-B magnet             
  //      FieldHeraS                   - field map for updated HERA-B magnet      
  //      FieldHermes                 - field map for HERMES magnet            
  //     FieldSC                   - magnetic field map for muon mode with gap (in center) 1520x1200 mm^2  (Mar2009)
  //     FieldSCmuon_16x13         - magnetic field map for muon mode with gap (in center) 1600x1300 mm^2  (Jul2009)
  //     FieldSC_16x13             - magnetic field map for rich mode with gap (in center) 1600x1300 mm^2  (Jul2009)

  if ( fieldName == "FieldActive" || fieldName == "FieldIron" || fieldName == "FieldMuonMagnet") 
    {
    field = new CbmFieldMapSym3(field_basename);
    }
  else 
    {   

      if ( fieldName == "FieldSC_16x13" || fieldName == "FieldSCmuon_16x13" || fieldName == "FieldSC"
// 	    || fieldName == "FieldAlligator" ||  fieldName == "FieldDipole" || fieldName == "FieldHera" || 
// 	   fieldName == "FieldHeraP" || fieldName == "FieldHeraS" || fieldName == "FieldHermes"  
	   )
	field = new CbmFieldMapSym2(field_basename);
      else 
	{
	  if ( fieldName == "FieldActive_Bs"  )
	    field = new CbmBsField(field_basename);
	  else 
	    {
	      if ( fieldName == "FieldProtvino" || fieldName == "FieldV04Pavel" ) 
		field = new CbmFieldMap(fieldName.Data());
	      else 
		{
		  cout << "=====> ERROR: Field map " << fieldName << " unknown!" << endl;
		  exit;
		}
	    }
	}
    }
  field->Init();
  field->Print();
  Int_t type = field->GetType();
  if ( type >=1 && type <= 3) 
    const char* mapFile = field->GetFileName();

  field->SetPosition(0., 0., field_Z_origin);

  sprintf(big_title,"Map:    %s      Z_magnet_center = %.0f",field_basename,field_Z_origin);    
  sprintf(stored_pdf_filename,"%s_1D_%s_%.0f_%.0f\.pdf",field_basename,ext_add[flag_yx_zx_z],field_Z_origin,fixed_coord);

  // ----------------------------------------------------------------------

  Int_t fnx = TMath::Nint( (fxmax-fxmin) * 3. );
  Int_t fny = TMath::Nint( (fymax-fymin) * 3. );
  Int_t fnz = TMath::Nint( (fzmax-fzmin) * 3. );
  Int_t fmx = TMath::Nint( (fxmax-fxmin) * 3. / 10. );
  Int_t fmy = TMath::Nint( (fymax-fymin) * 3. / 10. );

  // ----------------------------------------------------------------------

      TH1D *h[5][3][2];

  // ----------------------------------------------------------------------

  cout << endl;
  cout << "=====>  Filling histograms..." << endl;

  Double_t x, y, z, minx,maxx;
  Double_t bx, by, bz, bt, b,   po[3], BB[3];
  Double_t fdx = (fxmax-fxmin) / Double_t(fnx);
  Double_t fdy = (fymax-fymin) / Double_t(fny);
  Double_t fdz = (fzmax-fzmin) / Double_t(fnz);

  Int_t i,j,k, bin;
  char name[200],*nnam[3]={"x","y","z"},*nnam1[3]={"xz","xy","xyz"};
  Int_t xyz[5]={-20+xyz_shift,-10+xyz_shift,0+xyz_shift,10+xyz_shift,20+xyz_shift};
  Int_t h_colors[5]={kGray+2, kCyan, kBlack, kBlue, kRed} ; // 25,33,1,4,2};

  if (!flag_yx_zx_z)               // "yx"
    {
      z = fixed_coord;
      for (j=0;j<2;j++)
	{
	  if (!j)
	    {
	      minx=fymin;
	      maxx=fymax;
	    }
	  else
	    {
	      minx=fxmin;
	      maxx=fxmax;
	    }
	  for (i=0;i<3;i++)
	    {
	      for (k=0;k<5;k++)
		{
		  sprintf(name,"hB%s_%s%d",nnam[i],nnam[j],xyz[k]);
		  h[k][i][j]=new TH1D(name,name,fnx,minx,maxx);
		  (h[k][i][j])->SetLineColor(h_colors[k]);
		  (h[k][i][j])->SetLineWidth(1+(k<2));
		  (h[k][i][j])->SetStats(0);
		}
	      sprintf(name,"hB%s (%s)   z=%.0f   %s=%d,%d,%d,%d,%d",nnam[i],nnam[1-j],z,nnam[j],xyz[0],
		      xyz[1],xyz[2],xyz[3],xyz[4]);
	      (h[0][i][j])->SetTitle(name);
	    }
	}

      // ---> Loop over y axis 
      cout << "                           ... y axis: 1-2-3" << endl;
      for (Int_t iy=0; iy<=fny; iy++) 
	{
	  y = fymin + Double_t(iy) * fdy;
	  for (k=0;k<5;k++)
	    {
	      bin=iy; // (h[k][0][0])->FindBin(y);
	      x=xyz[k];
	      po[0]=x; po[1]=y; po[2]=z;
	      field->GetFieldValue(po,BB);
	      (h[k][0][0])->SetBinContent(bin,BB[0]/ 10.);  //  iy+1
	      (h[k][1][0])->SetBinContent(bin,BB[1]/ 10.); //  iy+1
	      (h[k][2][0])->SetBinContent(bin,BB[2]/ 10.); //  iy+1
	    }
	}

      // ---> Loop over x axis 
      cout << "                           ... x axis: 4-5-6   "  << fxmin << " " << fdx << " " << fxmax << endl;
      for (Int_t ix=0; ix<=fnx; ix++) 
	{
	  x = fxmin + Double_t(ix) * fdx;
	  for (k=0;k<5;k++)
	    {
	      bin=ix; //(h[k][0][1])->FindBin(x);
	      y=xyz[k];
	      po[0]=x; po[1]=y; po[2]=z;
	      field->GetFieldValue(po,BB);
	      (h[k][0][1])->SetBinContent(bin,BB[0]/ 10.);  // ix+1
	      (h[k][1][1])->SetBinContent(bin,BB[1]/ 10.);  // ix+1
	      (h[k][2][1])->SetBinContent(bin,BB[2]/ 10.);  // ix+1
	    }
	}
    }
  else              // "zx" or "z"
    {
      y = fixed_coord;
      Int_t j1;
      for (j=0;j<2;j++)
	{
	  if (!j)
	    {
	      minx=fzmin;
	      maxx=fzmax;
	      j1=0;
	    }
	  else
	    {
	      if (flag_yx_zx_z<2)   // "zx"
		{
		  minx=fxmin;
		  maxx=fxmax;
		  j1=2;
		}
	    }
	  for (i=0;i<3;i++)
	    {
	      for (k=0;k<5;k++)
		{
		  if ((flag_yx_zx_z<2)||(j==0))
		    sprintf(name,"hB%s_%s%d",nnam[i],nnam[j1],xyz[k]);
		  else
		    sprintf(name,"hB_%d_%s%d",k,nnam[i],xyz[k]);
		  h[k][i][j]=new TH1D(name,name,fnx,minx,maxx);
		  (h[k][i][j])->SetLineColor(h_colors[k]);
		  (h[k][i][j])->SetLineWidth(1+1*((k>0)&&(k<3)));
		  (h[k][i][j])->SetStats(0);
		}
	      if ((flag_yx_zx_z<2)||(j==0))
		sprintf(name,"hB%s (%s)   y=%.0f   %s=%d,%d,%d,%d,%d",nnam[i],nnam[2-j1],y,nnam[j1],xyz[0],
			xyz[1],xyz[2],xyz[3],xyz[4]);
	      else
		sprintf(name,"|B%s| (%s)   y=%.0f   %s=%d,%d,%d,%d,%d",nnam1[i],nnam[2-j1],y,nnam[j1],xyz[0],
			xyz[1],xyz[2],xyz[3],xyz[4]);
	      (h[0][i][j])->SetTitle(name);
	    }
	}

      // ---> Loop over z axis 
      cout << "                           ... z axis: 1-2-3" << endl;
      for (Int_t iz=0; iz<=fnz; iz++) 
	{
	  z = fzmin + Double_t(iz) * fdz;
	  for (k=0;k<5;k++)
	    {
	      bin=iz; // (h[k][0][0])->FindBin(z);
	      x=xyz[k];
	      po[0]=x; po[1]=y; po[2]=z;
	      field->GetFieldValue(po,BB);
	      (h[k][0][0])->SetBinContent(bin,BB[0]/ 10.);  // iz+1
	      (h[k][1][0])->SetBinContent(bin,BB[1]/ 10.);  // iz+1
	      (h[k][2][0])->SetBinContent(bin,BB[2]/ 10.);  // iz+1
	      if (flag_yx_zx_z>=2)   // "z"
		{
		  bx = BB[0]/ 10.; by = BB[1]/ 10.; bz = BB[2]/ 10.;
		  bt = TMath::Sqrt(bx*bx + bz*bz);
		  (h[k][0][1])->SetBinContent(iz+1,bt);
		  bt = TMath::Sqrt(bx*bx + by*by);
		  (h[k][1][1])->SetBinContent(iz+1,bt);
		  bt = TMath::Sqrt(bx*bx + by*by + bz*bz);
		  (h[k][2][1])->SetBinContent(iz+1,bt);
		}
	    }
	}

      if (flag_yx_zx_z<2)   // "zx"
	{
	  // ---> Loop over x axis 
	  //	  cout << "                           ... x axis: 4-5-6" << endl;
      cout << "                           ... x axis: 4-5-6   "  << fxmin << " " << fdx << " " << fxmax << endl;
	  for (Int_t ix=0; ix<=fnx; ix++) 
	    {
	      x = fxmin + Double_t(ix) * fdx;
	      for (k=0;k<5;k++)
		{
		  bin=ix; // (h[k][0][1])->FindBin(x);
		  z=xyz[k];
		  po[0]=x; po[1]=y; po[2]=z;
		  field->GetFieldValue(po,BB);
		  (h[k][0][1])->SetBinContent(bin,BB[0]/ 10.);  // ix+1 
		  (h[k][1][1])->SetBinContent(bin,BB[1]/ 10.);  // ix+1 
		  (h[k][2][1])->SetBinContent(bin,BB[2]/ 10.);  // ix+1 
		}
	    }
	}
    }


  // -------  Draw    ---------------------------------

  cout << endl << "=====>  Drawing..." << endl;

  gStyle->SetOptStat(0000);
  gStyle->SetTitleBorderSize(0.);
  gStyle->SetPadGridX(1);
  gStyle->SetPadGridY(1);

  TCanvas *master = new TCanvas("master", field_basename,9,32,1198,833);
  master->cd();

  TPad *padinf = new TPad("padinf", "",0.134855,0.92093,0.855809,0.989147);
  padinf->Draw();

  TPad* c1 = new TPad("c1", "",0.00622407,0.020155,0.993776,0.913178);
  c1->Draw();
  c1->cd();

  c1->Divide(3,2);

  Double_t max, min;

  for (Int_t kpad=0;kpad<6;kpad++)
    {
      c1->cd(kpad+1);
      gPad->SetTopMargin(0.12);
      (h[0][kpad%3][(kpad>=3)])->Draw();
      maxx=(h[0][kpad%3][(kpad>=3)])->GetMaximum();
      minx=(h[0][kpad%3][(kpad>=3)])->GetMinimum();
      for (k=1;k<5;k++)
	{
	  if (((h[k][kpad%3][(kpad>=3)])->GetMaximum())>maxx)
	    {
	      maxx = (h[k][kpad%3][(kpad>=3)])->GetMaximum();
	      maxx=maxx+0.05*fabs(maxx);
	      (h[0][kpad%3][(kpad>=3)])->SetMaximum(maxx);
	    }
	  if (((h[k][kpad%3][(kpad>=3)])->GetMinimum())<minx)
	    {
	      minx = (h[k][kpad%3][(kpad>=3)])->GetMinimum();
	      minx=minx-0.05*fabs(minx);
	      (h[0][kpad%3][(kpad>=3)])->SetMinimum(minx);
	    }
	  (h[k][kpad%3][(kpad>=3)])->Draw("same");
	}
      gPad->BuildLegend();
    }

   padinf->cd();
   TPaveText *pt = new TPaveText(0.0991096,0.111849,0.89981,0.909192,"br");
   pt->SetBorderSize(0);
   pt->SetFillColor(19);
   pt->SetTextSize(0.358974);
   TText *text = pt->AddText(big_title);
   pt->Draw();
   padinf->Modified();

   master->SaveAs(stored_pdf_filename);

   return 1; 
}


