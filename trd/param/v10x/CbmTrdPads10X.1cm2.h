// -------------------------------------------------------------------------
// -----                     CbmTrdPads header file                    -----
// -----               Created 03/12/2009 by D. Emschermann            -----
// -------------------------------------------------------------------------
//
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

// v1.1
  Int_t fst1_sect_count = 3;
  Float_t fst1_pad_type[6][3][4] =   // array of pad sizes in station 1
    { { { 46., 16., .5,  2.0 }, 
        { 46., 14., .5,  2.0 }, 
        { 46., 16., .5,  2.0 } },     
        //  8 x  20 mm +  7 x  20 mm +  8 x  20 mm,  92 * 23 = 2116 pads
      { { 46., 16., .5,  2.0 },  
        { 46., 14., .5,  2.0 },  
        { 46., 16., .5,  2.0 } },
        //  8 x  20 mm +  7 x  20 mm +  8 x  20 mm,  92 * 23 = 2116 pads
      { { 71., 34., .5,  2.0 }, 
        { 71.,  3., .5,  1.5 }, 
        { 71., 34., .5,  2.0 } },     
        // 17 x  20 mm +  2 x  15 mm + 17 x  20 mm, 142 * 36 = 5112 pads  
      { { 71., 34., .5,  2.0 }, 
        { 71.,  3., .5,  1.5 }, 
        { 71., 34., .5,  2.0 } },     
        // 17 x  20 mm +  2 x  15 mm + 17 x  20 mm, 142 * 36 = 5112 pads  
      { { 96., 16., .5,  2.0 },  
        { 96., 64., .5,  2.0 },  
        { 96., 16., .5,  2.0 } },
        //  8 x  20 mm + 32 x  20 mm +  8 x  20 mm, 192 * 48 = 9216 pads
      { { 96., 16., .5,  2.0 },  
        { 96., 64., .5,  2.0 },  
        { 96., 16., .5,  2.0 } } };
        //  8 x  20 mm + 32 x  20 mm +  8 x  20 mm, 192 * 48 = 9216 pads

  Int_t fst2_sect_count = 3;
  Float_t fst2_pad_type[6][3][4] =   // array of pad sizes in station 2
    { { { 46., 16., .5,  2.0 }, 
        { 46., 14., .5,  2.0 }, 
        { 46., 16., .5,  2.0 } },     
        //  8 x  20 mm  + 7 x  20 mm +  8 x  20 mm,  92 * 23 = 2116 pads
      { { 46., 16., .5,  2.0 },  
        { 46., 14., .5,  2.0 },  
        { 46., 16., .5,  2.0 } },
        //  8 x  20 mm +  7 x  20 mm +  8 x  20 mm,  92 * 23 = 2116 pads
      { { 71., 34., .5,  2.0 }, 
        { 71.,  3., .5,  1.5 }, 
        { 71., 34., .5,  2.0 } },     
        // 17 x  20 mm +  2 x  15 mm + 17 x  20 mm, 142 * 36 = 5112 pads  
      { { 71., 34., .5,  2.0 }, 
        { 71.,  3., .5,  1.5 }, 
        { 71., 34., .5,  2.0 } },     
        // 17 x  20 mm +  2 x  15 mm + 17 x  20 mm, 142 * 36 = 5112 pads  
      { { 96., 16., .5,  2.0 },  
        { 96., 64., .5,  2.0 },  
        { 96., 16., .5,  2.0 } },
        //  8 x  20 mm + 32 x  20 mm +  8 x  20 mm, 192 * 48 = 9216 pads
      { { 96., 16., .5,  2.0 },  
        { 96., 64., .5,  2.0 },  
        { 96., 16., .5,  2.0 } } };
        //  8 x  20 mm + 32 x  20 mm +  8 x  20 mm, 192 * 48 = 9216 pads

  Int_t fst3_sect_count = 3;
  Float_t fst3_pad_type[2][3][4] =   // array of pad sizes in station 3
    { { { 96., 16., .5,  2.0 },  
        { 96., 64., .5,  2.0 },  
        { 96., 16., .5,  2.0 } },
        //  8 x  20 mm + 32 x  20 mm +  8 x  20 mm, 192 * 48 = 9216 pads
      { { 96., 16., .5,  2.0 },  
        { 96., 64., .5,  2.0 },  
        { 96., 16., .5,  2.0 } } };
        //  8 x  20 mm + 32 x  20 mm +  8 x  20 mm, 192 * 48 = 9216 pads

#endif
