#include "ofMain.h"
#include "DianShou.h"

#include "ofAppGlutWindow.h"


// 点手 character recognition using leap motion
int main( ){

    ofAppGlutWindow window;
    ofSetupOpenGL(&window, 500,500, OF_WINDOW);			// <-------- setup the GL context

    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp( new DianShou());

}
