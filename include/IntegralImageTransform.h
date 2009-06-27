/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** IntegralImageTransform.h: Transforms a given image into its integral representation
**
*/

#ifndef _INTEGRALIMAGETRANSFORM_H_
#define _INTEGRALIMAGETRANSFORM_H_


#include "IImageTransform.h"
#include <boost/gil/gil_all.hpp>

using namespace boost::gil;

namespace cytrus{
	namespace alg{
		class IntegralImageTransform : public IImageTransform{
		public:

			// SrcView and DstView must both be GIL Image Views. The following conditions must be met:
			// ImageViewConcept<SrcView> >();
			// MutableImageViewConcept<DstView> >();
			// ColorSpacesCompatibleConcept<color_space_type<SrcView>::type, color_space_type<DstView>::type> >();
			template <typename SrcView, typename DstView>
			static void applyTransform(SrcView& src, DstView& dst);

			// IntegralImageView must be a GIL Image View. The following conditions must be met:
			// ImageViewConcept<SrcView> >();
			// The current implementation assumes the view has only one channel
			template <typename IntegralImageView>
			static float boxFilter(IntegralImageView& src, int xSt, int ySt, int boxHeight, int boxWidth);
		};
		
	}
}

#include "IntegralImageTransform.hpp"

#endif