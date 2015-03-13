//Generator for CbmTof Geometry
//Update 01/10/2012 nh
// 08/10/2012 add sensitive gas gaps 

#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "math.h"
#define PI 3.14159265

FILE *geof;
FILE *info;
FILE *parf;
using namespace std;

void TOFSMs(int, float, float, float, float);
void TOFSMo(int, float, float, float, float);
void TofPole(int, float, float, float);
void TOFBox(int, float, float, float, float, float, float);
int  TOFRegion(int, int, float, float, float, float, float, float, char *, int, float, float, 
float, float, float&, float&);

int main(void)
{
  //----------Initialization---------------------------------------
  const char* c_str();
  float B_factor, Xlim=0, Ylim=0, Dgap=0, Dplate=0;
  float Delta_Y_tmp, Dwall, box_xdim,box_ydim,width_box, Zthickness;
  int Nchannel = 0, chanl = 0, ngaps ;
  string path = "data/";
  string geoname  = path + "test.geo" ;
  geof = fopen(geoname.c_str(),  "w+");
  string infoname = path + "test.info";
  info = fopen(infoname.c_str(), "w+");
  string parname  = path + "par_tof.txt";
  parf = fopen(parname.c_str(),  "w+");
     
  //Initialize parameters

  // Common
  B_factor       = 1.;   // Relation between x and y  Dgap           = 0.2;   // Gap size    [mm]
  Dplate         = 1.0;   // Glass plate [mm]
  ngaps          = 8;     // Number of gaps
  Xlim           = 250;   // Begining of X segmentation [mm]
  Ylim           = 250;   // Begining of Y segmentation [mm]
  width_box      =   4;   // Width of the gas box [mm]
  box_xdim = 15000.;  // x extension of wall
  box_ydim = 11000.;  //y extension of wall 
  float box_zdim = 2000.;
  Dwall          = 6000; // Distance to the center of the TOF wall [mm]
  Zthickness     = ngaps*Dgap + (ngaps+1)*Dplate + 2*width_box; // RPC thickness [mm]
  

  //Print Header for the info file
  
  fprintf(info, " -------------------------------------------------------------------\n");
  fprintf(info, "    Tof Geometry : %s \n", geoname.c_str());
  fprintf(info, " -------------------------------------------------------------------\n");

  //Print Header for the geometry file

  fprintf(parf,"#####################################################################################\n");
  fprintf(parf,"# Geometry for the TOF detector                                                      \n");
  fprintf(parf,"# Format:                                                                            \n");
  fprintf(parf,"#                                                                                    \n");
  fprintf(parf,"# SMod Mod  Cell   smtype     X[mm]      Y[mm]     Z[mm]     Dx[mm]    Dy[mm]       \n");
  fprintf(parf,"#####################################################################################\n");
  fprintf(parf,"[TofGeoPar]                                                                          \n");
  fprintf(parf,"0\n");

  //----------RUN---------------------------------------                           

  float xpos=0.;
  float yposmax=4800.;
  float ypos1=700.;
  float dypos=500.;
  float dzpos=200.;
  float DZwall=2.*dzpos;
  float DXcol=1400.;
  int nSMs=0;
  int nSMo=0;
  int nPole=0;
  int nStrip=0;
  

cout <<  "Start placing SMs from "<<ypos1<<"  to "<< yposmax << endl;

for (float ypos=ypos1; ypos<yposmax; ypos+=dypos) {
     nSMs++;
     TOFSMs(nSMs, Dwall, 0., xpos,  ypos);
     nStrip+=5*32;
     
     nSMs++;
     TOFSMs(nSMs, Dwall, 0., xpos, -ypos);     
     nStrip+=5*32;

     if (nSMs>2) {   
      nSMs++;
      TOFSMs(nSMs, Dwall+dzpos, 0., xpos,  ypos-dypos/2.);
      nStrip+=5*32;

      nSMs++;
      TOFSMs(nSMs, Dwall+dzpos, 0., xpos, -ypos+dypos/2.);
      nStrip+=5*32;
     }
    }
  cout << nSMs << " SMs placed." << endl;
  // add mounting structure
  nPole++;
  TofPole(nPole, Dwall, 0., xpos);

  nPole++;
  TofPole(nPole, Dwall+dzpos, 0., xpos);

  // now place outer modules 
 xpos=100;

for(int i=1; i<4;i++){ 
 for (float ypos=0; ypos<yposmax; ypos+=dypos) {
     nSMo++;
     TOFSMo(nSMo, Dwall-i*DZwall, 0., xpos+i*DXcol,  ypos);
     nStrip+=5*32;
     nSMo++;
     TOFSMo(nSMo, Dwall-i*DZwall, 180., -xpos-i*DXcol,  ypos);
     nStrip+=5*32;

     if (nSMo>2) {
      nSMo++;
      TOFSMo(nSMo, Dwall-i*DZwall, 0., xpos+i*DXcol,  -ypos);      
      nStrip+=5*32;

      nSMo++;
      TOFSMo(nSMo, Dwall-i*DZwall, 180.,-xpos-i*DXcol,  -ypos);
      nStrip+=5*32;
     // 2. layer
      nSMo++;
      TOFSMo(nSMo, Dwall-i*DZwall+dzpos, 0., xpos+i*DXcol,  ypos-dypos/2.);      
      nStrip+=5*32;
      nSMo++;
      TOFSMo(nSMo, Dwall-i*DZwall+dzpos, 180.,-xpos-i*DXcol, ypos-dypos/2.);
      nStrip+=5*32;
      nSMo++;
      TOFSMo(nSMo, Dwall-i*DZwall+dzpos, 0., xpos+i*DXcol,  -ypos+dypos/2.);      
      nStrip+=5*32;
      nSMo++;
      TOFSMo(nSMo, Dwall-i*DZwall+dzpos, 180.,-xpos-i*DXcol, -ypos+dypos/2.);
      nStrip+=5*32;
     }
 }
  nPole++;
  TofPole(nPole, Dwall-i*DZwall, 180., xpos+i*DXcol);
  nPole++;
  TofPole(nPole, Dwall-i*DZwall+dzpos, 180., xpos+i*DXcol);
  nPole++;
  TofPole(nPole, Dwall-i*DZwall,  0., -xpos-i*DXcol);
  nPole++;
  TofPole(nPole, Dwall-i*DZwall+dzpos, 0., -xpos-i*DXcol);
}
  Nchannel=nStrip*2;

  cout << nSMo << " SMo placed." << endl;
  cout << nStrip << " strips," << Nchannel << " channels in setup." << endl;

  //---------------------END----------------------------
  
  fprintf(info, "\n");
  
  fprintf(info, "Total number of channels in the TOF : %d \n", Nchannel);
  fclose(geof);
  fclose(info);
  cout<<"\n"<<endl;
  cout<<"Geometry      written in " << geoname << endl;
  cout<<"\n"<<endl;
  cout<<"Documentation written in " << infoname << endl;
  cout<<"\n"<<endl;
  cout<<"Parameters    written in " << parname << endl;
  cout<<"\n"<<endl;

  return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------
void TOFSMs(int i, float Z, float phi, float xpos, float ypos){

  //i   = index of the first box/tower
  //Z   = distance to the target
  //phi = angle in along the Z axis
  //dy  = Thickness in Y
  //dx  = Thickness in X 
  //dz  = Thickness in Z
  //width_alu = Aluminum thickness

  float dx,dy,dz,width_aluxl,width_aluxr,width_aluy,width_aluz;

  //Aluminum box 

 dx=1800.;
 dy=500.;
 dz=200.;
 width_aluxl=50.;
 width_aluxr=10.;
 width_aluy=10.;
 width_aluz=1.;


  //Counter (module, inside)
  
  float cdx=350;
  float cdy=270;
  float cdz=80;
  int   nstrips=32;

  float dxpos=300.;
  float startxpos=-600.;
  float zoff=1.;
  float dzoff=50.;
  
  float gdx=320; //glass
  float gdy=269;
  float gdz=1;
  float ggdx=gdx;  //gas gap
  float ggdy=gdy;
  float ggdz=0.25;
  float gsdx=ggdx/float(nstrips);

  float dzpos=gdz+ggdz;
  float startzpos=-6.;
  float startzposg=-6.+gdz/2.;

// electronics
  float dxe=340.; //pcb dimensions 
  float dye=50.;
  float dze=3.;
  float yele=cdy/2.+dye/2.;
  
//
 if (i==1) {
  fprintf(geof,"tof%d#%d\n", i,i);
  fprintf(geof,"cave \n");
  fprintf(geof,"BOX\n");
  fprintf(geof,"aluminium \n");
  fprintf(geof, "  %f  %f  %f \n",-dx/2, -dy/2, -dz/2);
  fprintf(geof, "  %f  %f  %f \n",-dx/2,  dy/2, -dz/2);
  fprintf(geof, "  %f  %f  %f \n", dx/2,  dy/2, -dz/2);
  fprintf(geof, "  %f  %f  %f \n", dx/2, -dy/2, -dz/2);
  fprintf(geof, "  %f  %f  %f \n",-dx/2, -dy/2,  dz/2);
  fprintf(geof, "  %f  %f  %f \n",-dx/2,  dy/2,  dz/2);
  fprintf(geof, "  %f  %f  %f \n", dx/2,  dy/2,  dz/2);
  fprintf(geof, "  %f  %f  %f \n", dx/2, -dy/2,  dz/2);
  fprintf(geof, "\n");
  fprintf(geof, "%f %f %f \n"  , xpos,  ypos,   Z);
  fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 ,  0.0 );
  fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 ,  0.0 );
  fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 ,  1.0 ); 
  fprintf(geof,"//------------------------------------------ \n");

  //Gas (inside)
  fprintf(geof,"tof%dgas \n",i);
  fprintf(geof,"tof1#%d \n", i);
  fprintf(geof,"BOX\n");
  fprintf(geof,"RPCgas_noact \n");
  fprintf(geof, "  %f  %f  %f \n",-dx/2+width_aluxr,  -(dy/2-width_aluy), -(dz/2-width_aluz));
  fprintf(geof, "  %f  %f  %f \n",-dx/2+width_aluxr,   dy/2-width_aluy,   -(dz/2-width_aluz));
  fprintf(geof, "  %f  %f  %f \n", (dx/2-width_aluxl), dy/2-width_aluy,   -(dz/2-width_aluz));
  fprintf(geof, "  %f  %f  %f \n", (dx/2-width_aluxl),-(dy/2-width_aluy), -(dz/2-width_aluz));
  fprintf(geof, "  %f  %f  %f \n",-dx/2+width_aluxr,  -(dy/2-width_aluy),  dz/2-width_aluz);
  fprintf(geof, "  %f  %f  %f \n",-dx/2+width_aluxr,   dy/2-width_aluy,    dz/2-width_aluz);
  fprintf(geof, "  %f  %f  %f \n", (dx/2-width_aluxl), dy/2-width_aluy,    dz/2-width_aluz);
  fprintf(geof, "  %f  %f  %f \n", (dx/2-width_aluxl),-(dy/2-width_aluy),  dz/2-width_aluz);
  fprintf(geof, "\n");
  fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 0.0);
  fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);
  fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
  fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
  fprintf(geof,"//------------------------------------------ \n");

  for (int j=1; j<6; j++){ //loop over counters (modules)
   zoff=-zoff;
   if(j==1) {
    fprintf(geof,"t%dreg%dmod#%d \n",i,i,j);
    fprintf(geof,"tof%dgas \n", i);
    fprintf(geof,"BOX\n");
    fprintf(geof,"RPCgas_noact \n");
    fprintf(geof, "  %f  %f  %f \n",  cdx/2, -cdy/2, -cdz/2);
    fprintf(geof, "  %f  %f  %f \n",  cdx/2,  cdy/2, -cdz/2);
    fprintf(geof, "  %f  %f  %f \n", -cdx/2,  cdy/2, -cdz/2);
    fprintf(geof, "  %f  %f  %f \n", -cdx/2, -cdy/2, -cdz/2);
    fprintf(geof, "  %f  %f  %f \n",  cdx/2, -cdy/2,  cdz/2);
    fprintf(geof, "  %f  %f  %f \n",  cdx/2,  cdy/2,  cdz/2);
    fprintf(geof, "  %f  %f  %f \n", -cdx/2,  cdy/2,  cdz/2);
    fprintf(geof, "  %f  %f  %f \n", -cdx/2, -cdy/2,  cdz/2);
    fprintf(geof, "\n");  
    fprintf(geof, "%f %f %f \n"  , startxpos+(j-1)*dxpos , 0.0 , zoff*dzoff); //position
    fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
    fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
    fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
    fprintf(geof,"//------------------------------------------ \n");
    
    for (int k=1; k<10; k++){ //loop over glass plates and gas gaps.
     if(k==1) {
      fprintf(geof,"t1reg1gla#%d \n", k);
      fprintf(geof,"t1reg1mod#%d \n",j);
      fprintf(geof,"BOX\n");
      fprintf(geof,"RPCglass \n");
      fprintf(geof, "  %f  %f  %f \n",  gdx/2, -gdy/2, -gdz/2);
      fprintf(geof, "  %f  %f  %f \n",  gdx/2,  gdy/2, -gdz/2);
      fprintf(geof, "  %f  %f  %f \n", -gdx/2,  gdy/2, -gdz/2);
      fprintf(geof, "  %f  %f  %f \n", -gdx/2, -gdy/2, -gdz/2);
      fprintf(geof, "  %f  %f  %f \n",  gdx/2, -gdy/2,  gdz/2);
      fprintf(geof, "  %f  %f  %f \n",  gdx/2,  gdy/2,  gdz/2);
      fprintf(geof, "  %f  %f  %f \n", -gdx/2,  gdy/2,  gdz/2);
      fprintf(geof, "  %f  %f  %f \n", -gdx/2, -gdy/2,  gdz/2);
      fprintf(geof, "\n");  
      fprintf(geof, "%f %f %f \n"  , 0.0, 0.0 , startzpos+(k-1)*dzpos); //position
      fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
      fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
      fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
      fprintf(geof,"//------------------------------------------ \n");

      fprintf(geof,"t1reg1gap#%d \n", k);
      fprintf(geof,"t1reg1mod#%d \n",j);
      fprintf(geof,"BOX\n");
      fprintf(geof,"RPCgas_noact \n");
      fprintf(geof, "  %f  %f  %f \n",  ggdx/2, -ggdy/2, -ggdz/2);
      fprintf(geof, "  %f  %f  %f \n",  ggdx/2,  ggdy/2, -ggdz/2);
      fprintf(geof, "  %f  %f  %f \n", -ggdx/2,  ggdy/2, -ggdz/2);
      fprintf(geof, "  %f  %f  %f \n", -ggdx/2, -ggdy/2, -ggdz/2);
      fprintf(geof, "  %f  %f  %f \n",  ggdx/2, -ggdy/2,  ggdz/2);
      fprintf(geof, "  %f  %f  %f \n",  ggdx/2,  ggdy/2,  ggdz/2);
      fprintf(geof, "  %f  %f  %f \n", -ggdx/2,  ggdy/2,  ggdz/2);
      fprintf(geof, "  %f  %f  %f \n", -ggdx/2, -ggdy/2,  ggdz/2);
      fprintf(geof, "\n");  
      fprintf(geof, "%f %f %f \n"  , 0.0, 0.0 , startzposg+(k-1)*dzpos); //position
      fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
      fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
      fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
      fprintf(geof,"//------------------------------------------ \n");
      for (int l=0; l<nstrips; l++){
       fprintf(geof,"t1reg1stgap#%d \n", l);
       fprintf(geof,"t1reg1gap#%d \n", k);
       fprintf(geof,"BOX\n");
       fprintf(geof,"RPCgas \n");
       fprintf(geof, "  %f  %f  %f \n",  gsdx/2, -ggdy/2, -ggdz/2);
       fprintf(geof, "  %f  %f  %f \n",  gsdx/2,  ggdy/2, -ggdz/2);
       fprintf(geof, "  %f  %f  %f \n", -gsdx/2,  ggdy/2, -ggdz/2);
       fprintf(geof, "  %f  %f  %f \n", -gsdx/2, -ggdy/2, -ggdz/2);
       fprintf(geof, "  %f  %f  %f \n",  gsdx/2, -ggdy/2,  ggdz/2);
       fprintf(geof, "  %f  %f  %f \n",  gsdx/2,  ggdy/2,  ggdz/2);
       fprintf(geof, "  %f  %f  %f \n", -gsdx/2,  ggdy/2,  ggdz/2);
       fprintf(geof, "  %f  %f  %f \n", -gsdx/2, -ggdy/2,  ggdz/2);
       fprintf(geof, "\n");  
       fprintf(geof, "%f %f %f \n"  , -ggdx/2+(l+0.5)*gsdx, 0.0, 0.0) ; //position
       fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
       fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
       fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0);
       }      
     }else{ //k>0
      fprintf(geof,"t1reg1gla#%d \n", k);
      fprintf(geof,"t1reg1mod#%d \n",j);    
      fprintf(geof, "%f %f %f \n"  , 0.0, 0.0 , startzpos+(k-1)*dzpos); //position
      fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
      fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
      fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0);    
      if (k<9) {
       fprintf(geof,"t1reg1gap#%d \n", k);
       fprintf(geof,"t1reg1mod#%d \n",j);
       fprintf(geof, "%f %f %f \n"  , 0.0, 0.0 , startzposg+(k-1)*dzpos); //position
       fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
       fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
       fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
       fprintf(geof,"//------------------------------------------ \n");

       if (k==4){ //take geometry from center gap (#4)
	  cout << "write geo para1 for i="<<i<<" j="<<j <<endl;
	  
          for (int l=0; l<nstrips; l++){
               fprintf(parf, "%d \t %d \t %d \t %d \t %5.1f \t %5.1f \t %5.1f \t %2.1f \t %2.1f \n" , 
               i, j, l, 1,       
               xpos + startxpos+(j-1)*dxpos -ggdx/2+(l+0.5)*gsdx, 
               ypos, 
               Z + zoff*dzoff+startzposg+(k-1)*dzpos, gsdx, ggdy);
               }
       }
      
      }
     }
    } // k-loop end
