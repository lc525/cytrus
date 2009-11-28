/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** ObjectPoiStorage.cpp
**
*/
#include "stdafx.h"
#include "ObjectPoiStorage.h"
#include <vector>
#include "math.h"
//#include <boost/thread/thread.hpp> - futures

using namespace cytrus::alg;

ObjectPoiStorage* ObjectPoiStorage::instance = NULL; // initialize pointer
std::vector<std::vector<Poi>*> ObjectPoiStorage::_registeredObjects;

ObjectPoiStorage::ObjectPoiStorage(){
}

ObjectPoiStorage::~ObjectPoiStorage(){

}

ObjectPoiStorage* ObjectPoiStorage::getInstance(){
	if(instance==NULL){
		instance=new ObjectPoiStorage();
	}
	return instance;
}

int ObjectPoiStorage::registerObject(int startX, int startY, int width, int height, IPOIAlgorithm* poiSource){
	std::vector<Poi> iPts=poiSource->getPoiResult();
	std::vector<Poi>* objectPcts=new std::vector<Poi>();
	for(std::vector<Poi>::iterator vIt=iPts.begin(); vIt!=iPts.end(); vIt++){
		if(vIt->x > startX && vIt->x < startX+width && vIt->y < startY && vIt->y > startY-height){
			//the interest point is in the selected rectangle
			objectPcts->push_back(*vIt);
		}
	}
	_registeredObjects.push_back(objectPcts);
	//return _registeredObjects.size()-1;
	return objectPcts->size();
}

void ObjectPoiStorage::matchObjects(std::vector<Poi>& iPts){
	float dist, d1, d2;
	Poi* match;
	int objIterIndex=0;
	for(std::vector<std::vector<Poi>*>::iterator objIter=_registeredObjects.begin(); objIter!=_registeredObjects.end(); objIter++){
		for(std::vector<Poi>::iterator RpctIter=(*objIter)->begin(); RpctIter!=(*objIter)->end(); RpctIter++){
			d1 = d2 = FLT_MAX;

			//iterate over image points
			for(std::vector<Poi>::iterator pctIter=iPts.begin(); pctIter!=iPts.end(); pctIter++){
				// we only check for extremes of the same type (same Laplacian sign)
				if(pctIter->laplacianSign==RpctIter->laplacianSign){ 
					  dist = comparePOIs(*RpctIter, *pctIter);
					  if(dist<d1) // if this feature matches better than current best
					  {
						d2 = d1;
						d1 = dist;
						match=&(*pctIter);
					  }
					  else if(dist<d2) // this feature matches better than second best
					  {
						d2 = dist;
					  }
				}
			}
			if(d1/d2 < 0.7) 
			{ 
			  match->matchesObjectNr=objIterIndex;
			  match->matchedDistance=d1;
			}
		}
		objIterIndex++;
	}
}

float ObjectPoiStorage::comparePOIs(Poi& p1, Poi& p2){
  float sum=0;

  for(int i=0; i < 64; i++)
    sum += pow(p1.descriptor[i] - p2.descriptor[i],2);

  return sqrt(sum);
}

void ObjectPoiStorage::removeObject(int index){
	std::vector<std::vector<Poi>*>::iterator it=_registeredObjects.begin();
	std::advance(it,index);
	if(it!=_registeredObjects.end())
		_registeredObjects.erase(it);
}