/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** IntegralImageTransform.cpp: Transforms a given image into its integral representation
**
*/

#include "stdafx.h"
#include <boost/gil/gil_all.hpp>
#include "IntegralImageTransform.h"

using namespace cytrus::alg;
using namespace boost::gil;

//template <typename SrcView, typename DstView>
//void IntegralImageTransform::applyTransform(SrcView& src, DstView& dst){
//	gil_function_requires<ImageViewConcept<SrcView> >();
//	gil_function_requires<MutableImageViewConcept<DstView> >();
//	gil_function_requires<ColorSpacesCompatibleConcept<
//                            typename color_space_type<SrcView>::type, 
//                            typename color_space_type<DstView>::type> >();
//
//	DstView::xy_locator dst_loc = dst.xy_at(0,1);
//	DstView::xy_locator::cached_location_t above = dst_loc.cache_location(0,-1);
//
//	for (int y=0; y<src.height(); ++y) {
//		typename SrcView::x_iterator src_it = src.row_begin(y);
//		typename DstView::x_iterator dst_it = dst.row_begin(y);
//
//		unsigned int rowSum=0;
//		for (int x=0; x<src.width(); ++x) {
//			rowSum+=src_it[x];
//
//			if(y>0){
//				(*dst_it) = rowSum+dst_loc[above];
//				#ifdef DEBUG_OUTPUT
//					if(y==src.height()-1 && x==src.width()-1) 
//						std::cout<<"IntegralImage["<<src.height()<<"]"\
//											  <<"["<<src.width()<<"]="\
//											  <<rowSum+dst_loc[above]<<std::endl;
//				#endif
//				++dst_loc.x();
//			}
//			else{
//				(*dst_it) = rowSum;
//			}
//			++dst_it;             
//		}
//		if(y>0)
//			dst_loc+=point2<std::ptrdiff_t>(-dst.width(),1);
//	}
//}