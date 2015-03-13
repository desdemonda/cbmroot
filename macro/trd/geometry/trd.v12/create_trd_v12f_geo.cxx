//
//   Generator for CbmTrd Geometry
//
// ToDo 20120504 - David Emschermann 
// - check tilting in new keeping volume
//
// Update 20120522 - David Emschermann 
// - v12f derived from v12b (no febs)
// - more granularity in the inner part of station2 
//
// Update 20120508 - David Emschermann 
// - introduce new module naming:
//   trd1mod[1-8]# layer type [1-3] - layer number [1-4] - serial number [001 - 999]
// - only 2 module sizes - small, 60 cm - large, 100 cm
// - all modules are in the trd1 volume
// - layer offsets in z relative to 4500 m
//
// Update 20120504 - David Emschermann 
// - define a common keeping volume for all stations of the TRD
// - move reference point to planar front of reference volume (z = 4500 mm)
//   instead of center of keeping volume
// - rearrange module position definition with arrays for stations 1-2
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
int Tiltandshift(int Station_number, int Layer_number, float Layer_thickness, float Layer_pitch, float Position_Station[][4], int i) {

  // with common keeping volume, the reference is the front layer
  
  float station_z_offset[3] = { 0., 2250., 4500. };   // distance in z between 1st layer of stations 
  float z_offset = station_z_offset[Station_number-1] + Layer_thickness/2;

  // Station_thickness is unkown here
  // float z_offset = (-4*Layer_pitch/2)+Layer_thickness/2;


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
  float pos[3] = { Position_Station[i][0], Position_Station[i][1], z_offset + (Layer_pitch*Layer_number) };  // z + (pitch * [0-3])

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
  else if (Position_Station[i][3] == 0) {
    //horizontal position
    fprintf(geofile,"%f %f %f\n", pos[0], pos[1], pos[2]);
    //    fprintf(geofile,"%f %f %f\n", Position_Station[i][0], Position_Station[i][1], 0.);
    fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
  } 
  else {
    // vertical position
    fprintf(geofile,"%f %f %f\n", pos[0], pos[1], pos[2]);
    //    fprintf(geofile,"%f %f %f\n", Position_Station[i][0], Position_Station[i][1], 0.);
    fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
  }

}

