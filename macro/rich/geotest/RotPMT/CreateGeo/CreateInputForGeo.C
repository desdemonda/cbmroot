#include <iostream>
#include <fstream>  
void CreateInputForGeo(float PMTrotX=5., float PMTrotY=5.){

  int ShiftXmod10=(int(PMTrotX*10)) % 10;  
  float IntegerXValue=PMTrotX-(float (ShiftXmod10))/10.;
  int ShiftYmod10=(int(PMTrotY*10)) % 10;
  float IntegerYValue=PMTrotY-(float (ShiftYmod10))/10.;
  char ShiftXTxt[256]; char ShiftYTxt[256];

  sprintf(ShiftXTxt,"Xpos%dpoint%d",IntegerXValue,ShiftXmod10);
  
  sprintf(ShiftYTxt,"Ypos%dpoint%d",IntegerYValue,ShiftYmod10);
  if(PMTrotY<0){sprintf(ShiftYTxt,"Yneg%dpoint%d",-1.*IntegerYValue,-1.*ShiftYmod10);}

  char InputFileName[256];
  
  sprintf(InputFileName,"/data/cbmroot/geometry/rich/GeoOpt/RotPMT/GeoFiles/VeryOldGeo/DatFiles/rich_inputfile_RotPMT_%s_%s.dat",ShiftXTxt,ShiftYTxt);
  cout<<InputFileName<<endl; //continue;
  //return;
  string line;
  std::vector<std::string> lines;
  std::ofstream outfile (InputFileName);
  //get upper part
  outfile <<"RICH barrel"<< std::endl;
  outfile <<"  z     length   dWindows   dVessel    lRadiator  radiator"<< std::endl;
  outfile <<"1800.     1900.       0.25        5.        1500.    RICHgas_CO2_dis"<< std::endl;
  outfile <<"RICH mirror"<< std::endl;
  outfile <<" radius  thickness   material   yBeamhole  angle"<< std::endl;
  outfile <<"  3000.     6.      RICHglass     200.     -1."<< std::endl;
  outfile <<"RICH photodetector (PMT)"<< std::endl;
  outfile <<"  z    y      dx      dy    angle_x  angle_y  dGlass  dCathode  lTube   dSupport"<< std::endl;
  outfile <<" 200.  1275.  1000.   300.   "<<PMTrotX<<"      "<<-1*PMTrotY<<"        0.     0.5       0.        0."<< std::endl;
  outfile << "                  " << std::endl;
  outfile.close(); 
  
  return;
}
