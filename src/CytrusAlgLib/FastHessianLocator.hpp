/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** in namespace cytrus::alg
** -----------------------------------------------------------------
** FastHessianLocator.hpp (implementation) : Locating POI's using the determinant of the Hessian
** matrix. 
**
** Some parts are adapted from Bay's SURF implementation/notes:
** C. Evans, Research Into Robust Visual Features,
** MSc University of Bristol, 2008.     
**
*/

//-------------------------------------------------------
// box filter lobe sizes (precalculated values from C.Evans OpenSURF

static const int lobe_cache [] = {3,5,7,9,  5,9,13,17,  9,17,25,33,  17,33,49,65};
static const int lobe_cache_unique [] = {3,5,7,9,13,17,25,33,49,65};
static const int lobe_map [] = {0,1,2,3,1,3,4,5,3,5,6,7,5,7,8,9};
static const int border_cache [] = {14,26,50,98}; 

//-------------------------------------------------------

template <typename IntegralImageView>
void FastHessianLocator<IntegralImageView>::
    setParameters(const int octaves, 
              const int intervals, 
              const int sampling, 
              const float threshold){

    _octaves=octaves;
    _intervals=intervals;
    _sampling=sampling;
    _threshold=threshold;

	//if I don't initialise i_width and i_height, the program <<sometimes>> crashes in
	//release mode. (spent 3 days to find this bug, and it was a no-brainer) 
	i_width=0;  
	i_height=0;

}

template <typename IntegralImageView>
FastHessianLocator<IntegralImageView>::
    FastHessianLocator(IntegralImageView& intImg, 
                       const int octaves, 
                       const int intervals, 
                       const int sampling, 
                       const float threshold){

    hessianDet=NULL;
    setParameters(octaves,intervals,sampling,threshold);
    setSourceIntegralImg(intImg);
}

template <typename IntegralImageView>
FastHessianLocator<IntegralImageView>::
    FastHessianLocator(const int octaves, 
                       const int intervals, 
                       const int sampling, 
                       const float threshold){

    _img=NULL;
    hessianDet=NULL;
    setParameters(octaves,intervals,sampling,threshold);

}

template <typename IntegralImageView>
FastHessianLocator<IntegralImageView>::~FastHessianLocator(){
	if(hessianDet!=NULL){
		delete []hessianDet;
		hessianDet=0;
	}
}

template <typename IntegralImageView>
void FastHessianLocator<IntegralImageView>::setSourceIntegralImg(IntegralImageView& intImg){
    _img=&intImg;
    if(i_width!=_img->width() || i_height!=_img->height()){
        i_width=_img->width();
        i_height=_img->height();  
		try{
			if(hessianDet!=NULL){
				delete []hessianDet;
				hessianDet=0;
			}
			unsigned long size=_octaves*_intervals*i_width*i_height;
			hessianDet = new float[size];
			memset(hessianDet,0,size*sizeof(float));
		}
		catch(char * str){
			std::cerr<<"Not enough memory for computing SURF POI's"<<std::endl;
		}
    }
}


template <typename IntegralImageView>
void FastHessianLocator<IntegralImageView>::locatePOIInImage(std::vector<Poi>& iPts_out){
    
    iPts_out.clear();
    buildDet();

    for(int o=0; o < _octaves; o++) 
    {
        // For each octave double the sampling step of the previous
        int step = _sampling * (int)floor(pow(2.0f,o)+0.5f);
        int border = border_cache[o];

        // 3x3x3 non-max suppression over whole image
        for(int i = 1; i < _intervals-1; i += 3) {
          for(int r = border; r < i_height - border; r += 3*step) {
            for(int c = border; c < i_width - border; c += 3*step) {

              int i_max = -1, r_max = -1, c_max = -1;
              float max_val = 0;

              // Scan the pixels in this block to find the local extremum.
              for (int ii = i; ii < min(i+3, _intervals-1); ii += 1) {
                for (int rr = r; rr < min(r+3*step, i_height - border); rr += step) {
                  for (int cc = c; cc < min(c +3*step, i_width - border); cc += step) {

                    float val = getHessian(o, ii, cc, rr);
                    
                    // record the max value and its location
                    if (val > max_val) 
                    {
                      max_val = val;
                      i_max = ii;
                      r_max = rr;
                      c_max = cc;
                    }
                  }
                }
              }  
              
              // Check the block extremum is an extremum across boundaries.
              if (max_val > _threshold && i_max != -1 && isExtremum(o, i_max, c_max, r_max)) 
              {
                interpolateExtremum(iPts_out, o, i_max, r_max, c_max);
              }
            }
          }
        }
    }

}
    