//--------------------------------------------------------------------
int TrdModules1(int Station_number, int Layer_number, float* frame_width_array, float Layer_thickness, float Layer_pitch, int Chamber_number, float Position_Station[][4], float* Detector_size_x, float* Detector_size_y) {

  // create box of air with size of 'Detector_size_x[0] x Detector_size_y[0]' and fill
  // this box with the
  // radiator, mylar foil, gas, pad plane which have a size of 'Active_area_x[0] x Active_area_y[0]'  
  // the frames which have a width of 'frame_width' and are at the outer edges of the box

  int trd_number = 1;

  bool first_time_Module1 = true;
  bool first_time_Module2 = true;
  bool first_time_Module3 = true;
  bool first_time_Module4 = true;
  bool first_time_Module5 = true;
  bool first_time_Module6 = true;
  bool first_time_Module7 = true;
  bool first_time_Module8 = true;

  int Copy_number_Module1 = 1;
  int Copy_number_Module2 = 1;
  int Copy_number_Module3 = 1;
  int Copy_number_Module4 = 1;
  int Copy_number_Module5 = 1;
  int Copy_number_Module6 = 1;
  int Copy_number_Module7 = 1;
  int Copy_number_Module8 = 1;
  int copy_number;

  float  Active_area_x[2];
  float  Active_area_y[2];
  
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

    int   module_type = (int)Position_Station[i][2];
    int   j = ((int)Position_Station[i][2]-1)/4;
    float frame_width =  frame_width_array[j];

    // for debugging only
    //    printf("mod type -> j: %d -> %d\n", module_type, j);

// define modules only for first layer per station
// requires 4-digit notation
    if ( (first_time_Module1 && ( module_type == 1 ) && ( Layer_number == 0 ) && ( Station_number == 1 )) ||
         (first_time_Module2 && ( module_type == 2 ) && ( Layer_number == 0 ) && ( Station_number == 1 )) ||
         (first_time_Module3 && ( module_type == 3 ) && ( Layer_number == 0 ) && ( Station_number == 1 )) ||
         (first_time_Module4 && ( module_type == 4 ) && ( Layer_number == 0 ) && ( Station_number == 1 )) ||
         (first_time_Module5 && ( module_type == 5 ) && ( Layer_number == 0 ) && ( Station_number == 3 )) ||
         (first_time_Module6 && ( module_type == 6 ) && ( Layer_number == 0 ) && ( Station_number == 2 )) ||
         (first_time_Module7 && ( module_type == 7 ) && ( Layer_number == 0 ) && ( Station_number == 1 )) ||
         (first_time_Module8 && ( module_type == 8 ) && ( Layer_number == 0 ) && ( Station_number == 1 ))) {

// // requires 4-digit notation
//     if ( (first_time_Module1 && ( module_type == 1 ) && ( Layer_number == 0 )) ||
//          (first_time_Module2 && ( module_type == 2 ) && ( Layer_number == 0 )) ||
//          (first_time_Module3 && ( module_type == 3 ) && ( Layer_number == 0 )) ||
//          (first_time_Module4 && ( module_type == 4 ) && ( Layer_number == 0 )) ||
//          (first_time_Module5 && ( module_type == 5 ) && ( Layer_number == 0 )) ||
//          (first_time_Module6 && ( module_type == 6 ) && ( Layer_number == 0 )) ||
//          (first_time_Module7 && ( module_type == 7 ) && ( Layer_number == 0 )) ||
//          (first_time_Module8 && ( module_type == 8 ) && ( Layer_number == 0 ))) {

//    if ( (first_time_Module1 && ( module_type == 1 )) ||
//         (first_time_Module2 && ( module_type == 2 )) ||
//         (first_time_Module3 && ( module_type == 3 )) ||
//         (first_time_Module4 && ( module_type == 4 )) ||
//         (first_time_Module5 && ( module_type == 5 )) ||
//         (first_time_Module6 && ( module_type == 6 )) ||
//         (first_time_Module7 && ( module_type == 7 )) ||
//         (first_time_Module8 && ( module_type == 8 ))) {

      if (first_time_Module1 && ( module_type == 1 ) ) {
        copy_number = Copy_number_Module1;
      }
      if (first_time_Module2 && ( module_type == 2 ) ) {
        copy_number = Copy_number_Module2;
      }
      if (first_time_Module3 && ( module_type == 3 ) ) {
        copy_number = Copy_number_Module3;
      }
      if (first_time_Module4 && ( module_type == 4 ) ) {
        copy_number = Copy_number_Module4;
      }
      if (first_time_Module5 && ( module_type == 5 ) ) {
        copy_number = Copy_number_Module5;
      }
      if (first_time_Module6 && ( module_type == 6 ) ) {
        copy_number = Copy_number_Module6;
      }
      if (first_time_Module7 && ( module_type == 7 ) ) {
        copy_number = Copy_number_Module7;
      }
      if (first_time_Module8 && ( module_type == 8 ) ) {
        copy_number = Copy_number_Module8;
      }
  
      //      printf("Bin zum ersten mal hier. \n");
      printf("Bin zum ersten mal hier, type: %d, size %d. \n", module_type, j);

      fprintf(geofile,"//*********************************\n");

      // module numbering by 4-digit copy number
      //      fprintf(geofile,"trd%dmod%d#%d%03d\n", Station_number, module_type, Layer_number+1, copy_number);
      //      fprintf(geofile,"trd%d\n", Station_number);
      fprintf(geofile,"trd%dmod%d#%d%d%03d\n", trd_number, module_type, Station_number, Layer_number+1, copy_number);
      fprintf(geofile,"trd%d\n", trd_number);

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
      //      fprintf(geofile,"%f %f %f\n", Position_Station[i][0], Position_Station[i][1], 0.);

      Tiltandshift(Station_number, Layer_number, Layer_thickness, Layer_pitch, Position_Station, i);

      fprintf(geofile,"//*********************************\n");
  
  
      fprintf(geofile,"//*********************************\n");
      //      fprintf(geofile,"trd%dmod%dradiator\n", Station_number, module_type);
      fprintf(geofile,"trd%dmod%dradiator\n", trd_number, module_type);
      // module numbering by 4-digit copy number
      //      fprintf(geofile,"trd%dmod%d#%d%03d\n", Station_number, module_type, Layer_number+1, copy_number);
      fprintf(geofile,"trd%dmod%d#%d%d%03d\n", trd_number, module_type, Station_number, Layer_number+1, copy_number);

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
      if (Position_Station[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      fprintf(geofile,"//*********************************\n");
      //      fprintf(geofile,"trd%dmod%dgas\n", Station_number, module_type);
      fprintf(geofile,"trd%dmod%dgas\n", trd_number, module_type);
      // module numbering by 4-digit copy number
      //      fprintf(geofile,"trd%dmod%d#%d%03d\n", Station_number, module_type, Layer_number+1, copy_number);
      fprintf(geofile,"trd%dmod%d#%d%d%03d\n", trd_number, module_type, Station_number, Layer_number+1, copy_number);

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
      if (Position_Station[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      fprintf(geofile,"//*********************************\n");
      //      fprintf(geofile,"trd%dmod%dpadplane\n", Station_number, module_type);
      fprintf(geofile,"trd%dmod%dpadplane\n", trd_number, module_type);
      // module numbering by 4-digit copy number
      //      fprintf(geofile,"trd%dmod%d#%d%03d\n", Station_number, module_type, Layer_number+1, copy_number);
      fprintf(geofile,"trd%dmod%d#%d%d%03d\n", trd_number, module_type, Station_number, Layer_number+1, copy_number);

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
      if (Position_Station[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      fprintf(geofile,"//*********************************\n");
      //      fprintf(geofile,"trd%dmod%dmylar\n", Station_number, module_type);
      fprintf(geofile,"trd%dmod%dmylar\n", trd_number, module_type);
      // module numbering by 4-digit copy number
      //      fprintf(geofile,"trd%dmod%d#%d%03d\n", Station_number, module_type, Layer_number+1, copy_number);
      fprintf(geofile,"trd%dmod%d#%d%d%03d\n", trd_number, module_type, Station_number, Layer_number+1, copy_number);

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
      if (Position_Station[i][3] == 0) {
        //horizontal position
        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
      } 
      else {
        // vertical position
        fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
      }
      fprintf(geofile,"//*********************************\n");
  
      fprintf(geofile,"//*********************************\n");
      //      fprintf(geofile,"trd%dmod%delectronics\n", Station_number, module_type);
      fprintf(geofile,"trd%dmod%delectronics\n", trd_number, module_type);
      // module numbering by 4-digit copy number
      //      fprintf(geofile,"trd%dmod%d#%d%03d\n", Station_number, module_type, Layer_number+1, copy_number);
      fprintf(geofile,"trd%dmod%d#%d%d%03d\n", trd_number, module_type, Station_number, Layer_number+1, copy_number);

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
      if (Position_Station[i][3] == 0) {
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
	//        fprintf(geofile,"trd%dmod%dframe1#1\n", Station_number, module_type);
        fprintf(geofile,"trd%dmod%dframe1#1\n", trd_number, module_type);
        // module numbering by 4-digit copy number
	//        fprintf(geofile,"trd%dmod%d#%d%03d\n", Station_number, module_type, Layer_number+1, copy_number);
        fprintf(geofile,"trd%dmod%d#%d%d%03d\n", trd_number, module_type, Station_number, Layer_number+1, copy_number);

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
        if (Position_Station[i][3] == 0) {
          //horizontal position
          fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        } 
        else {
          // vertical position
          fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
        }

        fprintf(geofile,"//*********************************\n");
  
        fprintf(geofile,"//*********************************\n");
	//        fprintf(geofile,"trd%dmod%dframe1#2\n", Station_number, module_type);
        fprintf(geofile,"trd%dmod%dframe1#2\n", trd_number, module_type);
        // module numbering by 4-digit copy number
	//        fprintf(geofile,"trd%dmod%d#%d%03d\n", Station_number, module_type, Layer_number+1, copy_number);
        fprintf(geofile,"trd%dmod%d#%d%d%03d\n", trd_number, module_type, Station_number, Layer_number+1, copy_number);

        fprintf(geofile,"%f %f %f\n", 0., -(Active_area_y[j] + frame_width / 2 ), frame_position);
        if (Position_Station[i][3] == 0) {
          //horizontal position
          fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        } 
        else {
          // vertical position
          fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
        }
        fprintf(geofile,"//*********************************\n");
  
  
        fprintf(geofile,"//*********************************\n");
	//        fprintf(geofile,"trd%dmod%dframe2#1\n", Station_number, module_type);
        fprintf(geofile,"trd%dmod%dframe2#1\n", trd_number, module_type);
        // module numbering by 4-digit copy number
	//        fprintf(geofile,"trd%dmod%d#%d%03d\n", Station_number, module_type, Layer_number+1, copy_number);
        fprintf(geofile,"trd%dmod%d#%d%d%03d\n", trd_number, module_type, Station_number, Layer_number+1, copy_number);

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
        if (Position_Station[i][3] == 0) {
          //horizontal position
          fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        } 
        else {
          // vertical position
          fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
        }
        fprintf(geofile,"//*********************************\n");
  
        fprintf(geofile,"//*********************************\n");
	//        fprintf(geofile,"trd%dmod%dframe2#2\n", Station_number, module_type);
        fprintf(geofile,"trd%dmod%dframe2#2\n", trd_number, module_type);
        // module numbering by 4-digit copy number
	//        fprintf(geofile,"trd%dmod%d#%d%03d\n", Station_number, module_type, Layer_number+1, copy_number);
        fprintf(geofile,"trd%dmod%d#%d%d%03d\n", trd_number, module_type, Station_number, Layer_number+1, copy_number);

        fprintf(geofile,"%f %f %f\n", -(Active_area_x[j] + frame_width / 2 ), 0., frame_position);
        if (Position_Station[i][3] == 0) {
          //horizontal position
          fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
        } 
        else {
          // vertical position
          fprintf(geofile,"0.  1.  0.  -1.  0.  0.  0.  0.  1.\n");
        }

        fprintf(geofile,"//*********************************\n");
      }

      if (first_time_Module1 && ( module_type == 1 )) {
        first_time_Module1=false;
        Copy_number_Module1 = copy_number+1 ;
      }
      if (first_time_Module2 && ( module_type == 2 )) {
        first_time_Module2=false;
        Copy_number_Module2 = copy_number+1 ;
      }
      if (first_time_Module3 && ( module_type == 3 )) {
        first_time_Module3=false;
        Copy_number_Module3 = copy_number+1 ;
      }
      if (first_time_Module4 && ( module_type == 4 )) {
        first_time_Module4=false;
        Copy_number_Module4 = copy_number+1 ;
      }
      if (first_time_Module5 && ( module_type == 5 )) {
        first_time_Module5=false;
        Copy_number_Module5 = copy_number+1 ;
      }
      if (first_time_Module6 && ( module_type == 6 )) {
        first_time_Module6=false;
        Copy_number_Module6 = copy_number+1 ;
      }
      if (first_time_Module7 && ( module_type == 7 )) {
        first_time_Module7=false;
        Copy_number_Module7 = copy_number+1 ;
      }
      if (first_time_Module8 && ( module_type == 8 )) {
        first_time_Module8=false;
        Copy_number_Module8 = copy_number+1 ;
      }

    }
    else {

      if ( module_type == 1 ) {
        copy_number = Copy_number_Module1;
      } 
      else if ( module_type == 2 ) {
        copy_number = Copy_number_Module2;
      } 
      else if ( module_type == 3 ) {
        copy_number = Copy_number_Module3;
      } 
      else if ( module_type == 4 ) {
        copy_number = Copy_number_Module4;
      } 
      else if ( module_type == 5 ) {
        copy_number = Copy_number_Module5;
      } 
      else if ( module_type == 6 ) {
        copy_number = Copy_number_Module6;
      } 
      else if ( module_type == 7 ) {
        copy_number = Copy_number_Module7;
      } 
      else if ( module_type == 8 ) {
        copy_number = Copy_number_Module8;
      } 
      else {
        printf("There is something wrong. We don't have more than eight different detector types\n");
        printf("Break in TrdModules1.\n");
        return 1;
      }


      fprintf(geofile,"//*********************************\n");

      // module numbering by 4-digit copy number
      //      fprintf(geofile,"trd%dmod%d#%d%03d\n", Station_number, module_type, Layer_number+1, copy_number);
      fprintf(geofile,"trd%dmod%d#%d%d%03d\n", trd_number, module_type, Station_number, Layer_number+1, copy_number);

      //      fprintf(geofile,"trd%d\n", Station_number);
      fprintf(geofile,"trd%d\n", trd_number);
      // do not use layers anymore
      //      fprintf(geofile,"trd%dlayer#%d\n", Station_number, Layer_number+1);

      Tiltandshift(Station_number, Layer_number, Layer_thickness, Layer_pitch, Position_Station, i);

      fprintf(geofile,"//*********************************\n");
  
      if ( module_type == 1 ) {
        Copy_number_Module1 = copy_number+1 ;
      }
      if ( module_type == 2 ) {
        Copy_number_Module2 = copy_number+1 ;
      }
      if ( module_type == 3 ) {
        Copy_number_Module3 = copy_number+1 ;
      }
      if ( module_type == 4 ) {
        Copy_number_Module4 = copy_number+1 ;
      }
      if ( module_type == 5 ) {
        Copy_number_Module5 = copy_number+1 ;
      }
      if ( module_type == 6 ) {
        Copy_number_Module6 = copy_number+1 ;
      }
      if ( module_type == 7 ) {
        Copy_number_Module7 = copy_number+1 ;
      }
      if ( module_type == 8 ) {
        Copy_number_Module8 = copy_number+1 ;
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
  string geoname  = path + "trd_v12f.geo" ;
  string infoname = path + "trd_v12f.geo.info";
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
  float Detector_size_x[2];  // length in mm of a detector module in x-direction       
  float Detector_size_y[2];  // length in mm of a detector module in y-direction       
  float Frame_width[2];      // Width of detector frames in mm
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
  Frame_width[1]        = 20;
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
  fprintf(geofile,"// Number of stations: %d\n", Station_number);
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
    printf("DE inner radius trd%d:  %f\n", i+1, Inner_radius[i]);
    printf("DE outer radius trd%d: %f\n",  i+1, Outer_radius[i]);

    fprintf(geofile,"// Inner Radius of station %d: %f\n", i+1, Inner_radius[i]);
    fprintf(geofile,"// Outer Radius of station %d: %f\n", i+1, Outer_radius[i]);
    fprintf(geofile,"// z-position at the middle of the station %d: %f\n", i+1, Distance[i]+(Station_thickness/2));
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
		                  
  Detector_size_x[1] = 1000;  //  950;   // 1000;
  Detector_size_y[1] = 1000;  //  950;   // 1000;
		                  
  for(int i = 0; i < 2; i++)
  {
    printf("DE Module %d Detector size x: %7.2f\n", i, Detector_size_x[i]);
    printf("DE Module %d Detector size y: %7.2f\n", i, Detector_size_y[i]);
  }
  printf("\n");

  
  //------------------------------------------------------------------
  //------------------------------------------------------------------

  // calculate positions of small chambers in the first station
  // This is done according the geometrical layout of this station

  // Position_Stationx[][4] = module center in x, in y, module type [1-3], orientation [0,1]

  //------------------------------------------------------------------
  //------------------------------------------------------------------

// ### Station 1

  // v12x - module types in the inner sector of station1 - looking upstream
  int station1i[5][5] = { { 4,  3,  2,  3,  4 },
                          { 3,  1,  1,  1,  3 },
                          { 2,  1,  0,  1,  2 },
                          { 3,  1,  1,  1,  3 },
                          { 4,  3,  2,  3,  4 } };
  // number of modules 1x0, 8x1, 4x2, 8x3, 4x4

  // v12x - module types in the outer sector of station1 - looking upstream
  int station1o[9][11]= { { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
                          { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
                          { 0,  0,  8,  8,  7,  7,  7,  8,  8,  0,  0 },
                          { 0,  0,  8,  7,  0,  0,  0,  7,  8,  0,  0 },
                          { 0,  0,  8,  7,  0,  0,  0,  7,  8,  0,  0 },
                          { 0,  0,  8,  7,  0,  0,  0,  7,  8,  0,  0 },
                          { 0,  0,  8,  8,  7,  7,  7,  8,  8,  0,  0 },
                          { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
                          { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } };
  // number of modules 73x0, 0x5, 0x6, 12x7, 14x8
  // Station1 =  24 + 26;   // v12x

  int a;
  int Chamber_number_Station1 =  0;
  float Position_Station1[100][4];

  a=0;
  for ( int type = 1; type <= 4; type++)
    for ( int j = 4; j >= 0; j--)  // start from the bottom 
      for ( int i = 0; i < 5; i++) 
	if (station1i[j][i]==type)
          {
            int y = -(j-2);
            int x =   i-2;
            printf("a:%2d, pad type:%2d x:%2d, y:%2d   \n", a, type, x, y);
            Position_Station1[a][0] = Detector_size_x[0] * x;
            Position_Station1[a][1] = Detector_size_y[0] * y;

            Position_Station1[a][2] = type;  // pad plane type
            Position_Station1[a][3] = 0;
            a++;
          }

  for ( int type = 5; type <= 8; type++)
    for ( int j = 8; j >= 0; j--)  // start from the bottom 
      for ( int i = 0; i < 11; i++) 
	if (station1o[j][i]==type)
          {
            int y = -(j-4);
            int x =   i-5;
            printf("a:%2d, pad type:%2d x:%2d, y:%2d   \n", a, type, x, y);
            Position_Station1[a][0] = Detector_size_x[1] * x;
            Position_Station1[a][1] = Detector_size_y[1] * y;

            Position_Station1[a][2] = type;  // pad plane type
            Position_Station1[a][3] = 0;
            a++;
          }

  Chamber_number_Station1 = a;
  printf("station 1: declared %2d TRD modules\n", Chamber_number_Station1);

  //------------------------------------------------------------------
  //------------------------------------------------------------------

// ### Station 2

  // v12x - module types in the inner sector of station2 - looking upstream
  int station2i[5][5] = { { 4,  3,  3,  3,  4 },     // { 4,  3,  3,  3,  4 },  
                          { 3,  2,  1,  2,  3 },     // { 3,  2,  2,  2,  3 },  
                          { 3,  1,  0,  1,  3 },     // { 3,  2,  0,  2,  3 },  
                          { 3,  2,  1,  2,  3 },     // { 3,  2,  2,  2,  3 },  
                          { 4,  3,  3,  3,  4 } };   // { 4,  3,  3,  3,  4 } };
  // number of modules 1x0, 4x1, 4x2, 12x3, 4x4

  // v12x - module types in the outer sector of station2 - looking upstream
  int station2o[9][11]= { { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
                          { 0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0 },
                          { 0,  8,  8,  8,  6,  6,  6,  8,  8,  8,  0 },
                          { 0,  8,  8,  6,  0,  0,  0,  6,  8,  8,  0 },
                          { 0,  8,  8,  6,  0,  0,  0,  6,  8,  8,  0 },
                          { 0,  8,  8,  6,  0,  0,  0,  6,  8,  8,  0 },
                          { 0,  8,  8,  8,  6,  6,  6,  8,  8,  8,  0 },
                          { 0,  8,  8,  8,  8,  8,  8,  8,  8,  8,  0 },
                          { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } };
  // number of modules 45x0, 0x5, 12x6, 0x7, 42x8
  // Station2 =  78;   // v12x

  int Chamber_number_Station2 =  0;
  float Position_Station2[100][4];

  a=0;
  for ( int type = 1; type <= 4; type++)
    for ( int j = 4; j >= 0; j--)  // start from the bottom 
      for ( int i = 0; i < 5; i++) 
	if (station2i[j][i]==type)
          {
            int y = -(j-2);
            int x =   i-2;
            printf("a:%2d, pad type:%2d x:%2d, y:%2d   \n", a, type, x, y);

            Position_Station2[a][0] = Detector_size_x[0] * x;
            Position_Station2[a][1] = Detector_size_y[0] * y;

            Position_Station2[a][2] = type;  // pad plane type
            Position_Station2[a][3] = 0;
            a++;
          }

  for ( int type = 5; type <= 8; type++)
    for ( int j = 8; j >= 0; j--)  // start from the bottom 
      for ( int i = 0; i < 11; i++) 
	if (station2o[j][i]==type)
          {
            int y = -(j-4);
            int x =   i-5;
            printf("a:%2d, pad type:%2d x:%2d, y:%2d   \n", a, type, x, y);

            Position_Station2[a][0] = Detector_size_x[1] * x;
            Position_Station2[a][1] = Detector_size_y[1] * y;

            Position_Station2[a][2] = type;  // pad plane type
            Position_Station2[a][3] = 0;
            a++;
          }

  Chamber_number_Station2 = a;
  printf("station 2: declared %2d TRD modules\n", Chamber_number_Station2);

  //------------------------------------------------------------------
  //------------------------------------------------------------------

// ### Station 3

  // v12x - module types in the outer sector of station3 - looking upstream
  int station3[9][11] = { { 8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8 },
                          { 8,  8,  8,  7,  7,  7,  7,  7,  8,  8,  8 },
                          { 8,  8,  7,  7,  6,  6,  6,  7,  7,  8,  8 },
                          { 8,  8,  7,  6,  5,  5,  5,  6,  7,  8,  8 },
                          { 8,  8,  7,  6,  5,  0,  5,  6,  7,  8,  8 },
                          { 8,  8,  7,  6,  5,  5,  5,  6,  7,  8,  8 },
                          { 8,  8,  7,  7,  6,  6,  6,  7,  7,  8,  8 },
                          { 8,  8,  8,  7,  7,  7,  7,  7,  8,  8,  8 },
                          { 8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8 } };
  // number of modules 1x0, 8x5, 12x6, 24x7, 54x8
  // Station3 = 98;   // v12x

  int Chamber_number_Station3 = 0;
  float Position_Station3[100][4];

  a=0;
  for ( int type = 5; type <= 8; type++)
    for ( int j = 8; j >= 0; j--)  // start from the bottom 
      for ( int i = 0; i < 11; i++) 
	if (station3[j][i]==type)
          {
            int y = -(j-4);
            int x =   i-5;
            printf("a:%2d, pad type:%2d x:%2d, y:%2d   \n", a, type, x, y);

            Position_Station3[a][0] = Detector_size_x[1] * x;
            Position_Station3[a][1] = Detector_size_y[1] * y;

            Position_Station3[a][2] = type;  // pad plane type
            Position_Station3[a][3] = 0;
            a++;
          }

  Chamber_number_Station3 = a;
  printf("station 3: declared %2d TRD modules\n", Chamber_number_Station3);

  //------------------------------------------------------------------
  //------------------------------------------------------------------

// ### definition of common TRD keeping volume

    fprintf(geofile,"//*********************************\n");
//    fprintf(geofile,"trd%d\n",istation+1);
    fprintf(geofile,"trd%d\n",1);
    fprintf(geofile,"cave\n");
    fprintf(geofile,"PGON\n");
    fprintf(geofile,"air\n");
    fprintf(geofile,"4\n");
    fprintf(geofile,"45 360 4\n");

    fprintf(geofile,"%.1f %.1f %.1f \n", -1000., 3500., 4500.);
    fprintf(geofile,"%.1f %.1f %.1f \n",     0., 1500., 4500.);
    fprintf(geofile,"%.1f %.1f %.1f \n",     0.,  250., 7000.);
    fprintf(geofile,"%.1f %.1f %.1f \n",  5500.,  250., 7000.);

    fprintf(geofile,"0. 0. %.1f\n", 4500.);
    fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
    fprintf(geofile,"//*********************************\n");

// //*********************************
// trd10
// cave
// PGON
// air
// 4
// 45 360 4
// -1000.000000 3500.000000 4500.000000
// 0.000000 1500.000000 4500.000000
// 0.000000 250.000000 7000.000000
// 5500.000000 250.000000 7000.000000
// 0. 0. 4500.0
// 1.  0.  0.  0.  1.  0.  0.  0.  1.
// //*********************************

  //------------------------------------------------------------------
  //------------------------------------------------------------------

  for ( int istation = 0; istation < Station_number; istation++) {
      
    for (int ilayer = 0; ilayer < Layer_number; ilayer++){

// for all 4 layers
//      if (ilayer == 0){

// do not use layers anymore
//        fprintf(geofile,"//*********************************\n");
//        fprintf(geofile,"trd%dlayer#%d\n",istation+1, ilayer+1);
//        fprintf(geofile,"trd%d\n",istation+1);
//        fprintf(geofile,"PGON\n");
//        fprintf(geofile,"air\n");
//        fprintf(geofile,"2\n");
//        fprintf(geofile,"45 360 4\n");
////      fprintf(geofile,"-%f %f %f \n",Layer_thickness/2,(Inner_radius[istation]-0.5),(Outer_radius[istation]+500));
////      fprintf(geofile,"%f %f %f \n", Layer_thickness/2,(Inner_radius[istation]-0.5),(Outer_radius[istation]+500));
//    // same keeping volume size in x and y for stations and their layers
//    fprintf(geofile,"-%f %f %f \n",Layer_thickness/2, Inner_radius[istation], Outer_radius[istation]);
//    fprintf(geofile,"%f %f %f \n", Layer_thickness/2, Inner_radius[istation], Outer_radius[istation]);
//	// allow variable size in calculation of layer position
//        fprintf(geofile,"0. 0. %4.1f\n",(-Station_thickness/2)+Layer_thickness/2+(Layer_pitch*ilayer));
//        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
//        fprintf(geofile,"//*********************************\n");
  
        
        if ( istation+1 == 1 ) {
	  if ((TrdModules1(istation+1, ilayer , Frame_width, Layer_thickness, Layer_pitch, Chamber_number_Station1, Position_Station1, Detector_size_x, Detector_size_y ))) {
            printf("There was an error in TrdModules1\n");
            printf("Stop program\n");            
            return 1;
          }
        }
        else if ( istation+1 == 2 ) {
	  if ((TrdModules1(istation+1, ilayer , Frame_width, Layer_thickness, Layer_pitch, Chamber_number_Station2, Position_Station2, Detector_size_x, Detector_size_y ))) {
            printf("There was an error in TrdModules1\n");
            printf("Stop program\n");            
            return 1;
          }
	}
        else if ( istation+1 == 3 ) {
          if (ilayer+1 >= 3) printf("skipping TRD3 Layer %d\n", ilayer+1);
	  else if ((TrdModules1(istation+1, ilayer , Frame_width, Layer_thickness, Layer_pitch, Chamber_number_Station3, Position_Station3, Detector_size_x, Detector_size_y ))) {
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
//        fprintf(geofile,"trd%dlayer#%d\n",istation+1, ilayer+1);
//        fprintf(geofile,"trd%d\n",istation+1);
//	// allow variable size in calculation of layer position
//        fprintf(geofile,"0. 0. %4.1f\n",(-Station_thickness/2)+Layer_thickness/2+(Layer_pitch*ilayer));
//        fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
//        fprintf(geofile,"//*********************************\n");
//      }
  
    }
  }


//    // print old station names: trd1,2,3
//    for ( int istation = 0; istation < Station_number; istation++) {
//  
//      printf("Distance, Inner_radius, Outer_radius, Station_thickness: %f, %f, %f, %f \n",
//              Distance[istation], Inner_radius[istation], Outer_radius[istation], Station_thickness);
//      printf("Layer_number, Layer_thickness, layer_pitch, frame_width: %d, %f, %f, %f \n",
//  	   Layer_number, Layer_thickness, Layer_pitch, Frame_width[0], Frame_width[1]);
//    
//      // The distance is measured to the start of the station. In GEANT the
//      // volumes are defined from the center of the volume with half thickness
//      // to either side. This has to corrected here
//    
//      float Distance_center = Distance[istation] + Station_thickness/2;
//    
//      fprintf(geofile,"//*********************************\n");
//      fprintf(geofile,"trd%d\n",istation+1);
//      fprintf(geofile,"cave\n");
//      fprintf(geofile,"PGON\n");
//      fprintf(geofile,"air\n");
//  
//  // for tilted geometries
//  //
//  //    fprintf(geofile,"4\n");
//  //    fprintf(geofile,"45 360 4\n");
//  //    // hardcoded
//  //    if (istation+1 == 1) {
//  //    fprintf(geofile,"%f %f %f \n",-1300., 4300., 4600.);   // station 1
//  //    fprintf(geofile,"%f %f %f \n", -300., 1500., 4600.);   // station 1
//  //    }
//  //    if (istation+1 == 2) {
//  //    fprintf(geofile,"%f %f %f \n",-1800., 6200., 6600.);   // station 2
//  //    fprintf(geofile,"%f %f %f \n", -300., 2500., 6600.);   // station 2
//  //    }
//  //    if (istation+1 == 3) {
//  //    fprintf(geofile,"%f %f %f \n",-2300., 8100., 8600.);   // station 3
//  //    fprintf(geofile,"%f %f %f \n", -300., 3500., 8600.);   // station 3
//  //    }
//  //    // same keeping volume size in x and y for stations and their layers
//  //    fprintf(geofile,"-%f %f %f \n", Station_thickness/2, Inner_radius[istation], Outer_radius[istation]);
//  //    fprintf(geofile,"%f %f %f \n", Station_thickness/2, Inner_radius[istation], Outer_radius[istation]);
//  //    fprintf(geofile,"0. 0. %4.1f\n",Distance_center);
//  //    fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
//  //    fprintf(geofile,"//*********************************\n");
//  
//  // for planar geometries (v12a)
//      fprintf(geofile,"2\n");
//      fprintf(geofile,"45 360 4\n");
//      // same keeping volume size in x and y for stations and their layers
//      if (istation < 2)
//       {
//        fprintf(geofile,"-%f %f %f \n", Station_thickness/2, Inner_radius[istation], Outer_radius[istation]);
//        fprintf(geofile,"%f %f %f \n", Station_thickness/2, Inner_radius[istation], Outer_radius[istation]);
//       }
//      else
//       {
//        fprintf(geofile,"-%f %f %f \n", Station_thickness/2, Inner_radius[istation], Outer_radius[istation]);
//        fprintf(geofile,"%f %f %f \n",                  0., Inner_radius[istation], Outer_radius[istation]);
//       }
//      fprintf(geofile,"0. 0. %4.1f\n",Distance_center);
//      fprintf(geofile,"1.  0.  0.  0.  1.  0.  0.  0.  1.\n");
//      fprintf(geofile,"//*********************************\n");
//        
//    }

  fclose(geofile);
  fclose(infofile);
  //  fclose(parameterfile);

}

