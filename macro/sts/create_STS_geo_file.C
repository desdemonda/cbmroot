void create_STS_geo_file(char *geometry="sts-3maps-4strips")
{

  // macro to create STS geometry description files for CBMROOT
  // from a data table

  // Johann M. Heuser, GSI
  // v1.0 29 Jul 2005
  // v1.1 06 Feb 2006  changed to write 2-digit station numbers
  // v1.2 24 May 2006  only MAPS stations 1,2,(3 if present) are 
  //                   placed in vacuum "pipevac1", otherwhise in "cave"  



  char infile[256];       // geometry data file - input
  char outfile[256];      // geometry data file for CBMROOT - output
  char logfile[256];      // log file for the conversion
  const Int_t max = 20;   // max. number of stations
  Float_t d[max];         // detector thickness [mm]
  Float_t ri[max];        // inner radius       [mm]
  Float_t ro[max];        // outer radius       [mm]
  Float_t z[max];         // z position from target [mm]
  Float_t ai[max];        // inner acceptance [deg]
  Float_t ao[max];        // outer acceptance [deg]
  char type[20][max];     // detector type
  Int_t n,nn;             // counters
  char dummy[20];         //dummy text
  Float_t area[max];      // area [cm**2]

  printf("geometry: %s\n",geometry);

  sprintf(infile,"%s.dat",geometry);
  sprintf(outfile,"%s.geo",geometry);
  sprintf(logfile,"%s.log",geometry);

  printf("input file: %s\n",infile);
  if(!fopen(infile,"r")){
    printf("... does not exist.\n");
    printf("... exit.\n\n");
  }
  else{

    printf("output file: %s\n",outfile);
    printf("log file:    %s\n",logfile);

    printf("converting ...\n");

    FILE *fin= fopen(infile,"r");
    fscanf (fin, "%s",dummy);
    fscanf (fin, "%d", &n);
    for(Int_t i=0; i<16; i++){
      fscanf (fin, "%s",dummy);
    }
    for(Int_t i=0; i<n; i++){
      fscanf (fin, "%d %s %f %f %f %f %f %f", &nn, type[i],&d[i],&z[i],
                                        &ri[i],&ro[i],&ai[i],&ao[i]);
      //printf("%d %s %f %f %f %f %f %f\n",nn,type[i],d[i],z[i],
      //                                   ri[i],ro[i],ai[i],ao[i]);
    }
    fclose(fin);


    // calculate inner and outer radius of the disk
    for(Int_t i=0; i<n; i++) {
      if(ri[i]==0 && ai[i]!=0) ri[i] = z[i]*TMath::Tan(ai[i]/180*TMath::Pi()); 
      if(ro[i]==0 && ao[i]!=0) ro[i] = z[i]*TMath::Tan(ao[i]/180*TMath::Pi()); 
      // + area
      area[i] = TMath::Pi()* (ro[i]*ro[i] - ri[i]*ri[i])/1000000.;
    }

    // print info to screen and log file
    FILE *flog= fopen(logfile,"w");

    printf("detector layout:\n");
    for(Int_t i=0; i<n; i++) {
      printf("station: %02d  %5s  z=%4.0f mm   ri=%5.1f mm   ro=%5.1f mm   d=%4.3f mm   area=%7.5f m^2\n",
	      //printf("station: %2d  %5s z=%4.0f  ri=%5.1f  ro=%5.1f  d=%3.1f  area=%7.5f\n",
	     i+1,type[i],z[i],ri[i],ro[i],d[i],area[i]);  
    }

    fprintf(flog,"input file:  %s\n",infile);
    fprintf(flog,"output file: %s\n",outfile);
    fprintf(flog,"detector layout:\n");
    for(Int_t i=0; i<n; i++) {
      fprintf(flog,"station: %02d  %5s  z=%4.0f mm   ri=%5.1f mm   ro=%5.1f mm   d=%4.3f mm   area=%7.5f m^2\n",
	     i+1,type[i],z[i],ri[i],ro[i],d[i],area[i]);  
    }
    fclose(flog);



    // output to geo file
    FILE *fout= fopen(outfile,"w");
    for(Int_t i=0; i<nn; i++) {
      fprintf(fout,"//*********************************  %5s  z=%4.0f mm\n",type[i],z[i]);

      if((i+1)<10)  fprintf(fout,"stsstation0%d\n",i+1);
      else          fprintf(fout,"stsstation%d\n",i+1);
      
      if(i<3 && strcmp(type[i],"MAPS")==0) 
	fprintf(fout,"pipevac1\n");
      else  
	fprintf(fout,"cave\n");
 

      fprintf(fout,"TUBS\n");
      fprintf(fout,"silicon\n");
      fprintf(fout,"0. 0. -%4.3f\n",d[i]/2);
      fprintf(fout,"%3.1f %3.1f\n",ri[i],ro[i]);
      fprintf(fout,"0. 0. %4.3f\n",d[i]/2);
      fprintf(fout," \n");
      fprintf(fout,"0. 0. %4.1f\n",z[i]);
      fprintf(fout,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");

    }
    fprintf(fout,"//*********************************\n");
    
    fclose(fout);

    printf("... done.\n\n");
  }

}
