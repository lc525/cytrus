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
#include "FastHessianLocator.h"
#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

using namespace cytrus::alg;
using namespace boost::gil;

//Utility Functor for reasonable output of integral image
	struct get_in_256_range : public std::binary_function<long, long, short> {
		static unsigned long max;
		short operator()(unsigned long src_loc) const {
			return src_loc*255/max;
		}
	};

	unsigned long get_in_256_range::max=1;
//

SurfAlg::SurfAlg(IImageSource* imgSrc, POIAlgResult outputFunc):
	IPOIAlgorithm(imgSrc,
				  new FastHessianLocator<gray32_view_t>,
				  NULL,
				  outputFunc){
	
	_pWidth=-1;
	_pHeight=-1;
	_isCustomPrelSize=false;

	//Configure output modes
	std::pair<char*, int>* grayscale=new std::pair<char*, int>();
	grayscale->first="Grayscale";
	grayscale->second=2; // PixelFormats::Gray8
	_outputModes->push_back(grayscale);

	std::pair<char*, int>* integral=new std::pair<char*, int>();
	integral->first="Integral";
	integral->second=2;  // PixelFormats::Gray8
	_outputModes->push_back(integral);
}

SurfAlg::~SurfAlg(){
	if(_poiLoc!=NULL) delete _poiLoc;
	if(_poiDescr!=NULL) delete _poiDescr;
}

void SurfAlg::processImage(unsigned long dwSize, unsigned char* pbData){
	std::pair<int,int> size=_imgSource->getImageSize();
	int width=size.first;
	int height=size.second;
	ptrdiff_t myVal=dwSize/(height);
	rgb8c_view_t* prelView;
	rgb8_image_t* scaled=NULL;
	rgb8c_view_t myView=interleaved_view(width,height,(const rgb8_pixel_t*)pbData,myVal);

	//if the user decided to run on a resampled image, create the smaller image
	if(_isCustomPrelSize){
		width=_pWidth;
		height=_pHeight;
		scaled=new rgb8_image_t(width, height);
		resize_view(myView, view(*scaled), bilinear_sampler());
		prelView=(rgb8c_view_t*)&view(*scaled);
	}
	else{
		prelView=(rgb8c_view_t*)&myView;
	}


	gray8_image_t grImg(width,height);
	gray8_view_t grView=view(grImg);
	copy_pixels(color_converted_view<gray8_pixel_t>(*prelView), grView);
	
	//calculate integral img
	gray32_image_t integral(width,height);
	gray32_view_t integralView = view(integral);
	IntegralImageTransform::applyTransform(grView,integralView);
	

	FastHessianLocator<gray32_view_t>* locator=(FastHessianLocator<gray32_view_t>*)_poiLoc;
	locator->setSourceIntegralImg(integralView);
	locator->locatePOIInImage(iPts);

	unsigned long nSize=width*height*3;

	switch(_currentOutputMode){
		case 0:
			_outputAlgResult(nSize,(unsigned char*)interleaved_view_get_raw_data(*prelView)); break;
		case 1:
			_outputAlgResult(nSize/3,(unsigned char*)interleaved_view_get_raw_data(grView)); break;
		case 2:
			{
			gray8_image_t rangeImg(width,height);
			gray8_view_t rangedView=view(rangeImg);
			get_in_256_range::max=*(integralView.xy_at(width-1,height-1));
			transform_pixels(integralView,rangedView,get_in_256_range());
			_outputAlgResult(nSize/3,(unsigned char*)interleaved_view_get_raw_data(rangedView)); 
			break;
			}
		default:
			_outputAlgResult(dwSize,pbData);
	}

	//cleanup:
	if(scaled!=NULL) delete scaled;
}

bool SurfAlg::setProcessingSize(int newWidth, int newHeight){
	std::pair<int,int> size=_imgSource->getImageSize();
	int width=size.first;
	int height=size.second;
	if(newWidth>0 && newHeight>0){
		if(newWidth==width && newHeight==height){
			_isCustomPrelSize=false;
			return true;
		}
		_pWidth=newWidth;
		_pHeight=newHeight;
		_isCustomPrelSize=true;
		return true;
	}
	return false;
}
				
void SurfAlg::onSourceSizeChange(){
}