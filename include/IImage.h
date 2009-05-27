/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** IImage.h: Defines a common interface for images used in the cytrus algorithms
**
*/

#ifndef _IIMAGE_H_
#define _IIMAGE_H_

#ifndef CYTRUSALGLIB_API
#error "The file IImage.h can only be included as part of the CytrusAlgLib library. Please include CytrusAlgLib.h instead"
#endif

class CYTRUSALGLIB_API IImage{
public:
	//Image Characteristics
	int height, width;
	
	//Image Operations

	//Gets the grayscale version of the image
	virtual IImage* getGray()=0;
	//Calculates an integral image from the curent image.
	virtual IImage* getIntegralImage()=0;
	//Calculates the sum of the pixels from the rectangle defined by the start pixels
	//(top left corner) and the width and height.
	virtual float getBoxIntegral(int startPixelX, int startPixelY, int width, int height)=0;
};

#endif