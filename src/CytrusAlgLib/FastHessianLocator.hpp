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
	initSetup(const int octaves, 
		      const int intervals, 
		      const int sampling, 
		      const float threshold){

	_octaves=octaves;
	_intervals=intervals;
	_sampling=sampling;
	_threshold=threshold;

}

template <typename IntegralImageView>
FastHessianLocator<IntegralImageView>::
	FastHessianLocator(IntegralImageView& intImg, 
					   const int octaves, 
					   const int intervals, 
					   const int sampling, 
					   const float threshold){

	initSetup(octaves,intervals,sampling,threshold);
	setSourceIntegralImg(intImg);
}

template <typename IntegralImageView>
FastHessianLocator<IntegralImageView>::
	FastHessianLocator(const int octaves, 
					   const int intervals, 
					   const int sampling, 
					   const float threshold){

	_img=NULL;
	initSetup(octaves,intervals,sampling,threshold);

}

template <typename IntegralImageView>
void FastHessianLocator<IntegralImageView>::setSourceIntegralImg(IntegralImageView& intImg){
	_img=&intImg;
	if(i_width!=_img->width() || i_height!=_img->height()){
		i_width=_img->width();
		i_height=_img->height();
		
		if(hessianDet!=NULL) delete []hessianDet;
		hessianDet = new float [_octaves*_intervals*i_width*i_height];
		memset(hessianDet,0,sizeof(hessianDet));
	}
}


template <typename IntegralImageView>
void FastHessianLocator<IntegralImageView>::locatePOIInImage(std::vector<Poi>& iPts_out){
	
	iPts_out.clear();
	buildDet();


}

template <typename IntegralImageView>
void FastHessianLocator<IntegralImageView>::buildDet(){
	int l, w, b, border, samplingStep;
	float Dxx, Dyy, Dxy, inverse_area;

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
				Dxx = IntegralImageTransform::boxFilter(*_img, r - l + 1, c - b, 2*l - 1, w)
					- IntegralImageTransform::boxFilter(*_img, r - l + 1, c - l / 2, 2*l - 1, l)*3;
			    Dyy = IntegralImageTransform::boxFilter(*_img, r - b, c - l + 1, w, 2*l - 1)
				    - IntegralImageTransform::boxFilter(*_img, r - l / 2, c - l + 1, l, 2*l - 1)*3;
			    Dxy = + IntegralImageTransform::boxFilter(*_img, r - l, c + 1, l, l)
				      + IntegralImageTransform::boxFilter(*_img, r + 1, c - l, l, l)
					  - IntegralImageTransform::boxFilter(*_img, r - l, c - l, l, l)
					  - IntegralImageTransform::boxFilter(*_img, r + 1, c + 1, l, l);

			  // Normalise the filter responses with respect to their size
			  Dxx *= inverse_area;
			  Dyy *= inverse_area;
			  Dxy *= inverse_area;

			  // Get the sign of the laplacian
			  int lap_sign = (Dxx+Dyy >= 0 ? 1 : -1);

			  // Get the determinant of hessian response
			  float determinant = (Dxx*Dyy - pow(0.91f*Dxy,2));

			  hessianDet[(o*_intervals+i)*(i_width*i_height) + (r*i_width+c)] 
			  = (determinant < 0 ? 0 : lap_sign * determinant);
			}
		  }
		}
	}
}