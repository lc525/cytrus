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

            Poi_m(int x, int y){
                _x=x;
                _y=y;
            }

            Poi_m(){}
        };

    }
}