//! Interpolates a scale-space extremum's location and scale to subpixel
//! accuracy to form an image feature.
template <typename IntegralImageView>
void FastHessianLocator<IntegralImageView>::interpolateExtremum(std::vector<Poi>& iPts_out, int octv, int intvl, int r, int c)
{
  double xi = 0, xr = 0, xc = 0;
  int i = 0;
  float step = _sampling * (int)floor(pow(2.0f,octv)+0.5f);
  
  // Get the offsets to the actual location of the extremum
  interpolateStep( octv, intvl, r, c, &xi, &xr, &xc );

  // If point is sufficiently close to the actual extremum
  if( fabs( xi ) < 0.5  &&  fabs( xr ) < 0.5  &&  fabs( xc ) < 0.5 )
  {
    // Create POI and push onto POI's vector
    Poi ipt;
    ipt.x = (float) (c + step*xc);
    ipt.y = (float) (r + step*xr);
    ipt.scale = (float) ((1.2f/9.0f) * (3*(pow(2.0f, octv+1) * (intvl+xi+1)+1)));
    ipt.laplacianSign = getLaplacianSign(octv, intvl, c, r);
    iPts_out.push_back(ipt);
  }
}

//! Performs one step of extremum interpolation.
template <typename IntegralImageView>
void FastHessianLocator<IntegralImageView>::interpolateStep( int octv, int intvl, int r, int c, double* xi, double* xr, double* xc )
{
  double *dD;
  double *H; 
  double t[9];

  dD = deriv3D( octv, intvl, r, c );
  H = hessian3D( octv, intvl, r, c );

  //invert H:
    double det3H=(H[0]*(H[4]*H[8] - H[5]*H[7]) -  \
                  H[1]*(H[3]*H[8] - H[5]*H[6]) +  \
                  H[2]*(H[3]*H[7] - H[4]*H[6]));

    if( det3H != 0. )
    {
        det3H = 1./det3H;

        t[0] = (H[4] * H[8] - H[5] * H[7]) * det3H;
        t[1] = (H[2] * H[7] - H[1] * H[8]) * det3H;
        t[2] = (H[1] * H[5] - H[2] * H[4]) * det3H;
               
        t[3] = (H[5] * H[6] - H[3] * H[8]) * det3H;
        t[4] = (H[0] * H[8] - H[2] * H[6]) * det3H;
        t[5] = (H[2] * H[3] - H[0] * H[5]) * det3H;
               
        t[6] = (H[3] * H[7] - H[4] * H[6]) * det3H;
        t[7] = (H[1] * H[6] - H[0] * H[7]) * det3H;
        t[8] = (H[0] * H[4] - H[1] * H[3]) * det3H;
    }
   // t now contains H^-1


  // H^-1 * dD * -1
  *xi = (-1) * (t[6]*dD[0] + t[7]*dD[1] + t[8]*dD[2]);
  *xr = (-1) * (t[3]*dD[0] + t[4]*dD[1] + t[5]*dD[2]);
  *xc = (-1) * (t[0]*dD[0] + t[1]*dD[1] + t[2]*dD[2]);
  
  delete []dD;
  delete []H;
}

//! Return the sign of the laplacian (trace of the hessian)
template <typename IntegralImageView>
inline int FastHessianLocator<IntegralImageView>::getLaplacianSign(int o, int i, int c, int r)
{
  float res = (hessianDet[(o*_intervals+i)*(i_width*i_height) + (r*i_width+c)]);

  return (res >= 0 ? 1 : -1);
}

//! Return the value of the approximated determinant of hessian
template <typename IntegralImageView>
inline float FastHessianLocator<IntegralImageView>::getHessian(int o, int i, int c, int r)
{
  return fabs(hessianDet[(o*_intervals+i)*(i_width*i_height) + (r*i_width+c)]);
}

template <typename IntegralImageView>
double* FastHessianLocator<IntegralImageView>::deriv3D( int octv, int intvl, int r, int c ){
    double* dI;
    double dx, dy, ds;
    int step = _sampling * (int)floor(pow(2.0f,octv)+0.5f);

    dx = ( getHessian(octv,intvl, c+step, r ) -
        getHessian( octv,intvl, c-step, r ) ) / 2.0;
    dy = ( getHessian( octv,intvl, c, r+step ) -
        getHessian( octv,intvl, c, r-step ) ) / 2.0;
    ds = ( getHessian( octv,intvl+1, c, r ) -
        getHessian( octv,intvl-1, c, r ) ) / 2.0;

    dI=new double[3];
    dI[0]=dx;
    dI[1]=dy;
    dI[2]=ds;

    return dI;
}

