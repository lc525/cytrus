/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** FileImageSource.cpp
**
*/
#include "stdafx.h"
#include "FileImageSource.h"
//#include <boost/gil/extension/io_new/jpeg_io_old.hpp> // testing with the new IO, problems with EXIF data
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <iostream>


//#include <boost/thread/thread.hpp> - futures

using namespace cytrus::cameraHAL;
using namespace boost::gil;


FileImageSource::FileImageSource(){
	_sourceIsStarted=false;
	width=0;
	height=0;
	imageFile=new rgb8_image_t();
	//imageFileView=new rgb8_view_t();
}

FileImageSource::~FileImageSource(){
	if(imageFile!=NULL) delete imageFile;
}

void FileImageSource::startCapture(){
	try{
		jpeg_read_image(_path,*imageFile);
		width=imageFile->width();
		height=imageFile->height();
		imageDataSize=width*height*3;
		imageFileView=(rgb8_view_t*)&view(*imageFile);
		rgb8_image_t flippedImg(width,height);
		rgb8_view_t flipped=view(flippedImg);
		copy_pixels(flipped_up_down_view(*imageFileView), flipped);
		imageData=(unsigned char*)interleaved_view_get_raw_data(flipped);
		_sourceIsStarted=true;
		notifyConsumers();
	}
	catch(char * str){
		std::cerr<<"Format not supported"<<std::endl;
	}
}

void FileImageSource::setPath(const char* path){
	_path=path;
}


void FileImageSource::notifyConsumers(){
	if(_sourceIsStarted==true){
		for(std::set<IImageConsumer*>::iterator cIt=consumers.begin(); cIt!=consumers.end(); cIt++){
			(*cIt)->processImage(imageDataSize,(unsigned char*)imageData);
		}
	}
}

void FileImageSource::notifyConsumer(int consumerIndex){
	if(consumerIndex<consumers.size() && _sourceIsStarted==true){
		std::set<IImageConsumer*>::iterator cIt=consumers.begin();
		std::advance(cIt,consumerIndex);
		(*cIt)->processImage(imageDataSize,(unsigned char*)imageData);
	}
}

void FileImageSource::stopCapture(){
	_sourceIsStarted=false;
}

std::pair<int,int> FileImageSource::getImageSize(){
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