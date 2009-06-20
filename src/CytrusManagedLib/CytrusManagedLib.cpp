/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** CytrusManagedLib.cpp Assures .NET managed wrappers for the unmanaged cytrus code
**
*/

#include "stdafx.h"

#include "CytrusManagedLib.h"

using namespace cytrus::managed;
using namespace System::Windows::Media;

void CameraMgr::callImageCaptureEvent(int dwSize, unsigned char* pbData){
	/*if(lastdwSize==-1) lastdwSize=dwSize;
	else
		if(lastdwSize!=dwSize){
			onOutputModeChange(outputModes[alg->getCurrentOutputMode()]);
			lastdwSize=dwSize;
		}*/ // outputMode change events (not used)

	array<byte>^ byteArray = gcnew array< byte >(dwSize);
	Marshal::Copy((IntPtr)pbData,byteArray, 0, dwSize);
	onNewImageAvailable(byteArray);
}


CameraMgr::CameraMgr(){
	cList=gcnew ObservableCollection<String^>();
	outputModes=gcnew ObservableCollection<OutputMode^>();
	//lastdwSize=-1; // outputMode change events (not used)
	std::list<char*> cLst;
	std::list<std::pair<char*,int>*>* outputModesULst;

	cs=DirectShowCameraSource::getCameraInstance();
	cLst=cs->getAvailableCameras();
	int nr=cLst.size();

	//Marshal camera names:
	for(std::list<char*>::iterator it=cLst.begin(); it!=cLst.end(); it++){
		String^ name=gcnew String((*it));
		cList->Add(name);
	}

	//Marshal events
	fPtr= gcnew CaptureCallbackProc(this, &CameraMgr::callImageCaptureEvent);
	gch = GCHandle::Alloc(fPtr);
	IntPtr ip = Marshal::GetFunctionPointerForDelegate(fPtr);
	result = static_cast<POIAlgResult>(ip.ToPointer());

	//Initialise processing
	alg=new SurfAlg(cs, result);

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

CameraMgr::!CameraMgr(){
	gch.Free();
	delete alg;
}

void CameraMgr::selectCamera(int i){
	cs->setActiveCamera(i);
}

void CameraMgr::refreshCameraList(){
	cList->Clear();
	std::list<char*> cLst;
	cLst=cs->getAvailableCameras(true);
	int nr=cLst.size();

	//Marshal camera names:
	for(std::list<char*>::iterator it=cLst.begin(); it!=cLst.end(); it++){
		String^ name=gcnew String((*it));
		cList->Add(name);
	}

}

ObservableCollection<String^>^ CameraMgr::getCameraList(){
	return cList;
}

void CameraMgr::showPropertiesDialog(IntPtr window){
	cs->displayCameraPropertiesDialog((HWND)(window.ToPointer()));
}



void CameraMgr::setActiveOutputMode(int modeIndex){
	alg->setOutputMode(modeIndex);
}

ObservableCollection<OutputMode^>^ CameraMgr::getOutputModesList(){
	return outputModes;
}

bool CameraMgr::setProcessingSize(int width, int height){
	bool success=alg->setProcessingSize(width, height);
	if(success){
		_camWidth=width;
		_camHeight=height;
	}
	return success;
}

void CameraMgr::startCapture(){
	alg->run();

	_camWidth=cs->width;
	_camHeight=cs->height;

}


void CameraMgr::stopCapture(){
	cs->stopCapture();
}

