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
** poi.h: Defines data structures for the points of interest used
** in algorithms like SURF or SIFT
**
*/

#ifndef _POI_H_
#define _POI_H_

#ifndef CYTRUSALGLIB_API
#error "The file poi.h can only be included as part of the CytrusAlgLib library. Please include CytrusAlgLib.h instead"
#endif


class CYTRUSALGLIB_API Poi{
public:
	float x, y;
	float scale;
	float orientation;
	int laplacianSign;
	int descriptorSize;
	float* descriptor;
	float dx, dy;
	int clusterIndex;

	Poi(int descrSize=64) : orientation(0), descriptorSize(descrSize)
	{
		descriptor=new float[descriptorSize];
	}
};

#endif