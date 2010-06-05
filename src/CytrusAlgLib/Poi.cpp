/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
** Loosely Based on OpenSURF, by C.Evans (GNU GPL licence)
**
** -----------------------------------------------------------------
** Poi.cpp
**
*/

#include "stdafx.h"
#include "Poi.h"


Poi::Poi(int descrSize) : orientation(0), descriptorSize(descrSize)
{
	descriptor=NULL;
	matchesObjectNr=-1;
	matchedDistance=-1;
}


Poi::Poi(const Poi& other){
	if(this==&other) return;
	x=other.x;
	y=other.y;
	scale=other.scale;
	orientation=other.orientation;
	laplacianSign=other.laplacianSign;
	descriptorSize=other.descriptorSize;
	dx=other.dx;
	dy=other.dy;
	matchesObjectNr=other.matchesObjectNr;
	matchedDistance=other.matchedDistance;
	if(other.descriptor!=NULL){
		descriptor=new float[descriptorSize];
		for(int i=0; i<descriptorSize; i++)
			descriptor[i]=other.descriptor[i];
	}
	else{
		descriptor=NULL;
	}
}

Poi::~Poi(){ 
	if(descriptor!=NULL){
		delete []descriptor; 
	}
}

Poi& Poi::operator=(const Poi &rhs){
	if(this != &rhs){
		if(descriptor!=NULL){
			delete []descriptor;
			descriptor=NULL;
		}
		x=rhs.x;
		y=rhs.y;
		scale=rhs.scale;
		orientation=rhs.orientation;
		laplacianSign=rhs.laplacianSign;
		descriptorSize=rhs.descriptorSize;
		dx=rhs.dx;
		dy=rhs.dy;
		matchesObjectNr=rhs.matchesObjectNr;
		matchedDistance=rhs.matchedDistance;
		if(rhs.descriptor!=NULL){
			descriptor=new float[descriptorSize];
			for(int i=0; i<descriptorSize; i++)
				descriptor[i]=rhs.descriptor[i];
		}
	}
	return *this;
}


float* Poi::getDescriptor(){
	if(descriptor==NULL){
		descriptor=new float[descriptorSize];
	}
	return descriptor;
}