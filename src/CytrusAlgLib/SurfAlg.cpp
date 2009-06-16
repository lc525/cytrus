/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** SurfAlg.cpp
**
*/
#include "stdafx.h"
#include "SurfAlg.h"
#include "IntegralImageTransform.h"
#include <boost/gil/gil_all.hpp>

using namespace cytrus::alg;
using namespace boost::gil;


SurfAlg::SurfAlg(IImageSource* imgSrc, POIAlgResult outputFunc):IPOIAlgorithm(imgSrc,NULL,NULL,outputFunc){

}

void SurfAlg::processImage(unsigned long dwSize, unsigned char* pbData){
	int height=240;
	int width=320;
	ptrdiff_t myVal=dwSize/(height);
	rgb8c_view_t myView=interleaved_view(width,height,(const rgb8_pixel_t*)pbData,myVal);
	gray8_image_t grImg(width,height);
	gray8_view_t grView=view(grImg);
	copy_pixels(color_converted_view<gray8_pixel_t>(flipped_up_down_view(myView)), grView);
	
	//calculate integral img
	gray32_image_t integral(width,height);
	gray32_view_t dstView = view(integral);
	
	IntegralImageTransform::applyTransform(grView,dstView);
	
	_outputAlgResult(dwSize,(unsigned char*)interleaved_view_get_raw_data(dstView));
	//_outputAlgResult(dwSize,pbData);
}
				
void SurfAlg::onSourceSizeChange(){
}