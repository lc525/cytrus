/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** CytrusAlgLib.h : Dll function declarations. The dll is used for 
** performing image-based transformation algorithms.
**
*/
#ifdef CYTRUSALGLIB_EXPORTS
#define CYTRUSALGLIB_API __declspec(dllexport)
#else
#define CYTRUSALGLIB_API __declspec(dllimport)
#endif

// #define Configuration Section:
// 1.Debug options DEBUG | DEBUG_TRACE |RELEASE
#define DEBUG

#include "poi.h"
#include "iimage.h"
#include <vector>



/* The class implements the SURF algorithm for a image given as input (in the constructor).
** Its results are stored as a list of POIs (Points Of Interest), with a characteristic vector
** of size 64.
*/
class CYTRUSALGLIB_API Surf {
private:
	std::vector<Poi> &_ipts; //points of interest
	IImage *_intImg; // the integral image, owned pointer.
	
	// When the calculateDescriptors function iterates over POIs, pctIndex 
	// provides the current POI index to the orientation and descriptor functions.
	int pctIndex;

public:
	~Surf();
	
	// Standard Constructor
	// Parameters:
	//	img: the input image
	//	uSURF: set to true if the application does not need rotational invariance 
	//         (+/- 15 deg. still allowed).
	//		   U-SURF is the variation of SURF that doesn't provide rotational invariance
	//		   (the descriptors are not oriented). This might prove usefull when speed
	//		   is really important.
	Surf(IImage *img, bool uSURF);

	// returns the POI-s detected in the image
	std::vector<Poi> * computePoi();
	 

private:
	// Parameters:
	// uSurf=true applies the U-SURF algorithm
	// uSurf=false applies the usual SURF algorithm
	void calculateDescriptors(bool uSURF);

	void getPOI_Orientation();
	void getPOI_Descriptor();
	void getPOI_UDescriptor();

};
