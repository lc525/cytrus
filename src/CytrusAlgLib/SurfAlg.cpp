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
	std::pair<char*, int>* grayscale=new std::pair<char*, int>();
	grayscale->first="Grayscale";
	grayscale->second=2;
	_outputModes->push_back(grayscale);

	std::pair<char*, int>* integral=new std::pair<char*, int>();
	integral->first="Integral";
	integral->second=2;
	_outputModes->push_back(integral);
}

void SurfAlg::processImage(unsigned long dwSize, unsigned char* pbData){
	std::pair<int,int> size=_imgSource->getImageSize();
	int width=size.first;
	int height=size.second;
	ptrdiff_t myVal=dwSize/(height);
	rgb8c_view_t myView=interleaved_view(width,height,(const rgb8_pixel_t*)pbData,myVal);
	//rgb8c_view_t normalView=flipped_up_down_view(myView);
	gray8_image_t grImg(width,height);
	gray8_view_t grView=view(grImg);
	copy_pixels(color_converted_view<gray8_pixel_t>(flipped_up_down_view(myView)), grView);
	
	//calculate integral img
	gray32_image_t integral(width,height);
	gray32_view_t dstView = view(integral);
	
	IntegralImageTransform::applyTransform(grView,dstView);


	switch(_currentOutputMode){
		case 0:
			_outputAlgResult(dwSize,pbData); break;
		case 1:
			_outputAlgResult(dwSize/3,(unsigned char*)interleaved_view_get_raw_data(grView)); break;
		case 2:
			_outputAlgResult(dwSize/3,(unsigned char*)interleaved_view_get_raw_data(dstView)); break;
		default:
			_outputAlgResult(dwSize,pbData);
	}
}
				
void SurfAlg::onSourceSizeChange(){
}