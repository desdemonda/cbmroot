#ifndef L1Extrapolation_h
#define L1Extrapolation_h

#include "CbmL1Def.h"
#include "L1Field.h"
#include "L1TrackPar.h"

//#define cnst static const fvec
#define cnst const fvec  

// #define ANALYTICEXTRAPOLATION
#ifdef ANALYTICEXTRAPOLATION
inline void L1Extrapolate
( 
 L1TrackPar &T, // input track parameters (x,y,tx,ty,Q/p) and cov.matrix
 fvec        z_out  , // extrapolate to this z position
 fvec       qp0    , // use Q/p linearisation at this value
 L1FieldRegion &F,
 fvec *w = 0
 )
{
  //cout<<"Extrapolation..."<<endl;
  //
  //  Part of the analytic extrapolation formula with error (c_light*B*dz)^4/4!
  //  
  
  cnst ZERO = 0.0, ONE = 1.;
  cnst c_light = 0.000299792458, c_light_i = 1./c_light;

  cnst  
    c1 = 1., c2 = 2., c3 = 3., c4 = 4., c6 = 6., c9 = 9., c15 = 15., c18 = 18., c45 = 45.,
    c2i = 1./2., c3i = 1./3., c6i = 1./6., c12i = 1./12.;

  const fvec qp = T.qp;
  const fvec dz = (z_out - T.z);
  const fvec dz2 = dz*dz;
  const fvec dz3 = dz2*dz;

  // construct coefficients 

  const fvec x   = T.tx;
  const fvec y   = T.ty;
  const fvec xx  = x*x;
  const fvec xy = x*y;
  const fvec yy = y*y;
  const fvec y2 = y*c2;
  const fvec x2 = x*c2;
  const fvec x4 = x*c4;
  const fvec xx31 = xx*c3+c1;
  const fvec xx159 = xx*c15+c9;

  const fvec Ay = -xx-c1;
  const fvec Ayy = x*(xx*c3+c3);
  const fvec Ayz = -c2*xy; 
  const fvec Ayyy = -(c15*xx*xx+c18*xx+c3);

  const fvec Ayy_x = c3*xx31;
  const fvec Ayyy_x = -x4*xx159;

  const fvec Bx = yy+c1; 
  const fvec Byy = y*xx31; 
  const fvec Byz = c2*xx+c1;
  const fvec Byyy = -xy*xx159;

  const fvec Byy_x = c6*xy;
  const fvec Byyy_x = -y*(c45*xx+c9);
  const fvec Byyy_y = -x*xx159;

  // end of coefficients calculation

  const fvec t2   = c1 + xx + yy;
  const fvec t    = sqrt( t2 );
  const fvec h    = qp0*c_light;
  const fvec ht   = h*t;

  // get field integrals
  const fvec ddz = T.z-F.z0;
  const fvec Fx0 = F.cx0 + F.cx1*ddz + F.cx2*ddz*ddz;
  const fvec Fx1 = (F.cx1 + c2*F.cx2*ddz)*dz;
  const fvec Fx2 = F.cx2*dz2;
  const fvec Fy0 = F.cy0 + F.cy1*ddz + F.cy2*ddz*ddz;
  const fvec Fy1 = (F.cy1 + c2*F.cy2*ddz)*dz;
  const fvec Fy2 = F.cy2*dz2;
  const fvec Fz0 = F.cz0 + F.cz1*ddz + F.cz2*ddz*ddz;
  const fvec Fz1 = (F.cz1 + c2*F.cz2*ddz)*dz;
  const fvec Fz2 = F.cz2*dz2;

  // 

  const fvec sx = ( Fx0 + Fx1*c2i + Fx2*c3i  );
  const fvec sy = ( Fy0 + Fy1*c2i + Fy2*c3i );
  const fvec sz = ( Fz0 + Fz1*c2i + Fz2*c3i );

  const fvec Sx = ( Fx0*c2i + Fx1*c6i + Fx2*c12i );
  const fvec Sy = ( Fy0*c2i + Fy1*c6i + Fy2*c12i );
  const fvec Sz = ( Fz0*c2i + Fz1*c6i + Fz2*c12i );

  fvec syz;
  { 
    cnst 
      d = 1./360., 
      c00 = 30.*6.*d, c01 = 30.*2.*d,   c02 = 30.*d,
      c10 = 3.*40.*d, c11 = 3.*15.*d,   c12 = 3.*8.*d, 
      c20 = 2.*45.*d, c21 = 2.*2.*9.*d, c22 = 2.*2.*5.*d;
    syz = Fy0*( c00*Fz0 + c01*Fz1 + c02*Fz2) 
      +   Fy1*( c10*Fz0 + c11*Fz1 + c12*Fz2) 
      +   Fy2*( c20*Fz0 + c21*Fz1 + c22*Fz2) ;
  }

  fvec Syz;
  {
    cnst 
      d = 1./2520., 
      c00 = 21.*20.*d, c01 = 21.*5.*d, c02 = 21.*2.*d,
      c10 =  7.*30.*d, c11 =  7.*9.*d, c12 =  7.*4.*d, 
      c20 =  2.*63.*d, c21 = 2.*21.*d, c22 = 2.*10.*d;
    Syz = Fy0*( c00*Fz0 + c01*Fz1 + c02*Fz2 ) 
      +   Fy1*( c10*Fz0 + c11*Fz1 + c12*Fz2 ) 
      +   Fy2*( c20*Fz0 + c21*Fz1 + c22*Fz2 ) ;
  }

  const fvec syy  = sy*sy*c2i;
  const fvec syyy = syy*sy*c3i;

  fvec Syy ;   
  {
    cnst  
    d= 1./2520., c00= 420.*d, c01= 21.*15.*d, c02= 21.*8.*d,
    c03= 63.*d, c04= 70.*d, c05= 20.*d;
    Syy =  Fy0*(c00*Fy0+c01*Fy1+c02*Fy2) + Fy1*(c03*Fy1+c04*Fy2) + c05*Fy2*Fy2 ;
  }
  
  fvec Syyy;
  {
    cnst 
      d = 1./181440., 
      c000 =   7560*d, c001 = 9*1008*d, c002 = 5*1008*d, 
      c011 = 21*180*d, c012 = 24*180*d, c022 =  7*180*d, 
      c111 =    540*d, c112 =    945*d, c122 =    560*d, c222 = 112*d;
    const fvec Fy22 = Fy2*Fy2;
    Syyy = Fy0*( Fy0*(c000*Fy0+c001*Fy1+c002*Fy2)+ Fy1*(c011*Fy1+c012*Fy2)+c022*Fy22 )
      +    Fy1*( Fy1*(c111*Fy1+c112*Fy2)+c122*Fy22) + c222*Fy22*Fy2                  ;
  }
  
  
  const fvec sA1   = sx*xy   + sy*Ay   + sz*y ;
  const fvec sA1_x = sx*y - sy*x2 ;
  const fvec sA1_y = sx*x + sz ;

  const fvec sB1   = sx*Bx   - sy*xy   - sz*x ;
  const fvec sB1_x = -sy*y - sz ;
  const fvec sB1_y = sx*y2 - sy*x ;

  const fvec SA1   = Sx*xy   + Sy*Ay   + Sz*y ;
  const fvec SA1_x = Sx*y - Sy*x2 ;
  const fvec SA1_y = Sx*x + Sz;

  const fvec SB1   = Sx*Bx   - Sy*xy   - Sz*x ;
  const fvec SB1_x = -Sy*y - Sz;
  const fvec SB1_y = Sx*y2 - Sy*x;


  const fvec sA2   = syy*Ayy   + syz*Ayz ;
  const fvec sA2_x = syy*Ayy_x - syz*y2 ;
  const fvec sA2_y = -syz*x2 ;
  const fvec sB2   = syy*Byy   + syz*Byz  ;
  const fvec sB2_x = syy*Byy_x + syz*x4 ;
  const fvec sB2_y = syy*xx31 ;
  
  const fvec SA2   = Syy*Ayy   + Syz*Ayz ;
  const fvec SA2_x = Syy*Ayy_x - Syz*y2 ;
  const fvec SA2_y = -Syz*x2 ;
  const fvec SB2   = Syy*Byy   + Syz*Byz ;
  const fvec SB2_x = Syy*Byy_x + Syz*x4 ;
  const fvec SB2_y = Syy*xx31 ;

  const fvec sA3   = syyy*Ayyy  ;
  const fvec sA3_x = syyy*Ayyy_x;
  const fvec sB3   = syyy*Byyy  ;
  const fvec sB3_x = syyy*Byyy_x;
  const fvec sB3_y = syyy*Byyy_y;

 
  const fvec SA3   = Syyy*Ayyy  ;
  const fvec SA3_x = Syyy*Ayyy_x;
  const fvec SB3   = Syyy*Byyy  ;
  const fvec SB3_x = Syyy*Byyy_x;
  const fvec SB3_y = Syyy*Byyy_y;

  const fvec ht1 = ht*dz;
  const fvec ht2 = ht*ht*dz2;
  const fvec ht3 = ht*ht*ht*dz3;
  const fvec ht1sA1 = ht1*sA1;
  const fvec ht1sB1 = ht1*sB1;
  const fvec ht1SA1 = ht1*SA1;
  const fvec ht1SB1 = ht1*SB1;
  const fvec ht2sA2 = ht2*sA2;
  const fvec ht2SA2 = ht2*SA2;
  const fvec ht2sB2 = ht2*sB2;
  const fvec ht2SB2 = ht2*SB2;
  const fvec ht3sA3 = ht3*sA3;
  const fvec ht3sB3 = ht3*sB3; 
  const fvec ht3SA3 = ht3*SA3;
  const fvec ht3SB3 = ht3*SB3;

  fvec initialised = ZERO;
  if(w) //TODO use operator {?:}
  {
    const fvec zero = ZERO;
    initialised = fvec( zero < *w );
  }
  else
  {
    const fvec one = ONE;
    const fvec zero = ZERO;
    initialised = fvec( zero < one );
  }

  T.x  += (((x + ht1SA1 + ht2SA2 + ht3SA3)*dz)&initialised) ;
  T.y  += (((y + ht1SB1 + ht2SB2 + ht3SB3)*dz)&initialised) ;
  T.tx += ((ht1sA1 + ht2sA2 + ht3sA3)&initialised);
  T.ty += ((ht1sB1 + ht2sB2 + ht3sB3)&initialised);
  T.z  += ((dz)&initialised);

  const fvec ctdz  = c_light*t*dz;
  const fvec ctdz2 = c_light*t*dz2;

  const fvec dqp = qp - qp0;
  const fvec t2i = c1*rcp(t2);// /t2;
  const fvec xt2i = x*t2i;
  const fvec yt2i = y*t2i;
  const fvec tmp0 = ht1SA1 + c2*ht2SA2 + c3*ht3SA3;
  const fvec tmp1 = ht1SB1 + c2*ht2SB2 + c3*ht3SB3;
  const fvec tmp2 = ht1sA1 + c2*ht2sA2 + c3*ht3sA3;
  const fvec tmp3 = ht1sB1 + c2*ht2sB2 + c3*ht3sB3;

  const fvec j02 = dz*(c1 + xt2i*tmp0 + ht1*SA1_x + ht2*SA2_x + ht3*SA3_x);
  const fvec j12 = dz*(     xt2i*tmp1 + ht1*SB1_x + ht2*SB2_x + ht3*SB3_x);
  const fvec j22 =     c1 + xt2i*tmp2 + ht1*sA1_x + ht2*sA2_x + ht3*sA3_x ;
  const fvec j32 =          xt2i*tmp3 + ht1*sB1_x + ht2*sB2_x + ht3*sB3_x ;
    
  const fvec j03 = dz*(     yt2i*tmp0 + ht1*SA1_y + ht2*SA2_y );
  const fvec j13 = dz*(c1 + yt2i*tmp1 + ht1*SB1_y + ht2*SB2_y + ht3*SB3_y );
  const fvec j23 =          yt2i*tmp2 + ht1*sA1_y + ht2*sA2_y  ;
  const fvec j33 =     c1 + yt2i*tmp3 + ht1*sB1_y + ht2*sB2_y + ht3*sB3_y ;
    
  const fvec j04 = ctdz2*( SA1 + c2*ht1*SA2 + c3*ht2*SA3 );
  const fvec j14 = ctdz2*( SB1 + c2*ht1*SB2 + c3*ht2*SB3 );
  const fvec j24 = ctdz *( sA1 + c2*ht1*sA2 + c3*ht2*sA3 );
  const fvec j34 = ctdz *( sB1 + c2*ht1*sB2 + c3*ht2*sB3 );
  

  // extrapolate inverse momentum
  
  T.x +=((j04*dqp)&initialised);
  T.y +=((j14*dqp)&initialised);
  T.tx+=((j24*dqp)&initialised);
  T.ty+=((j34*dqp)&initialised);

 //          covariance matrix transport 
 
  const fvec c42 = T.C42, c43 = T.C43;

  const fvec cj00 = T.C00 + T.C20*j02 + T.C30*j03 + T.C40*j04;
//  const fvec cj10 = T.C10 + T.C21*j02 + T.C31*j03 + T.C41*j04;
  const fvec cj20 = T.C20 + T.C22*j02 + T.C32*j03 + c42*j04;
  const fvec cj30 = T.C30 + T.C32*j02 + T.C33*j03 + c43*j04;
 
  const fvec cj01 = T.C10 + T.C20*j12 + T.C30*j13 + T.C40*j14;
  const fvec cj11 = T.C11 + T.C21*j12 + T.C31*j13 + T.C41*j14;
  const fvec cj21 = T.C21 + T.C22*j12 + T.C32*j13 + c42*j14;
  const fvec cj31 = T.C31 + T.C32*j12 + T.C33*j13 + c43*j14;

//  const fvec cj02 = T.C20*j22 + T.C30*j23 + T.C40*j24;
//  const fvec cj12 = T.C21*j22 + T.C31*j23 + T.C41*j24;
  const fvec cj22 = T.C22*j22 + T.C32*j23 + c42*j24;
  const fvec cj32 = T.C32*j22 + T.C33*j23 + c43*j24;

//  const fvec cj03 = T.C20*j32 + T.C30*j33 + T.C40*j34;
//  const fvec cj13 = T.C21*j32 + T.C31*j33 + T.C41*j34;
  const fvec cj23 = T.C22*j32 + T.C32*j33 + c42*j34;
  const fvec cj33 = T.C32*j32 + T.C33*j33 + c43*j34;

  T.C40+= ((c42*j02 + c43*j03 + T.C44*j04) & initialised); // cj40
  T.C41+= ((c42*j12 + c43*j13 + T.C44*j14) & initialised); // cj41
  T.C42 = ((c42*j22 + c43*j23 + T.C44*j24) & initialised) + (T.C42&(!initialised)); // cj42
  T.C43 = ((c42*j32 + c43*j33 + T.C44*j34) & initialised) + (T.C43&(!initialised)); // cj43

  T.C00 = ((cj00 + j02*cj20 + j03*cj30 + j04*T.C40) & initialised) + (T.C00&(!initialised));
  T.C10 = ((cj01 + j02*cj21 + j03*cj31 + j04*T.C41) & initialised) + (T.C10&(!initialised));
  T.C11 = ((cj11 + j12*cj21 + j13*cj31 + j14*T.C41) & initialised) + (T.C11&(!initialised));

  T.C20 = ((j22*cj20 + j23*cj30 + j24*T.C40) & initialised) + (T.C20&(!initialised)) ;
  T.C30 = ((j32*cj20 + j33*cj30 + j34*T.C40) & initialised) + (T.C30&(!initialised)) ;
  T.C21 = ((j22*cj21 + j23*cj31 + j24*T.C41) & initialised) + (T.C21&(!initialised)) ;
  T.C31 = ((j32*cj21 + j33*cj31 + j34*T.C41) & initialised) + (T.C31&(!initialised)) ;
  T.C22 = ((j22*cj22 + j23*cj32 + j24*T.C42) & initialised) + (T.C22&(!initialised)) ;
  T.C32 = ((j32*cj22 + j33*cj32 + j34*T.C42) & initialised) + (T.C32&(!initialised)) ;
  T.C33 = ((j32*cj23 + j33*cj33 + j34*T.C43) & initialised) + (T.C33&(!initialised)) ;
  //cout<<"Extrapolation ok"<<endl;

}
#else
inline void L1Extrapolate // extrapolates track parameters and returns jacobian for extrapolation of CovMatrix
(
 L1TrackPar &T, // input track parameters (x,y,tx,ty,Q/p) and cov.matrix
 fvec        z_out  , // extrapolate to this z position
 fvec       qp0    , // use Q/p linearisation at this value
 const L1FieldRegion &F,
 fvec *w = 0
 )
{
    //
  // Forth-order Runge-Kutta method for solution of the equation
  // of motion of a particle with parameter qp = Q /P
  //              in the magnetic field B()
  //
  //        | x |          tx
  //        | y |          ty
  // d/dz = | tx| = ft * ( ty * ( B(3)+tx*b(1) ) - (1+tx**2)*B(2) )
  //        | ty|   ft * (-tx * ( B(3)+ty+b(2)   - (1+ty**2)*B(1) )  ,
  //
  //   where  ft = c_light*qp*sqrt ( 1 + tx**2 + ty**2 ) .
  //
  //  In the following for RK step  :
  //
  //     x=x[0], y=x[1], tx=x[3], ty=x[4].
  //
  //========================================================================
  //
  //  NIM A395 (1997) 169-184; NIM A426 (1999) 268-282.
  //
  //  the routine is based on LHC(b) utility code
  //
  //========================================================================


  cnst ZERO = 0.0, ONE = 1.;
  cnst c_light = 0.000299792458;
  
  const fvec a[4] = {0.0f, 0.5f, 0.5f, 1.0f};
  const fvec c[4] = {1.0f/6.0f, 1.0f/3.0f, 1.0f/3.0f, 1.0f/6.0f};
  const fvec b[4] = {0.0f, 0.5f, 0.5f, 1.0f};
  
  int step4;
  fvec k[16],x0[4],x[4],k1[16];
  fvec Ax[4],Ay[4],Ax_tx[4],Ay_tx[4],Ax_ty[4],Ay_ty[4];

  //----------------------------------------------------------------

  fvec qp_in = T.qp;
  const fvec z_in  = T.z;
  const fvec h     = (z_out - T.z);
  fvec hC    = h * c_light;
  x0[0] = T.x; x0[1] = T.y;
  x0[2] = T.tx; x0[3] = T.ty;
  //
  //   Runge-Kutta step
  //

  int step;
  int i;

  fvec B[4][3];
  for (step = 0; step < 4; ++step) {
    F.Get( z_in  + a[step] * h, B[step] );
  }
  
  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
      }
      else
      {
        x[i] = x0[i] + b[step] * k[step*4-4+i];
      }
    }

    fvec tx = x[2]; 
    fvec ty = x[3]; 
    fvec tx2 = tx * tx; 
    fvec ty2 = ty * ty; 
    fvec txty = tx * ty;
    fvec tx2ty21= 1.f + tx2 + ty2; 
    // if( tx2ty21 > 1.e4 ) return 1;
    fvec I_tx2ty21 = 1.f / tx2ty21 * qp0;
    fvec tx2ty2 = sqrt(tx2ty21 ) ; 
    //   fvec I_tx2ty2 = qp0 * hC / tx2ty2 ; unsused ???
    tx2ty2 *= hC; 
    fvec tx2ty2qp = tx2ty2 * qp0;
    Ax[step] = ( txty*B[step][0] + ty*B[step][2] - ( 1.f + tx2 )*B[step][1] ) * tx2ty2;
    Ay[step] = (-txty*B[step][1] - tx*B[step][2] + ( 1.f + ty2 )*B[step][0] ) * tx2ty2;

    Ax_tx[step] = Ax[step]*tx*I_tx2ty21 + ( ty*B[step][0]-2.f*tx*B[step][1])*tx2ty2qp;
    Ax_ty[step] = Ax[step]*ty*I_tx2ty21 + ( tx*B[step][0]+       B[step][2])*tx2ty2qp;
    Ay_tx[step] = Ay[step]*tx*I_tx2ty21 + (-ty*B[step][1]-       B[step][2])*tx2ty2qp;
    Ay_ty[step] = Ay[step]*ty*I_tx2ty21 + (-tx*B[step][1]+2.f*ty*B[step][0])*tx2ty2qp;

    step4 = step * 4;
    k[step4  ] = tx * h;
    k[step4+1] = ty * h;
    k[step4+2] = Ax[step] * qp0;
    k[step4+3] = Ay[step] * qp0;

  }  // end of Runge-Kutta steps

  fvec initialised = ZERO;
  if(w) //TODO use operator {?:}
  {
    const fvec zero = ZERO;
    initialised = fvec( zero < *w );
  }
  else
  {
    const fvec one = ONE;
    const fvec zero = ZERO;
    initialised = fvec( zero < one );
  }
  
  {
    T.x =   ( (x0[0]+c[0]*k[0]+c[1]*k[4+0]+c[2]*k[8+0]+c[3]*k[12+0]) & initialised) + ( (!initialised) & T.x);
    T.y =   ( (x0[1]+c[0]*k[1]+c[1]*k[4+1]+c[2]*k[8+1]+c[3]*k[12+1]) & initialised) + ( (!initialised) & T.y);
    T.tx =  ( (x0[2]+c[0]*k[2]+c[1]*k[4+2]+c[2]*k[8+2]+c[3]*k[12+2]) & initialised) + ( (!initialised) & T.tx);
    T.ty =  ( (x0[3]+c[0]*k[3]+c[1]*k[4+3]+c[2]*k[8+3]+c[3]*k[12+3]) & initialised) + ( (!initialised) & T.ty);
    T.z =   ( z_out & initialised)  + ( (!initialised) & T.z);
  }

  //
  //     Derivatives    dx/dqp
  //

  x0[0] = 0.f; x0[1] = 0.f; x0[2] = 0.f; x0[3] = 0.f;

  //
  //   Runge-Kutta step for derivatives dx/dqp

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
      } else
      {
        x[i] = x0[i] + b[step] * k1[step*4-4+i];
      }
    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    k1[step4+2] = Ax[step] + Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    k1[step4+3] = Ay[step] + Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

  }  // end of Runge-Kutta steps for derivatives dx/dqp

  fvec J[25];

  for (i = 0; i < 4; ++i ) {
    J[20+i]=x0[i]+c[0]*k1[i]+c[1]*k1[4+i]+c[2]*k1[8+i]+c[3]*k1[12+i];
  }
  J[24] = 1.;
  //
  //      end of derivatives dx/dqp
  //

  //     Derivatives    dx/tx
  //

  x0[0] = 0.f; x0[1] = 0.f; x0[2] = 1.f; x0[3] = 0.f;

  //
  //   Runge-Kutta step for derivatives dx/dtx
  //

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];
      }
      else
        if ( i!=2 )
      {
        x[i] = x0[i] + b[step] * k1[step*4-4+i];
      }
    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
        // k1[step4+2] = Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    k1[step4+3] = Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

  }  // end of Runge-Kutta steps for derivatives dx/dtx

  for (i = 0; i < 4; ++i ) {
    if(i != 2)
    {
      J[10+i]=x0[i]+c[0]*k1[i]+c[1]*k1[4+i]+c[2]*k1[8+i]+c[3]*k1[12+i];
    }
  }
  //      end of derivatives dx/dtx
  J[12] = 1.f;
  J[14] = 0.f;

  //     Derivatives    dx/ty
  //

  x0[0] = 0.f; x0[1] = 0.f; x0[2] = 0.f; x0[3] = 1.f;

  //
  //   Runge-Kutta step for derivatives dx/dty
  //

  for (step = 0; step < 4; ++step) {
    for(i=0; i < 4; ++i) {
      if(step == 0) {
        x[i] = x0[i];           // ty fixed
      } else 
        if(i!=3) {
        x[i] = x0[i] + b[step] * k1[step*4-4+i];
      }

    }
    step4 = step * 4;
    k1[step4  ] = x[2] * h;
    k1[step4+1] = x[3] * h;
    k1[step4+2] = Ax_tx[step] * x[2] + Ax_ty[step] * x[3];
    //    k1[step4+3] = Ay_tx[step] * x[2] + Ay_ty[step] * x[3];

  }  // end of Runge-Kutta steps for derivatives dx/dty

  for (i = 0; i < 3; ++i ) {
    J[15+i]=x0[i]+c[0]*k1[i]+c[1]*k1[4+i]+c[2]*k1[8+i]+c[3]*k1[12+i];
  }
  //      end of derivatives dx/dty
  J[18] = 1.;
  J[19] = 0.;

  //
  //    derivatives dx/dx and dx/dy

  for(i = 0; i < 10; ++i){ J[i] = 0.;}
  J[0] = 1.; J[6] = 1.;

  fvec dqp = qp_in - qp0;
  
  { // update parameters
    T.x += ((J[5*4+0]*dqp) & initialised);
    T.y += ((J[5*4+1]*dqp) & initialised);
    T.tx +=((J[5*4+2]*dqp) & initialised);
    T.ty +=((J[5*4+3]*dqp) & initialised);
  }
   
  //          covariance matrix transport 
  
