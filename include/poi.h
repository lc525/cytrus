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
** Poi.h: Defines data structures for the points of interest used
** in algorithms like SURF or SIFT
**
*/

#ifndef _POI_H_
#define _POI_H_

#ifndef CYTRUSALGLIB_API
	#ifdef CYTRUSALGLIB_EXPORTS
		#define CYTRUSALGLIB_API __declspec(dllexport)
	#else
		#define CYTRUSALGLIB_API __declspec(dllimport)
	#endif
#endif


class CYTRUSALGLIB_API Poi{
private:
	float* descriptor;
public:
	float x, y;
	float scale;
	float orientation;
	int laplacianSign;
	int descriptorSize;
	float dx, dy;
	//int clusterIndex; - not used yet
	int matchesObjectNr;
	float matchedDistance;

	Poi(int descrSize=64);
	Poi(const Poi& other);
	Poi& operator=(const Poi &rhs);
	
	virtual ~Poi();

	float* getDescriptor();
};

#endif