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
using namespace cytrus::cameraHAL;


IImageConsumer::IImageConsumer(IImageSource* imgSource){
	_imgSource=imgSource;
}