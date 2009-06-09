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
using namespace cytrus::alg;


SurfAlg::SurfAlg(IImageSource* imgSrc, POIAlgResult outputFunc):IPOIAlgorithm(imgSrc,NULL,NULL,outputFunc){

}

void SurfAlg::processImage(unsigned long dwSize, unsigned char* pbData){
	//no processing, transmit the image as taken from the source
	_outputAlgResult(dwSize,pbData);
}
				
void SurfAlg::onSourceSizeChange(){
}