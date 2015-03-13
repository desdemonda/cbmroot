// -------------------------------------------------------------------------
// -----                     CbmTrdPads header file                    -----
// -----               Created 03/12/2009 by D. Emschermann            -----
// -------------------------------------------------------------------------
//
// v1.7 - 20110914 DE - v11x - 7.5 mm pad width - 4 cm2
// v1.6 - 20110914 DE - v11x - 7.5 mm pad width - 3 cm2
// v1.5 - 20110914 DE - v11x - 7.5 mm pad width - 2 cm2
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

// v1.7
  Int_t fst1_sect_count = 3;
  Float_t fst1_pad_type[6][3][4] =   // array of pad sizes in station 1
    { { { 57., 12., .75,  6.0 }, 
        { 57., 33., .75,  5.5 }, 
        { 57., 12., .75,  6.0 } },     
        //  2 x  60 mm +  6 x  55 mm +  2 x  60 mm,   76 * 10 =  760 pads
      { { 57., 12., .75,  6.0 }, 
        { 57., 33., .75,  5.5 }, 
        { 57., 12., .75,  6.0 } },     
        //
      { { 57., 12., .75,  6.0 }, 
        { 57., 33., .75,  5.5 }, 
        { 57., 12., .75,  6.0 } },     
        //
      { { 57., 12., .75,  6.0 }, 
        { 57., 33., .75,  5.5 }, 
        { 57., 12., .75,  6.0 } },     
        //
      { { 96., 30., .75,  6.0 },  
        { 96., 36., .75,  6.0 },  
        { 96., 30., .75,  6.0 } },
        //  5 x  60 mm +  6 x  60 mm +  5 x  60 mm,  128 * 16 = 2048 pads
      { { 96., 30., .75,  6.0 },  
        { 96., 36., .75,  6.0 },  
        { 96., 30., .75,  6.0 } } };
        //

  Int_t fst2_sect_count = 3;
  Float_t fst2_pad_type[6][3][4] =   // array of pad sizes in station 2
    { { { 57., 12., .75,  6.0 }, 
        { 57., 33., .75,  5.5 }, 
        { 57., 12., .75,  6.0 } },     
        //
      { { 57., 12., .75,  6.0 }, 
        { 57., 33., .75,  5.5 }, 
        { 57., 12., .75,  6.0 } },     
        //
      { { 57., 12., .75,  6.0 }, 
        { 57., 33., .75,  5.5 }, 
        { 57., 12., .75,  6.0 } },     
        //
      { { 57., 12., .75,  6.0 }, 
        { 57., 33., .75,  5.5 }, 
        { 57., 12., .75,  6.0 } },     
        //
      { { 96., 30., .75,  6.0 },  
        { 96., 36., .75,  6.0 },  
        { 96., 30., .75,  6.0 } },
        //
      { { 96., 30., .75,  6.0 },  
        { 96., 36., .75,  6.0 },  
        { 96., 30., .75,  6.0 } } };
        //

  Int_t fst3_sect_count = 3;
  Float_t fst3_pad_type[2][3][4] =   // array of pad sizes in station 3
    { { { 96., 30., .75,  6.0 },  
        { 96., 36., .75,  6.0 },  
        { 96., 30., .75,  6.0 } },
        //  5 x  60 mm +  6 x  60 mm +  5 x  60 mm,  128 * 16 = 2048 pads
      { { 96., 30., .75,  6.0 },  
        { 96., 36., .75,  6.0 },  
        { 96., 30., .75,  6.0 } } };
        //

#endif
