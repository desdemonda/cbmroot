/**
 * \file cbmrootlibs.C
 * \brief Macro loads CBMROOT libraries.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2008
 */

void cbmrootlibs()
{
//   gSystem->Load("libboost_filesystem");
   gSystem->Load("libboost_regex");

   gSystem->Load("libGeoBase");
   gSystem->Load("libParBase");
   gSystem->Load("libBase");
   gSystem->Load("libCbmBase");
   gSystem->Load("libCbmData");
   gSystem->Load("libCbmGenerators");
   gSystem->Load("libField");
   gSystem->Load("libGen");
   gSystem->Load("libPassive");
   gSystem->Load("libTrkBase");
   gSystem->Load("libGeane");

   gSystem->Load("libKF");

   gSystem->Load("libSts");
   gSystem->Load("libTrd");
   gSystem->Load("libEcal");

   gSystem->Load("libL1");
   gSystem->Load("libMvd");

   gSystem->Load("libLittrackparallel");
   gSystem->Load("libLittrack");

   gSystem->Load("libRich");
   gSystem->Load("libMuch");

   gSystem->Load("libTof");

   gSystem->Load("libGlobal");
//   gSystem->Load("libcudalittrack");

   gSystem->Load("libMinuit2"); // Needed for field fitter

   gSystem->Load("libClustering");
}
