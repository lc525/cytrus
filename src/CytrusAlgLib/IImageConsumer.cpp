/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** IImageConsumer.cpp
**
*/
#include "stdafx.h"
#include "IImageConsumer.h"
#include "IImageSource.h"
using namespace cytrus::cameraHAL;

IImageConsumer::~IImageConsumer(){
	_imgSource->removeImageConsumer(this);
}


IImageConsumer::IImageConsumer(IImageSource* imgSource, int index){
	_imgSource=imgSource;
	consumerIndex=index;
	_imgSource->registerImageConsumer(this);
}