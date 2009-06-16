/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** IImageTransform.h: General interface for Image transformations
**
*/

#ifndef _IIMAGETRANSFORM_H_
#define _IIMAGETRANSFORM_H_

#ifndef CYTRUSALGLIB_API
	#ifdef CYTRUSALGLIB_EXPORTS
		#define CYTRUSALGLIB_API __declspec(dllexport)
	#else
		#define CYTRUSALGLIB_API __declspec(dllimport)
	#endif
#endif

namespace cytrus{
	namespace alg{
		class CYTRUSALGLIB_API IImageTransform{
		public:
			template <typename SrcView, typename DstView>
			void applyTransform(SrcView& src, DstView& dst);
		};
	}
}

#endif