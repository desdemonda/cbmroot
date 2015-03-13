//
//   Generator for CbmTrd Geometry
//
//
// Update 20111129 - David Emschermann 
// - remove TRD3 layer 3 and 4 (10 layers in total)
// - plain layers with flat keeping volumes
// - increase layer pitch to 500 mm 
// - increase radiator thickness to 360 mm (for foam radiator) 
//
// Update 20110616 - David Emschermann 
// - 12 mm gas thickness (instead of 6 mm) by default 
//
// Update 20110616 - David Emschermann 
// - allow variable frame width - 15 / 20 mm 
// - resulting in 570 x 570 mm2 and 960 x 960 mm2 sized pad planes
// - multiple of 30 mm allow for 5 mm and 7.5 mm wide pads
//
// Update 20110602 - David Emschermann 
// - change order of modules
//
// Update 20110523 - David Emschermann 
// - core of stations 1 and 2 changed to to 5x5-1 single sized modules, instead of 8 + 12
//
// Update 20100702 - David Emschermann 
// - remove layer keeping volumes and layers
//
// Update 20100631 - David Emschermann 
// - adapt trd1/2/3 keeping volume to tilted setup
//
// Update 20100630 - David Emschermann 
// - introduce layer_pitch in addition to layer_thickness
//
// Update 20100630 - David Emschermann 
// - experimental branch for tilted TRD geometries
//
// Update 20100629 - David Emschermann 
// - hardcode the keeping volumes for stations and layers
//
// Update 20100625 - David Emschermann 
// - hardcode the detector module sizes: 500 mm, 750 mm, 1000 mm
//
// Update 20100622 - David Emschermann 
// - change the order of 1x1m^2 modules to allow for tilting
//
// Update 20100617 - David Emschermann 
// - move TRD in z to 4500 / 6750 / 9000 mm
// - change calculation of thicknesses
// - 12 mm gas thickness -> jun10_v01
// -  6 mm gas thickness -> jun10_v02 (standard)
// - add comments about geometry to head of output file
//
// Update 20100527 - David Emschermann 
// - replace carbon frames by G10
//
// Update 20100526 - David Emschermann 
// - change the order of modules to match the pad definition for jun10
//
// Update 20091127 - David Emschermann 
// - all chambers have squared geometry
//
// Update 20081120 - F. Uhlig
// - Changes made that geometry is now independent of Trd-positions.
//

#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#define PI 3.14159265
FILE *geofile;
FILE *infofile;
//FILE *parameterfile;
using namespace std;


int Tiltandshift(int, int, float, float, float, int);
int TrdModules1(int, int, float, float, float, int, float, float*, float*);


//--------------------------------------------------------------------
int Tiltandshift(int Station_number, int Layer_number, float Layer_thickness, float Layer_pitch, float Position_Station1[][4], int i) {

  // Station_thickness is unkown here
  float z_offset = (-4*Layer_pitch/2)+Layer_thickness/2;

  //  float rho[3] = { 0., 0., 0. } ;   // no tilt
  //  float rho[3] = { 1.5/4.5, 2.5/6.75, 3.5/9.0 } ;   // hardcoded tilting angle

  // corresponds to angles of 18.43, 20.32 and 21.25 deg for stations 1, 2, 3
  // incident angle at kink position is orthogonal for tracks from the vertex 
  
//  float rot[3][3]={ { 1500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number) }, 
//                    { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number) }, 
//  		      { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number) } };

  // v01, v02 // no tilting
  float rot[12][4]={ { 1500., 0., 0., 0. }, 
                     { 1500., 0., 0., 0. }, 
                     { 1500., 0., 0., 0. }, 
                     { 1500., 0., 0., 0. }, 
		       	     	       	       	     
                     { 2500., 0., 0., 0. }, 
                     { 2500., 0., 0., 0. }, 
                     { 2500., 0., 0., 0. }, 
                     { 2500., 0., 0., 0. }, 
		       	     	       	       	     
                     { 3500., 0., 0., 0. }, 
                     { 3500., 0., 0., 0. }, 
                     { 3500., 0., 0., 0. }, 
                     { 3500., 0., 0., 0. } };

//  // hardcoded tilt position for the layers in stations 1, 2, 3 - kink position, angle
//  // v03 // tilt around each layer front
//  float rot[12][4]={ { 1500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number), 1.5/4.5 }, 
//                     { 1500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number), 1.5/4.5 }, 
//                     { 1500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number), 1.5/4.5 }, 
//                     { 1500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number), 1.5/4.5 }, 
//
//                     { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number), 2.5/6.75 }, 
//                     { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number), 2.5/6.75 }, 
//                     { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number), 2.5/6.75 }, 
//                     { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number), 2.5/6.75 }, 
//
//                     { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number), 3.5/9.0 }, 
//                     { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number), 3.5/9.0 }, 
//                     { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number), 3.5/9.0 }, 
//                     { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * Layer_number), 3.5/9.0 } };
//

//  // v04 // tilt around each station front
//  float rot[12][4]={ { 1500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 1.5/4.5 }, 
//                     { 1500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 1.5/4.5 }, 
//                     { 1500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 1.5/4.5 }, 
//                     { 1500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 1.5/4.5 }, 
//	   	        
//                     { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 2.5/6.75 }, 
//                     { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 2.5/6.75 }, 
//                     { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 2.5/6.75 }, 
//                     { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 2.5/6.75 }, 
//	   	        
//                     { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 3.5/9.0 }, 
//                     { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 3.5/9.0 }, 
//                     { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 3.5/9.0 }, 
//                     { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 3.5/9.0 } };

//  // v05 // tilt around each layer front
//  float rot[12][4]={ { 1500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 1.5/4.5 }, 
//                     { 1500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 1.5/4.5 }, 
//                     { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 2), 1.5/4.5 }, 
//                     { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 2), 1.5/4.5 }, 
//	   	        
//                     { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 2.5/6.75 }, 
//                     { 2500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 2.5/6.75 }, 
//                     { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 2), 2.5/6.75 }, 
//                     { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 2), 2.5/6.75 }, 
//	   	        
//                     { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 3.5/9.0 }, 
//                     { 3500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 0), 3.5/9.0 }, 
//                     { 4500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 2), 3.5/9.0 }, 
//                     { 4500., 0., z_offset - Layer_thickness/2. + (Layer_pitch * 2), 3.5/9.0 } };

  float ve1[3];   // buffer vector
  float ve2[3];   // buffer vector

// rotation around y-axis
//    /   cos phi   0   sin phi \
//    |   0         1   0       |
//    \ - sin phi   0   cos phi /

  // x, y, z position vector
  float pos[3] = { Position_Station1[i][0], Position_Station1[i][1], z_offset + (Layer_pitch*Layer_number) };

  // determin sign of x-coordinate
  int sign = 1;
  if  ( pos[0] < 0 ) sign = -1;

  if (abs(pos[0]) > rot[4*(Station_number-1)+Layer_number][0])  // if module located in tilt area
  {
    //    float phi = rho[Station_number-1] * sign;  // +x side : tilt +angle // -x side : tilt -angle
    float phi = rot[4*(Station_number-1)+Layer_number][3] * sign;

    //    printf("op1 %f %f %f\n", pos[0], pos[1], pos[2]);

    // vector from rotation point to detector center
    ve1[0] = pos[0] - sign * rot[4*(Station_number-1)+Layer_number][0];
    ve1[1] = pos[1] -        rot[4*(Station_number-1)+Layer_number][1];
    ve1[2] = pos[2] -        rot[4*(Station_number-1)+Layer_number][2];

    //    ve1[0] = pos[0] - sign * rot[Station_number-1][0];
    //    ve1[1] = pos[1] -        rot[Station_number-1][1];
    //    ve1[2] = pos[2] -        rot[Station_number-1][2];

    // rotation around angle
    ve2[0] =  cos(phi) * ve1[0] +           sin(phi) * ve1[2];
    ve2[1] =                      ve1[1]                     ;
    ve2[2] = -sin(phi) * ve1[0] +           cos(phi) * ve1[2];

    // add rotated vector to rotation point
    pos[0] = sign * rot[4*(Station_number-1)+Layer_number][0] + ve2[0];
    pos[1] =        rot[4*(Station_number-1)+Layer_number][1] + ve2[1];
    pos[2] =        rot[4*(Station_number-1)+Layer_number][2] + ve2[2];

//    pos[0] = sign * rot[Station_number-1][Layer_number][0] + ve2[0];
//    pos[1] =        rot[Station_number-1][Layer_number][1] + ve2[1];
//    pos[2] =        rot[Station_number-1][Layer_number][2] + ve2[2];

//    printf("op2 %f %f %f\n", ve1[0], ve1[1], ve1[2]);
//    printf("op3 %f %f %f\n", ve2[0], ve2[1], ve2[2]);
//    printf("op4 %f %f %f\n", pos[0], pos[1], pos[2]);

    fprintf(geofile,"%f %f %f\n", pos[0], pos[1], pos[2]);
    fprintf(geofile,"%f %f %f %f %f %f %f %f %f\n", 
   	     cos(phi), 0.,  sin(phi), 0., 1., 0., -sin(phi), 0.,  cos(phi) );

  }
  else if (Position_Station1[i][3] == 0) {
    //horizontal position
    fprintf(geofile,"%f %f %f\n", pos[0], pos[1], pos[2]);
    //    fprintf(geofile,"%f %f %f\n", Position_Station1[i][0], Position_Station1[i][1], 0.);
    fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
  } 
  else {
    // vertical position
    fprintf(geofile,"%f %f %f\n", pos[0], pos[1], pos[2]);
    //    fprintf(geofile,"%f %f %f\n", Position_Station1[i][0], Position_Station1[i][1], 0.);
    fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
  }

}

