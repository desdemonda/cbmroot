#include <string>

using namespace std;

void run_ascii_generator(Int_t nEvents = 100000){

   TString script = TString(gSystem->Getenv("SCRIPT"));

   string asciiFile = "/Users/slebedev/Development/cbm/data/simulations/richprototype/epi.ascii.dat";
   double minMomentum = 7.;
   double maxMomentum = 7.;

   if (script == "yes") {
      minMomentum = TString(gSystem->Getenv("MIN_MOMENTUM")).Atof();
      maxMomentum = TString(gSystem->Getenv("MAX_MOMENTUM")).Atof();
      asciiFile = string(gSystem->Getenv("IN_ASCII_FILE"));
   }

   // Open output file
   ofstream *file = new ofstream(asciiFile.c_str());

   // Initialize RN generator
   gRandom->SetSeed(10);

   for(Int_t iev = 0; iev < nEvents; iev++) {
      // Generate vertex coordinates
      double vz = -100;
      double vx = gRandom->Gaus(0., 0.9);
      double vy = gRandom->Gaus(0., 0.4);

      // Write out event header
      (*file) << 1 << " " << (iev+1) << " " << vx << " " << vy << " " << vz << endl;

      // Select electron or pion
      double type = iev % 2; //gRandom->Uniform(0., 1.);
      int pdg = -211;
      if(type == 1) {
         pdg = 11;
      }

      // Generate momentum
      double p = gRandom->Uniform(minMomentum, maxMomentum);
      // Generate polar angle
      double theta = TMath::Abs(gRandom->Gaus(0., 5.1e-3));
      // Generate azimuthal angle
      double phi = gRandom->Uniform(-1.*TMath::Pi(), TMath::Pi());
      // Calculate momentum components
      double pt = p*TMath::Sin(theta);
      double px = pt*TMath::Cos(phi);
      double py = pt*TMath::Sin(phi);
      double pz = p*TMath::Cos(theta);

      // Write out particles information
      (*file) << pdg << " " << px << " " << py << " " << pz << endl;

      // Status output
      if(0 == (iev%10000)) cout << iev << "   " << pdg << endl;
   }// events

   // Close output file
   file->close();
}



