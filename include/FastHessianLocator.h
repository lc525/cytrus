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
							   const int intSample, 
							   const float threshold);
			protected:

			IntegralImageView* _img;
			int _octaves;
			int _intervals;
			int _intSample;
			int _threshold;

			public:
				FastHessianLocator(IntegralImageView& intImg, 
								   const int octaves = OCTAVES, 
								   const int intervals = INTERVALS, 
								   const int intSample = INT_SAMPLE, 
								   const float threshold = THRES);

				FastHessianLocator(const int octaves = OCTAVES, 
								   const int intervals = INTERVALS, 
								   const int intSample = INT_SAMPLE, 
								   const float threshold = THRES);

				void setSourceIntegralImg(IntegralImageView& intImg);

				virtual void locatePOIInImage(std::vector<Poi>& iPts_out);
		};
	}
}

#include "FastHessianLocator.hpp"

#endif