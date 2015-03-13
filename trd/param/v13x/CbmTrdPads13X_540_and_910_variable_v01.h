// -------------------------------------------------------------------------
// -----                     CbmTrdPads header file                    -----
// -----               Created 03/12/2009 by D. Emschermann            -----
// -------------------------------------------------------------------------
//
// 
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

// v2.7
Int_t fst1_sect_count = 3;
Float_t fst1_pad_type[8][3][4] =   // array of pad geometries in the TRD (trd1mod[1-8])
  { { { 54., 18., 54./80,  1.50 },  
      { 54., 18., 54./80,  1.50 },  
      { 54., 18., 54./80,  1.50 } },
      // mod1
      //  1.0125 cm2
      // 6.75 mm wide pads (540mm / 80pads)
      // 36 x  15 mm                            ,   80 * 36 = 2880 pads
      // either  6 FEBs a 6 rows  6 * 6 = 36 rows
      // or      9 FEBs a 4 rows  9 * 4 = 36 rows
      // or     18 FEBs a 2 rows 18 * 2 = 36 rows

//    { { 54.,  9., 54./80,  3.00 },  
//      { 54., 36., 54./80,  3.00 },  
//      { 54.,  9., 54./80,  3.00 } },
//      // mod2 - ultimate - alternative
//      //  2.0250 cm2
//      // 6.75 mm wide pads (540mm / 80pads)
//      // 18 x  30 mm                            ,   80 * 18 = 1440 pads
//      // either  3 FEBs a 6 rows  3 * 6 = 18 rows
//      // or      9 FEBs a 2 rows  9 * 2 = 18 rows
    { { 54., 22., 54./80,  2.75 },  
      { 54., 10., 54./80,  2.50 },  
      { 54., 22., 54./80,  2.75 } },
      // mod2
      //  1.6875 cm2, 1.8563 cm2
      // 6.75 mm wide pads (540mm / 80pads)
      //  8 x  27.5 mm +  4 x  25.0 mm +  8 x  27.5 mm,   80 * 20 = 1600 pads
      // either  5 FEBs a 4 rows  5 * 4 = 20 rows
      // or     10 FEBs a 2 rows 10 * 2 = 20 rows

//    { { 54.,  4.5, 54./80,  4.50 },  
//      { 54., 45.0, 54./80,  4.50 },  
//      { 54.,  4.5, 54./80,  4.50 } },
//      // mod3 - ultimate - alternative
//      //  3.0375 cm2
//      // 6.75 mm wide pads (540mm / 80pads)
//      // 12 x  45 mm                            ,   80 * 12 =  960 pads
//      // either  2 FEBs a 6 rows  2 * 6 = 12 rows
//      // or      3 FEBs a 4 rows  3 * 4 = 12 rows
//      // or      6 FEBs a 2 rows  6 * 2 = 12 rows
    { { 54., 13.5, 54./80,  6.75 },  
      { 54., 27.0, 54./80,  6.75 },  
      { 54., 13.5, 54./80,  6.75 } },
      // mod3
      //  4.5563 cm2
      // 6.75 mm wide pads (540mm / 80pads)
      //  8 x  67.5 mm                          ,   80 *  8 =  640 pads
      // either  2 FEBs a 4 rows  2 * 4 =  8 rows
      // or      4 FEBs a 2 rows  4 * 2 =  8 rows

    { { 54., 13.5, 54./80,  6.75 },  
      { 54., 27.0, 54./80,  6.75 },  
      { 54., 13.5, 54./80,  6.75 } },
      // mod4 same as mod3
      //  4.5563 cm2
      // 6.75 mm wide pads (540mm / 80pads)
      //  8 x  67.5 mm                          ,   80 *  8 =  640 pads
      // either  2 FEBs a 4 rows  2 * 4 =  8 rows
      // or      4 FEBs a 2 rows  4 * 2 =  8 rows

//---

    { { 91., 37.5, 91./128,  3.75 },  
      { 91., 16.0, 91./128,  4.00 },  
      { 91., 37.5, 91./128,  3.75 } },
      // mod5
      //  2.6660 cm2, 2.8438 cm2
      // 7.11 mm wide pads (910mm / 128pads)
      // 10 x  37.5 mm +  4 x  40.0 mm + 10 x  37.5 mm,  128 * 24 = 3072 pads
      // either  6 FEBs a 4 rows  6 * 4 = 24 rows
      // or     12 FEBs a 2 rows 12 * 2 = 24 rows

    { { 91., 30.0, 91./128,  7.50 },  
      { 91., 31.0, 91./128,  7.75 },  
      { 91., 30.0, 91./128,  7.50 } },
      // mod6
      //  5.3320 cm2, 5.5098 cm2
      // 7.11 mm wide pads (910mm / 128pads)
      //  4 x  75.0 mm +  4 x  77.5 mm +  4 x  75.0 mm,  128 * 12 = 1536 pads
      // either  3 FEBs a 4 rows  3 * 4 = 12 rows
      // or      6 FEBs a 2 rows  6 * 2 = 12 rows

    { { 91., 22.5, 91./128, 11.25 },  
      { 91., 46.0, 91./128, 11.50 },  
      { 91., 22.5, 91./128, 11.25 } },
      // mod7
      //  7,9980 cm2, 8.1758 cm2
      // 7.11 mm wide pads (910mm / 128pads)
      //  2 x 112.5 mm +  4 x 115.0 mm +  2 x 112.5 mm,  128 *  8 = 1024 pads
      // either  2 FEBs a 4 rows  2 * 4 =  8 rows
      // or      4 FEBs a 2 rows  4 * 2 =  8 rows

    { { 91., 30.0, 91./128, 15.00 },  
      { 91., 31.0, 91./128, 15.50 },  
      { 91., 30.0, 91./128, 15.00 } } };
      // mod8
      //  10.6641 cm2, 11.0195 cm2
      // 7.11 mm wide pads (910mm / 128pads)
      //  2 x 150.0 mm +  2 x 155.0 mm +  2 x 150.0 mm,  128 *  6 =  768 pads
      // either  3 FEBs a 2 rows  3 * 2 =  6 rows

#endif