// back inside SM - gas 
    fprintf(geof,"t1pcb#%d \n",j);
    fprintf(geof,"tof%dgas \n", i);
    fprintf(geof,"BOX\n");
    fprintf(geof,"carbon \n"); // should be PCB + electronics -> media.geo
    fprintf(geof, "  %f  %f  %f \n",  dxe/2, -dye/2, -dze/2);
    fprintf(geof, "  %f  %f  %f \n",  dxe/2,  dye/2, -dze/2);
    fprintf(geof, "  %f  %f  %f \n", -dxe/2,  dye/2, -dze/2);
    fprintf(geof, "  %f  %f  %f \n", -dxe/2, -dye/2, -dze/2);
    fprintf(geof, "  %f  %f  %f \n",  dxe/2, -dye/2,  dze/2);
    fprintf(geof, "  %f  %f  %f \n",  dxe/2,  dye/2,  dze/2);
    fprintf(geof, "  %f  %f  %f \n", -dxe/2,  dye/2,  dze/2);
    fprintf(geof, "  %f  %f  %f \n", -dxe/2, -dye/2,  dze/2);
    fprintf(geof, "\n");
    fprintf(geof, "%f %f %f \n"  , startxpos+(j-1)*dxpos , yele , zoff*dzoff); 
    fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 ,  0.0 );
    fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 ,  0.0 );
    fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 ,  1.0 ); 
    fprintf(geof,"//------------------------------------------ \n");
    fprintf(geof,"t1pcb#%d \n", j);
    fprintf(geof,"tof%dgas \n", i);
    fprintf(geof, "%f %f %f \n"  , startxpos+(j-1)*dxpos ,-yele , zoff*dzoff); 
    fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 ,  0.0 );
    fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 ,  0.0 );
    fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 ,  1.0 ); 
    fprintf(geof,"//------------------------------------------ \n");
   }else{ //j>1
    fprintf(geof,"t%dreg%dmod#%d \n",i,i,j);
    fprintf(geof,"tof%dgas \n", i);
    fprintf(geof, "%f %f %f \n"  , startxpos+(j-1)*dxpos , 0.0 , zoff*dzoff); //position
    fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
    fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
    fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
    fprintf(geof,"//------------------------------------------ \n");  

    // cout << "write geo para for i="<<i<<" j="<<j <<endl;
   
      for (int l=0; l<nstrips; l++){
       int k=4;  
       fprintf(parf, "%d \t %d \t %d \t %d \t %5.1f \t %5.1f \t %5.1f \t %2.1f \t %2.1f \n" , 
               i, j, l, 1,       
               xpos + startxpos+(j-1)*dxpos -ggdx/2+(l+0.5)*gsdx, 
               ypos, 
               Z + zoff*dzoff+startzposg+(k-1)*dzpos, gsdx, ggdy);
      }
    fprintf(geof,"t1pcb#%d \n", j);
    fprintf(geof,"tof%dgas \n", i);
    fprintf(geof, "%f %f %f \n"  , startxpos+(j-1)*dxpos , yele , zoff*dzoff); 
    fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
    fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
    fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
    fprintf(geof,"//------------------------------------------ \n"); 
    fprintf(geof,"t1pcb#%d \n", j);
    fprintf(geof,"tof%dgas \n", i);
    fprintf(geof, "%f %f %f \n"  , startxpos+(j-1)*dxpos ,-yele , zoff*dzoff); 
    fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
    fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
    fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
    fprintf(geof,"//------------------------------------------ \n");    
   }
   
   }
  }  else {//i>0
   fprintf(geof,"tof1#%d\n", i);
   fprintf(geof,"cave \n");
   fprintf(geof, "%f %f %f \n"  , xpos,  ypos,   Z);
   fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 ,  0.0 );
   fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 ,  0.0 );
   fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 ,  1.0 ); 
   fprintf(geof,"//------------------------------------------ \n");
   
   zoff=1;
   for (int j=1; j<6; j++){ //loop over counters (modules)
      zoff=-zoff;
      for (int l=0; l<nstrips; l++){
       int k=4;  
       fprintf(parf, "%d \t %d \t %d \t %2d \t %5.1f \t %5.1f \t %5.1f \t %2.1f \t %2.1f \n" , 
               i, j, l, 1,       
               xpos + startxpos+(j-1)*dxpos -ggdx/2+(l+0.5)*gsdx, 
               ypos, 
               Z + zoff*dzoff+startzposg+(k-1)*dzpos, gsdx, ggdy);
      }  
   }
   
   }

 }

