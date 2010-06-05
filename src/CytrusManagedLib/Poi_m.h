/* Carata Lucian, BSc Thesis Work
** Technical University of Iasi, 
** Faculty of Automatic Control and Computer Enigineering, 2009.
** 
** Author: Carata Lucian
** Project: Cytrus
** License notice:  GNU GPL
**
** -----------------------------------------------------------------
** Poi_m.h Managed class for storing points of interest
**
*/

namespace cytrus {
    namespace managed{

        public ref class Poi_m
        {
        private:
            int _x, _y;
			float _orientation;
			float _scale;
			int _matchesObjectNr;
			float _matchedDistance;

        public:

            property int X
            {
                int get(){
                    return _x;
                }

                void set(int val){
                    _x=val;
                }
            }

            property int Y
            {
                int get(){
                    return _y;
                }

                void set(int val){
                    _y=val;
                }
            }

			property int MatchesObjectNo
            {
                int get(){
                    return _matchesObjectNr;
                }

                void set(int val){
                    _matchesObjectNr=val;
                }
            }

			property float Orientation
            {
                float get(){
                    return _orientation;
                }

                void set(float val){
                    _orientation=val;
                }
            }

			property float MatchedDistance
            {
                float get(){
                    return _matchedDistance;
                }

                void set(float val){
                    _matchedDistance=val;
                }
            }

			property float Scale
            {
                int get(){
                    return _scale;
                }

                void set(int val){
                    _scale=val;
                }
            }

            Poi_m(int x, int y){
                _x=x;
                _y=y;
            }

            Poi_m(){}
        };

    }
}