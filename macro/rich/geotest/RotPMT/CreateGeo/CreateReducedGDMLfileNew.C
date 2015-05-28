#include <iostream>
#include <fstream>  
void CreateReducedGDMLfileNew(float PMTrotX=5, float PMTrotY=5, int RotMir=-10){

  
  float Mirror_angle = RotMir;;
  float PMT_X_addend = 0;
  float PMT_Y_addend = 0;
  float PMT_Z_addend = 0;
  float DeltaPMT_Width=275., DeltaPMT_Height=150.;
  float PMT_width = 1000 + DeltaPMT_Width;
  float PMT_height = 600 + DeltaPMT_Height;
  
  // float PMT_X_rot_addend = -32.952765;
  // float PMT_Y_rot_addend = -18.477;
  
  float DefaultRotX=32.952765; float DefaultRotY=18.477;
  
  if(RotMir==1){DefaultRotX=10.952765.;}
  if(RotMir==-1){DefaultRotX=14.952765.;}
  
  char RotMirText[256];
  if(RotMir<0){sprintf( RotMirText,"RotMir_m%d",RotMir*-1);}
  else{sprintf( RotMirText,"RotMir_p%d",RotMir);}
  
  char GeoFileName[256];
  // char* InFileUpper="GdmlUpperPart.txt";//[256];
  // char* InFileLower="GdmlLowerPart.txt";//[256];
  char* InFileUpper="CreateGeo/GdmlUpperPart_reduced.txt";//[256];
  char* InFileLower="CreateGeo/GdmlLowerPart_reduced.txt";//[256];
  //char* InFileLower="CreateGeo/GdmlLowerPart_CO2.txt";//[256];
  
  int ShiftXmod10=(int(PMTrotX*10)) % 10;  
  float IntegerXValue=PMTrotX-(float (ShiftXmod10))/10.;
  int ShiftYmod10=(int(PMTrotY*10)) % 10;
  float IntegerYValue=PMTrotY-(float (ShiftYmod10))/10.;
  
  char ShiftXTxt[256]; char ShiftYTxt[256];
  sprintf(ShiftXTxt,"Xpos%dpoint%d",IntegerXValue,ShiftXmod10);
  sprintf(ShiftYTxt,"Ypos%dpoint%d",IntegerYValue,ShiftYmod10);
  if(PMTrotY<0){sprintf(ShiftYTxt,"Yneg%dpoint%d",-1.*IntegerYValue,-1.*ShiftYmod10);}
  
  std::ifstream infile1 (InFileUpper);
  std::ifstream infile2 (InFileLower);
  //sprintf(GeoFileName,"/hera/cbm/users/tariq/cbmroot/geometry/rich/GeoOpt/RotPMT/NewGeo/rich_geo_%s_RotPMT_%s_%s.gdml",RotMirText,ShiftXTxt,ShiftYTxt);
  sprintf(GeoFileName,"/data/cbmroot/geometry/rich/GeoOpt/RotPMT/NewGeo/rich_geo_%s_RotPMT_%s_%s.gdml",RotMirText,ShiftXTxt,ShiftYTxt);
  
  cout<<GeoFileName<<endl; //continue;
  //return;
  string line;
  std::vector<std::string> lines;
  std::ofstream outfile (GeoFileName);
  //get upper part
  while ( getline (infile1,line) ){outfile << line<<std::endl;}
  
  outfile << "                  " << std::endl;
  outfile <<"<variable name=\"Mirror_angle\" value=\""<<Mirror_angle<<"\"/>" << std::endl;
  outfile <<"<variable name=\"PMT_X_addend\" value=\""<<PMT_X_addend<<"\"/>" << std::endl;
  outfile <<"<variable name=\"PMT_Y_addend\" value=\""<<PMT_Y_addend<<"\"/>" << std::endl;
  outfile <<"<variable name=\"PMT_Z_addend\" value=\""<<PMT_Z_addend<<"\"/>" << std::endl;
  outfile <<"<variable name=\"PMT_X_rot_addend\" value=\""<<PMTrotX-DefaultRotX<<"\"/>";  
  outfile << " <!-- rot x ="<< PMTrotX <<" --> " << std::endl;
  outfile <<"<variable name=\"PMT_Y_rot_addend\" value=\""<<PMTrotY-DefaultRotY<<"\"/>";
  outfile << " <!-- rot y ="<< PMTrotY <<" --> " << std::endl;

  outfile <<"<variable name=\"PMT_width\" value=\""<<PMT_width<<"\"/>" << std::endl;
  outfile <<"<variable name=\"PMT_height\" value=\""<<PMT_height<<"\"/>" << std::endl;
  outfile << "                  " << std::endl;
  while ( getline (infile2,line) ){outfile << line<<std::endl;}
  // cout<<"####################################################"<<endl;
  outfile.close(); 
  
  
}