void TOFSMo(int i, float Z, float phi, float xpos, float ypos){

  //i   = index of the first box/tower
  //Z   = distance to the target
  //phi = angle in along the Z axis
  //dy  = Thickness in Y
  //dx  = Thickness in X 
  //dz  = Thickness in Z
  //width_alu = Aluminum thickness

  float dx,dy,dz,width_alux,gasoff,width_aluy,width_aluz;

  //Aluminum box

 dx=1800.;
 dy=500.;
 dz=200.;
 width_alux=30.;
 gasoff=-20.; // to get asymmetric wall thicknesses
 width_aluy=10.;
 width_aluz=1.;

  //Counter (inside)
  
  float cdx=350;
  float cdy=300;
  float cdz=80;
  int   nstrips=32;

  float dxpos=280.;
  float startxpos=-660.;
  float phic=20./180.*PI;// counter tilt angle
  float cphi=cos(phic);
  float sphi=sin(phic);

  float gdx=320; //glass
  float gdy=270;
  float gdz=1;
  float ggdx=gdx;  //gas gap
  float ggdy=gdy;
  float ggdz=0.25;
  float gsdx=ggdx/float(nstrips);
   
  float dzpos=gdz+ggdz;
  float startzpos=-6.;
  float startzposg=-6.+gdz/2.;

// electronics
  float dxe=340.; //pcb dimensions 
  float dye=50.;
  float dze=3.;
  float yele=cdy/2.+dye/2.;

 float cph=cos(phi/180.*PI);
 float sph=sin(phi/180.*PI);

 if (i==1) {
  fprintf(geof,"tof%d#%d\n", 2,i);
  fprintf(geof,"cave \n");
  fprintf(geof,"BOX\n");
  fprintf(geof,"aluminium \n");
  fprintf(geof, "  %f  %f  %f \n",  dx/2, -dy/2, -dz/2);
  fprintf(geof, "  %f  %f  %f \n",  dx/2,  dy/2, -dz/2);
  fprintf(geof, "  %f  %f  %f \n", -dx/2,  dy/2, -dz/2);
  fprintf(geof, "  %f  %f  %f \n", -dx/2, -dy/2, -dz/2);
  fprintf(geof, "  %f  %f  %f \n",  dx/2, -dy/2,  dz/2);
  fprintf(geof, "  %f  %f  %f \n",  dx/2,  dy/2,  dz/2);
  fprintf(geof, "  %f  %f  %f \n", -dx/2,  dy/2,  dz/2);
  fprintf(geof, "  %f  %f  %f \n", -dx/2, -dy/2,  dz/2);
  fprintf(geof, "\n");
  fprintf(geof, "%f %f %f \n"  , xpos,  ypos,   Z);
  fprintf(geof, "%f %f %f   "  , cph , -sph ,  0.0 );
  fprintf(geof, "%f %f %f   "  , sph ,  cph ,  0.0 );
  fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 ,  1.0 ); 
  fprintf(geof,"//------------------------------------------ \n");

  //Gas (inside)
  fprintf(geof,"tof%dgas \n",2);
  fprintf(geof,"tof2#%d \n", i);
  fprintf(geof,"BOX\n");
  fprintf(geof,"RPCgas_noact \n");
  fprintf(geof, "  %f  %f  %f \n",  dx/2-width_alux,  -(dy/2-width_aluy), -(dz/2-width_aluz));
  fprintf(geof, "  %f  %f  %f \n",  dx/2-width_alux,   dy/2-width_aluy,   -(dz/2-width_aluz));
  fprintf(geof, "  %f  %f  %f \n", -(dx/2-width_alux), dy/2-width_aluy,   -(dz/2-width_aluz));
  fprintf(geof, "  %f  %f  %f \n", -(dx/2-width_alux),-(dy/2-width_aluy), -(dz/2-width_aluz));
  fprintf(geof, "  %f  %f  %f \n",  dx/2-width_alux,  -(dy/2-width_aluy),  dz/2-width_aluz);
  fprintf(geof, "  %f  %f  %f \n",  dx/2-width_alux,   dy/2-width_aluy,    dz/2-width_aluz);
  fprintf(geof, "  %f  %f  %f \n", -(dx/2-width_alux), dy/2-width_aluy,    dz/2-width_aluz);
  fprintf(geof, "  %f  %f  %f \n", -(dx/2-width_alux),-(dy/2-width_aluy),  dz/2-width_aluz);
  fprintf(geof, "\n");
  fprintf(geof, "%f %f %f \n"  , gasoff , 0.0 , 0.0);
  fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);
  fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
  fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
  fprintf(geof,"//------------------------------------------ \n");

  for (int j=1; j<6; j++){ //loop over counters
   if(j==1) {
    fprintf(geof,"t%dreg%dmod#%d \n",2,1,j);
    fprintf(geof,"tof%dgas \n", 2);
    fprintf(geof,"BOX\n");
    fprintf(geof,"RPCgas_noact \n");
    fprintf(geof, "  %f  %f  %f \n",  cdx/2, -cdy/2, -cdz/2);
    fprintf(geof, "  %f  %f  %f \n",  cdx/2,  cdy/2, -cdz/2);
    fprintf(geof, "  %f  %f  %f \n", -cdx/2,  cdy/2, -cdz/2);
    fprintf(geof, "  %f  %f  %f \n", -cdx/2, -cdy/2, -cdz/2);
    fprintf(geof, "  %f  %f  %f \n",  cdx/2, -cdy/2,  cdz/2);
    fprintf(geof, "  %f  %f  %f \n",  cdx/2,  cdy/2,  cdz/2);
    fprintf(geof, "  %f  %f  %f \n", -cdx/2,  cdy/2,  cdz/2);
    fprintf(geof, "  %f  %f  %f \n", -cdx/2, -cdy/2,  cdz/2);
    fprintf(geof, "\n");  
    
    fprintf(geof, "%f %f %f \n"  , startxpos+(j-1)*dxpos , 0.0 , 0.); //position
    fprintf(geof, "%f %f %f   "  , cphi , 0.0 , sphi);  //rotation matrix 
    fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
    fprintf(geof, "%f %f %f \n"  , -sphi , 0.0 , cphi);
    fprintf(geof,"//------------------------------------------ \n");
  
     
    for (int k=1; k<10; k++){ //loop over glass plates 
     if(k==1) {
      fprintf(geof,"t2reg1gla#%d \n", k);
      fprintf(geof,"t2reg1mod#%d \n",j);
      fprintf(geof,"BOX\n");
      fprintf(geof,"RPCglass \n");
      fprintf(geof, "  %f  %f  %f \n",  gdx/2, -gdy/2, -gdz/2);
      fprintf(geof, "  %f  %f  %f \n",  gdx/2,  gdy/2, -gdz/2);
      fprintf(geof, "  %f  %f  %f \n", -gdx/2,  gdy/2, -gdz/2);
      fprintf(geof, "  %f  %f  %f \n", -gdx/2, -gdy/2, -gdz/2);
      fprintf(geof, "  %f  %f  %f \n",  gdx/2, -gdy/2,  gdz/2);
      fprintf(geof, "  %f  %f  %f \n",  gdx/2,  gdy/2,  gdz/2);
      fprintf(geof, "  %f  %f  %f \n", -gdx/2,  gdy/2,  gdz/2);
      fprintf(geof, "  %f  %f  %f \n", -gdx/2, -gdy/2,  gdz/2);
      fprintf(geof, "\n");  
      fprintf(geof, "%f %f %f \n"  , 0.0, 0.0 , startzpos+(k-1)*dzpos); //position
      fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
      fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
      fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
      fprintf(geof,"//------------------------------------------ \n");

      fprintf(geof,"t2reg1gap#%d \n", k);
      fprintf(geof,"t2reg1mod#%d \n",j);
      fprintf(geof,"BOX\n");
      fprintf(geof,"RPCgas_noact \n");
      fprintf(geof, "  %f  %f  %f \n",  ggdx/2, -ggdy/2, -ggdz/2);
      fprintf(geof, "  %f  %f  %f \n",  ggdx/2,  ggdy/2, -ggdz/2);
      fprintf(geof, "  %f  %f  %f \n", -ggdx/2,  ggdy/2, -ggdz/2);
      fprintf(geof, "  %f  %f  %f \n", -ggdx/2, -ggdy/2, -ggdz/2);
      fprintf(geof, "  %f  %f  %f \n",  ggdx/2, -ggdy/2,  ggdz/2);
      fprintf(geof, "  %f  %f  %f \n",  ggdx/2,  ggdy/2,  ggdz/2);
      fprintf(geof, "  %f  %f  %f \n", -ggdx/2,  ggdy/2,  ggdz/2);
      fprintf(geof, "  %f  %f  %f \n", -ggdx/2, -ggdy/2,  ggdz/2);
      fprintf(geof, "\n");  
      fprintf(geof, "%f %f %f \n"  , 0.0, 0.0 , startzposg+(k-1)*dzpos); //position
      fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
      fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
      fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
      fprintf(geof,"//------------------------------------------ \n");
      for (int l=0; l<nstrips; l++){
       fprintf(geof,"t2reg1stgap#%d \n", l);
       fprintf(geof,"t2reg1gap#%d \n", k);
       fprintf(geof,"BOX\n");
       fprintf(geof,"RPCgas \n");
       fprintf(geof, "  %f  %f  %f \n",  gsdx/2, -ggdy/2, -ggdz/2);
       fprintf(geof, "  %f  %f  %f \n",  gsdx/2,  ggdy/2, -ggdz/2);
       fprintf(geof, "  %f  %f  %f \n", -gsdx/2,  ggdy/2, -ggdz/2);
       fprintf(geof, "  %f  %f  %f \n", -gsdx/2, -ggdy/2, -ggdz/2);
       fprintf(geof, "  %f  %f  %f \n",  gsdx/2, -ggdy/2,  ggdz/2);
       fprintf(geof, "  %f  %f  %f \n",  gsdx/2,  ggdy/2,  ggdz/2);
       fprintf(geof, "  %f  %f  %f \n", -gsdx/2,  ggdy/2,  ggdz/2);
       fprintf(geof, "  %f  %f  %f \n", -gsdx/2, -ggdy/2,  ggdz/2);
       fprintf(geof, "\n");  
       fprintf(geof, "%f %f %f \n"  , -ggdx/2+(l+0.5)*gsdx, 0.0, 0.0) ; //position
       fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
       fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
       fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0);
      }
     }else{ //k>0
      fprintf(geof,"t2reg1gla#%d \n", k);
      fprintf(geof,"t2reg1mod#%d \n",j);    
      fprintf(geof, "%f %f %f \n"  , 0.0, 0.0 , startzpos+(k-1)*dzpos); //position
      fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
      fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
      fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0);    
      if (k<9){	  
       fprintf(geof,"t2reg1gap#%d \n", k);
       fprintf(geof,"t2reg1mod#%d \n",j);
       fprintf(geof, "%f %f %f \n"  , 0.0, 0.0 , startzposg+(k-1)*dzpos); //position
       fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);  //rotation matrix 
       fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
       fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
       fprintf(geof,"//------------------------------------------ \n");
       if (k==4){ //take geometry from center gap (#4)
	  cout << "write geo para2 for i="<<i<<" j="<<j <<endl;
	  
          for (int l=0; l<nstrips; l++){
               fprintf(parf, "%d \t %d \t %d \t %d \t %5.1f \t %5.1f \t %5.1f \t %2.1f \t %2.1f \n" , 
               i, j, l, 2,       
	       xpos + cph*(gasoff + startxpos+(j-1)*dxpos + cphi*(-ggdx/2+(l+0.5)*gsdx)), 
               ypos, 
               Z + startzposg+(k-1)*dzpos-sphi*(-ggdx/2+(l+0.5)*gsdx), gsdx, ggdy);
	  }
       }
       
      }
      
     }
     
    } // k-loop end 
