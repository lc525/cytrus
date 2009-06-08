/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** DirectShowCameraSource.cpp
**
*/
#include "stdafx.h"
#include "DirectShowCameraSource.h"
#include <iostream>
//#include <boost/thread/thread.hpp>

using namespace cytrus::cameraHAL;

#ifdef WIN32
#include "comutil.h" // link with comsuppw.lib


DirectShowCameraSource* DirectShowCameraSource::instance = 0; // initialize pointer


DirectShowCameraSource::DirectShowCameraSource(){
	CoInitializeEx(NULL,COINIT_MULTITHREADED);
	_currentCamera=NO_CAMERA;
	deviceHandle=NULL;
	_cameraIsStarted=false;
	width=0;
	height=0;
	getCameraList();
}

DirectShowCameraSource::~DirectShowCameraSource(){
	CoUninitialize();
}

void DirectShowCameraSource::getCameraList(){
	int currentNo=_availableCameras.size();
	if(currentNo>0){
		for(std::list<char*>::iterator nIt=_availableCameras.begin(); nIt!=_availableCameras.end(); nIt++){
			delete (*nIt);
		}
	}
	_availableCameras.clear();
	RefreshCameraList(&_nrAvailableCameras);
	for(int i=0; i<_nrAvailableCameras; i++){
		BSTR pbstrName;
		IUnknown** iunk=new IUnknown*;
		GetCameraDetails(i,iunk,&pbstrName);
		char* name=_com_util::ConvertBSTRToString(pbstrName);
		SysFreeString(pbstrName);
		_availableCameras.push_back(name);
		delete []iunk;
	}
}

DirectShowCameraSource* DirectShowCameraSource::getCameraInstance(){
	// TODO: make this thread safe (boost lock)
	if(instance==NULL){
		instance=new DirectShowCameraSource();
	}
	return instance;
}

std::list<char*> DirectShowCameraSource::getAvailableCameras(bool refresh){
	if(refresh==true) getCameraList();
	return _availableCameras;
}

void DirectShowCameraSource::setActiveCamera(int cIndex){
	_currentCamera=cIndex;
	BSTR pbstrName;
	IUnknown** iunk=new IUnknown*;
	GetCameraDetails(cIndex,iunk,&pbstrName);
	deviceHandle=iunk;
	SysFreeString(pbstrName);
}

void DirectShowCameraSource::notifyConsumers(){
}

void DirectShowCameraSource::startCapture(){
	PFN_CaptureCallback cb=&DirectShowCameraSource::callbackFunc;
	StartCamera(deviceHandle[0],cb, &width, &height);
}

void DirectShowCameraSource::stopCapture(){
	StopCamera();
}

std::pair<int,int> DirectShowCameraSource::getImageSize(){
	std::pair<int, int> val;
	if(_cameraIsStarted==true){
		val.first=width;
		val.second=height;
	}
	else{
		val.first=-1;
		val.second=-1;
	}
	return val;
}

void __stdcall DirectShowCameraSource::callbackFunc(DWORD dwSize, BYTE* pbData){
	//for(int i=0; i<consumers
	std::cout<<"Processing image..."<<std::endl;
}

#endif