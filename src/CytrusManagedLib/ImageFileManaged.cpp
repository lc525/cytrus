/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** ImageFileManaged.cpp
**
*/

#include "stdafx.h"
#include "ImageFileManaged.h"

//#define DEBUG_EVENTLOG // define to write alg worker information to Windows Event Log
#define MAX_PROCESSING_THREADS 4

using namespace cytrus::managed;
using namespace System::Runtime::InteropServices;

void ImageFileMgr::callImageCaptureEvent(int dwSize, unsigned char* pbData, int index){
	array<byte>^ byteArray = gcnew array< byte >(dwSize);
    List<Poi_m^>^ poiArray = gcnew List<Poi_m^>(120);

	Marshal::Copy((IntPtr)pbData,byteArray, 0, dwSize);

    std::vector<Poi> pctLst=alg->getPoiResult();
    for(std::vector<Poi>::iterator it=pctLst.begin(); it!=pctLst.end(); it++){
        poiArray->Add(gcnew Poi_m(it->x, it->y));
    }

    onImageAvailableForRendering(byteArray, poiArray);
}


ImageFileMgr::ImageFileMgr(){
	isInvalid=false;
    outputModes=gcnew ObservableCollection<OutputMode^>();

	fs=new FileImageSource();

    //lastdwSize=-1; // outputMode change events (not used)
    std::list<std::pair<char*,int>*>* outputModesULst;


    //Marshal events
    fPtr= gcnew RenderResultCallbackProc(this, &ImageFileMgr::callImageCaptureEvent);
    gch = GCHandle::Alloc(fPtr);
    IntPtr ip = Marshal::GetFunctionPointerForDelegate(fPtr);
    result = static_cast<POIAlgResult>(ip.ToPointer());
    alg=new SurfAlg(fs, result, -1);
    outputModesULst=alg->getOutputModes();
    for(std::list<std::pair<char*,int>*>::iterator it=outputModesULst->begin(); it!=outputModesULst->end(); it++){
        String^ name=gcnew String((*it)->first);
        PixelFormat^ pf;
        switch((*it)->second){
            case 1: pf=PixelFormats::Bgr24; break;
            case 2: pf=PixelFormats::Gray8; break;
        }
        OutputMode^ om=gcnew OutputMode();
        om->modeName=name;
        om->pixelFormat=pf;

        outputModes->Add(om);
    }
}

void ImageFileMgr::setImagePath(String^ path){
	filePath=(char*)Marshal::StringToHGlobalAnsi(path).ToPointer();
	fs->setPath(filePath);
}

ImageFileMgr::~ImageFileMgr(){
	this->!ImageFileMgr();
}

void ImageFileMgr::freeResources(){
	if(gch.IsAllocated) gch.Free();
	fs->stopCapture();
	if(alg!=NULL){
		delete alg;
		alg=NULL;
	}

	Marshal::FreeHGlobal((IntPtr)filePath);
	isInvalid=true;
}

ImageFileMgr::!ImageFileMgr(){
	if(gch.IsAllocated) gch.Free();
	if(fs!=NULL){
		delete fs;
		fs=NULL;
	}
}

void ImageFileMgr::setActiveOutputMode(int modeIndex){
	alg->setOutputMode(modeIndex);
}

ObservableCollection<OutputMode^>^ ImageFileMgr::getOutputModesList(){
	return outputModes;
}

bool ImageFileMgr::setProcessingSize(int width, int height){
	bool success;
    success=alg->setProcessingSize(width, height);
    return success;
}

void ImageFileMgr::startImageProcessing(){
	if(!isInvalid){
		alg->run();
	}
	else{
		isInvalid=false;
		alg=new SurfAlg(fs, result, -1);
		alg->run();
	}
}
