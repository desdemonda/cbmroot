// -------------------------------------------------------------------------
// -----                     CbmTrdPads header file                    -----
// -----               Created 03/12/2009 by D. Emschermann            -----
// -------------------------------------------------------------------------
//
// 
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

// v2.6
Int_t fst1_sect_count = 3;
Float_t fst1_pad_type[8][3][4] =   // array of pad geometries in the TRD (trd1mod[1-8])
  { { { 54., 18., 54./80,  1.5 },  
      { 54., 18., 54./80,  1.5 },  
      { 54., 18., 54./80,  1.5 } },
      // mod1
      //  1.0125 cm2, 6.75 mm wide pads (540mm / 80pads)
      // 12 x  15 mm + 12 x  15 mm + 12 x  15 mm,   80 * 36 = 2880 pads
    { { 54., 18., 54./80,  1.5 },  
      { 54., 18., 54./80,  1.5 },  
      { 54., 18., 54./80,  1.5 } },
      // mod2
      //  1.0125 cm2, 6.75 mm wide pads (540mm / 80pads)
      // 12 x  15 mm + 12 x  15 mm + 12 x  15 mm,   80 * 36 = 2880 pads
    { { 54., 18., 54./80,  1.5 },  
      { 54., 18., 54./80,  1.5 },  
      { 54., 18., 54./80,  1.5 } },
      // mod3
      //  1.0125 cm2, 6.75 mm wide pads (540mm / 80pads)
      // 12 x  15 mm + 12 x  15 mm + 12 x  15 mm,   80 * 36 = 2880 pads
    { { 54., 18., 54./80,  1.5 },  
      { 54., 18., 54./80,  1.5 },  
      { 54., 18., 54./80,  1.5 } },
      // mod4
      //  1.0125 cm2, 6.75 mm wide pads (540mm / 80pads)
      // 12 x  15 mm + 12 x  15 mm + 12 x  15 mm,   80 * 36 = 2880 pads
//---
    { { 91., 45., 91./128,  1.5 },  
      { 91.,  1., 91./128,  1.0 },  
      { 91., 45., 91./128,  1.5 } },
      // mod5
      //  1.0664 cm2, 7.11 mm wide pads (910mm / 128pads)
      // 30 x  15 mm +  1 x  10 mm + 30 x  15 mm,  128 * 61 = 7808 pads
    { { 91., 45., 91./128,  1.5 },  
      { 91.,  1., 91./128,  1.0 },  
      { 91., 45., 91./128,  1.5 } },
      // mod6
      //  1.0664 cm2, 7.11 mm wide pads (910mm / 128pads)
      // 30 x  15 mm +  1 x  10 mm + 30 x  15 mm,  128 * 61 = 7808 pads
    { { 91., 45., 91./128,  1.5 },  
      { 91.,  1., 91./128,  1.0 },  
      { 91., 45., 91./128,  1.5 } },
      // mod7
      //  1.0664 cm2, 7.11 mm wide pads (910mm / 128pads)
      // 30 x  15 mm +  1 x  10 mm + 30 x  15 mm,  128 * 61 = 7808 pads
    { { 91., 45., 91./128,  1.5 },  
      { 91.,  1., 91./128,  1.0 },  
      { 91., 45., 91./128,  1.5 } } };
      // mod8
      //  1.0664 cm2, 7.11 mm wide pads (910mm / 128pads)
      // 30 x  15 mm +  1 x  10 mm + 30 x  15 mm,  128 * 61 = 7808 pads

#endif
