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
** FastHessianLocator.h : Locating POI's using the determinant of the Hessian
** matrix. 
**
** Some parts are adapted from Bay's SURF implementation/notes:
** C. Evans, Research Into Robust Visual Features,
** MSc University of Bristol, 2008.     
**
*/

#ifndef _FASTHESSIANLOCATOR_H_
#define _FASTHESSIANLOCATOR_H_

#ifndef CYTRUSALGLIB_API
	#ifdef CYTRUSALGLIB_EXPORTS
		#define CYTRUSALGLIB_API __declspec(dllexport)
	#else
		#define CYTRUSALGLIB_API __declspec(dllimport)
	#endif
#endif

#include <vector>
#include "poi.h"
#include "ILocator.h"

namespace cytrus{
	namespace alg{

		static const int OCTAVES = 3;
		static const int INTERVALS = 4;
		static const float THRES = 0.002f;
		static const int INT_SAMPLE = 2;

		template <typename IntegralImageView>
		class FastHessianLocator:public ILocator{

			private:
				void initSetup(const int octaves, 
							   const int intervals, 
							   const int sampling, 
							   const float threshold);

				void buildDet(); // this fills the hessianDet with values calculated
								 // for pixels in the image (sampled)
			protected:

			IntegralImageView* _img;
			int i_width, i_height;

			int _octaves;
			int _intervals;
			int _sampling;
			int _threshold;

			float* hessianDet; // array that contains

			public:
				FastHessianLocator(IntegralImageView& intImg, 
								   const int octaves = OCTAVES, 
								   const int intervals = INTERVALS, 
								   const int sampling = INT_SAMPLE, 
								   const float threshold = THRES);

				FastHessianLocator(const int octaves = OCTAVES, 
								   const int intervals = INTERVALS, 
								   const int sampling = INT_SAMPLE, 
								   const float threshold = THRES);

				void setSourceIntegralImg(IntegralImageView& intImg);

				
				// Outputs in the given vector the interest points detected in the
				// image, having only the location and scale information
				// computed for them. 
				virtual void locatePOIInImage(std::vector<Poi>& iPts_out);
		};
	}
}

#include "FastHessianLocator.hpp"

#endif