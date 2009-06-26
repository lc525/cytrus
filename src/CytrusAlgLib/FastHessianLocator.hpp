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

template <typename IntegralImageView>
void FastHessianLocator<IntegralImageView>::
	initSetup(const int octaves, 
		      const int intervals, 
		      const int intSample, 
		      const float threshold){

	_octaves=octaves;
	_intervals=intervals;
	_intSample=intSample;
	_threshold=threshold;

}

template <typename IntegralImageView>
FastHessianLocator<IntegralImageView>::
	FastHessianLocator(IntegralImageView& intImg, 
					   const int octaves, 
					   const int intervals, 
					   const int intSample, 
					   const float threshold){

	_img=intImg;
	initSetup(octaves,intervals,intSample,threshold);

}

template <typename IntegralImageView>
FastHessianLocator<IntegralImageView>::
	FastHessianLocator(const int octaves, 
					   const int intervals, 
					   const int intSample, 
					   const float threshold){

	initSetup(octaves,intervals,intSample,threshold);

}

template <typename IntegralImageView>
void FastHessianLocator<IntegralImageView>::setSourceIntegralImg(IntegralImageView& intImg){
	_img=intImg;
}


template <typename IntegralImageView>
void FastHessianLocator<IntegralImageView>::locatePOIInImage(std::vector<Poi>& iPts_out){

}