//   // if ( C_in&&C_out ) CbmKFMath::multQtSQ( 5, J, C_in, C_out); // TODO
//   j(0,2) = J[5*2 + 0];
//   j(1,2) = J[5*2 + 1];
//   j(2,2) = J[5*2 + 2];
//   j(3,2) = J[5*2 + 3];
//     
//   j(0,3) = J[5*3 + 0];
//   j(1,3) = J[5*3 + 1];
//   j(2,3) = J[5*3 + 2];
//   j(3,3) = J[5*3 + 3];
//     
//   j(0,4) = J[5*4 + 0];
//   j(1,4) = J[5*4 + 1];
//   j(2,4) = J[5*4 + 2];
//   j(3,4) = J[5*4 + 3];
  
  const fvec c42 = T.C42, c43 = T.C43;

  const fvec cj00 = T.C00 + T.C20*J[5*2 + 0] + T.C30*J[5*3 + 0] + T.C40*J[5*4 + 0];
  // const fvec cj10 = T.C10 + T.C21*J[5*2 + 0] + T.C31*J[5*3 + 0] + T.C41*J[5*4 + 0];
  const fvec cj20 = T.C20 + T.C22*J[5*2 + 0] + T.C32*J[5*3 + 0] + c42*J[5*4 + 0];
  const fvec cj30 = T.C30 + T.C32*J[5*2 + 0] + T.C33*J[5*3 + 0] + c43*J[5*4 + 0];
 
  const fvec cj01 = T.C10 + T.C20*J[5*2 + 1] + T.C30*J[5*3 + 1] + T.C40*J[5*4 + 1];
  const fvec cj11 = T.C11 + T.C21*J[5*2 + 1] + T.C31*J[5*3 + 1] + T.C41*J[5*4 + 1];
  const fvec cj21 = T.C21 + T.C22*J[5*2 + 1] + T.C32*J[5*3 + 1] + c42*J[5*4 + 1];
  const fvec cj31 = T.C31 + T.C32*J[5*2 + 1] + T.C33*J[5*3 + 1] + c43*J[5*4 + 1];

  // const fvec cj02 = T.C20*J[5*2 + 2] + T.C30*J[5*3 + 2] + T.C40*J[5*4 + 2];
  // const fvec cj12 = T.C21*J[5*2 + 2] + T.C31*J[5*3 + 2] + T.C41*J[5*4 + 2];
  const fvec cj22 = T.C22*J[5*2 + 2] + T.C32*J[5*3 + 2] + c42*J[5*4 + 2];
  const fvec cj32 = T.C32*J[5*2 + 2] + T.C33*J[5*3 + 2] + c43*J[5*4 + 2];

  // const fvec cj03 = T.C20*J[5*2 + 3] + T.C30*J[5*3 + 3] + T.C40*J[5*4 + 3];
  // const fvec cj13 = T.C21*J[5*2 + 3] + T.C31*J[5*3 + 3] + T.C41*J[5*4 + 3];
  const fvec cj23 = T.C22*J[5*2 + 3] + T.C32*J[5*3 + 3] + c42*J[5*4 + 3];
  const fvec cj33 = T.C32*J[5*2 + 3] + T.C33*J[5*3 + 3] + c43*J[5*4 + 3];

  T.C40 += ((c42*J[5*2 + 0] + c43*J[5*3 + 0] + T.C44*J[5*4 + 0]) & initialised); // cj40
  T.C41 += ((c42*J[5*2 + 1] + c43*J[5*3 + 1] + T.C44*J[5*4 + 1]) & initialised); // cj41
  T.C42 = ((c42*J[5*2 + 2] + c43*J[5*3 + 2] + T.C44*J[5*4 + 2]) & initialised) + ((!initialised) & T.C42);
  T.C43 = ((c42*J[5*2 + 3] + c43*J[5*3 + 3] + T.C44*J[5*4 + 3]) & initialised) + ((!initialised) & T.C43);

  T.C00 = ((cj00 + J[5*2 + 0]*cj20 + J[5*3 + 0]*cj30 + J[5*4 + 0]*T.C40)& initialised) + ((!initialised) & T.C00);
  T.C10 = ((cj01 + J[5*2 + 0]*cj21 + J[5*3 + 0]*cj31 + J[5*4 + 0]*T.C41) & initialised) + ((!initialised) & T.C10);
  T.C11 = ((cj11 + J[5*2 + 1]*cj21 + J[5*3 + 1]*cj31 + J[5*4 + 1]*T.C41) & initialised) + ((!initialised) & T.C11);

  T.C20 = ((J[5*2 + 2]*cj20 + J[5*3 + 2]*cj30 + J[5*4 + 2]*T.C40) & initialised) + ((!initialised) & T.C20);
  T.C30 = ((J[5*2 + 3]*cj20 + J[5*3 + 3]*cj30 + J[5*4 + 3]*T.C40) & initialised) + ((!initialised) & T.C30);
  T.C21 = ((J[5*2 + 2]*cj21 + J[5*3 + 2]*cj31 + J[5*4 + 2]*T.C41) & initialised) + ((!initialised) & T.C21);
  T.C31 = ((J[5*2 + 3]*cj21 + J[5*3 + 3]*cj31 + J[5*4 + 3]*T.C41) & initialised) + ((!initialised) & T.C31);
  T.C22 = ((J[5*2 + 2]*cj22 + J[5*3 + 2]*cj32 + J[5*4 + 2]*T.C42) & initialised) + ((!initialised) & T.C22);
  T.C32 = ((J[5*2 + 3]*cj22 + J[5*3 + 3]*cj32 + J[5*4 + 3]*T.C42) & initialised) + ((!initialised) & T.C32);
  T.C33 = ((J[5*2 + 3]*cj23 + J[5*3 + 3]*cj33 + J[5*4 + 3]*T.C43) & initialised) + ((!initialised) & T.C33);
}
#endif //ANALYTICEXTRAPOLATION

