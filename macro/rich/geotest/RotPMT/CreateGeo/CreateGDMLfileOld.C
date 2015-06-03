#include <iostream>
#include <fstream>  
void CreateGDMLfileOld(float PMTrotX=5, float PMTrotY=-5, int RotMir=-10){//int RotMir=-10){
  float DefaultRotX=34.159435;
  float DefaultRotY=18.477;
  if(RotMir==-1){
    DefaultRotX=17.536;
    DefaultRotY=18.477;
  }
  if(RotMir==1){
    DefaultRotX=17.536;
    DefaultRotY=18.477;
  }
  char RotMirText[256];
  if(RotMir<0){sprintf( RotMirText,"RotMir_m%d",RotMir*-1);}
  else{sprintf( RotMirText,"RotMir_p%d",RotMir);}
  
  
  char GeoFileName[256];
  char MirrText[256];
  if(RotMir<0){sprintf( MirrText,"RotMir_m%d",RotMir*-1);}
  else{sprintf( MirrText,"RotMir_p%d",RotMir);}
  
  //for(int ShiftX=0; ShiftX<Dimx; ShiftX++){
  int ShiftXmod10=(int(PMTrotX*10)) % 10;
  
  float IntegerXValue=PMTrotX-(float (ShiftXmod10))/10.;
  //for(int ShiftY=0; ShiftY<Dimy; ShiftY++){
  int ShiftYmod10=(int(PMTrotY*10)) % 10;
  float IntegerYValue=PMTrotY-(float (ShiftYmod10))/10.;
  
  char ShiftXTxt[256]; char ShiftYTxt[256];
  sprintf(ShiftXTxt,"Xpos%dpoint%d",IntegerXValue,ShiftXmod10);
  sprintf(ShiftYTxt,"Ypos%dpoint%d",IntegerYValue,ShiftYmod10);
  if(PMTrotY<0){sprintf(ShiftYTxt,"Yneg%dpoint%d",-1.*IntegerYValue,-1.*ShiftYmod10);}

  char InFileUpper[256];
  if(RotMir<0){sprintf(InFileUpper,"CreateGeo/Upper_RotMir_m%d.txt",RotMir*-1);}
  if(RotMir==1){sprintf(InFileUpper,"CreateGeo/Upper_RotMir_p%d.txt",RotMir);}
  std::ifstream infile1 (InFileUpper);
  std::ifstream infile2 ("CreateGeo/Lower.txt");
  sprintf(GeoFileName,"/hera/cbm/users/tariq/cbmroot/geometry/rich/GeoOpt/RotPMT/OldGeo/rich_geo_%s_RotPMT_%s_%s.gdml",RotMirText,ShiftXTxt,ShiftYTxt);
  
  cout<<GeoFileName<<endl; 
  //return;
  //continue;
  string line;
  std::vector<std::string> lines;
  std::ofstream outfile (GeoFileName);
  //get upper part
  while ( getline (infile1,line) ){outfile << line<<std::endl;}
  
  outfile << "                  " << std::endl;
  outfile << "                  " << std::endl;
  outfile <<"<variable name=\"pmt_rot_x_addend\" value=\""<<PMTrotX-DefaultRotX<<"\"/>";
  outfile << " <!-- rot x ="<< PMTrotX <<" --> " << std::endl;

  outfile <<"<variable name=\"pmt_rot_y_addend\" value=\""<<PMTrotY-DefaultRotY<<"\"/>";
  outfile << " <!-- rot y ="<< PMTrotY <<" --> " << std::endl;
  
  //<<std::endl;
  outfile << "                  " << std::endl; 
  outfile << "                  " << std::endl;
  while ( getline (infile2,line) ){outfile << line<<std::endl;}
  // cout<<"####################################################"<<endl;
  outfile.close();

 
 
}
//gSystem->Load("libSts"); Exec("ls ..... ")

/*
#include <stdio.h>

int main ()
{
  char str [80];
  float f;
  FILE * pFile;

  pFile = fopen ("myfile.txt","w+");
  fprintf (pFile, "%f %s", 3.1416, "PI");
  rewind (pFile);
  fscanf (pFile, "%f", &f);
  fscanf (pFile, "%s", str);
  fclose (pFile);
  printf ("I have read: %f and %s \n",f,str);
  return 0;
}
*/
