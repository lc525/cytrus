/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** IImageSource.cpp
**
*/
#include "stdafx.h"
#include "IImageSource.h"
using namespace cytrus::cameraHAL;

typedef std::set<IImageConsumer*> imageConsumerSet;

IImageSource::~IImageSource(){
	for(imageConsumerSet::iterator it=consumers.begin(); it!=consumers.end(); it++){
		delete (*it);
	}
}


bool IImageSource::registerImageConsumer(IImageConsumer *c){
	if(!_sourceIsStarted){
		std::pair<imageConsumerSet::iterator, bool> p=consumers.insert(c);
		return p.second;
	}
	else{
		return false;
	}
}

bool IImageSource::removeImageConsumer(IImageConsumer *c){
	if(!_sourceIsStarted){
		imageConsumerSet::const_iterator foundConsumer=consumers.find(c);
		if(foundConsumer!=consumers.end()){
			consumers.erase(foundConsumer);
			return true;
		}
		return false;
	}
	else{
		return false;
	}
}