inline void L1Extrapolate0
( 
 L1TrackPar &T, // input track parameters (x,y,tx,ty,Q/p) and cov.matrix
 fvec       z_out  , // extrapolate to this z position
 L1FieldRegion &F
 )
{
  //
  //  Part of the analytic extrapolation formula with error (c_light*B*dz)^4/4!
  //  
  
  cnst c_light = 0.000299792458;

  cnst  
    c1 = 1.,
    c2i = 1./2., c3i = 1./3., c6i = 1./6., c12i = 1./12.;

  const fvec qp = T.qp;
  const fvec dz = (z_out - T.z);
  const fvec dz2 = dz*dz;

  // construct coefficients 

  const fvec x   = T.tx;
  const fvec y   = T.ty;
  const fvec xx  = x*x;
  const fvec xy = x*y;
  const fvec yy = y*y;
  const fvec Ay = -xx-c1;
  const fvec Bx = yy+c1; 
  const fvec ct    = c_light*sqrt( c1 + xx + yy );

  const fvec dzc2i  = dz*c2i;
  const fvec dz2c3i = dz2*c3i;
  const fvec dzc6i  = dz*c6i;
  const fvec dz2c12i= dz2*c12i;

  const fvec sx = ( F.cx0 + F.cx1*dzc2i + F.cx2*dz2c3i  );
  const fvec sy = ( F.cy0 + F.cy1*dzc2i + F.cy2*dz2c3i );
  const fvec sz = ( F.cz0 + F.cz1*dzc2i + F.cz2*dz2c3i );

  const fvec Sx = ( F.cx0*c2i + F.cx1*dzc6i + F.cx2*dz2c12i );
  const fvec Sy = ( F.cy0*c2i + F.cy1*dzc6i + F.cy2*dz2c12i );
  const fvec Sz = ( F.cz0*c2i + F.cz1*dzc6i + F.cz2*dz2c12i );
  
  
  const fvec ctdz  = ct*dz;
  const fvec ctdz2 = ct*dz2;
    
  const fvec j04 = ctdz2* ( Sx*xy   + Sy*Ay   + Sz*y );
  const fvec j14 = ctdz2* ( Sx*Bx   - Sy*xy   - Sz*x );
  const fvec j24 = ctdz * ( sx*xy   + sy*Ay   + sz*y );
  const fvec j34 = ctdz * ( sx*Bx   - sy*xy   - sz*x );
    
  T.x += x*dz + j04*qp;
  T.y += y*dz + j14*qp;
  T.tx+=j24*qp;
  T.ty+=j34*qp;
  T.z  += dz;

  //          covariance matrix transport 

  const fvec cj00 = T.C00 + T.C20*dz + T.C40*j04;
//  const fvec cj10 = T.C10 + T.C21*dz + T.C41*j04;
  const fvec cj20 = T.C20 + T.C22*dz + T.C42*j04;
  const fvec cj30 = T.C30 + T.C32*dz + T.C43*j04;
 
  const fvec cj01 = T.C10 + T.C30*dz + T.C40*j14;
  const fvec cj11 = T.C11 + T.C31*dz + T.C41*j14;
  const fvec cj21 = T.C21 + T.C32*dz + T.C42*j14;
  const fvec cj31 = T.C31 + T.C33*dz + T.C43*j14;
 
//  const fvec cj02 = T.C20 +  T.C40*j24;
//  const fvec cj12 = T.C21 +  T.C41*j24;
  const fvec cj22 = T.C22 +  T.C42*j24;
  const fvec cj32 = T.C32 +  T.C43*j24;
 
//  const fvec cj03 = T.C30 + T.C40*j34;
//  const fvec cj13 = T.C31 + T.C41*j34;
//  const fvec cj23 = T.C32 + T.C42*j34;
  const fvec cj33 = T.C33 + T.C43*j34;

  T.C40+= T.C42*dz + T.C44*j04; // cj40
  T.C41+= T.C43*dz + T.C44*j14; // cj41
  T.C42+=            T.C44*j24; // cj42
  T.C43+=            T.C44*j34; // cj43

  T.C00 = cj00 + dz*cj20 + j04*T.C40;
  T.C10 = cj01 + dz*cj21 + j04*T.C41;
  T.C11 = cj11 + dz*cj31 + j14*T.C41;

  T.C20 = cj20  + j24*T.C40 ;
  T.C30 = cj30  + j34*T.C40 ;
  T.C21 = cj21  + j24*T.C41 ;
  T.C31 = cj31  + j34*T.C41 ;
  T.C22 = cj22  + j24*T.C42 ;
  T.C32 = cj32  + j34*T.C42 ;
  T.C33 = cj33  + j34*T.C43 ;

}