// back inside SM - gas 
    fprintf(geof,"t2pcb#%d \n",j);
    fprintf(geof,"tof%dgas \n", 2);
    fprintf(geof,"BOX\n");
    fprintf(geof,"carbon \n"); // should be PCB + electronics -> media.geo
    fprintf(geof, "  %f  %f  %f \n",  dxe/2, -dye/2, -dze/2);
    fprintf(geof, "  %f  %f  %f \n",  dxe/2,  dye/2, -dze/2);
    fprintf(geof, "  %f  %f  %f \n", -dxe/2,  dye/2, -dze/2);
    fprintf(geof, "  %f  %f  %f \n", -dxe/2, -dye/2, -dze/2);
    fprintf(geof, "  %f  %f  %f \n",  dxe/2, -dye/2,  dze/2);
    fprintf(geof, "  %f  %f  %f \n",  dxe/2,  dye/2,  dze/2);
    fprintf(geof, "  %f  %f  %f \n", -dxe/2,  dye/2,  dze/2);
    fprintf(geof, "  %f  %f  %f \n", -dxe/2, -dye/2,  dze/2);
    fprintf(geof, "\n");    
    fprintf(geof, "%f %f %f \n"  , startxpos+(j-1)*dxpos , yele , 0.); //position
    fprintf(geof, "%f %f %f   "  , cphi , 0.0 , sphi);  //rotation matrix 
    fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
    fprintf(geof, "%f %f %f \n"  , -sphi , 0.0 , cphi);
    fprintf(geof,"//------------------------------------------ \n");

    fprintf(geof,"t2pcb#%d \n",j);
    fprintf(geof,"tof%dgas \n", 2);
    fprintf(geof, "%f %f %f \n"  , startxpos+(j-1)*dxpos ,-yele , 0.); //position
    fprintf(geof, "%f %f %f   "  , cphi , 0.0 , sphi);  //rotation matrix 
    fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
    fprintf(geof, "%f %f %f \n"  , -sphi , 0.0 , cphi);
    fprintf(geof,"//------------------------------------------ \n");
   }else{ //j>1
    fprintf(geof,"t%dreg%dmod#%d \n",2,1,j);
    fprintf(geof,"tof%dgas \n", 2);
    fprintf(geof, "%f %f %f \n"  , startxpos+(j-1)*dxpos , 0.0 , 0.); //position
    fprintf(geof, "%f %f %f   "  , cphi , 0.0 , sphi);  //rotation matrix 
    fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
    fprintf(geof, "%f %f %f \n"  , -sphi , 0.0 , cphi);     
    fprintf(geof,"//------------------------------------------ \n");

    //cout << "write geo para2 for i="<<i<<" j="<<j <<endl;
   
      for (int l=0; l<nstrips; l++){
       int k=4;  
       fprintf(parf, "%d \t %d \t %d \t %d \t %5.1f \t %5.1f \t %5.1f \t %2.1f \t %2.1f \n" , 
               i, j, l, 2,       
	       xpos + cph*(gasoff + startxpos+(j-1)*dxpos + cphi*(-ggdx/2+(l+0.5)*gsdx)), 
               ypos, 
               Z + startzposg+(k-1)*dzpos-sphi*(-ggdx/2+(l+0.5)*gsdx), gsdx, ggdy);
      }

    fprintf(geof,"t2pcb#%d \n",j);
    fprintf(geof,"tof%dgas \n", 2);
    fprintf(geof, "%f %f %f \n"  , startxpos+(j-1)*dxpos , yele , 0.); //position
    fprintf(geof, "%f %f %f   "  , cphi , 0.0 , sphi);  //rotation matrix 
    fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
    fprintf(geof, "%f %f %f \n"  , -sphi , 0.0 , cphi); 
    fprintf(geof,"//------------------------------------------ \n");

    fprintf(geof,"t2pcb#%d \n",j);
    fprintf(geof,"tof%dgas \n", 2);
    fprintf(geof, "%f %f %f \n"  , startxpos+(j-1)*dxpos ,-yele , 0.); //position
    fprintf(geof, "%f %f %f   "  , cphi , 0.0 , sphi);  //rotation matrix 
    fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
    fprintf(geof, "%f %f %f \n"  , -sphi , 0.0 , cphi); 
    fprintf(geof,"//------------------------------------------ \n");
    }
   }
  }  else {//i>0
   fprintf(geof,"tof2#%d\n", i);
   fprintf(geof,"cave \n");
   fprintf(geof, "%f %f %f \n"  , xpos,  ypos,   Z);
   fprintf(geof, "%f %f %f   "  , cph , -sph ,  0.0 );
   fprintf(geof, "%f %f %f   "  , sph ,  cph ,  0.0 );
   fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 ,  1.0 ); 
   fprintf(geof,"//------------------------------------------ \n");
   
   for (int j=1; j<6; j++){ //loop over counters (modules)
      for (int l=0; l<nstrips; l++){
       int k=4;  
       fprintf(parf, "%d \t %d \t %d \t %2d \t %5.1f \t %5.1f \t %5.1f \t %2.1f \t %2.1f \n" , 
               i, j, l, 2,       
	       xpos + cph*(gasoff + startxpos+(j-1)*dxpos + cphi*(-ggdx/2+(l+0.5)*gsdx)), 
               ypos, 
               Z + startzposg+(k-1)*dzpos-sphi*(-ggdx/2+(l+0.5)*gsdx), gsdx, ggdy);
      } 
   }
 }
}


