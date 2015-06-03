#include <iostream>
#include <fstream>  
void CreateGDMLfileNew(float PMTrotX=5, float PMTrotY=5, int RotMir=-10){

  float RICH_entrance_width = 3500;
  float RICH_exit_width = 5136;
  float RICH_height = 6000;
  float RICH_length = 1997.5;
  float entrance_width_addend = 2;

  float DeltaPMT_Width=275., DeltaPMT_Height=150.;
  float pmt_width = 1000 + DeltaPMT_Width;
  float pmt_height = 600 + DeltaPMT_Height;
  
  float RICH_mirror_angle = RotMir;

 float pmt_pos_x_addend = 0;
  float pmt_pos_y_addend = 0;
  float pmt_pos_z_addend = 0;
  // float pmt_rot_x_addend = -27.952765;
  // float pmt_rot_y_addend = -13.477;
 

  
  float RICH_mirror_Y_shift = 805;

  float DefaultRotX=32.952765; float DefaultRotY=18.477;
  if(RotMir==1){DefaultRotX=10.952765.;}
  if(RotMir==-1){DefaultRotX=14.952765.;}
  
  char RotMirText[256];
  if(RotMir<0){sprintf( RotMirText,"RotMir_m%d",RotMir*-1);}
  else{sprintf( RotMirText,"RotMir_p%d",RotMir);}
  
  char GeoFileName[256];
  // char* InFileUpper="GdmlUpperPart.txt";//[256];
  // char* InFileLower="GdmlLowerPart.txt";//[256];
  char* InFileUpper="CreateGeo/GdmlUpperPart.txt";//[256];
  char* InFileLower="CreateGeo/GdmlLowerPart.txt";//[256];
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
  outfile <<"<variable name=\"RICH_entrance_width\" value=\""<<RICH_entrance_width<<"\"/>" << std::endl;
  outfile <<"<variable name=\"RICH_exit_width\" value=\""<<RICH_exit_width<<"\"/>" << std::endl;
  outfile <<"<variable name=\"RICH_height\" value=\""<<RICH_height<<"\"/>" << std::endl;
  outfile <<"<variable name=\"RICH_length\" value=\""<<RICH_length<<"\"/>" << std::endl;
  outfile <<"<variable name=\"entrance_width_addend\" value=\""<<entrance_width_addend<<"\"/>" << std::endl;
  
  outfile <<"<variable name=\"RICH_mirror_angle\" value=\""<<RICH_mirror_angle<<"\"/>" << std::endl;
  outfile <<"<variable name=\"RICH_mirror_Y_shift\" value=\""<<RICH_mirror_Y_shift<<"\"/>" << std::endl;

  outfile <<"<variable name=\"pmt_width\" value=\""<<pmt_width<<"\"/>" << std::endl;
  outfile <<"<variable name=\"pmt_height\" value=\""<<pmt_height<<"\"/>" << std::endl;
  outfile <<"<variable name=\"pmt_pos_x_addend\" value=\""<<pmt_pos_x_addend<<"\"/>" << std::endl;
  outfile <<"<variable name=\"pmt_pos_y_addend\" value=\""<<pmt_pos_y_addend<<"\"/>" << std::endl;
  outfile <<"<variable name=\"pmt_pos_z_addend\" value=\""<<pmt_pos_z_addend<<"\"/>" << std::endl;

  outfile <<"<variable name=\"pmt_rot_x_addend\" value=\""<<PMTrotX-DefaultRotX<<"\"/>";  
  outfile << " <!-- rot x ="<< PMTrotX <<" --> " << std::endl;
  outfile <<"<variable name=\"pmt_rot_y_addend\" value=\""<<PMTrotY-DefaultRotY<<"\"/>";
  outfile << " <!-- rot y ="<< PMTrotY <<" --> " << std::endl;

 
  outfile << "                  " << std::endl;
  while ( getline (infile2,line) ){outfile << line<<std::endl;}
  // cout<<"####################################################"<<endl;
  outfile.close(); 
  
  
}