inline void L1ExtrapolateLine( L1TrackPar &T, fvec z_out)
{
  fvec dz = (z_out - T.z);

  T.x += T.tx*dz;
  T.y += T.ty*dz;
  T.z +=      dz;

  const fvec dzC32_in = dz * T.C32;  

  T.C21 += dzC32_in;
  T.C10 += dz * (  T.C21 + T.C30 );

  const fvec C20_in = T.C20;

  T.C20 += dz * T.C22;
  T.C00 += dz * ( T.C20 + C20_in );

  const fvec C31_in = T.C31;

  T.C31 += dz * T.C33;
  T.C11 += dz * ( T.C31 + C31_in );
  T.C30 += dzC32_in;

  T.C40 += dz * T.C42;
  T.C41 += dz * T.C43;
}

inline void L1ExtrapolateXC00Line( const L1TrackPar &T, fvec z_out, fvec& x, fvec& C00 )
{
  const fvec dz = (z_out - T.z);
  x = T.x + T.tx*dz;
  C00 = T.C00 + dz * ( 2*T.C20 + dz * T.C22 );
}

inline void L1ExtrapolateYC11Line( const L1TrackPar &T, fvec z_out, fvec& y, fvec& C11 )
{
  const fvec dz = (z_out - T.z);
  y = T.y + T.ty*dz;
  C11 = T.C11 + dz * ( 2*T.C31 + dz * T.C33 );
}