//--------------------------------------------------------------------
int TrdModules1(int Station_number, int Layer_number, float* frame_width_array, float Layer_thickness, float Layer_pitch, int Chamber_number, float Position_Station1[][4], float* Detector_size_x, float* Detector_size_y) {

  // create box of air with size of 'Detector_size_x[0] x Detector_size_y[0]' and fill
  // this box with the
  // radiator, mylar foil, gas, pad plane which have a size of 'Active_area_x[0] x Active_area_y[0]'  
  // the frames which have a width of 'frame_width' and are at the outer edges of the box

  bool four_digits = true;    // use 4-digit copy notation encoding the layer
  //  bool four_digits = false;   // use 4-digit copy notation encoding the layer

  bool first_time_Module1 = true;
  bool first_time_Module2 = true;
  bool first_time_Module3 = true;

  int Copy_number_Module1 = 1;
  int Copy_number_Module2 = 1;
  int Copy_number_Module3 = 1;
  int copy_number;

  float  Active_area_x[3];
  float  Active_area_y[3];
  
//  Active_area_x[0] = Detector_size_x[0] - frame_width;
//  Active_area_y[0] = Detector_size_x[0] / 2 - frame_width;
//  Active_area_x[1] = Detector_size_x[1] - frame_width;
//  Active_area_y[1] = Detector_size_x[1] / 2 - frame_width;
//  Active_area_x[2] = Detector_size_x[2] - frame_width;
//  Active_area_y[2] = Detector_size_x[2] / 2 - frame_width;
  // squared 
  Active_area_x[0] = Detector_size_x[0] /2 - frame_width_array[0];
  Active_area_y[0] = Detector_size_y[0] /2 - frame_width_array[0];
  Active_area_x[1] = Detector_size_x[1] /2 - frame_width_array[1];
  Active_area_y[1] = Detector_size_y[1] /2 - frame_width_array[1];
  Active_area_x[2] = Detector_size_x[2] /2 - frame_width_array[2];
  Active_area_y[2] = Detector_size_y[2] /2 - frame_width_array[2];
 

  // 12 mm gas (from Jun10) - intelligent
  //  float radiator_thickness    =  29.000 /2.;
  // v12a
  float radiator_thickness    =  360.000 /2.;
  float radiator_position     =  radiator_thickness - Layer_thickness /2.;
  float gas_thickness         =  12.000 /2.;
  //  float gas_thickness         =   6.000 /2.;
  float gas_position          =  radiator_position + radiator_thickness + gas_thickness;
  float padplane_thickness    =   0.030 /2.;
  float padplane_position     =  gas_position + gas_thickness + padplane_thickness;
  float mylar_thickness       =   1.500 /2.;
  float mylar_position        =  padplane_position + padplane_thickness + mylar_thickness;
  float electronics_thickness =   0.070 /2.;
  float electronics_position  =  mylar_position + mylar_thickness + electronics_thickness;
  float frame_thickness       =  radiator_thickness + gas_thickness + padplane_thickness + mylar_thickness + electronics_thickness;
  float frame_position        =  frame_thickness - Layer_thickness /2.;

//  // 12 mm gas (Jun10)
//  float radiator_thickness    =  29. /2.;
//  float radiator_position     =  14.5 - Layer_thickness /2.;
//  float gas_thickness         =  12. /2.;
//  float gas_position          =  35. - Layer_thickness /2.;
//  float padplane_thickness    =   0.03 /2.;
//  float padplane_position     =  41.015 - Layer_thickness /2.;
//  float mylar_thickness       =   1.5 /2.;
//  float mylar_position        =  41.78 - Layer_thickness /2.;
//  float electronics_thickness =   0.07 /2.;
//  float electronics_position  =  42.565 - Layer_thickness /2.;
//  float frame_thickness       =  42.6 /2.;
//  float frame_position        =  21.3 - Layer_thickness /2.;
//
//  //  6 mm gas (Jun10)
//  float radiator_thickness    =  29. /2.;
//  float radiator_position     =  14.5 - Layer_thickness /2.;
//  float gas_thickness         =   6. /2.;
//  float gas_position          =  32. - Layer_thickness /2.;
//  float padplane_thickness    =   0.03 /2.;
//  float padplane_position     =  35.015 - Layer_thickness /2.;
//  float mylar_thickness       =   1.5 /2.;
//  float mylar_position        =  35.78 - Layer_thickness /2.;
//  float electronics_thickness =   0.07 /2.;
//  float electronics_position  =  36.565 - Layer_thickness /2.;
//  float frame_thickness       =  36.6 /2.;
//  float frame_position        =  18.3 - Layer_thickness /2.;

//-//  // classical approach
//-//  float radiator_thickness    =  14.5;
//-//  float radiator_position     = -15.5 - 45.;
//-//  float gas_thickness         =   6.;
//-//  float gas_position          =   5. - 45.;
//-//  float padplane_thickness    =   0.015;
//-//  float padplane_position     =  11.015 - 45.;
//-//  float mylar_thickness       =   0.75;
//-//  float mylar_position        =  11.78 - 45.;
//-//  float electronics_thickness =   0.035;
//-//  float electronics_position  =  12.565 - 45;
//-//  float frame_thickness       =  21.3;
//-//  float frame_position        = -53.7;   // -75 + 21.3 = -53.7

//-//  // prior to Jun10 - 6 mm
//-//  float radiator_thickness = 14.5;
//-//  float radiator_position = -15.5;
//-//  float gas_thickness = 3.;
//-//  float gas_position = 2.;
//-//  float padplane_thickness = 0.015;
//-//  float padplane_position = 5.015;
//-//  float mylar_thickness = 0.75;
//-//  float mylar_position = 5.78;
//-//  float electronics_thickness = 0.035;
//-//  float electronics_position = 6.565;
//-//  float frame_thickness = 18.300;
//-//  float frame_position = -11.7;   // -30 + 18.3 = -11,7

  printf("%7.3f half radiator_thickness    \n", radiator_thickness   );
  printf("%7.3f      radiator_position     \n", radiator_position    );
  printf("%7.3f half gas_thickness         \n", gas_thickness        );
  printf("%7.3f      gas_position          \n", gas_position         );
  printf("%7.3f half padplane_thickness    \n", padplane_thickness   );
  printf("%7.3f      padplane_position     \n", padplane_position    );
  printf("%7.3f half mylar_thickness       \n", mylar_thickness      );
  printf("%7.3f      mylar_position        \n", mylar_position       );
  printf("%7.3f half electronics_thickness \n", electronics_thickness);
  printf("%7.3f      electronics_position  \n", electronics_position );
  printf("%7.3f half frame_thickness       \n", frame_thickness      );
  printf("%7.3f      frame_position        \n", frame_position       );

  for (int i=0; i< Chamber_number; i++){

    int   module_number = (int)Position_Station1[i][2];
    int   j = (int)Position_Station1[i][2]-1;
    float frame_width =  frame_width_array[module_number-1];

// define modules only for first layer per station
// requires 4-digit notation
    if ( (first_time_Module1 && ( module_number == 1 ) && ( Layer_number == 0 )) ||
         (first_time_Module2 && ( module_number == 2 ) && ( Layer_number == 0 )) ||
         (first_time_Module3 && ( module_number == 3 ) && ( Layer_number == 0 ))) {

//    if ( (first_time_Module1 && ( module_number == 1 )) ||
//         (first_time_Module2 && ( module_number == 2 )) ||
//         (first_time_Module3 && ( module_number == 3 ))) {

      if (first_time_Module1 && ( module_number == 1 ) ) {
        copy_number = Copy_number_Module1;
      }
      if (first_time_Module2 && ( module_number == 2 ) ) {
        copy_number = Copy_number_Module2;
      }
      if (first_time_Module3 && ( module_number == 3 ) ) {
        copy_number = Copy_number_Module3;
      }
  
      printf("Bin zum ersten mal hier. \n");

      fprintf(geofile,"//*********************************\n");

      if (four_digits)
        // module numbering by 4-digit copy number
        fprintf(geofile,"trd%dmod%d#%d%03d\n",Station_number, module_number, Layer_number+1, copy_number);
      else
        // module numbering by layer digit in module name
        fprintf(geofile,"trd%dmod%dl%d#%d\n",Station_number, module_number, Layer_number+1, copy_number);

      fprintf(geofile,"trd%d\n",Station_number);
      // do not use layers anymore
      //      fprintf(geofile,"trd%dlayer#%d\n",Station_number, Layer_number+1);
      fprintf(geofile,"BOX\n");
      fprintf(geofile,"air\n");
      fprintf(geofile,"%f %f %f\n", Detector_size_x[j] /2 ,-Detector_size_y[j] /2 , -Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n", Detector_size_x[j] /2 , Detector_size_y[j] /2 , -Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] /2 , Detector_size_y[j] /2 , -Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] /2 ,-Detector_size_y[j] /2 , -Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n", Detector_size_x[j] /2 ,-Detector_size_y[j] /2 ,  Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n", Detector_size_x[j] /2 , Detector_size_y[j] /2 ,  Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] /2 , Detector_size_y[j] /2 ,  Layer_thickness/2);
      fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] /2 ,-Detector_size_y[j] /2 ,  Layer_thickness/2);
      //      fprintf(geofile,"%f %f %f\n", Position_Station1[i][0], Position_Station1[i][1], 0.);

      Tiltandshift(Station_number, Layer_number, Layer_thickness, Layer_pitch, Position_Station1, i);

      fprintf(geofile,"//*********************************\n");
  
  
      fprintf(geofile,"//*********************************\n");
      fprintf(geofile,"trd%dmod%dradiator\n",Station_number, module_number);

      if (four_digits)
        // module numbering by 4-digit copy number
        fprintf(geofile,"trd%dmod%d#%d%03d\n",Station_number, module_number, Layer_number+1, copy_number);
      else
        // module numbering by layer digit in module name
        fprintf(geofile,"trd%dmod%dl%d#%d\n",Station_number, module_number, Layer_number+1, copy_number);

      fprintf(geofile,"BOX\n");
      //      fprintf(geofile,"polypropylene\n");
      fprintf(geofile,"pefoam20\n");  // v12a
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] ,-Active_area_y[j] , -radiator_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] , Active_area_y[j] , -radiator_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] , Active_area_y[j] , -radiator_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , -radiator_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] ,-Active_area_y[j] ,  radiator_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] , Active_area_y[j] ,  radiator_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] , Active_area_y[j] ,  radiator_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] ,  radiator_thickness);
      fprintf(geofile,"%f %f %f\n",0. ,0. ,radiator_position);
      if (Position_Station1[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      fprintf(geofile,"//*********************************\n");
      fprintf(geofile,"trd%dmod%dgas\n",Station_number, module_number);

      if (four_digits)
        // module numbering by 4-digit copy number
        fprintf(geofile,"trd%dmod%d#%d%03d\n",Station_number, module_number, Layer_number+1, copy_number);
      else
        // module numbering by layer digit in module name
        fprintf(geofile,"trd%dmod%dl%d#%d\n",Station_number, module_number, Layer_number+1, copy_number);

      fprintf(geofile,"BOX\n");
      fprintf(geofile,"TRDgas\n");
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] ,-Active_area_y[j] , -gas_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] , Active_area_y[j] , -gas_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] , Active_area_y[j] , -gas_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , -gas_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] ,-Active_area_y[j] ,  gas_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] , Active_area_y[j] ,  gas_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] , Active_area_y[j] ,  gas_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] ,  gas_thickness);
      fprintf(geofile,"%f %f %f\n", 0., 0., gas_position);
      if (Position_Station1[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      fprintf(geofile,"//*********************************\n");
      fprintf(geofile,"trd%dmod%dpadplane\n",Station_number, module_number);

      if (four_digits)
        // module numbering by 4-digit copy number
        fprintf(geofile,"trd%dmod%d#%d%03d\n",Station_number, module_number, Layer_number+1, copy_number);
      else
        // module numbering by layer digit in module name
        fprintf(geofile,"trd%dmod%dl%d#%d\n",Station_number, module_number, Layer_number+1, copy_number);

      fprintf(geofile,"BOX\n");
      fprintf(geofile,"goldcoatedcopper\n");
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] ,-Active_area_y[j] ,- padplane_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] , Active_area_y[j] , -padplane_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] , Active_area_y[j] ,- padplane_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , -padplane_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] ,-Active_area_y[j] ,  padplane_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] , Active_area_y[j] ,  padplane_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] , Active_area_y[j] ,  padplane_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] ,  padplane_thickness);
      fprintf(geofile,"%f %f %f\n", 0., 0., padplane_position);
      if (Position_Station1[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      fprintf(geofile,"//*********************************\n");
      fprintf(geofile,"trd%dmod%dmylar\n",Station_number, module_number);

      if (four_digits)
        // module numbering by 4-digit copy number
        fprintf(geofile,"trd%dmod%d#%d%03d\n",Station_number, module_number, Layer_number+1, copy_number);
      else
        // module numbering by layer digit in module name
        fprintf(geofile,"trd%dmod%dl%d#%d\n",Station_number, module_number, Layer_number+1, copy_number);

      fprintf(geofile,"BOX\n");
      fprintf(geofile,"mylar\n");
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] ,-Active_area_y[j] , -mylar_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] , Active_area_y[j] , -mylar_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] , Active_area_y[j] , -mylar_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , -mylar_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] ,-Active_area_y[j] ,  mylar_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] , Active_area_y[j] ,  mylar_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] , Active_area_y[j] ,  mylar_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] ,  mylar_thickness);
      fprintf(geofile,"%f %f %f\n", 0., 0., mylar_position);
      if (Position_Station1[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      fprintf(geofile,"//*********************************\n");
      fprintf(geofile,"trd%dmod%delectronics\n",Station_number, module_number);

      if (four_digits)
        // module numbering by 4-digit copy number
        fprintf(geofile,"trd%dmod%d#%d%03d\n",Station_number, module_number, Layer_number+1, copy_number);
      else
        // module numbering by layer digit in module name
        fprintf(geofile,"trd%dmod%dl%d#%d\n",Station_number, module_number, Layer_number+1, copy_number);

      fprintf(geofile,"BOX\n");
      fprintf(geofile,"goldcoatedcopper\n");
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] ,-Active_area_y[j] , -electronics_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] , Active_area_y[j] , -electronics_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] , Active_area_y[j] , -electronics_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] , -electronics_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] ,-Active_area_y[j] ,  electronics_thickness);
      fprintf(geofile,"%f %f %f\n", Active_area_x[j] , Active_area_y[j] ,  electronics_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] , Active_area_y[j] ,  electronics_thickness);
      fprintf(geofile,"%f %f %f\n",-Active_area_x[j] ,-Active_area_y[j] ,  electronics_thickness);
      fprintf(geofile,"%f %f %f\n", 0., 0., electronics_position);
      if (Position_Station1[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      if ( frame_width > 0.) {
        fprintf(geofile,"//*********************************\n");
        fprintf(geofile,"trd%dmod%dframe1#1\n",Station_number, module_number);

      if (four_digits)
        // module numbering by 4-digit copy number
        fprintf(geofile,"trd%dmod%d#%d%03d\n",Station_number, module_number, Layer_number+1, copy_number);
      else
        // module numbering by layer digit in module name
        fprintf(geofile,"trd%dmod%dl%d#%d\n",Station_number, module_number, Layer_number+1, copy_number);

        fprintf(geofile,"BOX\n");
        fprintf(geofile,"G10\n");
        fprintf(geofile,"%f %f %f\n", Detector_size_x[j] /2 ,-frame_width/2 , -frame_thickness);
        fprintf(geofile,"%f %f %f\n", Detector_size_x[j] /2 , frame_width/2 , -frame_thickness);
        fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] /2 , frame_width/2 , -frame_thickness);
        fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] /2 ,-frame_width/2 , -frame_thickness);
        fprintf(geofile,"%f %f %f\n", Detector_size_x[j] /2 ,-frame_width/2 ,  frame_thickness);
        fprintf(geofile,"%f %f %f\n", Detector_size_x[j] /2 , frame_width/2 ,  frame_thickness);
        fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] /2 , frame_width/2 ,  frame_thickness);
        fprintf(geofile,"%f %f %f\n",-Detector_size_x[j] /2 ,-frame_width/2 ,  frame_thickness);
        fprintf(geofile,"%f %f %f\n", 0., (Active_area_y[j] + frame_width / 2 ), frame_position);
        if (Position_Station1[i][3] == 0) {
          //horizontal position
          fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        } 
        else {
          // vertical position
          fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
        }

        fprintf(geofile,"//*********************************\n");
  
        fprintf(geofile,"//*********************************\n");
        fprintf(geofile,"trd%dmod%dframe1#2\n",Station_number, module_number);

      if (four_digits)
        // module numbering by 4-digit copy number
        fprintf(geofile,"trd%dmod%d#%d%03d\n",Station_number, module_number, Layer_number+1, copy_number);
      else
        // module numbering by layer digit in module name
        fprintf(geofile,"trd%dmod%dl%d#%d\n",Station_number, module_number, Layer_number+1, copy_number);

        fprintf(geofile,"%f %f %f\n", 0., -(Active_area_y[j] + frame_width / 2 ), frame_position);
        if (Position_Station1[i][3] == 0) {
          //horizontal position
          fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        } 
        else {
          // vertical position
          fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
        }
        fprintf(geofile,"//*********************************\n");
  
  
        fprintf(geofile,"//*********************************\n");
        fprintf(geofile,"trd%dmod%dframe2#1\n",Station_number, module_number);

      if (four_digits)
        // module numbering by 4-digit copy number
        fprintf(geofile,"trd%dmod%d#%d%03d\n",Station_number, module_number, Layer_number+1, copy_number);
      else
        // module numbering by layer digit in module name
        fprintf(geofile,"trd%dmod%dl%d#%d\n",Station_number, module_number, Layer_number+1, copy_number);

        fprintf(geofile,"BOX\n");
        fprintf(geofile,"G10\n");
        fprintf(geofile,"%f %f %f\n",  frame_width/2, -Active_area_y[j], -frame_thickness);
        fprintf(geofile,"%f %f %f\n",  frame_width/2,  Active_area_y[j], -frame_thickness);
        fprintf(geofile,"%f %f %f\n", -frame_width/2,  Active_area_y[j], -frame_thickness);
        fprintf(geofile,"%f %f %f\n", -frame_width/2, -Active_area_y[j], -frame_thickness);
        fprintf(geofile,"%f %f %f\n",  frame_width/2, -Active_area_y[j],  frame_thickness);
        fprintf(geofile,"%f %f %f\n",  frame_width/2,  Active_area_y[j],  frame_thickness);
        fprintf(geofile,"%f %f %f\n", -frame_width/2,  Active_area_y[j],  frame_thickness);
        fprintf(geofile,"%f %f %f\n", -frame_width/2, -Active_area_y[j],  frame_thickness);
        fprintf(geofile,"%f %f %f\n", (Active_area_x[j] + frame_width / 2 ), 0., frame_position);
        if (Position_Station1[i][3] == 0) {
          //horizontal position
          fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        } 
        else {
          // vertical position
          fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
        }
        fprintf(geofile,"//*********************************\n");
  
        fprintf(geofile,"//*********************************\n");
        fprintf(geofile,"trd%dmod%dframe2#2\n",Station_number, module_number);

      if (four_digits)
        // module numbering by 4-digit copy number
        fprintf(geofile,"trd%dmod%d#%d%03d\n",Station_number, module_number, Layer_number+1, copy_number);
      else
        // module numbering by layer digit in module name
        fprintf(geofile,"trd%dmod%dl%d#%d\n",Station_number, module_number, Layer_number+1, copy_number);

        fprintf(geofile,"%f %f %f\n", -(Active_area_x[j] + frame_width / 2 ), 0., frame_position);
        if (Position_Station1[i][3] == 0) {
          //horizontal position
          fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        } 
        else {
          // vertical position
          fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
        }

        fprintf(geofile,"//*********************************\n");
      }

      if (first_time_Module1 && ( module_number == 1 )) {
        first_time_Module1=false;
        Copy_number_Module1 = copy_number+1 ;
      }
      if (first_time_Module2 && ( module_number == 2 ) ) {
        first_time_Module2=false;
        Copy_number_Module2 = copy_number+1 ;
      }
      if (first_time_Module3 && ( module_number == 3 ) ) {
        first_time_Module3=false;
        Copy_number_Module3 = copy_number+1 ;
      }

    }
    else {

      if ( module_number == 1 ) {
        copy_number = Copy_number_Module1;
      } 
      else if ( module_number == 2 ) {
        copy_number = Copy_number_Module2;
      } 
      else if ( module_number == 3 ) {
        copy_number = Copy_number_Module3;
      } 
      else {
        printf("There is something wrong. We don't have more than three different detector sizes\n");
        printf("Break in TrdModules1.\n");
        return 1;
      }


      fprintf(geofile,"//*********************************\n");

      if (four_digits)
        // module numbering by 4-digit copy number
        fprintf(geofile,"trd%dmod%d#%d%03d\n",Station_number, module_number, Layer_number+1, copy_number);
      else
        // module numbering by layer digit in module name
        fprintf(geofile,"trd%dmod%dl%d#%d\n",Station_number, module_number, Layer_number+1, copy_number);

      fprintf(geofile,"trd%d\n",Station_number);
      // do not use layers anymore
      //      fprintf(geofile,"trd%dlayer#%d\n",Station_number, Layer_number+1);

      Tiltandshift(Station_number, Layer_number, Layer_thickness, Layer_pitch, Position_Station1, i);

      fprintf(geofile,"//*********************************\n");
  
      if ( module_number == 1 ) {
        Copy_number_Module1 = copy_number+1 ;
      }
      if ( module_number == 2 ) {
        Copy_number_Module2 = copy_number+1 ;
      }
      if ( module_number == 3 ) {
        Copy_number_Module3 = copy_number+1 ;
      }
  
    }

  }
  
  return 0;

}




