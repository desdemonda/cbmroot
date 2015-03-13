// -------------------------------------------------------------------------
// -----                    CbmTrdPadsMap header file                  -----
// -----               Created 11/04/2013 by D. Emschermann            -----
// -------------------------------------------------------------------------
//
// v0.1 - 20130411 DE - branched from CbmTrdPads.h

#ifndef CBMTRDPADSMAP_H
#define CBMTRDPADSMAP_H

#include <map>
#include <vector>
using std::vector;
using std::map;
using std::pair;

typedef map<Int_t, vector<Int_t> > VectorMap;
typedef map<Int_t, VectorMap > MapOfVectorMap;

MapOfVectorMap GetModuleInfoMap()
{

  vector<Int_t> moduleType1Station1(100, 0);
  vector<Int_t> moduleType2Station1(100, 1);
  vector<Int_t> moduleType3Station1(100, 2);
  vector<Int_t> moduleType4Station1(100, 3);
  vector<Int_t> moduleType5Station1(100, 4);
  vector<Int_t> moduleType6Station1(100, 5);
  vector<Int_t> moduleType7Station1(100, 6);
  vector<Int_t> moduleType8Station1(100, 7);

  VectorMap moduleTypeMapStation1;
  moduleTypeMapStation1.insert( pair<Int_t, vector<Int_t> >(1, moduleType1Station1));
  moduleTypeMapStation1.insert( pair<Int_t, vector<Int_t> >(2, moduleType2Station1));
  moduleTypeMapStation1.insert( pair<Int_t, vector<Int_t> >(3, moduleType3Station1));
  moduleTypeMapStation1.insert( pair<Int_t, vector<Int_t> >(4, moduleType4Station1));
  moduleTypeMapStation1.insert( pair<Int_t, vector<Int_t> >(5, moduleType5Station1));
  moduleTypeMapStation1.insert( pair<Int_t, vector<Int_t> >(6, moduleType6Station1));
  moduleTypeMapStation1.insert( pair<Int_t, vector<Int_t> >(7, moduleType7Station1));
  moduleTypeMapStation1.insert( pair<Int_t, vector<Int_t> >(8, moduleType8Station1));

  vector<Int_t> moduleType1Station2(100, 0);
  vector<Int_t> moduleType2Station2(100, 1);
  vector<Int_t> moduleType3Station2(100, 2);
  vector<Int_t> moduleType4Station2(100, 3);
  vector<Int_t> moduleType5Station2(100, 4);
  vector<Int_t> moduleType6Station2(100, 5);
  vector<Int_t> moduleType7Station2(100, 6);
  vector<Int_t> moduleType8Station2(100, 7);

  VectorMap moduleTypeMapStation2;
  moduleTypeMapStation2.insert( pair<Int_t, vector<Int_t> >(1, moduleType1Station2));
  moduleTypeMapStation2.insert( pair<Int_t, vector<Int_t> >(2, moduleType2Station2));
  moduleTypeMapStation2.insert( pair<Int_t, vector<Int_t> >(3, moduleType3Station2));
  moduleTypeMapStation2.insert( pair<Int_t, vector<Int_t> >(4, moduleType4Station2));
  moduleTypeMapStation2.insert( pair<Int_t, vector<Int_t> >(5, moduleType5Station2));
  moduleTypeMapStation2.insert( pair<Int_t, vector<Int_t> >(6, moduleType6Station2));
  moduleTypeMapStation2.insert( pair<Int_t, vector<Int_t> >(7, moduleType7Station2));
  moduleTypeMapStation2.insert( pair<Int_t, vector<Int_t> >(8, moduleType8Station2));

  vector<Int_t> moduleType1Station3(100, 0);
  vector<Int_t> moduleType2Station3(100, 1);
  vector<Int_t> moduleType3Station3(100, 2);
  vector<Int_t> moduleType4Station3(100, 3);
  vector<Int_t> moduleType5Station3(100, 4);
  vector<Int_t> moduleType6Station3(100, 5);
  vector<Int_t> moduleType7Station3(100, 6);
  vector<Int_t> moduleType8Station3(100, 7);

  VectorMap moduleTypeMapStation3;
  moduleTypeMapStation3.insert( pair<Int_t, vector<Int_t> >(1, moduleType1Station3));
  moduleTypeMapStation3.insert( pair<Int_t, vector<Int_t> >(2, moduleType2Station3));
  moduleTypeMapStation3.insert( pair<Int_t, vector<Int_t> >(3, moduleType3Station3));
  moduleTypeMapStation3.insert( pair<Int_t, vector<Int_t> >(4, moduleType4Station3));
  moduleTypeMapStation3.insert( pair<Int_t, vector<Int_t> >(5, moduleType5Station3));
  moduleTypeMapStation3.insert( pair<Int_t, vector<Int_t> >(6, moduleType6Station3));
  moduleTypeMapStation3.insert( pair<Int_t, vector<Int_t> >(7, moduleType7Station3));
  moduleTypeMapStation3.insert( pair<Int_t, vector<Int_t> >(8, moduleType8Station3));

  map<Int_t, VectorMap > moduleTypeStationMap;

  moduleTypeStationMap.insert( pair<Int_t, VectorMap >(1, moduleTypeMapStation1));
  moduleTypeStationMap.insert( pair<Int_t, VectorMap >(2, moduleTypeMapStation2));
  moduleTypeStationMap.insert( pair<Int_t, VectorMap >(3, moduleTypeMapStation3));

  return moduleTypeStationMap;
}

#endif

