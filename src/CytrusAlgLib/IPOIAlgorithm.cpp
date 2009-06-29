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

IPOIAlgorithm::IPOIAlgorithm(IImageSource* imgSrc, ILocator* poiLocator, IDescriptor* poiDescriptor, POIAlgResult outputFunc, int index):IImageConsumer(imgSrc, index){
	_poiLoc=poiLocator;
	_poiDescr=poiDescriptor;
	_outputAlgResult=outputFunc;
	_outputModes=new std::list<std::pair<char*,int>*>();
	std::pair<char*, int>* normal=new std::pair<char*, int>();
	normal->first="Normal";
	normal->second=1;
	_outputModes->push_back(normal);
	_currentOutputMode=0;
}

IPOIAlgorithm::~IPOIAlgorithm(){
	for(std::list<std::pair<char*,int>*>::iterator it=_outputModes->begin(); it!=_outputModes->end(); it++){
		delete (*it);
	}
	delete _outputModes;
}

void IPOIAlgorithm::setOutputMode(int mode){
	if(mode>=0 && mode<_outputModes->size())
		_currentOutputMode=mode;
}

int IPOIAlgorithm::getCurrentOutputMode(){
	return _currentOutputMode;
}

std::list<std::pair<char*,int>*>* IPOIAlgorithm::getOutputModes(){
	return _outputModes;
}

void IPOIAlgorithm::run(){
	_imgSource->startCapture();
}

std::vector<Poi> IPOIAlgorithm::getPoiResult(){
	return iPts;
}