int main(void)
{
  const char* c_str();

 //-----------Files------------------------------------------------
  string path = "./";
  //  string geoname  = path + "trd_segmented.geo" ;
  //  string geoname  = path + "trd_squared_modules_jun10.geo" ;
  //  string geoname  = path + "trd_squared_modules_jun10_150mm_pitch.geo" ;
  //
  //  string geoname  = path + "trd_squared_modules_jul10_v01.geo" ; // 12 mm gas
  //  string geoname  = path + "trd_squared_modules_jul10_v02.geo" ; //  6 mm gas
  //  string geoname  = path + "trd_squared_modules_jul10_v03.geo" ; // tilted, 6 mm gas
  //  string geoname  = path + "trd_squared_modules_jul10_v04.geo" ; // tilted, aligned, 6 mm gas
  //  string geoname  = path + "trd_squared_modules_jul10_v05.geo" ; // tilted layer pairs, 6 mm gas
  //  string geoname  = path + "trd_may11_v11a.geo" ; // test
  //  string infoname = path + "trd_may11_v11a.geo.info";
  string geoname  = path + "trd_v12a.geo" ; // test
  string infoname = path + "trd_v12a.geo.info";
  //  string parametername  = path + "trd_segmented.txt";

  geofile = fopen(geoname.c_str(),  "w+");
  infofile = fopen(infoname.c_str(), "w+");
  //  parameterfile = fopen(parametername.c_str(),  "w+");


  //---------- Declaration of parameters ---------------------------------------
  float Distance[3];         // Distance between target an the front of the TRD stations
  float Station_thickness;   // Thickness of one TRD station
  float Layer_thickness;     // Thickness of one layer of a TRD station
  float Layer_pitch;         // Distance between 2 adjacent layers of a TRD station
  int   Layer_number;        // Number of detector layers per station
  int   Station_number;      // Number of TRD stations in the setup
  float Detector_size_x[3];  // length in mm of a detector module in x-direction       
  float Detector_size_y[3];  // length in mm of a detector module in y-direction       
  //  float Frame_width;         // Width of detector frames in mm
  float Frame_width[3];      // Width of detector frames in mm
  float Inner_radius[3];     // Inner acceptance in mm
  float Outer_radius[3];     // Outer acceptance in mm

  //--------- Initialization of parameters -------------------------------------  

// moving closer to RICH and MUCH
  Distance[0]          = 4500;
  Distance[1]          = 6750;
  Distance[2]          = 9000;
//// with 150 mm thick layers
//  Distance[0]          = 5000;
//  Distance[1]          = 7000;
//  Distance[2]          = 9000;
//// with  60 mm thick layers (Dec09)
//  Distance[0]          = 5000;
//  Distance[1]          = 7250;
//  Distance[2]          = 9500;
//// in 2006
//  Distance[0]          = 4000;
//  Distance[1]          = 6750;
//  Distance[2]          = 9500;
  Station_number       = 3;
  Layer_number         = 4;
  //  Layer_thickness      = 60;
  //  Layer_thickness      = 135;   // adapt layer thickness to TRD prototypes (Jun10)
  //  Layer_pitch          = 150;   // Distance between 2 adjacent layers of a TRD station
  // v12a
  Layer_thickness      = 495;   // adapt layer thickness to TRD prototypes (Jun10)
  Layer_pitch          = 500;   // Distance between 2 adjacent layers of a TRD station
  //  Station_thickness    = Layer_number * Layer_thickness;
  Station_thickness    = Layer_number * Layer_pitch;
  float Inner_acceptance[3]  = {50, 50, 50};
  float Outer_acceptance[3]  = {500, 500, 500};
  // DE with frames
  Frame_width[0]        = 15;
  Frame_width[1]        = 15;
  Frame_width[2]        = 20;
  //  Frame_width          = 20;
  //  Frame_width          = 0;

  //--------- Basic calculations ----------------------------------------------
  // Here the size of the detector is calculated on the basis of the acceptance
  // which ranges from 50 to 500 mrad.
  // There is also a check if the detector station overlaps with the beampipe
  // Some basic information of the setup are written to the geofile 

  fprintf(geofile,"//*********************************************************\n");
  fprintf(geofile,"// TRD Jul10 geometry by DE\n");
  fprintf(geofile,"//*********************************************************\n");
  fprintf(geofile,"// stations located at 4500 / 6750 / 9000 mm in z\n");
  fprintf(geofile,"// with frontside of radiators of the 1st layer  \n");
  fprintf(geofile,"// v01 - 12 mm gas thickness		            \n");
  fprintf(geofile,"// v02 -  6 mm gas thickness (standard)          \n");
  fprintf(geofile,"// v03 - tilted, 6 mm gas thickness              \n");
  fprintf(geofile,"// v04 - tilted, aligned, 6 mm gas thickness     \n");
  fprintf(geofile,"// v05 - tilted layer pairs, 6 mm gas thickness  \n");
  fprintf(geofile,"//*********************************************************\n");
  fprintf(geofile,"//*********************************************************\n");
  fprintf(geofile,"// This is the geometry file of the following configuration\n");
  fprintf(geofile,"// Number of stations: %d\n",Station_number);
  fprintf(geofile,"// Number of layers per station: %d\n",Layer_number);
  fprintf(geofile,"//*********************************************************\n");

// 29.06.2010 - DE - hardcoded keeping volumes
  Inner_radius[0] =  240.;  //  250 -  10
  Outer_radius[0] = 3600.;  // 4300 + 100 // v12a
  //  Outer_radius[0] = 4600.;  // 4500 + 100
  Inner_radius[1] =  240.;  //  250 -  10
  Outer_radius[1] = 4600.;  // 5700 + 100 // v12a
  //  Outer_radius[1] = 6600.;  // 6500 + 100
  Inner_radius[2] =  490.;  //  500 -  10
  Outer_radius[2] = 5600.;  // 7100 + 100 // v12a
  //  Outer_radius[2] = 8600.;  // 8500 + 100

  for(int i = 0; i < Station_number; i++) {
//    float bla=Inner_acceptance[i]/1000.;
//    float bla1=Outer_acceptance[i]/1000.;
//
//    // F.U. 20.02.07
//    // Outer radius increased by factor 1.5 to keep all modules in th
//    // keeping volume since the size in y-direction
//    // was increased by a factor of 1.5
//    // There is also a problem with the inner radius of station 2 which is
//    // to large at the standard distance from the target. this has to be lower
//    // by 12 cm
//    Inner_radius[i] = Distance[i]*tan(bla);
//    if ( i== 1 ) {
//      Inner_radius[i] = 250.;
//    }
//
//   //Outer_radius[i] = Distance[i]*tan(bla1);
//    Outer_radius[i] = Distance[i]*tan(bla1)*1.6;
//
//    float radius_beampipe = 200.5; // constant radius after RICH
//    if (Inner_radius[i] <= radius_beampipe) {
//      printf("radius beampipe: %f\n",radius_beampipe);
//      printf("inner radius trd: %f\n",Inner_radius[i]);
//      Inner_radius[i]=radius_beampipe + 5;
//      printf("Inner radius of trd station %d was inside the beam pipe. Increased radius to %f\n",
//              i+1, Inner_radius[i]);
//    }

    printf("DE inner radius trd%d:  %f\n", i+1, Inner_radius[i]);
    printf("DE outer radius trd%d: %f\n",  i+1, Outer_radius[i]);

    fprintf(geofile,"// Inner Radius of station %d: %f\n", i+1, Inner_radius[i]);
    fprintf(geofile,"// Outer Radius of station %d: %f\n", i+1, Outer_radius[i]);
    fprintf(geofile,"// z-position at the middle of the station %d: %f\n",i+1,
            Distance[i]+(Station_thickness/2));
    fprintf(geofile,"//*********************************************************\n");

  }

//  // Calculate the sizes of all three detector modules. All sizes depend on the radius
//  // of the inner acceptance of the first station.
//  // At least this is correct is the stations are at 5000, 7250 and 9500 mm. If this is
//  // still okay when the stations are shifted has to be checked.
//
//  Detector_size_x[0] = roundf(Inner_radius[0]);  // DE = 250 for z=5000
//  if ((int)Detector_size_x[0]%2) Detector_size_x[0]++;
//
//  // Calculate size of the complete detector with frames in x-direction which is the double
//  // size of the inner acceptance for geometrical reasons. The y-direction is twice as much
//  // as the length in x-direction
//
//  Detector_size_x[0] *= 2;                       // DE =  500
//  // squared, x=y
//  Detector_size_y[0] = Detector_size_x[0];   // DE = 1000
//  //  Detector_size_y[0] = Detector_size_x[0] * 2;   // DE = 1000
//
//
//  // Calculate the sizes of the lager detector modules according to the size of the smallest
//  // modules. The calculations follow the geometrical layout of the stations.
//
//  Detector_size_x[1] = Detector_size_x[0] * 3 / 2 ;  // DE =  750 
//  Detector_size_y[1] = Detector_size_y[0] * 3 / 2 ;  // DE = 1500
//
//  Detector_size_x[2] = Detector_size_x[0] * 2 ;  // DE = 1000
//  Detector_size_y[2] = Detector_size_y[0] * 2 ;  // DE = 2000


//  // fixed detector sizes, independent of z Position
//  Detector_size_x[0] =  500;                 // x =  500 mm
//  Detector_size_y[0] = Detector_size_x[0];   // y =  500 mm
//
//  Detector_size_x[1] =  750;                 // x =  750 mm 
//  Detector_size_y[1] = Detector_size_x[1];   // y =  750 mm 
//
//  Detector_size_x[2] = 1000;                 // x = 1000 mm
//  Detector_size_y[2] = Detector_size_x[2];   // y = 1000 mm


  // squared detectors
  Detector_size_x[0] =  600;  //  570;   //  600;
  Detector_size_y[0] =  600;  //  570;   //  600;
		                  
  Detector_size_x[1] =  600;  //  570;   //  600;
  Detector_size_y[1] =  600;  //  570;   //  600;
		                  
  Detector_size_x[2] = 1000;  //  950;   // 1000;
  Detector_size_y[2] = 1000;  //  950;   // 1000;


  for(int i = 0; i < Station_number; i++)
  {
    printf("DE Module %d Detector size x: %7.2f\n", i, Detector_size_x[i]);
    printf("DE Module %d Detector size y: %7.2f\n", i, Detector_size_y[i]);
  }
  printf("\n");

  
  // calculate positions of small chambers in the first station
  // This is done according the geometrical layout of this station


// ### Station 1

  //------------------------------------------------------------------
  //trd1mod1-1

  float x101 = Detector_size_x[0] *  1;
  float y101 = Detector_size_y[0] *  0;
	    	    
  float x102 = -x101;
  float y102 = -y101;
	    	    	    
  float x103 = Detector_size_x[0] *  0;
  float y103 = Detector_size_y[0] *  1;
	    	    
  float x104 = -x103;
  float y104 = -y103;
	    	    	    
  //------------------------------------------------------------------
  //trd1mod1-2

  float x105 = Detector_size_x[0] *  1;
  float y105 = Detector_size_y[0] *  1;
	    
  float x106 = Detector_size_x[0] *  1;
  float y106 = Detector_size_y[0] * -1;
	    
  float x107 = -x105;
  float y107 = -y105;
	    	    
  float x108 = -x106;
  float y108 = -y106;

  //------------------------------------------------------------------
  //trd1mod2-1

  float x201 = Detector_size_x[0] *  2;
  float y201 = Detector_size_y[0] *  0;

  float x202 = -x201;
  float y202 = -y201;

  float x203 = Detector_size_x[1] *  0;
  float y203 = Detector_size_y[1] *  2;

  float x204 = -x203;
  float y204 = -y203;
	       
  //------------------------------------------------------------------
  //trd1mod2-2

  float x205 = Detector_size_x[0] *  2;
  float y205 = Detector_size_y[0] *  1;
	                               
  float x206 = Detector_size_x[0] *  2;
  float y206 = Detector_size_y[0] * -1;
	       	    	    	       
  float x207 = -x205;		       
  float y207 = -y205;		       
	       	    	    	       
  float x208 = -x206;
  float y208 = -y206;
	                               	     	    
  float x209 = Detector_size_x[1] *  1;
  float y209 = Detector_size_y[1] *  2;
	       	     	    	       	     	    
  float x210 = Detector_size_x[1] * -1;
  float y210 = Detector_size_y[1] *  2;
	      	     	    
  float x211 = -x209;
  float y211 = -y209;
	    	     	    
  float x212 = -x210;
  float y212 = -y210;
	  	     	    
  float x213 = Detector_size_x[1] *  2;
  float y213 = Detector_size_y[1] *  2;
	    	     	    
  float x214 = Detector_size_x[1] * -2;
  float y214 = Detector_size_y[1] *  2;
	    	     	    
  float x215 = -x213;
  float y215 = -y213;
	    	     	    
  float x216 = -x214;
  float y216 = -y214;

  //------------------------------------------------------------------
  //trd1mod3-1

  float x301 = Detector_size_x[2] *  2;
  float y301 = Detector_size_y[2] *  0;
	  	    
  float x302 = -x301;
  float y302 = -y301;
	  	    
  float x303 = Detector_size_x[2] *  2;
  float y303 = Detector_size_y[2] *  1;
	  	    
  float x304 = Detector_size_x[2] *  2;
  float y304 = Detector_size_y[2] * -1;
	  	    
  float x305 = -x303;
  float y305 = -y303;
	  	    
  float x306 = -x304;
  float y306 = -y304;
	  	    
  //------------------------------------------------------------------
  //trd1mod3-1

// top and bottom row                  
  float x307 = Detector_size_x[2] *  0;
  float y307 = Detector_size_y[2] *  2;
	                               
  float x308 = -x307;
  float y308 = -y307;

  float x309 = Detector_size_x[2] *  1;
  float y309 = Detector_size_y[2] *  2;
	                               
  float x310 = Detector_size_x[2] * -1;
  float y310 = Detector_size_y[2] *  2;
	       	       
  float x311 = -x309;
  float y311 = -y309;
	       	     
  float x312 = -x310;
  float y312 = -y310;
	  	    		       	       
// vertical column no 1
  float x313 = Detector_size_x[2] *  2;
  float y313 = Detector_size_y[2] *  2;
	       			     	       
  float x314 = Detector_size_x[2] *  2;
  float y314 = Detector_size_y[2] * -2;

  float x315 = -x313;
  float y315 = -y313;
	       	     
  float x316 = -x314;
  float y316 = -y314;
	       
// vertical column no 2
  float x317 = Detector_size_x[2] *  3;
  float y317 = Detector_size_y[2] *  2;
	       	       		       	       
  float x318 = Detector_size_x[2] *  3;
  float y318 = Detector_size_y[2] *  1;
	       	    		       	       
  float x319 = Detector_size_x[2] *  3;
  float y319 = Detector_size_y[2] *  0;
	       	    		       	     	       
  float x320 = Detector_size_x[2] *  3;
  float y320 = Detector_size_y[2] * -1;
	       	    		       	       
  float x321 = Detector_size_x[2] *  3;
  float y321 = Detector_size_y[2] * -2;
	       	     	    
  float x322 = -x317;
  float y322 = -y317;
	       	     
  float x323 = -x318;
  float y323 = -y318;
	       	     
  float x324 = -x319;
  float y324 = -y319;
	       	     
  float x325 = -x320;
  float y325 = -y320;
	       	     
  float x326 = -x321;
  float y326 = -y321;
// end of v11x

// vertical column no 3
  float x327 = Detector_size_x[2] *  4;
  float y327 = Detector_size_y[2] *  2;
	       	    		      
  float x328 = Detector_size_x[2] *  4;
  float y328 = Detector_size_y[2] *  1;
	       	    		      
  float x329 = Detector_size_x[2] *  4;
  float y329 = Detector_size_y[2] *  0;
	       	    		      
  float x330 = Detector_size_x[2] *  4;
  float y330 = Detector_size_y[2] * -1;
	       	    		      
  float x331 = Detector_size_x[2] *  4;
  float y331 = Detector_size_y[2] * -2;
	  	    
  float x332 = -x327; 
  float y332 = -y327; 
	  	       
  float x333 = -x328; 
  float y333 = -y328; 
	  	       
  float x334 = -x329; 
  float y334 = -y329; 
	  	       
  float x335 = -x330; 
  float y335 = -y330; 
	  	       
  float x336 = -x331; 
  float y336 = -y331; 

  //------------------------------------------------------------------

  // This array stores all information about the geometrical layout of the first TRD
  // station. First row is x-position, second row is y-position, third row is the
  // chamber type and the last row gives information about the orientation of the
  // chamber

  //int Chamber_number_Station1 = 28;
  //int Chamber_number_Station1 =  8 + 12 + 36;   // = 56 (jul10)
  //int Chamber_number_Station1 =      24 + 36;   // = 60 (jan11)

  int Chamber_number_Station1 =      24 + 26;   // = 50 (may11)

  float Position_Station1[][4] = { {x101,y101,1,0},{x102,y102,1,0},{x103,y103,1,0},{x104,y104,1,0},
				   {x105,y105,1,0},{x106,y106,1,0},{x107,y107,1,0},{x108,y108,1,0},

				   {x201,y201,2,0},{x202,y202,2,0},{x203,y203,2,0},{x204,y204,2,0},
				   {x205,y205,2,0},{x206,y206,2,0},{x207,y207,2,0},{x208,y208,2,0},
				   {x209,y209,2,0},{x210,y210,2,0},{x211,y211,2,0},{x212,y212,2,0},
				   {x213,y213,2,0},{x214,y214,2,0},{x215,y215,2,0},{x216,y216,2,0}, // jan11

				   {x301,y301,3,0},{x302,y302,3,0},{x303,y303,3,0},{x304,y304,3,0},
				   {x305,y305,3,0},{x306,y306,3,0},{x307,y307,3,0},{x308,y308,3,0},
				   {x309,y309,3,0},{x310,y310,3,0},
				   {x311,y311,3,0},{x312,y312,3,0},{x313,y313,3,0},{x314,y314,3,0},
				   {x315,y315,3,0},{x316,y316,3,0},{x317,y317,3,0},{x318,y318,3,0},
				   {x319,y319,3,0},{x320,y320,3,0},
				   {x321,y321,3,0},{x322,y322,3,0},{x323,y323,3,0},{x324,y324,3,0},
				   {x325,y325,3,0},{x326,y326,3,0}

// remove outer column in May11
//                                                                 {x327,y327,3,0},{x328,y328,3,0},
//				   {x329,y329,3,0},{x330,y330,3,0},
//				   {x331,y331,3,0},{x332,y332,3,0},{x333,y333,3,0},{x334,y334,3,0},
//				   {x335,y335,3,0},{x336,y336,3,0}
};

  //------------------------------------------------------------------

// ### Station 2

// top and bottom	       
  float x337 = Detector_size_x[2] *  4;
  float y337 = Detector_size_y[2] *  3;
				       
  float x338 = Detector_size_x[2] *  4;
  float y338 = Detector_size_y[2] * -3;
				       
  float x339 = Detector_size_x[2] *  3;
  float y339 = Detector_size_y[2] *  3;
				       
  float x340 = Detector_size_x[2] *  3;
  float y340 = Detector_size_y[2] * -3;
				       
  float x341 = Detector_size_x[2] *  2;
  float y341 = Detector_size_y[2] *  3;
				       
  float x342 = Detector_size_x[2] *  2;
  float y342 = Detector_size_y[2] * -3;
				       
  float x343 = Detector_size_x[2] *  1;
  float y343 = Detector_size_y[2] *  3;
				       
  float x344 = Detector_size_x[2] *  1;
  float y344 = Detector_size_y[2] * -3;
				       
  float x345 = Detector_size_x[2] *  0;
  float y345 = Detector_size_y[2] *  3;
// vertical column no 4
  float x346 = Detector_size_x[2] *  5;
  float y346 = Detector_size_y[2] *  3;
				       
  float x347 = Detector_size_x[2] *  5;
  float y347 = Detector_size_y[2] *  2;
				       
  float x348 = Detector_size_x[2] *  5;
  float y348 = Detector_size_y[2] *  1;
				       
  float x349 = Detector_size_x[2] *  5;
  float y349 = Detector_size_y[2] *  0;
				       
  float x350 = Detector_size_x[2] *  5;
  float y350 = Detector_size_y[2] * -1;
				       
  float x351 = Detector_size_x[2] *  5;
  float y351 = Detector_size_y[2] * -2;
				       
  float x352 = Detector_size_x[2] *  5;
  float y352 = Detector_size_y[2] * -3;
// vertical column no 5
  // only 5 modules jan11 (was 6)

  float x353 = Detector_size_x[2] *  6;
  float y353 = Detector_size_y[2] *  2;

  float x354 = Detector_size_x[2] *  6;
  float y354 = Detector_size_y[2] *  1;

  float x355 = Detector_size_x[2] *  6;
  float y355 = Detector_size_y[2] *  0;

  float x356 = Detector_size_x[2] *  6;
  float y356 = Detector_size_y[2] * -1;

  float x357 = Detector_size_x[2] *  6;
  float y357 = Detector_size_y[2] * -2;

  //  float x358 = Detector_size_x[2] *  6;	 
  //  float y358 = Detector_size_y[2] * -2.5;

  //------------------------------------------------------------------

  //  int Chamber_number_Station2 =   50;
  //  int Chamber_number_Station2 =  100;
  //  int Chamber_number_Station2 =  Chamber_number_Station1 + 23 * 2;   // = 100 (jun10)
  //  int Chamber_number_Station2 =  Chamber_number_Station1 + 22 * 2;   // = 102 (jan11)

  int Chamber_number_Station2 =  Chamber_number_Station1 + 10 + 9 * 2;   // =  78 (may11)

  float Position_Station2[][4] = { {x101,y101,1,0},{x102,y102,1,0},{x103,y103,1,0},{x104,y104,1,0},
				   {x105,y105,1,0},{x106,y106,1,0},{x107,y107,1,0},{x108,y108,1,0},

				   {x201,y201,2,0},{x202,y202,2,0},{x203,y203,2,0},{x204,y204,2,0},
				   {x205,y205,2,0},{x206,y206,2,0},{x207,y207,2,0},{x208,y208,2,0},
				   {x209,y209,2,0},{x210,y210,2,0},{x211,y211,2,0},{x212,y212,2,0},
				   {x213,y213,2,0},{x214,y214,2,0},{x215,y215,2,0},{x216,y216,2,0}, // jan11

				   {x301,y301,3,0},{x302,y302,3,0},{x303,y303,3,0},{x304,y304,3,0},
				   {x305,y305,3,0},{x306,y306,3,0},{x307,y307,3,0},{x308,y308,3,0},
				   {x309,y309,3,0},{x310,y310,3,0},
				   {x311,y311,3,0},{x312,y312,3,0},{x313,y313,3,0},{x314,y314,3,0},
				   {x315,y315,3,0},{x316,y316,3,0},{x317,y317,3,0},{x318,y318,3,0},
				   {x319,y319,3,0},{x320,y320,3,0},
				   {x321,y321,3,0},{x322,y322,3,0},{x323,y323,3,0},{x324,y324,3,0},
				   {x325,y325,3,0},{x326,y326,3,0},{x327,y327,3,0},{x328,y328,3,0},
				   {x329,y329,3,0},{x330,y330,3,0},

				   {x331,y331,3,0},{x332,y332,3,0},{x333,y333,3,0},{x334,y334,3,0},
				   {x335,y335,3,0},{x336,y336,3,0},

  //------------------------------------------------------------------
                                                                   {x337,y337,3,0},{x338,y338,3,0},
				   {x339,y339,3,0},{x340,y340,3,0},
				   {x341,y341,3,0},{x342,y342,3,0},{x343,y343,3,0},{x344,y344,3,0},
 				   {x345,y345,3,0},

  //------------------------------------------------------------------
                                                                       {-x337,-y337,3,0},{-x338,-y338,3,0},
				   {-x339,-y339,3,0},{-x340,-y340,3,0},
				   {-x341,-y341,3,0},{-x342,-y342,3,0},{-x343,-y343,3,0},{-x344,-y344,3,0},
				   {-x345,-y345,3,0}

  //------------------------------------------------------------------

// remove outer column in May11
//				                   {x346,y346,3,0},{x347,y347,3,0},{x348,y348,3,0},
//				   {x349,y349,3,0},{x350,y350,3,0},
//				   {x351,y351,3,0},{x352,y352,3,0},{x353,y353,3,0},{x354,y354,3,0},
//				   {x355,y355,3,0},{x356,y356,3,0},{x357,y357,3,0},//{x358,y358,3,0},

  //------------------------------------------------------------------

// remove outer column in May11
//				                     {-x346,-y346,3,0},{-x347,-y347,3,0},{-x348,-y348,3,0},
//				   {-x349,-y349,3,0},{-x350,-y350,3,0},
//				   {-x351,-y351,3,0},{-x352,-y352,3,0},{-x353,-y353,3,0},{-x354,-y354,3,0},
//				   {-x355,-y355,3,0},{-x356,-y356,3,0},{-x357,-y357,3,0}//,{-x358,-y358,3,0}

};

  //------------------------------------------------------------------

// ### Station 3

  //  int Chamber_number_Station3 = 72;
  //  int Chamber_number_Station3 = 144;   // jun10
  //  int Chamber_number_Station3 = 126;   // jan11
  int Chamber_number_Station3 = 98;   // may11
  float Position_Station3[144][4];

  // v12x
  int station3[9][11] = { { 4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4 },
                          { 4,  4,  4,  3,  3,  3,  3,  3,  4,  4,  4 },
                          { 4,  4,  3,  3,  2,  2,  2,  3,  3,  4,  4 },
                          { 4,  4,  3,  2,  1,  1,  1,  2,  3,  4,  4 },
                          { 4,  4,  3,  2,  1,  0,  1,  2,  3,  4,  4 },
                          { 4,  4,  3,  2,  1,  1,  1,  2,  3,  4,  4 },
                          { 4,  4,  3,  3,  2,  2,  2,  3,  3,  4,  4 },
                          { 4,  4,  4,  3,  3,  3,  3,  3,  4,  4,  4 },
                          { 4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4 } };
  // number of modules 1x0, 8x1, 12x2, 24x3, 54x4

  int a = 0;
  for ( int type = 1; type <=4; type++)
    for ( int j = 0; j < 9; j++) 
      for ( int i = 0; i < 11; i++) 
	if (station3[j][i]==type)
          {
            int y = -(j-4);
            int x =   i-5;
            printf("a:%2d, type:%2d x:%2d, y:%2d   \n", a, type, x, y);
            Position_Station3[a][0] = Detector_size_x[2] * x;
            Position_Station3[a][1] = Detector_size_y[2] * y;
            Position_Station3[a][2] = 3;
            Position_Station3[a][3] = 0;
            a++;
          }

  //------------------------------------------------------------------
  //------------------------------------------------------------------

  for ( int counter = 0; counter < Station_number; counter++) {

    printf("Distance, Inner_radius, Outer_radius, Station_thickness: %f, %f, %f, %f \n",
            Distance[counter], Inner_radius[counter], Outer_radius[counter], Station_thickness);
    printf("Layer_number, Layer_thickness, layer_pitch, frame_width: %d, %f, %f, %f, %f, %f \n",
	   Layer_number, Layer_thickness, Layer_pitch, Frame_width[0], Frame_width[1], Frame_width[2]);
    printf("Detector_size_x %d: %f\n",counter,Detector_size_x[counter]);
  
    // The distance is measured to the start of the station. In GEANT the
    // volumes are defined from the center of the volume with half thickness
    // to either side. This has to corrected here
  
    float Distance_center = Distance[counter] + Station_thickness/2;
  
    fprintf(geofile,"//*********************************\n");
    fprintf(geofile,"trd%d\n",counter+1);
    fprintf(geofile,"cave\n");
    fprintf(geofile,"PGON\n");
    fprintf(geofile,"air\n");
//    fprintf(geofile,"2\n");
//    fprintf(geofile,"45 360 4\n");
////  fprintf(geofile,"-%f %f %f \n",Station_thickness/2,(Inner_radius[counter]-1),(Outer_radius[counter]+1010));
////  fprintf(geofile,"%f %f %f \n", Station_thickness/2,(Inner_radius[counter]-1),(Outer_radius[counter]+1010));
//    // same keeping volume size in x and y for stations and their layers
//    fprintf(geofile,"-%f %f %f \n",Station_thickness/2, Inner_radius[counter], Outer_radius[counter]);
//    fprintf(geofile,"%f %f %f \n", Station_thickness/2, Inner_radius[counter], Outer_radius[counter]);

// for tilted geometries
//
//    fprintf(geofile,"4\n");
//    fprintf(geofile,"45 360 4\n");
//    // hardcoded
//    if (counter+1 == 1) {
//    fprintf(geofile,"%f %f %f \n",-1300., 4300., 4600.);   // station 1
//    fprintf(geofile,"%f %f %f \n", -300., 1500., 4600.);   // station 1
//    }
//    if (counter+1 == 2) {
//    fprintf(geofile,"%f %f %f \n",-1800., 6200., 6600.);   // station 2
//    fprintf(geofile,"%f %f %f \n", -300., 2500., 6600.);   // station 2
//    }
//    if (counter+1 == 3) {
//    fprintf(geofile,"%f %f %f \n",-2300., 8100., 8600.);   // station 3
//    fprintf(geofile,"%f %f %f \n", -300., 3500., 8600.);   // station 3
//    }
//    // same keeping volume size in x and y for stations and their layers
//    fprintf(geofile,"-%f %f %f \n",Station_thickness/2, Inner_radius[counter], Outer_radius[counter]);
//    fprintf(geofile,"%f %f %f \n", Station_thickness/2, Inner_radius[counter], Outer_radius[counter]);
//    fprintf(geofile,"0. 0. %4.1f\n",Distance_center);
//    fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
//    fprintf(geofile,"//*********************************\n");

// for planar geometries (v12a)
    fprintf(geofile,"2\n");
    fprintf(geofile,"45 360 4\n");
    // same keeping volume size in x and y for stations and their layers
    if (counter < 2)
     {
      fprintf(geofile,"-%f %f %f \n",Station_thickness/2, Inner_radius[counter], Outer_radius[counter]);
      fprintf(geofile,"%f %f %f \n", Station_thickness/2, Inner_radius[counter], Outer_radius[counter]);
     }
    else
     {
      fprintf(geofile,"-%f %f %f \n",Station_thickness/2, Inner_radius[counter], Outer_radius[counter]);
      fprintf(geofile,"%f %f %f \n",                  0., Inner_radius[counter], Outer_radius[counter]);
     }
    fprintf(geofile,"0. 0. %4.1f\n",Distance_center);
    fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
    fprintf(geofile,"//*********************************\n");
      
    for (int i = 0; i < Layer_number; i++){

// for all 4 layers
//      if (i == 0){

// do not use layers anymore
//        fprintf(geofile,"//*********************************\n");
//        fprintf(geofile,"trd%dlayer#%d\n",counter+1, i+1);
//        fprintf(geofile,"trd%d\n",counter+1);
//        fprintf(geofile,"PGON\n");
//        fprintf(geofile,"air\n");
//        fprintf(geofile,"2\n");
//        fprintf(geofile,"45 360 4\n");
////      fprintf(geofile,"-%f %f %f \n",Layer_thickness/2,(Inner_radius[counter]-0.5),(Outer_radius[counter]+500));
////      fprintf(geofile,"%f %f %f \n", Layer_thickness/2,(Inner_radius[counter]-0.5),(Outer_radius[counter]+500));
//    // same keeping volume size in x and y for stations and their layers
//    fprintf(geofile,"-%f %f %f \n",Layer_thickness/2, Inner_radius[counter], Outer_radius[counter]);
//    fprintf(geofile,"%f %f %f \n", Layer_thickness/2, Inner_radius[counter], Outer_radius[counter]);
//	// allow variable size in calculation of layer position
//        fprintf(geofile,"0. 0. %4.1f\n",(-Station_thickness/2)+Layer_thickness/2+(Layer_pitch*i));
//        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
//        fprintf(geofile,"//*********************************\n");
  
        
        if ( counter == 0 ) {
	  if ((TrdModules1(counter+1, i , Frame_width, Layer_thickness, Layer_pitch, Chamber_number_Station1, Position_Station1, Detector_size_x, Detector_size_y ))) {
            printf("There was an error in TrdModules1\n");
            printf("Stop program\n");            
            return 1;
          }
        }
        else if ( counter == 1 ) {
	  if ((TrdModules1(counter+1, i , Frame_width, Layer_thickness, Layer_pitch, Chamber_number_Station2, Position_Station2, Detector_size_x, Detector_size_y ))) {
            printf("There was an error in TrdModules1\n");
            printf("Stop program\n");            
            return 1;
          }
	}
        else if ( counter == 2 ) {
          if (i >= 2) printf("skipping TRD3 Layer %d\n", i+1);
	  else if ((TrdModules1(counter+1, i , Frame_width, Layer_thickness, Layer_pitch, Chamber_number_Station3, Position_Station3, Detector_size_x, Detector_size_y ))) {
            printf("There was an error in TrdModules1\n");
            printf("Stop program\n");            
            return 1;
          }
	}
	  
	  
// for all 4 layers
//      }

// do not use layers anymore
//      else {
//        // copy only the first layer and shift to the right positions
//        fprintf(geofile,"//*********************************\n");
//        fprintf(geofile,"trd%dlayer#%d\n",counter+1, i+1);
//        fprintf(geofile,"trd%d\n",counter+1);
//	// allow variable size in calculation of layer position
//        fprintf(geofile,"0. 0. %4.1f\n",(-Station_thickness/2)+Layer_thickness/2+(Layer_pitch*i));
//        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
//        fprintf(geofile,"//*********************************\n");
//      }
  
    }
  }


  fclose(geofile);
  fclose(infofile);
  //  fclose(parameterfile);

}

