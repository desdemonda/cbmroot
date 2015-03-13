#ifndef _L1HitPoint_h_
#define _L1HitPoint_h_

		/// contain strips positions and coordinates of hit
struct L1HitPoint{
  fscal x, y;
  fscal z; // TODO: may be we should use iz
  fscal u, v; // x\u, v - front and back strips; x, y, z - coordinates of hits
  unsigned short int n; // number of event
  double t;
    
  L1HitPoint(fscal x_, fscal y_, fscal z_, fscal v_, fscal u_, unsigned short int n_ = 0, double t_ = 0): x(x_), y(y_), z(z_), u(u_), v(v_), n(n_),t(t_){};
};

#endif
