/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** IntegralImageTransform.hpp: Transforms a given image into its integral representation
**
*/


template <typename SrcView, typename DstView>
void IntegralImageTransform::applyTransform(SrcView& src, DstView& dst){
	gil_function_requires<ImageViewConcept<SrcView> >();
	gil_function_requires<MutableImageViewConcept<DstView> >();
	gil_function_requires<ColorSpacesCompatibleConcept<
							typename color_space_type<SrcView>::type, 
							typename color_space_type<DstView>::type> >();

	DstView::xy_locator dst_loc = dst.xy_at(0,1);
	DstView::xy_locator::cached_location_t above = dst_loc.cache_location(0,-1);

	for (int y=0; y<src.height(); ++y) {
		typename SrcView::x_iterator src_it = src.row_begin(y);
		typename DstView::x_iterator dst_it = dst.row_begin(y);

		unsigned int rowSum=0;
		for (int x=0; x<src.width(); ++x) {
			rowSum+=src_it[x];

			if(y>0){
				(*dst_it) = rowSum+dst_loc[above];
				#ifdef DEBUG_OUTPUT
					if(y==src.height()-1 && x==src.width()-1) 
						std::cout<<"IntegralImage["<<src.height()<<"]"\
											  <<"["<<src.width()<<"]="\
											  <<rowSum+dst_loc[above]<<std::endl;
				#endif
				++dst_loc.x();
			}
			else{
				(*dst_it) = rowSum;
			}
			++dst_it;             
		}
		if(y>0)
			dst_loc+=point2<std::ptrdiff_t>(-dst.width(),1);
	}
}

template <typename IntegralImageView>
float IntegralImageTransform::boxFilter(IntegralImageView* src, int xSt, int ySt, int boxHeight, int boxWidth){
	gil_function_requires<ImageViewConcept<IntegralImageView> >();
	
	int width=src->width();
	int height=src->height();

	int rowStart = min(xSt, height) - 1;
	int colStart = min(ySt, width)  - 1;
	int rowEnd = min(xSt + boxHeight, height) - 1;
	int colEnd = min(ySt + boxWidth,  width)  - 1;

	//checks
	if(rowStart<0) rowStart=0;
	if(colStart<0) colStart=0;
	if(rowEnd<0) rowEnd=0;
	if(colEnd<0) colEnd=0;

	IntegralImageView::xy_locator A,B,C,D;
	A=src->xy_at(colStart,rowStart);
	C=src->xy_at(colStart,rowEnd);
	B=src->xy_at(colEnd,rowStart);
	D=src->xy_at(colEnd,rowEnd);

	float val=(*A)-(*B)-(*C)+(*D);
	return max(0, val);

}