inline void L1ExtrapolateC10Line( const L1TrackPar &T, fvec z_out, fvec& C10 )
{
  const fvec dz = (z_out - T.z);
  C10 = T.C10 + dz * ( T.C21 + T.C30 + dz * T.C32 );
}

inline void L1ExtrapolateJXY // is not used currently
( 
 fvec &tx, fvec &ty, fvec &qp, // input track parameters
 fvec  dz  , // extrapolate to this dz 
 L1FieldRegion &F,
 fvec &extrDx, fvec &extrDy, fvec extrJ[]
 )
{
  //
  //  Part of the analytic extrapolation formula with error (c_light*B*dz)^4/4!
  //  
  
//   cnst ZERO = 0.0, ONE = 1.;
  cnst c_light = 0.000299792458;

  cnst  
    c1 = 1., c2 = 2., c3 = 3., c4 = 4., c6 = 6., c9 = 9., c15 = 15., c18 = 18., c45 = 45.,
    c2i = 1./2., c6i = 1./6., c12i = 1./12.;

  fvec dz2 = dz*dz;
  fvec dz3 = dz2*dz;

  // construct coefficients 

  fvec x   = tx;
  fvec y   = ty;
  fvec xx  = x*x;
  fvec xy = x*y;
  fvec yy = y*y;
  fvec y2 = y*c2;
  fvec x2 = x*c2;
  fvec x4 = x*c4;
  fvec xx31 = xx*c3+c1;
  fvec xx159 = xx*c15+c9;

  fvec Ay = -xx-c1;
  fvec Ayy = x*(xx*c3+c3);
  fvec Ayz = -c2*xy; 
  fvec Ayyy = -(c15*xx*xx+c18*xx+c3);

  fvec Ayy_x = c3*xx31;
  fvec Ayyy_x = -x4*xx159;

  fvec Bx = yy+c1; 
  fvec Byy = y*xx31; 
  fvec Byz = c2*xx+c1;
  fvec Byyy = -xy*xx159;

  fvec Byy_x = c6*xy;
  fvec Byyy_x = -y*(c45*xx+c9);
  fvec Byyy_y = -x*xx159;

  // end of coefficients calculation

  fvec t2   = c1 + xx + yy;
  fvec t    = sqrt( t2 );
  fvec h    = qp*c_light;
  fvec ht   = h*t;

  // get field integrals
  fvec Fx0 = F.cx0;
  fvec Fx1 = F.cx1*dz;
  fvec Fx2 = F.cx2*dz2;
  fvec Fy0 = F.cy0;
  fvec Fy1 = F.cy1*dz;
  fvec Fy2 = F.cy2*dz2;
  fvec Fz0 = F.cz0;
  fvec Fz1 = F.cz1*dz;
  fvec Fz2 = F.cz2*dz2;

  // 

  fvec Sx = ( Fx0*c2i + Fx1*c6i + Fx2*c12i );
  fvec Sy = ( Fy0*c2i + Fy1*c6i + Fy2*c12i );
  fvec Sz = ( Fz0*c2i + Fz1*c6i + Fz2*c12i );

  fvec Syz;
  {
    cnst 
      d = 1./2520., 
      c00 = 21.*20.*d, c01 = 21.*5.*d, c02 = 21.*2.*d,
      c10 =  7.*30.*d, c11 =  7.*9.*d, c12 =  7.*4.*d, 
      c20 =  2.*63.*d, c21 = 2.*21.*d, c22 = 2.*10.*d;
    Syz = Fy0*( c00*Fz0 + c01*Fz1 + c02*Fz2 ) 
      +   Fy1*( c10*Fz0 + c11*Fz1 + c12*Fz2 ) 
      +   Fy2*( c20*Fz0 + c21*Fz1 + c22*Fz2 ) ;
  }

  fvec Syy ;   
  {
    cnst  
    d= 1./2520., c00= 420.*d, c01= 21.*15.*d, c02= 21.*8.*d,
    c03= 63.*d, c04= 70.*d, c05= 20.*d;
    Syy =  Fy0*(c00*Fy0+c01*Fy1+c02*Fy2) + Fy1*(c03*Fy1+c04*Fy2) + c05*Fy2*Fy2 ;
  }
  
  fvec Syyy;
  {
    cnst 
      d = 1./181440., 
      c000 =   7560*d, c001 = 9*1008*d, c002 = 5*1008*d, 
      c011 = 21*180*d, c012 = 24*180*d, c022 =  7*180*d, 
      c111 =    540*d, c112 =    945*d, c122 =    560*d, c222 = 112*d;
    fvec Fy22 = Fy2*Fy2;
    Syyy = Fy0*( Fy0*(c000*Fy0+c001*Fy1+c002*Fy2)+ Fy1*(c011*Fy1+c012*Fy2)+c022*Fy22 )
      +    Fy1*( Fy1*(c111*Fy1+c112*Fy2)+c122*Fy22) + c222*Fy22*Fy2                  ;
  }
    
  fvec SA1   = Sx*xy   + Sy*Ay   + Sz*y ;
  fvec SA1_x = Sx*y - Sy*x2 ;
  fvec SA1_y = Sx*x + Sz;

  fvec SB1   = Sx*Bx   - Sy*xy   - Sz*x ;
  fvec SB1_x = -Sy*y - Sz;
  fvec SB1_y = Sx*y2 - Sy*x;

  fvec SA2   = Syy*Ayy   + Syz*Ayz ;
  fvec SA2_x = Syy*Ayy_x - Syz*y2 ;
  fvec SA2_y = -Syz*x2 ;
  fvec SB2   = Syy*Byy   + Syz*Byz ;
  fvec SB2_x = Syy*Byy_x + Syz*x4 ;
  fvec SB2_y = Syy*xx31 ;

  fvec SA3   = Syyy*Ayyy  ;
  fvec SA3_x = Syyy*Ayyy_x;
  fvec SB3   = Syyy*Byyy  ;
  fvec SB3_x = Syyy*Byyy_x;
  fvec SB3_y = Syyy*Byyy_y;

  fvec ht1 = ht*dz;
  fvec ht2 = ht*ht*dz2;
  fvec ht3 = ht*ht*ht*dz3;
  fvec ht1SA1 = ht1*SA1;
  fvec ht1SB1 = ht1*SB1;
  fvec ht2SA2 = ht2*SA2;
  fvec ht2SB2 = ht2*SB2;
  fvec ht3SA3 = ht3*SA3;
  fvec ht3SB3 = ht3*SB3;

  extrDx = ( x + ht1SA1 + ht2SA2 + ht3SA3 )*dz ;
  extrDy = ( y + ht1SB1 + ht2SB2 + ht3SB3 )*dz ;

  fvec ctdz2 = c_light*t*dz2;

  fvec t2i = c1*rcp(t2);// /t2;
  fvec xt2i = x*t2i;
  fvec yt2i = y*t2i;
  fvec tmp0 = ht1SA1 + c2*ht2SA2 + c3*ht3SA3;
  fvec tmp1 = ht1SB1 + c2*ht2SB2 + c3*ht3SB3;

  extrJ[0] = dz*(c1 + xt2i*tmp0 + ht1*SA1_x + ht2*SA2_x + ht3*SA3_x);  // j02
  extrJ[1] = dz*(     yt2i*tmp0 + ht1*SA1_y + ht2*SA2_y );             // j03
  extrJ[2] = ctdz2*( SA1 + c2*ht1*SA2 + c3*ht2*SA3 );                  // j04
  extrJ[3] = dz*(     xt2i*tmp1 + ht1*SB1_x + ht2*SB2_x + ht3*SB3_x);  // j12    
  extrJ[4] = dz*(c1 + yt2i*tmp1 + ht1*SB1_y + ht2*SB2_y + ht3*SB3_y ); // j13
  extrJ[5] = ctdz2*( SB1 + c2*ht1*SB2 + c3*ht2*SB3 );                  // j14

}

