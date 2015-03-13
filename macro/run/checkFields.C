// -------------------------------------------------------------------------------
//
// Macro to plot selected parts of magnetic field (map or B-splined) and auto save to pdf 
//
// E.Litvinenko   12/05/2006    -  file "checkFields.C" 
// last update    22/06/2006    -  field map "FieldDipole" added     
//
// usage examples:
//    .x checkFields.C ("FieldActive",   50,145,0) - map     field, centre in Z=50 sm, fixed z=145, 6 pads B(y),B(x)
//    .x checkFields.C ("FieldActive_Bs",50,20,1)  - splined field, centre in Z=50 sm, fixed y=20,  6 pads B(z),B(x)
//    .x checkFields.C ("FieldAlligator",0,20,2)   - map     field, centre in Z=0 sm,  fixed y=20,  6 pads B(z),|B|(z)
//    .x checkFields.C ("FieldDipole",    0,150,0) - map     field, centre in Z= 0 sm, fixed z=150, 6 pads B(y),B(x)
// ----------------------------------------------------------------------------------

int checkFields (const char *field_basename="field_v12a", double field_Z_origin=0, double fixed_coord=170, int flag_yx_zx_z=0)
{

  TString fieldName = field_basename;

  // ---> Regions of interest for the field
  Double_t fxmin =  -160.;    // along x axis
  Double_t fxmax =   160.;
  Double_t fymin =  -160.;    // along y axis
  Double_t fymax =   160.;
  Double_t fzmin =  -50.;    // along z axis
  Double_t fzmax =   300.;

  char big_title[200];
  char stored_pdf_filename[200];
  TString* ext_add = new TString[3];
  ext_add[0]="xy";
  ext_add[1]="xz";
  ext_add[2]="z";

  // -------  Get magnetic field  -----------------------------------------

  CbmFieldMap* field = NULL;

  //  Possible magnetic fields:

  //  field_v12a
  //  field_v12b

  if ( fieldName == "field_v12b" ) {
    field = new CbmFieldMapSym3(field_basename);
  } else if ( fieldName == "field_v12a" ) {
	field = new CbmFieldMapSym2(field_basename);
  } else { 
	out << "=====> ERROR: Field map " << fieldName << " unknown!" << endl;
	exit;
  }
  field->Init();
  field->Print();
  Int_t type = field->GetType();
  if ( type >=1 && type <= 3) 
    const char* mapFile = field->GetFileName();

  field->SetPosition(0., 0., field_Z_origin);

  sprintf(big_title,"Map:    %s      Z_magnet_center = %.0f",field_basename,field_Z_origin);    
  sprintf(stored_pdf_filename,"%s_1D_%s_%.0f_%.0f.pdf",field_basename,ext_add[flag_yx_zx_z].Data(),field_Z_origin,fixed_coord);

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

  Int_t i,j,k;
  char name[200];
  TString* nnam = new TString[3];
  nnam[0]="x";
  nnam[1]="y";
  nnam[2]="z";
  TString* nnam1 = new TString[3];
  nnam1[0]="xz";
  nnam1[1]="xy";
  nnam1[2]="xyz";
  Int_t xyz[5]={-20,-10,0,10,20};
  Int_t h_colors[5]={25,33,1,4,2};

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
		  sprintf(name,"hB%s_%s%d",nnam[i].Data(),nnam[j].Data(),xyz[k]);
		  h[k][i][j]=new TH1D(name,name,fnx,minx,maxx);
		  (h[k][i][j])->SetLineColor(h_colors[k]);
		  (h[k][i][j])->SetLineWidth(1+(k<2));
		  (h[k][i][j])->SetStats(0);
		}
	      sprintf(name,"hB%s (%s)   z=%.0f   %s=%d,%d,%d,%d,%d",nnam[i].Data(),nnam[1-j].Data(),z,nnam[j].Data(),xyz[0],
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
	      x=xyz[k];
	      po[0]=x; po[1]=y; po[2]=z;
	      field->GetFieldValue(po,BB);
	      (h[k][0][0])->SetBinContent(iy+1,BB[0]/ 10.);
	      (h[k][1][0])->SetBinContent(iy+1,BB[1]/ 10.);
	      (h[k][2][0])->SetBinContent(iy+1,BB[2]/ 10.);
	    }
	}

      // ---> Loop over x axis 
      cout << "                           ... x axis: 4-5-6" << endl;
      for (Int_t ix=0; ix<=fnx; ix++) 
	{
	  x = fxmin + Double_t(ix) * fdx;
	  for (k=0;k<5;k++)
	    {
	      y=xyz[k];
	      po[0]=x; po[1]=y; po[2]=z;
	      field->GetFieldValue(po,BB);
	      (h[k][0][1])->SetBinContent(ix+1,BB[0]/ 10.);
	      (h[k][1][1])->SetBinContent(ix+1,BB[1]/ 10.);
	      (h[k][2][1])->SetBinContent(ix+1,BB[2]/ 10.);
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
		    sprintf(name,"hB%s_%s%d",nnam[i].Data(),nnam[j1].Data(),xyz[k]);
		  else
		    sprintf(name,"hB_%d_%s%d",k,nnam[i].Data(),xyz[k]);
		  h[k][i][j]=new TH1D(name,name,fnx,minx,maxx);
		  (h[k][i][j])->SetLineColor(h_colors[k]);
		  (h[k][i][j])->SetLineWidth(1+(k<2));
		  (h[k][i][j])->SetStats(0);
		}
	      if ((flag_yx_zx_z<2)||(j==0))
		sprintf(name,"hB%s (%s)   y=%.0f   %s=%d,%d,%d,%d,%d",nnam[i].Data(),nnam[2-j1].Data(),y,nnam[j1].Data(),xyz[0],
			xyz[1],xyz[2],xyz[3],xyz[4]);
	      else
		sprintf(name,"|B%s| (%s)   y=%.0f   %s=%d,%d,%d,%d,%d",nnam1[i].Data(),nnam[2-j1].Data(),y,nnam[j1].Data(),xyz[0],
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
	      x=xyz[k];
	      po[0]=x; po[1]=y; po[2]=z;
	      field->GetFieldValue(po,BB);
	      (h[k][0][0])->SetBinContent(iz+1,BB[0]/ 10.);
	      (h[k][1][0])->SetBinContent(iz+1,BB[1]/ 10.);
	      (h[k][2][0])->SetBinContent(iz+1,BB[2]/ 10.);
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
	  cout << "                           ... x axis: 4-5-6" << endl;
	  for (Int_t ix=0; ix<=fnx; ix++) 
	    {
	      x = fxmin + Double_t(ix) * fdx;
	      for (k=0;k<5;k++)
		{
		  z=xyz[k];
		  po[0]=x; po[1]=y; po[2]=z;
		  field->GetFieldValue(po,BB);
		  (h[k][0][1])->SetBinContent(ix+1,BB[0]/ 10.);
		  (h[k][1][1])->SetBinContent(ix+1,BB[1]/ 10.);
		  (h[k][2][1])->SetBinContent(ix+1,BB[2]/ 10.);
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

   return 0; 
}


