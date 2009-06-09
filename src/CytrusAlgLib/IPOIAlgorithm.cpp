/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** IPOIAlgorithm.cpp
**
*/
#include "stdafx.h"
#include "IPOIAlgorithm.h"
#include "IImageSource.h"

using namespace cytrus::alg;

IPOIAlgorithm::IPOIAlgorithm(IImageSource* imgSrc, ILocator* poiLocator, IDescriptor* poiDescriptor, POIAlgResult outputFunc):IImageConsumer(imgSrc){
	_poiLoc=poiLocator;
	_poiDescr=poiDescriptor;
	_outputAlgResult=outputFunc;
}

void IPOIAlgorithm::run(){
	_imgSource->startCapture();
}