inline void L1ExtrapolateJXY0
( 
 fvec &tx, fvec &ty, // input track slopes
 fvec  dz  , // extrapolate to this dz position
 L1FieldRegion &F,
  fvec &extrDx, fvec &extrDy, fvec &J04, fvec &J14
 )
{
  cnst c_light = 0.000299792458, c1 = 1., c2i = 0.5, c6i = 1./6., c12i = 1./12.;

  fvec dz2 = dz*dz;
  fvec dzc6i = dz*c6i;
  fvec dz2c12i = dz2*c12i;

  fvec xx  = tx*tx;
  fvec yy = ty*ty;
  fvec xy  = tx*ty;

  fvec Ay = -xx-c1;
  fvec Bx = yy+c1; 

  fvec ctdz2 = c_light*sqrt( c1 + xx + yy )*dz2;

  fvec Sx = F.cx0*c2i + F.cx1*dzc6i + F.cx2*dz2c12i ;
  fvec Sy = F.cy0*c2i + F.cy1*dzc6i + F.cy2*dz2c12i ;
  fvec Sz = F.cz0*c2i + F.cz1*dzc6i + F.cz2*dz2c12i ;
    
  extrDx = ( tx )*dz ;
  extrDy = ( ty )*dz ;
  J04 = ctdz2 * (Sx*xy   + Sy*Ay   + Sz*ty);
  J14 = ctdz2 * (Sx*Bx   - Sy*xy   - Sz*tx);  
}

#undef cnst

#endif
