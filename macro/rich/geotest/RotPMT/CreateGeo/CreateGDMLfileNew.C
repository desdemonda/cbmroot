#include <iostream>
#include <fstream>  
void CreateGDMLfileNew(float PMTrotX=5, float PMTrotY=5, int RotMir=-10){
  //float DefaultRotX=34.159435; float DefaultRotY=18.477; //for rotmir=-10 degrees  and no rot pmt (0,0) (old values)
  //if(RotMir==1){ DefaultRotX=17.536; DefaultRotY=18.477; } old values as taken from -1 degrees

  float DefaultRotX=27.952765 + 5.; float DefaultRotY=13.477 +5.; int RICH_mirror_Y_shift=805;
  float pmt_pos_x_addend=0., pmt_pos_y_addend=0., pmt_pos_z_addend=0.;
  float pmt_width=1000.; float pmt_height=600;

  if(RotMir==1){
    DefaultRotX=6.418315 + 5.; DefaultRotY=13.477 + 5.; RICH_mirror_Y_shift=835;
    pmt_pos_x_addend=0.; pmt_pos_y_addend=0.; pmt_pos_z_addend=0.;
  }
  char RotMirText[256];
  if(RotMir<0){sprintf( RotMirText,"RotMir_m%d",RotMir*-1);}
  else{sprintf( RotMirText,"RotMir_p%d",RotMir);}
  
  char GeoFileName[256];
  char* InFileUpper="CreateGeo/GdmlUpperPart.txt";//[256];
  char* InFileLower="CreateGeo/GdmlLowerPart.txt";//[256];

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
  sprintf(GeoFileName,"/data/cbmroot/geometry/rich/GeoOpt/RotPMT/GeoFiles/NewGeo/rich_geo_%s_RotPMT_%s_%s.gdml",RotMirText,ShiftXTxt,ShiftYTxt);
 
  cout<<GeoFileName<<endl; //continue;
  //return;
  string line;
  std::vector<std::string> lines;
  std::ofstream outfile (GeoFileName);
  //get upper part
  while ( getline (infile1,line) ){outfile << line<<std::endl;}
  
  outfile << "                  " << std::endl;
  outfile <<"<variable name=\"RICH_mirror_Y_shift\" value=\""<<RICH_mirror_Y_shift<<"\"/>" << std::endl;
  outfile <<"<variable name=\"RICH_mirror_angle\" value=\""<<RotMir<<"\"/>" << std::endl;
  outfile <<"<variable name=\"pmt_rot_x_addend\" value=\""<<PMTrotX-DefaultRotX<<"\"/>";
  outfile << " <!-- rot x ="<< PMTrotX <<" --> " << std::endl;
  outfile <<"<variable name=\"pmt_rot_y_addend\" value=\""<<PMTrotY-DefaultRotY<<"\"/>";
  outfile << " <!-- rot y ="<< PMTrotY <<" --> " << std::endl;
  outfile <<"<variable name=\"pmt_pos_x_addend\" value=\""<<pmt_pos_x_addend<<"\"/>" << std::endl;
  outfile <<"<variable name=\"pmt_pos_y_addend\" value=\""<<pmt_pos_y_addend<<"\"/>" << std::endl;
  outfile <<"<variable name=\"pmt_pos_z_addend\" value=\""<<pmt_pos_z_addend<<"\"/>" << std::endl;
  outfile <<"<variable name=\"pmt_width\" value=\""<<pmt_width<<"\"/>" << std::endl;
  outfile <<"<variable name=\"pmt_height\" value=\""<<pmt_height<<"\"/>" << std::endl;
  outfile << "                  " << std::endl;
  while ( getline (infile2,line) ){outfile << line<<std::endl;}
  // cout<<"####################################################"<<endl;
  outfile.close(); 


}