//-------------------------------------------------------------------------------------
void TofPole(int i, float Z, float phi, float xpos){

  //i   = index of the first box/tower
  //Z   = distance to the target
  //phi = angle in along the Z axis
  //dy  = Thickness in Y
  //dx  = Thickness in X 
  //dz  = Thickness in Z
  //width_alu = Aluminum thickness

  float dx,dy,dz,width_alux,width_aluy,width_aluz;

 float dxb=1800.;
 dx=200.;
 dy=10000.;
 dz=200.;
 width_alux=50.;
 width_aluy=50.;
 width_aluz=50.;

  float cphi=cos(phi/180.*PI);
  float sphi=sin(phi/180.*PI);

  //Aluminum box

 if (i==1) {
  fprintf(geof,"tof%dpol#%d\n", i,i);
  fprintf(geof,"cave \n");
  fprintf(geof,"BOX\n");
  fprintf(geof,"aluminium \n");
  fprintf(geof, "  %f  %f  %f \n",  dx/2, -dy/2, -dz/2);
  fprintf(geof, "  %f  %f  %f \n",  dx/2,  dy/2, -dz/2);
  fprintf(geof, "  %f  %f  %f \n", -dx/2,  dy/2, -dz/2);
  fprintf(geof, "  %f  %f  %f \n", -dx/2, -dy/2, -dz/2);
  fprintf(geof, "  %f  %f  %f \n",  dx/2, -dy/2,  dz/2);
  fprintf(geof, "  %f  %f  %f \n",  dx/2,  dy/2,  dz/2);
  fprintf(geof, "  %f  %f  %f \n", -dx/2,  dy/2,  dz/2);
  fprintf(geof, "  %f  %f  %f \n", -dx/2, -dy/2,  dz/2);
  fprintf(geof, "\n");
  fprintf(geof, "%f %f %f \n"  , xpos+cphi*(-dx/2.-dxb/2.),  0.,   Z);
  fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 ,  0.0 );
  fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 ,  0.0 );
  fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 ,  1.0 ); 
  fprintf(geof,"//------------------------------------------ \n");

  //Gas (inside)
  fprintf(geof,"tp%dair \n",i);
  fprintf(geof,"tof1pol#%d \n",i);
  fprintf(geof,"BOX\n");
  fprintf(geof,"air \n");
  fprintf(geof, "  %f  %f  %f \n",  dx/2-width_alux,  -(dy/2-width_aluy), -(dz/2-width_aluz));
  fprintf(geof, "  %f  %f  %f \n",  dx/2-width_alux,   dy/2-width_aluy,   -(dz/2-width_aluz));
  fprintf(geof, "  %f  %f  %f \n", -(dx/2-width_alux), dy/2-width_aluy,   -(dz/2-width_aluz));
  fprintf(geof, "  %f  %f  %f \n", -(dx/2-width_alux),-(dy/2-width_aluy), -(dz/2-width_aluz));
  fprintf(geof, "  %f  %f  %f \n",  dx/2-width_alux,  -(dy/2-width_aluy),  dz/2-width_aluz);
  fprintf(geof, "  %f  %f  %f \n",  dx/2-width_alux,   dy/2-width_aluy,    dz/2-width_aluz);
  fprintf(geof, "  %f  %f  %f \n", -(dx/2-width_alux), dy/2-width_aluy,    dz/2-width_aluz);
  fprintf(geof, "  %f  %f  %f \n", -(dx/2-width_alux),-(dy/2-width_aluy),  dz/2-width_aluz);
  fprintf(geof, "\n");
  fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 0.0);
  fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 , 0.0);
  fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 , 0.0);
  fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 , 1.0); 
  fprintf(geof,"//------------------------------------------ \n");
 }  else {//i>0
  fprintf(geof,"tof1pol#%d\n", i);
  fprintf(geof,"cave \n"); 
  fprintf(geof, "%f %f %f \n"  , xpos+cphi*(-dx/2.-dxb/2.),  0.,   Z);
  fprintf(geof, "%f %f %f   "  , 1.0 , 0.0 ,  0.0 );
  fprintf(geof, "%f %f %f   "  , 0.0 , 1.0 ,  0.0 );
  fprintf(geof, "%f %f %f \n"  , 0.0 , 0.0 ,  1.0 );
  fprintf(geof,"//------------------------------------------ \n");
 }

}



