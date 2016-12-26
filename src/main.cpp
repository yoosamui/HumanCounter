/*
 *  created by Juan R. Gonzalez & Juan Alvarez 2016
 */
#include "ofMain.h"
#include "ofApp.h"
#include "XSystem.h"



int main()
{
    // ofLogToFile("LogFile.txt", true);
     // http://abstrakt.vade.info/?p=210
     // https://github.com/vade/ofxQCPlugin/blob/master/src/ofxQCBaseWindowProxy.h
     ofSetupOpenGL(800, 500, OF_WINDOW);
    // ofSetupOpenGL(ofGetWindowManager(), 800,500, OF_WINDOW); 
   // ofAppGlutWindow window;
	//ofSetupOpenGL(&window, 800,500, OF_WINDOW);

     
     ofApp* app = new ofApp();
     
     XSystem s;
     vector<string> v;
     if( s.getAllInterfaces(&v) > 1 ) {
         string interface = v.at(1);
         v.clear();
         if ( s.getMacAddress(interface, &v) == 0 ){
             if( v.size() == 4)
                app->setMachineInfo(v.at(0),v.at(1),v.at(2),v.at(3));
         }
     }
     ofRunApp(app);

}
