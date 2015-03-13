void create_RICH_geo_file(char *geometry="rich-2900-N2-0")
{
char infile[256];	// geometry data file - input
char outfile[256];	// geometry data file for CBMROOT - output
char logfile[256];	// log file

char dummy[200][10];	// dummy text

Float_t zBarrel;	// z-position RICH detector (start)
Float_t lBarrel;	// length RICH barrel
Float_t dWindow;	// thickness entrance and exit window, material: kapton
Float_t dVessel;	// thickness gas vessel, material: aluminum
Float_t lRadiator;	// length of radiator
char gas[20];		// radiator gas (see media.geo file)

Float_t radius;		// radius of mirror
Float_t dMirror;	// thickness of mirror
char mirror[20];	// material of mirror (see media.geo file)
Float_t yBeam;		// hole for beam in y (x=1.5*y)
Float_t angle;

Float_t zPMT,yPMT;	// z and y position photodetector plane
Float_t dx,dy;		// x-y size of PMT plane
Float_t anglePMTx;	// angle photodetector plane (tilt around x-axis)
Float_t anglePMTy;	// angle photodetector plane (tilt around y-axis)
Float_t dGlass;		// thickness glass window of PMT tubes (PMTglass)
Float_t dCathode;	// thickness photocathode (CsI)
Float_t lTube;		// length PMT tube (air for eff. thickness)
Float_t dSupport;	// effective thickness support structure (aluminum)

Float_t xM,yM,xB1,xB2,yB;	// x,y sizes for mirror and barrel
Float_t xE1,xE2,zI1,xI1,zI2,xI2;
Float_t r1,r2;
Float_t rE1,rE2,rI1,rI2,rC,rM;
Float_t theta1,theta2,phi1,phi2;	// angles for mirror
Float_t theta3,phi3,phi4;
Float_t yMC,zMC;	// center of mirror SPHE

printf("\n");
printf("geometry: %s\n",geometry);
printf("\n");

sprintf(infile,"%s.dat",geometry);
sprintf(outfile,"%s.geo",geometry);
sprintf(logfile,"%s.log",geometry);
printf("\n");

// read in input file (if existing)
printf("input file: %s\n",infile);
if(!fopen(infile,"r")){
   printf("... does not exist. \n");
   printf("... exit. \n\n");

}
else{

   printf("output file: %s\n",outfile);
   printf("logfile: %s\n",logfile);
   printf("\n");
   printf("read in datafile:\n");
   printf("--------------------------------------------------------------\n");

   FILE *fin = fopen(infile,"r");
   fscanf (fin,"%s %s",dummy[1],dummy[2]);
   printf ("%s %s\n",dummy[1],dummy[2]);
   fscanf (fin,"%s %s %s %s %s %s",dummy[1],dummy[2],dummy[3],dummy[4],dummy[5],dummy[6]);
   printf ("%s %s %s %s %s %s\n",dummy[1],dummy[2],dummy[3],dummy[4],dummy[5],dummy[6]);
   fscanf (fin,"%f %f %f %f %f %s", &zBarrel, &lBarrel, &dWindow, &dVessel, &lRadiator, gas);
   printf ("%6.2f %6.2f %6.2f %6.2f %6.2f %s\n",zBarrel, lBarrel, dWindow, dVessel, lRadiator, gas);
   fscanf (fin,"%s %s",dummy[1],dummy[2]);
   printf ("%s %s\n",dummy[1],dummy[2]);
   fscanf (fin,"%s %s %s %s %s",dummy[1],dummy[2],dummy[3],dummy[4],dummy[5]);
   printf ("%s %s %s %s %s\n",dummy[1],dummy[2],dummy[3],dummy[4],dummy[5]);
   fscanf (fin,"%f %f %s %f %f", &radius, &dMirror, mirror, &yBeam, &angle);
   printf ("%6.2f %6.2f %s %6.2f %6.2f\n", radius, dMirror, mirror, yBeam, angle);
   fscanf (fin,"%s %s %s",dummy[1],dummy[2],dummy[3]);
   printf ("%s %s %s\n",dummy[1],dummy[2],dummy[3]);
   fscanf (fin,"%s %s %s %s %s %s %s %s %s %s",dummy[1],dummy[2],dummy[3],dummy[4],dummy[5],dummy[6],dummy[7],dummy[8],dummy[9],dummy[10]);
   printf ("%s %s %s %s %s %s %s %s %s %s\n",dummy[1],dummy[2],dummy[3],dummy[4],dummy[5],dummy[6],dummy[7],dummy[8],dummy[9],dummy[10]);
   fscanf (fin,"%f %f %f %f %f %f %f %f %f %f", &zPMT, &yPMT, &dx, &dy, &anglePMTx, &anglePMTy, &dGlass, &dCathode, &lTube, &dSupport);
   printf ("%6.2f %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f\n", zPMT, yPMT, dx, dy, anglePMTx, anglePMTy, dGlass, dCathode, lTube, dSupport);
   printf("--------------------------------------------------------------\n");
   printf("\n");
   fclose(fin);



//calculate parameters

// size of RICH mirror (acceptance: 25 degree)
yM = (zBarrel+lRadiator+zPMT)*TMath::Tan(25./180.*TMath::Pi());
xM = 1.5*yM;

// size of RICH barrel (front part in x smaller by 800mm)
yB = (zBarrel+lBarrel)*TMath::Tan(25./180.*TMath::Pi())+300.;
if (yB < (yPMT+dy)) yB=(yPMT+dy)+100;
xB2 = 1.5*yB;
xB1 = xB2 - 800.;

// end entrance window in x
xE1 = (xB2-xB1)/lBarrel*dWindow+xB1;
xE2 = (xB2-xB1)/lBarrel*(lBarrel-dWindow)+xB1;

// imaginary plane for registration of points needed for track extrapolation to PMTplane (distance to mirror: 900mm for angle=0)
// take care: necessary distance to mirror depends on angle!
// thickness 10mm
// start
zI1 = zPMT+lRadiator-900.;
xI1 = (xB2-xB1)/lBarrel*(zI1-dWindow)+(xB1-dVessel);
// end
zI2 = zPMT+lRadiator-900.+10.;
xI2 = (xB2-xB1)/lBarrel*(zI2-dWindow)+(xB1-dVessel);

// beam cone:
// at z=1600mm (end of STS beam-pipe) outer radius =32.5mm
// at end of RICH detector outer radius = 200.5mm
r1=32.5;
r2=200.5;
rC=0.5;    // thickness of carbon wall
rE1=(r2-r1)/lBarrel*dWindow+r1;
rE2=(r2-r1)/lBarrel*(lBarrel-dWindow)+r1;
rI1=(r2-r1)/lBarrel*zI1+r1;
rI2=(r2-r1)/lBarrel*zI2+r1;
// beam cone at mirror:
rM=(r2-r1)/lBarrel*(lRadiator+zPMT)+r1;
if ((rM+30.) > yBeam) yBeam=rM+30.;      // add 3cm for mirror support
if ((TMath::Tan(2.5/180.*TMath::Pi())*(zBarrel+zPMT+lRadiator)) > yBeam) yBeam = TMath::Tan(2.5/180.*TMath::Pi())*(zBarrel+zPMT+lRadiator); // agreement: inner hole 2.5 degrees


printf("check pipe.geo! --> modify? \n");
printf(" ... third last line of pipe1: %6.2f  %6.2f %6.2f ?\n",zBarrel,r1-rC,r1);
printf(" ... third last line of pipevac1: %6.2f  0. %6.2f ?\n",zBarrel,r1-rC);
printf(" ... 4th line of pipe2: %6.2f  0. %6.2f ?\n",zBarrel+lBarrel,r2);
printf(" ... 4th line of pipevac2: %6.2f  0. %6.2f ?\n",zBarrel+lBarrel,r2-rC);
printf("\n");

// mirror parameters (angles in degree for SPHE)
theta1=(TMath::Pi()/2.-TMath::ATan(yM/2./radius))*180./TMath::Pi();
theta2=(TMath::Pi()/2.+TMath::ATan(yM/2./radius))*180./TMath::Pi();
phi1=(TMath::Pi()/2.-TMath::ATan(xM/radius))*180./TMath::Pi();
phi2=(TMath::Pi()/2.+TMath::ATan(xM/radius))*180./TMath::Pi();

theta3=theta1-TMath::ATan(yBeam/radius)*180./TMath::Pi();
phi3=(TMath::Pi()/2.-TMath::ATan((1.5*yBeam)/radius))*180./TMath::Pi();
phi4=(TMath::Pi()/2.+TMath::ATan((1.5*yBeam)/radius))*180./TMath::Pi();


// ... (center of SPHE)
yMC=yM/2.*TMath::Cos(angle/180.*TMath::Pi())+yBeam+TMath::Tan(angle/180.*TMath::Pi())*radius;
zMC=(lRadiator+zPMT)-radius;

//output to geo file
FILE *fout = fopen(outfile,"w");

fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"// RICH barrel\n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1\n");
fprintf(fout,"cave\n");
fprintf(fout,"TRAP\n");
fprintf(fout,"aluminium\n");
fprintf(fout,"%6.2f -%6.2f 0. \n ",xB1,yB);
fprintf(fout,"%6.2f %6.2f 0. \n ",xB1,yB);
fprintf(fout,"-%6.2f %6.2f 0. \n ",xB1,yB);
fprintf(fout,"-%6.2f -%6.2f 0. \n ",xB1,yB);
fprintf(fout,"%6.2f -%6.2f %6.2f \n ",xB2,yB,lBarrel);
fprintf(fout,"%6.2f %6.2f %6.2f \n ",xB2,yB,lBarrel);
fprintf(fout,"-%6.2f %6.2f %6.2f \n ",xB2,yB,lBarrel);
fprintf(fout,"-%6.2f -%6.2f %6.2f \n ",xB2,yB,lBarrel);
fprintf(fout,"0. 0. %6.2f\n",zBarrel);
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout," \n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1entrance\n");
fprintf(fout,"rich1\n");
fprintf(fout,"TRAP\n");
fprintf(fout,"kapton\n");
fprintf(fout,"%6.2f -%6.2f 0. \n ",xB1-dVessel,yB-dVessel);
fprintf(fout,"%6.2f %6.2f 0. \n ",xB1-dVessel,yB-dVessel);
fprintf(fout,"-%6.2f %6.2f 0. \n ",xB1-dVessel,yB-dVessel);
fprintf(fout,"-%6.2f -%6.2f 0. \n ",xB1-dVessel,yB-dVessel);
fprintf(fout,"%6.2f -%6.2f %6.2f \n ",xE1-dVessel,yB-dVessel,dWindow);
fprintf(fout,"%6.2f %6.2f %6.2f \n ",xE1-dVessel,yB-dVessel,dWindow);
fprintf(fout,"-%6.2f %6.2f %6.2f \n ",xE1-dVessel,yB-dVessel,dWindow);
fprintf(fout,"-%6.2f -%6.2f %6.2f \n ",xE1-dVessel,yB-dVessel,dWindow);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout," \n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1exit\n");
fprintf(fout,"rich1\n");
fprintf(fout,"TRAP\n");
fprintf(fout,"kapton\n");
fprintf(fout,"%6.2f -%6.2f %6.2f \n ",xE2-dVessel,yB-dVessel,(lBarrel-dWindow));
fprintf(fout,"%6.2f %6.2f %6.2f \n ",xE2-dVessel,yB-dVessel,(lBarrel-dWindow));
fprintf(fout,"-%6.2f %6.2f %6.2f \n ",xE2-dVessel,yB-dVessel,(lBarrel-dWindow));
fprintf(fout,"-%6.2f -%6.2f %6.2f \n ",xE2-dVessel,yB-dVessel,(lBarrel-dWindow));
fprintf(fout,"%6.2f -%6.2f %6.2f \n ",xB2-dVessel,yB-dVessel,lBarrel);
fprintf(fout,"%6.2f %6.2f %6.2f \n ",xB2-dVessel,yB-dVessel,lBarrel);
fprintf(fout,"-%6.2f %6.2f %6.2f \n ",xB2-dVessel,yB-dVessel,lBarrel);
fprintf(fout,"-%6.2f -%6.2f %6.2f \n ",xB2-dVessel,yB-dVessel,lBarrel);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout," \n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"rich1\n");
fprintf(fout,"TRAP\n");
fprintf(fout,"%s\n",gas);
fprintf(fout,"%6.2f -%6.2f %6.2f \n ",(xE1-dVessel),(yB-dVessel),dWindow);
fprintf(fout,"%6.2f %6.2f %6.2f \n ",(xE1-dVessel),(yB-dVessel),dWindow);
fprintf(fout,"-%6.2f %6.2f %6.2f \n ",(xE1-dVessel),(yB-dVessel),dWindow);
fprintf(fout,"-%6.2f -%6.2f %6.2f \n ",(xE1-dVessel),(yB-dVessel),dWindow);
fprintf(fout,"%6.2f -%6.2f %6.2f \n ",xI1,(yB-dVessel),zI1);
fprintf(fout,"%6.2f %6.2f %6.2f \n ",xI1,(yB-dVessel),zI1);
fprintf(fout,"-%6.2f %6.2f %6.2f \n ",xI1,(yB-dVessel),zI1);
fprintf(fout,"-%6.2f -%6.2f %6.2f \n ",xI1,(yB-dVessel),zI1);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout," \n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1gas2\n");
fprintf(fout,"rich1\n");
fprintf(fout,"TRAP\n");
fprintf(fout,"%s+\n",gas);
fprintf(fout,"%6.2f -%6.2f %6.2f \n ",xI1,(yB-dVessel),zI1);
fprintf(fout,"%6.2f %6.2f %6.2f \n ",xI1,(yB-dVessel),zI1);
fprintf(fout,"-%6.2f %6.2f %6.2f \n ",xI1,(yB-dVessel),zI1);
fprintf(fout,"-%6.2f -%6.2f %6.2f \n ",xI1,(yB-dVessel),zI1);
fprintf(fout,"%6.2f -%6.2f %6.2f \n ",xI2,(yB-dVessel),zI2);
fprintf(fout,"%6.2f %6.2f %6.2f \n ",xI2,(yB-dVessel),zI2);
fprintf(fout,"-%6.2f %6.2f %6.2f \n ",xI2,(yB-dVessel),zI2);
fprintf(fout,"-%6.2f -%6.2f %6.2f \n ",xI2,(yB-dVessel),zI2);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout," \n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1gas3\n");
fprintf(fout,"rich1\n");
fprintf(fout,"TRAP\n");
fprintf(fout,"%s\n",gas);
fprintf(fout,"%6.2f -%6.2f %6.2f \n ",xI2,(yB-dVessel),zI2);
fprintf(fout,"%6.2f %6.2f %6.2f \n ",xI2,(yB-dVessel),zI2);
fprintf(fout,"-%6.2f %6.2f %6.2f \n ",xI2,(yB-dVessel),zI2);
fprintf(fout,"-%6.2f -%6.2f %6.2f \n ",xI2,(yB-dVessel),zI2);
fprintf(fout,"%6.2f -%6.2f %6.2f \n ",xE2-dVessel,(yB-dVessel),(lBarrel-dWindow));
fprintf(fout,"%6.2f %6.2f %6.2f \n ",xE2-dVessel,(yB-dVessel),(lBarrel-dWindow));
fprintf(fout,"-%6.2f %6.2f %6.2f \n ",xE2-dVessel,(yB-dVessel),(lBarrel-dWindow));
fprintf(fout,"-%6.2f -%6.2f %6.2f \n ",xE2-dVessel,(yB-dVessel),(lBarrel-dWindow));
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout," \n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"// beam cone\n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1_cone1\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"CONE\n");
fprintf(fout,"carbon\n");
fprintf(fout,"0. 0. %6.2f\n",dWindow);
fprintf(fout,"0. %6.2f\n",rE1);
fprintf(fout,"0. 0. %6.2f\n",zI1);
fprintf(fout,"0. %6.2f\n",rI1);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1_cone1_vac\n");
fprintf(fout,"rich1_cone1\n");
fprintf(fout,"CONE\n");
fprintf(fout,"vacuum\n");
fprintf(fout,"0. 0. %6.2f\n",dWindow);
fprintf(fout,"0. %6.2f\n",rE1-rC);
fprintf(fout,"0. 0. %6.2f\n",zI1);
fprintf(fout,"0. %6.2f\n",rI1-rC);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout," \n");
fprintf(fout,"rich1_cone2\n");
fprintf(fout,"rich1gas2\n");
fprintf(fout,"CONE\n");
fprintf(fout,"carbon\n");
fprintf(fout,"0. 0. %6.2f\n",zI1);
fprintf(fout,"0. %6.2f\n",rI1);
fprintf(fout,"0. 0. %6.2f\n",zI2);
fprintf(fout,"0. %6.2f\n",rI2);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1_cone2_vac\n");
fprintf(fout,"rich1_cone2\n");
fprintf(fout,"CONE\n");
fprintf(fout,"vacuum\n");
fprintf(fout,"0. 0. %6.2f\n",zI1);
fprintf(fout,"0. %6.2f\n",rI1-rC);
fprintf(fout,"0. 0. %6.2f\n",zI2);
fprintf(fout,"0. %6.2f\n",rI2-rC);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout," \n");
fprintf(fout,"rich1_cone3\n");
fprintf(fout,"rich1gas3\n");
fprintf(fout,"CONE\n");
fprintf(fout,"carbon\n");
fprintf(fout,"0. 0. %6.2f\n",zI2);
fprintf(fout,"0. %6.2f\n",rI2);
fprintf(fout,"0. 0. %6.2f\n",lBarrel-dWindow);
fprintf(fout,"0. %6.2f\n",rE2);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1_cone3_vac\n");
fprintf(fout,"rich1_cone3\n");
fprintf(fout,"CONE\n");
fprintf(fout,"vacuum\n");
fprintf(fout,"0. 0. %6.2f\n",zI2);
fprintf(fout,"0. %6.2f\n",rI2-rC);
fprintf(fout,"0. 0. %6.2f\n",lBarrel-dWindow);
fprintf(fout,"0. %6.2f\n",rE2-rC);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout," \n");
fprintf(fout,"rich1_cone1_cap\n");
fprintf(fout,"rich1entrance\n");
fprintf(fout,"CONE\n");
fprintf(fout,"carbon\n");
fprintf(fout,"0. 0. 0.\n");
fprintf(fout,"0. %6.2f\n",r1);
fprintf(fout,"0. 0. %6.2f\n",dWindow);
fprintf(fout,"0. %6.2f\n",rE1);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1_cone1_cap_vac\n");
fprintf(fout,"rich1_cone1_cap\n");
fprintf(fout,"CONE\n");
fprintf(fout,"vacuum\n");
fprintf(fout,"0. 0. 0.\n");
fprintf(fout,"0. %6.2f\n",r1-rC);
fprintf(fout,"0. 0. %6.2f\n",dWindow);
fprintf(fout,"0. %6.2f\n",rE1-rC);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout," \n");
fprintf(fout,"rich1_cone2_cap\n");
fprintf(fout,"rich1exit\n");
fprintf(fout,"CONE\n");
fprintf(fout,"carbon\n");
fprintf(fout,"0. 0. %6.2f\n",lBarrel-dWindow);
fprintf(fout,"0. %6.2f\n",rE2);
fprintf(fout,"0. 0. %6.2f\n",lBarrel);
fprintf(fout,"0. %6.2f\n",r2);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1_cone2_cap_vac\n");
fprintf(fout,"rich1_cone2_cap\n");
fprintf(fout,"CONE\n");
fprintf(fout,"vacuum\n");
fprintf(fout,"0. 0. %6.2f\n",lBarrel-dWindow);
fprintf(fout,"0. %6.2f\n",rE2-rC);
fprintf(fout,"0. 0. %6.2f\n",lBarrel);
fprintf(fout,"0. %6.2f\n",r2-rC);
fprintf(fout,"0. 0. 0.\n");
fprintf(fout," 1. 0. 0. 0. 1. 0. 0. 0. 1.\n");
fprintf(fout," \n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"// mirror\n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1mgl#1\n");
fprintf(fout,"rich1gas3\n");
fprintf(fout,"SPHE\n");
fprintf(fout,"%s\n",mirror);
fprintf(fout," %6.2f %6.2f\n",radius,radius+dMirror);
fprintf(fout," %6.2f %6.2f\n",theta1,theta2);
fprintf(fout," %6.2f %6.2f\n",phi1,phi2);
fprintf(fout,"0. %6.2f %6.2f\n",yMC,zMC);
fprintf(fout," 1. 0. 0. 0. %6.4f %6.4f 0. %6.4f %6.4f\n",TMath::Cos((90.+angle)/180.*TMath::Pi()), -TMath::Sin((90.+angle)/180.*TMath::Pi()), TMath::Sin((90.+angle)/180.*TMath::Pi()), TMath::Cos((90.+angle)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1mglLU#1\n");
fprintf(fout,"rich1gas3\n");
fprintf(fout,"SPHE\n");
fprintf(fout,"%s\n",mirror);
fprintf(fout," %6.2f %6.2f\n",radius,radius+dMirror);
fprintf(fout," %6.2f %6.2f\n",theta3,theta1);
fprintf(fout," %6.2f %6.2f\n",phi4,phi2);
fprintf(fout,"0. %6.2f %6.2f\n",yMC,zMC);
fprintf(fout," 1. 0. 0. 0. %6.4f %6.4f 0. %6.4f %6.4f\n",TMath::Cos((90.+angle)/180.*TMath::Pi()), -TMath::Sin((90.+angle)/180.*TMath::Pi()), TMath::Sin((90.+angle)/180.*TMath::Pi()), TMath::Cos((90.+angle)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1mglRU#1\n");
fprintf(fout,"rich1gas3\n");
fprintf(fout,"SPHE\n");
fprintf(fout,"%s\n",mirror);
fprintf(fout," %6.2f %6.2f\n",radius,radius+dMirror);
fprintf(fout," %6.2f %6.2f\n",theta3,theta1);
fprintf(fout," %6.2f %6.2f\n",phi1,phi3);
fprintf(fout,"0. %6.2f %6.2f\n",yMC,zMC);
fprintf(fout," 1. 0. 0. 0. %6.4f %6.4f 0. %6.4f %6.4f\n",TMath::Cos((90.+angle)/180.*TMath::Pi()), -TMath::Sin((90.+angle)/180.*TMath::Pi()), TMath::Sin((90.+angle)/180.*TMath::Pi()), TMath::Cos((90.+angle)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1mgl#2\n");
fprintf(fout,"rich1gas3\n");
fprintf(fout,"0. -%6.2f %6.2f\n",yMC,zMC);
fprintf(fout," -1. 0. 0. 0. %6.4f %6.4f 0. %6.4f %6.4f\n",TMath::Cos((90.-angle)/180.*TMath::Pi()), TMath::Sin((90.-angle)/180.*TMath::Pi()), TMath::Sin((90.-angle)/180.*TMath::Pi()), -TMath::Cos((90.-angle)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1mglLU#2\n");
fprintf(fout,"rich1gas3\n");
fprintf(fout,"0. -%6.2f %6.2f\n",yMC,zMC);
fprintf(fout," -1. 0. 0. 0. %6.4f %6.4f 0. %6.4f %6.4f\n",TMath::Cos((90.-angle)/180.*TMath::Pi()), TMath::Sin((90.-angle)/180.*TMath::Pi()), TMath::Sin((90.-angle)/180.*TMath::Pi()), -TMath::Cos((90.-angle)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"rich1mglRU#2\n");
fprintf(fout,"rich1gas3\n");
fprintf(fout,"0. -%6.2f %6.2f\n",yMC,zMC);
fprintf(fout," -1. 0. 0. 0. %6.4f %6.4f 0. %6.4f %6.4f\n",TMath::Cos((90.-angle)/180.*TMath::Pi()), TMath::Sin((90.-angle)/180.*TMath::Pi()), TMath::Sin((90.-angle)/180.*TMath::Pi()), -TMath::Cos((90.-angle)/180.*TMath::Pi()));
fprintf(fout," \n");
fprintf(fout,"//-----------------------------------------------------------\n");
fprintf(fout,"// photodetector\n");
fprintf(fout,"//-----------------------------------------------------------\n");
if (dGlass > 0 ){
fprintf(fout,"rich1dgl#1\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"BOX\n");
fprintf(fout,"PMTglass\n");
fprintf(fout,"%6.2f  -%6.2f  %6.2f\n",dx/2.,dy,-dGlass);
fprintf(fout,"%6.2f   %6.2f  %6.2f\n",dx/2.,dy,-dGlass);
fprintf(fout,"-%6.2f   %6.2f  %6.2f\n",dx/2.,dy,-dGlass);
fprintf(fout,"-%6.2f  -%6.2f  %6.2f\n",dx/2.,dy,-dGlass);
fprintf(fout," %6.2f -%6.2f  0.\n",dx/2.,dy);
fprintf(fout," %6.2f %6.2f  0.\n",dx/2.,dy);
fprintf(fout,"-%6.2f  %6.2f  0.\n",dx/2.,dy);
fprintf(fout,"-%6.2f  -%6.2f  0.\n",dx/2.,dy);
fprintf(fout,"%6.2f %6.2f %6.2f\n",dx/2.,yPMT,zPMT);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
}
fprintf(fout,"rich1d#1\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"BOX\n");
fprintf(fout,"CsI\n");
fprintf(fout,"%6.2f  -%6.2f  %6.2f\n",dx/2.,dy,-dCathode);
fprintf(fout,"%6.2f   %6.2f  %6.2f\n",dx/2.,dy,-dCathode);
fprintf(fout,"-%6.2f   %6.2f  %6.2f\n",dx/2.,dy,-dCathode);
fprintf(fout,"-%6.2f  -%6.2f  %6.2f\n",dx/2.,dy,-dCathode);
fprintf(fout,"%6.2f  -%6.2f  0.\n",dx/2.,dy);
fprintf(fout,"%6.2f  %6.2f  0.\n",dx/2.,dy);
fprintf(fout,"-%6.2f  %6.2f  0.\n",dx/2.,dy);
fprintf(fout,"-%6.2f  -%6.2f  0.\n",dx/2.,dy);
fprintf(fout,"%6.2f %6.2f %6.2f\n",dx/2.,yPMT,zPMT-dGlass);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()));
 fprintf(fout,"//-----------------------------------------------------------\n");
if (lTube > 0 ){
fprintf(fout,"rich1dtube#1\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"BOX\n");
fprintf(fout,"air\n");
fprintf(fout,"%6.2f  -%6.2f  %6.2f\n",dx/2.,dy,-lTube);
fprintf(fout,"%6.2f   %6.2f  %6.2f\n",dx/2.,dy,-lTube);
fprintf(fout,"-%6.2f   %6.2f  %6.2f\n",dx/2.,dy,-lTube);
fprintf(fout,"-%6.2f  -%6.2f  %6.2f\n",dx/2.,dy,-lTube);
fprintf(fout,"%6.2f  -%6.2f  0.\n",dx/2.,dy);
fprintf(fout,"%6.2f  %6.2f  0.\n",dx/2.,dy);
fprintf(fout,"-%6.2f  %6.2f  0.\n",dx/2.,dy);
fprintf(fout,"-%6.2f  -%6.2f  0.\n",dx/2.,dy);
fprintf(fout,"%6.2f %6.2f %6.2f\n",dx/2.,yPMT,zPMT-dGlass-dCathode);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
}
if (dSupport > 0 ){
fprintf(fout,"rich1dsupport#1\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"BOX\n");
fprintf(fout,"aluminium\n");
fprintf(fout,"%6.2f  -%6.2f  %6.2f\n",dx/2.,dy,-dSupport);
fprintf(fout,"%6.2f   %6.2f  %6.2f\n",dx/2.,dy,-dSupport);
fprintf(fout,"-%6.2f   %6.2f  %6.2f\n",dx/2.,dy,-dSupport);
fprintf(fout,"-%6.2f  -%6.2f  %6.2f\n",dx/2.,dy,-dSupport);
fprintf(fout,"%6.2f  -%6.2f  0.\n",dx/2.,dy);
fprintf(fout,"%6.2f  %6.2f  0.\n",dx/2.,dy);
fprintf(fout,"-%6.2f  %6.2f  0.\n",dx/2.,dy);
fprintf(fout,"-%6.2f  -%6.2f  0.\n",dx/2.,dy);
fprintf(fout,"%6.2f %6.2f %6.2f\n",dx/2.,yPMT,zPMT-dGlass-dCathode-lTube);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
}
if (dGlass > 0 ){
fprintf(fout,"rich1dgl#2\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"%6.2f -%6.2f %6.2f\n",dx/2.,yPMT,zPMT);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((-anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
}
fprintf(fout,"rich1d#2\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"%6.2f -%6.2f %6.2f\n",dx/2.,yPMT,zPMT-dGlass);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((-anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
if (lTube > 0 ){
fprintf(fout,"rich1dtube#2\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"%6.2f -%6.2f %6.2f\n",dx/2.,yPMT,zPMT-dGlass-dCathode);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((-anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
}
if (dSupport > 0 ){
fprintf(fout,"rich1dsupport#2\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"%6.2f -%6.2f %6.2f\n",dx/2.,yPMT,zPMT-dGlass-dCathode-lTube);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((-anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
}
if (dGlass > 0 ){
fprintf(fout,"rich1dgl#3\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"-%6.2f %6.2f %6.2f\n",dx/2.,yPMT,zPMT);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
}
fprintf(fout,"rich1d#3\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"-%6.2f %6.2f %6.2f\n",dx/2.,yPMT,zPMT-dGlass);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
if (lTube > 0 ){
fprintf(fout,"rich1dtube#3\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"-%6.2f %6.2f %6.2f\n",dx/2.,yPMT,zPMT-dGlass-dCathode);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
}
if (dSupport > 0 ){
fprintf(fout,"rich1dsupport#3\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"-%6.2f %6.2f %6.2f\n",dx/2.,yPMT,zPMT-dGlass-dCathode-lTube);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
}
if (dGlass > 0 ){
fprintf(fout,"rich1dgl#4\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"-%6.2f -%6.2f %6.2f\n",dx/2.,yPMT,zPMT);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((-anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
}
fprintf(fout,"rich1d#4\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"-%6.2f -%6.2f %6.2f\n",dx/2.,yPMT,zPMT-dGlass);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((-anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
if (lTube > 0 ){
fprintf(fout,"rich1dtube#4\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"-%6.2f -%6.2f %6.2f\n",dx/2.,yPMT,zPMT-dGlass-dCathode);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((-anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
}
if (dSupport > 0 ){
fprintf(fout,"rich1dsupport#4\n");
fprintf(fout,"rich1gas1\n");
fprintf(fout,"-%6.2f -%6.2f %6.2f\n",dx/2.,yPMT,zPMT-dGlass-dCathode-lTube);
fprintf(fout," %6.4f 0. %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f %6.4f\n",
 TMath::Cos((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTy)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi()), 
 -TMath::Sin((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()),
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Sin((-anglePMTy)/180.*TMath::Pi()),
 TMath::Sin((-anglePMTx)/180.*TMath::Pi()), 
 TMath::Cos((-anglePMTx)/180.*TMath::Pi())*TMath::Cos((-anglePMTy)/180.*TMath::Pi()));
fprintf(fout,"//-----------------------------------------------------------\n");
}

fclose(fout);

printf("new geometry file done\n\n");

}

}
