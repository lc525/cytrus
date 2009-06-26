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
//#include <boost/thread/thread.hpp> - futures

using namespace cytrus::cameraHAL;

#ifdef WIN32
#include "comutil.h" // link with comsuppw.lib


DirectShowCameraSource* DirectShowCameraSource::instance = NULL; // initialize pointer
DWORD DirectShowCameraSource::imageDataSize=0;
BYTE* DirectShowCameraSource::imageData=NULL;
NewImageAvailableCallback DirectShowCameraSource::signalNewImageAvailable=NULL;

DirectShowCameraSource::DirectShowCameraSource(){
	CoInitializeEx(NULL,COINIT_MULTITHREADED);
	_currentCamera=NO_CAMERA;
	deviceHandle=NULL;
	_sourceIsStarted=false;
	width=0;
	height=0;
	getCameraList();
}

DirectShowCameraSource::~DirectShowCameraSource(){
	if(_sourceIsStarted) stopCapture();
	CleanupCameraInfo();
	int currentNo=_availableCameras.size();
	if(currentNo>0){
		for(std::list<char*>::iterator nIt=_availableCameras.begin(); nIt!=_availableCameras.end(); nIt++){
			delete (*nIt);
		}
	}
	if(deviceHandle!=NULL) 
		delete []deviceHandle;
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

DirectShowCameraSource* DirectShowCameraSource::getCameraInstance(NewImageAvailableCallback callback){
	signalNewImageAvailable=callback;
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
	if(deviceHandle!=NULL){
		delete []deviceHandle;	
	}
	_currentCamera=cIndex;
	BSTR pbstrName;
	IUnknown** iunk=new IUnknown*;
	GetCameraDetails(cIndex,iunk,&pbstrName);
	deviceHandle=iunk;
	SysFreeString(pbstrName);
}

void DirectShowCameraSource::displayCameraPropertiesDialog(HWND hwnd){
	if(_currentCamera!=NO_CAMERA){
		DisplayCameraPropertiesDialog(deviceHandle[0],hwnd);
	}
}

void DirectShowCameraSource::notifyConsumers(){
	for(std::set<IImageConsumer*>::iterator cIt=instance->consumers.begin(); cIt!=instance->consumers.end(); cIt++){
		(*cIt)->processImage(imageDataSize,(unsigned char*)imageData);
	}
}

void DirectShowCameraSource::notifyConsumer(int consumerIndex){
	if(consumerIndex<instance->consumers.size()){
		std::set<IImageConsumer*>::iterator cIt=instance->consumers.begin();
		std::advance(cIt,consumerIndex);
		(*cIt)->processImage(imageDataSize,(unsigned char*)imageData);
	}
}

void DirectShowCameraSource::startCapture(){
	_sourceIsStarted=true;
	PFN_CaptureCallback cb=&DirectShowCameraSource::callbackFunc;
	StartCamera(deviceHandle[0],cb, &width, &height);
}

void DirectShowCameraSource::stopCapture(){
	StopCamera();
	_sourceIsStarted=false;
}

std::pair<int,int> DirectShowCameraSource::getImageSize(){
	std::pair<int, int> val;
	if(_sourceIsStarted==true){
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
	if(imageDataSize!=dwSize){
		imageDataSize=dwSize;
			imageData=(BYTE*)realloc((void*)imageData, sizeof(BYTE)*dwSize);
			if(imageData==NULL) exit(1);
	}
	memcpy_s((void*)imageData, sizeof(BYTE)*dwSize, pbData, sizeof(BYTE)*dwSize);
	signalNewImageAvailable();
}

#endif