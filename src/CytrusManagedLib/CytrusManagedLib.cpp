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
using namespace System::Threading;

void CameraMgr::callImageCaptureEvent(int dwSize, unsigned char* pbData){
	/*if(lastdwSize==-1) lastdwSize=dwSize;
	else
		if(lastdwSize!=dwSize){
			onOutputModeChange(outputModes[alg->getCurrentOutputMode()]);
			lastdwSize=dwSize;
		}*/ // outputMode change events (not used)

	array<byte>^ byteArray = gcnew array< byte >(dwSize);
	Marshal::Copy((IntPtr)pbData,byteArray, 0, dwSize);
	onImageAvailableForRendering(byteArray);
}

void CameraMgr::newImageAvailableEvent(){
	//on different thread:
	int workerThreads;
	int completionPortThreads;
	ThreadPool::GetAvailableThreads(workerThreads,completionPortThreads);
	if(workerThreads>0) //drop frames instead of creating a huge number of threads
		ThreadPool::QueueUserWorkItem(gcnew WaitCallback(this, &CameraMgr::cameraNotifyConsumers));
	//Thread^ t1=gcnew Thread(gcnew ThreadStart(this, &CameraMgr::cameraNotifyConsumers));
	//t1->Name="Surf processing thread";
	//t1->Start();
}

void CameraMgr::cameraNotifyConsumers(Object^ o){
	int code=Thread::CurrentThread->GetHashCode();
	int index;
	if(!threadIndexes->ContainsKey(code))
		threadIndexes->Add(code,thNr++);
	else
		index=threadIndexes[code];
	
	//on different thread:
	cs->notifyConsumer(index);
}


CameraMgr::CameraMgr(){
	threadIndexes=gcnew Dictionary<int,int>();
	alg_ProcessingPool=new std::list<IPOIAlgorithm*>();
	cList=gcnew ObservableCollection<String^>();
	outputModes=gcnew ObservableCollection<OutputMode^>();
	//lastdwSize=-1; // outputMode change events (not used)
	std::list<char*> cLst;
	std::list<std::pair<char*,int>*>* outputModesULst;

	newImage= gcnew NewImageCallback(this, &CameraMgr::newImageAvailableEvent);
	nigch = GCHandle::Alloc(newImage);
	IntPtr ipni = Marshal::GetFunctionPointerForDelegate(newImage);
	newImageAvailable = static_cast<NewImageAvailableCallback>(ipni.ToPointer());
	cs=DirectShowCameraSource::getCameraInstance(newImageAvailable);
	cLst=cs->getAvailableCameras();
	int nr=cLst.size();

	//Marshal camera names:
	for(std::list<char*>::iterator it=cLst.begin(); it!=cLst.end(); it++){
		String^ name=gcnew String((*it));
		cList->Add(name);
	}

	//Marshal events
	fPtr= gcnew RenderResultCallbackProc(this, &CameraMgr::callImageCaptureEvent);
	gch = GCHandle::Alloc(fPtr);
	IntPtr ip = Marshal::GetFunctionPointerForDelegate(fPtr);
	result = static_cast<POIAlgResult>(ip.ToPointer());

	int workerThreads;
	int completionPortThreads;
	ThreadPool::GetMaxThreads(workerThreads,completionPortThreads);
	//Initialise processing
	for(int u=0; u<workerThreads; u++){
		IPOIAlgorithm* pw=new SurfAlg(cs, result);
		alg_ProcessingPool->push_back(pw);
	}

	IPOIAlgorithm* alg=*(alg_ProcessingPool->begin());
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
	nigch.Free();
	for(std::list<IPOIAlgorithm*>::iterator it=alg_ProcessingPool->begin(); it!=alg_ProcessingPool->end(); it++){
		delete *it;
	}
	delete alg_ProcessingPool;
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
	for(std::list<IPOIAlgorithm*>::iterator it=alg_ProcessingPool->begin(); it!=alg_ProcessingPool->end(); it++){
		(*it)->setOutputMode(modeIndex);
	}
}

ObservableCollection<OutputMode^>^ CameraMgr::getOutputModesList(){
	return outputModes;
}

bool CameraMgr::setProcessingSize(int width, int height){
	bool success;
	for(std::list<IPOIAlgorithm*>::iterator it=alg_ProcessingPool->begin(); it!=alg_ProcessingPool->end(); it++){
		success=(*it)->setProcessingSize(width, height);
		if(success){
			_camWidth=width;
			_camHeight=height;
		}
	}
	return success;
}

void CameraMgr::startCapture(){
	cs->startCapture();
	_camWidth=cs->width;
	_camHeight=cs->height;
}


void CameraMgr::stopCapture(){
	cs->stopCapture();
}

