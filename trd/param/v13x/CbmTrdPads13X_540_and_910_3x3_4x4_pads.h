// -------------------------------------------------------------------------
// -----                     CbmTrdPads header file                    -----
// -----               Created 03/12/2009 by D. Emschermann            -----
// -------------------------------------------------------------------------
//
// 
// v2.10- 20130926 DE - v13x - 9 or 16 pads (3x3 / 4x4) for each padplane (3 sectors) for testing
// v2.9 - 20130926 DE - v13x - single pad (1x1) for each padplane (only 1 sector) for testing
// v2.8 - 20130624 DE - v13x - few pads (3x3, 5x5) for testing
// v2.7 - 20130327 DE - v13g - shrinked TRD (540 and 910 mm) - variable pad size 
// v2.6 - 20130327 DE - v13g - shrinked TRD (540 and 910 mm) - 1 cm2 fixed pad size for trd1mod[1-8]
// v2.5 - 20120606 DE - v12f - 1 cm2 fixed pad size for trd1mod[1-8]
// v2.4 - 20120511 DE - v12f - modify sector size of mod1 to look different from mod2
// v2.3 - 20120511 DE - v12c mod5 decrease pad size to 3.0 cm2 from 4.5 cm2
// v2.2 - 20120117 DE - adapt many padplane structure for v12x
// v1.4 - 20110616 DE - adapted to v11X with variable frame size - 570 mm and 960 mm - 7.5 mm pad width - 1 cm2
// v1.3 - 20110616 DE - adapted to v11X with variable frame size - 570 mm and 960 mm - 5.0 mm pad width - 1 cm2
// v1.2 - 20110524 DE - adapted to v11X (May11) with 24 TRD in the inner zone
// v1.1 - 20110523 DE - 5x20 mm2 sized pads almost everywhere, tracking test
// v1.0 - 20100708 DE - reduce pad size in trd1mod1-2 to 20/30 from 30/35 
//                      and in trd1mod3-1 to 80/80 from 95/100
// v0.9 - 20100601 DE - improve pad size ballance in trd1mod1-2 to 30/35 from 30/40
// v0.8 - 20100601 DE - implement 5 sectors per module for code testing
// v0.7 - 20100531 DE - implement 3 sectors of n*0.5 mm long pads per module
// v0.6 - 20100530 DE - implement 2 sectors with identical pads per module
// v0.5 - 20100525 DE - reduce the pad length in trd1mod1-2 of station 1 
//                      to 30.67 mm
// v0.4 - 20100520 FU - Implemented modules with single sector and same
//                      pad geometry as in version 0.3
// v0.3 - 20100511 DE - 5 mm pad width everywhere, fractional pad length
// v0.2 - 20091207 DE - fixed typo in pad x size in large module
// v0.1 - 20091204 DE - initial version

#ifndef CBMTRDPADS_H
#define CBMTRDPADS_H

// v2.10
Int_t fst1_sect_count = 3;
Float_t fst1_pad_type[8][3][4] =   // array of pad geometries in the TRD (trd1mod[1-8])
  { { { 54., 18.0, 54./3, 18.0 },  
      { 54., 18.0, 54./3, 18.0 },  
      { 54., 18.0, 54./3, 18.0 } },
      // mod1
      // 3x3 pads
    { { 54., 18.0, 54./3, 18.0 },  
      { 54., 18.0, 54./3, 18.0 },  
      { 54., 18.0, 54./3, 18.0 } },
      // mod2
      // 3x3 pads
    { { 54., 18.0, 54./3, 18.0 },  
      { 54., 18.0, 54./3, 18.0 },  
      { 54., 18.0, 54./3, 18.0 } },
      // mod3
      // 3x3 pads
    { { 54., 18.0, 54./3, 18.0 },  
      { 54., 18.0, 54./3, 18.0 },  
      { 54., 18.0, 54./3, 18.0 } },
      // mod4
      // 3x3 pads

//---

    { { 91., 22.75, 91./4, 22.75 },  
      { 91., 45.50, 91./4, 22.75 },  
      { 91., 22.75, 91./4, 22.75 } },
      // mod5
      // 4x4 pads
    { { 91., 22.75, 91./4, 22.75 },  
      { 91., 45.50, 91./4, 22.75 },  
      { 91., 22.75, 91./4, 22.75 } },
      // mod6
      // 4x4 pads
    { { 91., 22.75, 91./4, 22.75 },  
      { 91., 45.50, 91./4, 22.75 },  
      { 91., 22.75, 91./4, 22.75 } },
      // mod7
      // 4x4 pads
    { { 91., 22.75, 91./4, 22.75 },  
      { 91., 45.50, 91./4, 22.75 },  
      { 91., 22.75, 91./4, 22.75 } } };
      // mod8
      // 4x4 pads

#endif