template <typename IntegralImageView>
double* FastHessianLocator<IntegralImageView>::hessian3D(int octv, int intvl, int r, int c ){
  double *H;
  double v, dxx, dyy, dss, dxy, dxs, dys;
  int step = _sampling * (int)floor(pow(2.0f,octv)+0.5f);

  v = getHessian( octv,intvl, c, r );
  dxx = ( getHessian( octv,intvl, c+step, r ) + 
    getHessian( octv,intvl, c-step, r ) - 2 * v );
  dyy = ( getHessian( octv,intvl, c, r+step ) +
    getHessian( octv,intvl, c, r-step ) - 2 * v );
  dss = ( getHessian( octv,intvl+1, c, r ) +
    getHessian( octv,intvl-1, c, r ) - 2 * v );
  dxy = ( getHessian( octv,intvl, c+step, r+step ) -
    getHessian( octv,intvl, c-step, r+step ) -
    getHessian( octv,intvl, c+step, r-step ) +
    getHessian( octv,intvl, c-step, r-step ) ) / 4.0;
  dxs = ( getHessian( octv,intvl+1, c+step, r ) -
    getHessian( octv,intvl+1, c-step, r ) -
    getHessian( octv,intvl-1, c+step, r ) +
    getHessian( octv,intvl-1, c-step, r ) ) / 4.0;
  dys = ( getHessian( octv,intvl+1, c, r+step ) -
    getHessian( octv,intvl+1, c, r-step ) -
    getHessian( octv,intvl-1, c, r+step ) +
    getHessian( octv,intvl-1, c, r-step ) ) / 4.0;

  //H = cvCreateMat( 3, 3, CV_64FC1 );

  H=new double[9];

  H[0]=dxx;
  H[1]=dxy;
  H[2]=dxs;
  H[3]=dxy;
  H[4]=dyy;
  H[5]=dys;
  H[6]=dxs;
  H[7]=dys;
  H[8]=dss;

  return H;
}

template <typename IntegralImageView>
int FastHessianLocator<IntegralImageView>::isExtremum(int octv, int intvl, int c, int r){
  int step = _sampling * (int)floor(pow(2.0f,octv)+0.5f);

  // Bounds check
  if (intvl - 1 < 0 || intvl + 1 > _intervals - 1 
    || c - step < 0 || c + step > i_width 
    || r - step < 0 || r + step > i_height)
  {
    return 0;
  }

  float val = getHessian(octv,intvl, c, r);

  // Check for maximum 
  for(int ii = intvl-1; ii <= intvl+1; ++ii )
    for(int cc = c - step; cc <= c + step; cc+=step )
      for(int rr = r - step; rr <= r + step; rr+=step ) 
        if (ii != 0 || cc != 0 || rr != 0)
          if(getHessian(octv, ii, cc, rr) > val)
            return 0;

  return 1;
}

template <typename IntegralImageView>
void FastHessianLocator<IntegralImageView>::buildDet(){
    int l, w, b, border, samplingStep;
    float Dxx, Dyy, Dxy, inverse_area;
	//unsigned long val=_octaves*_intervals*i_width*i_height;
    for(int o=0; o<_octaves; o++) 
    {
        samplingStep = _sampling * (int)floor(pow(2.0f,o)+0.5f);
        border = border_cache[o];

        for(int i=0; i<_intervals; i++) {

          l = lobe_cache[o*_intervals + i]; 
          w = 3 * l;                      
          b = w / 2;        
          inverse_area = 1.0f/(w * w);     

          for(int r = border; r < i_height - border; r += samplingStep) 
          {
            for(int c = border; c < i_width - border; c += samplingStep) 
            {
                Dxx = IntegralImageTransform::boxFilter(_img, r - l + 1, c - b, 2*l - 1, w)
                    - IntegralImageTransform::boxFilter(_img, r - l + 1, c - l / 2, 2*l - 1, l)*3;
                Dyy = IntegralImageTransform::boxFilter(_img, r - b, c - l + 1, w, 2*l - 1)
                    - IntegralImageTransform::boxFilter(_img, r - l / 2, c - l + 1, l, 2*l - 1)*3;
                Dxy = + IntegralImageTransform::boxFilter(_img, r - l, c + 1, l, l)
                      + IntegralImageTransform::boxFilter(_img, r + 1, c - l, l, l)
                      - IntegralImageTransform::boxFilter(_img, r - l, c - l, l, l)
                      - IntegralImageTransform::boxFilter(_img, r + 1, c + 1, l, l);
                //Dxx=0;
                //Dyy=0;
                //Dxy=0;

              // Normalise the filter responses with respect to their size
              Dxx *= inverse_area;
              Dyy *= inverse_area;
              Dxy *= inverse_area;

              // Get the sign of the laplacian
              int lap_sign = (Dxx+Dyy >= 0 ? 1 : -1);

              // Get the determinant of hessian response
              float determinant = (Dxx*Dyy - pow(0.91f*Dxy,2));

			  unsigned long ind=(o*_intervals+i)*(i_width*i_height) + (r*i_width+c);
			  hessianDet[ind] = (determinant < 0 ? 0 : lap_sign * determinant);
            }
          }
        }
    }
}