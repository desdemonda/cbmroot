// -------------------------------------------------------------------------
// -----                     CbmTrdPads header file                    -----
// -----               Created 03/12/2009 by D. Emschermann            -----
// -------------------------------------------------------------------------
//
// v10f - 20110525 DE - SIS100 setup consisting of station 2 renamed to TRD1
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

// v10f - SIS100
  Int_t fst1_sect_count = 3;
  Float_t fst1_pad_type[6][3][4] =   // array of pad sizes in station 1
    { { { 46., 18., .5,  4.5 }, 
        { 46., 10., .5,  5.0 }, 
        { 46., 18., .5,  4.5 } },      
        //  4 x  45 mm + 2 x  50 mm +  4 x  45 mm,  92 * 10 =  920 pads mod1
      { { 46., 18., .5,  4.5 }, 
        { 46., 10., .5,  5.0 }, 
        { 46., 18., .5,  4.5 } },
        //  4 x  45 mm + 2 x  50 mm +  4 x  45 mm,  92 * 10 =  920 pads mod1
      { { 71., 32., .5,  8.0 }, 
        { 71.,  7., .5,  7.0 }, 
        { 71., 32., .5,  8.0 } },     
        //  4 x  80 mm + 1 x  70 mm +  4 x  80 mm, 142 *  9 = 1278 pads mod2
      { { 71., 30., .5, 10.0 }, 
        { 71., 11., .5, 11.0 }, 
        { 71., 30., .5, 10.0 } },      
        //  3 x 100 mm + 1 x 110 mm +  3 x 100 mm, 142 *  7 =  994 pads mod2
      { { 96., 38., .5, 19.0 }, 
        { 96., 20., .5, 20.0 }, 
        { 96., 38., .5, 19.0 } },
        //  2 x 190 mm + 1 x 200 mm +  2 x 190 mm, 192 *  5 =  960 pads mod3
      { { 96., 38., .5, 19.0 }, 
        { 96., 20., .5, 20.0 }, 
        { 96., 38., .5, 19.0 } } };
        //  2 x 190 mm + 1 x 200 mm +  2 x 190 mm, 192 *  5 =  960 pads mod3

  Int_t fst2_sect_count = 3;
  Float_t fst2_pad_type[6][3][4] =   // array of pad sizes in station 2
    { { { 46., 16., .5,  2.0 }, 
        { 46., 14., .5,  2.0 }, 
        { 46., 16., .5,  2.0 } },     
        //  8 x  20 mm + 7 x  20 mm +  8 x  20 mm,  92 * 23 = 2116 pads mod1
      { { 46., 21., .5,  3.0 }, 
        { 46.,  4., .5,  2.0 }, 
        { 46., 21., .5,  3.0 } },     
        //  7 x  30 mm + 2 x  20 mm +  7 x  30 mm,  92 * 16 = 1472 pads mod1
      { { 71., 32., .5,  4.0 }, 
        { 71.,  7., .5,  3.5 }, 
        { 71., 32., .5,  4.0 } },     
        //  8 x  40 mm + 2 x  35 mm +  8 x  40 mm, 142 * 18 = 2556 pads mod2
      { { 71., 32., .5,  8.0 }, 
        { 71.,  7., .5,  7.0 }, 
        { 71., 32., .5,  8.0 } },     
        //  4 x  80 mm + 1 x  70 mm +  4 x  80 mm, 142 *  9 = 1278 pads mod2
      { { 96., 38., .5,  9.5 }, 
        { 96., 20., .5, 10.0 }, 
        { 96., 38., .5,  9.5 } },     
        //  4 x  95 mm + 2 x 100 mm +  4 x  95 mm, 192 * 10 = 1920 pads mod3
      { { 96., 38., .5, 19.0 }, 
        { 96., 20., .5, 20.0 }, 
        { 96., 38., .5, 19.0 } } };
        //  2 x 190 mm + 1 x 200 mm +  2 x 190 mm, 192 *  5 =  960 pads mod3

  Int_t fst3_sect_count = 3;
  Float_t fst3_pad_type[2][3][4] =   // array of pad sizes in station 3
    { { { 96., 32., .5,  8.0 }, 
        { 96., 32., .5,  8.0 }, 
        { 96., 32., .5,  8.0 } },     
        //  4 x  80 mm + 4 x  80 mm +  4 x  80 mm, 192 * 12 = 2304 pads mod3
      { { 96., 38., .5, 19.0 }, 
        { 96., 20., .5, 20.0 }, 
        { 96., 38., .5, 19.0 } } };
        //  2 x 190 mm + 1 x 200 mm +  2 x 190 mm, 192 *  5 =  960 pads mod3

#endif





