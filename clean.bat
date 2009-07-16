cd bin
echo y | del *.*
cd ..
cd lib
attrib +h libjpeg.lib
echo y| del *.*
attrib -h libjpeg.lib
