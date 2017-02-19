/*
 *  created by Juan R. Gonzalez & Juan Alvarez 2016
 */
#pragma once

#include "ofMain.h"
#include "Configuration.h"
#include "constants.h"
#include <algorithm>
#include "BlobTracker.h"
#include "ofxCv.h"
#include <stdio.h>
#include "ofxHttpUtils.h"
#include "ofxOpenCv.h"
#include "ofxCvHaarFinder.h"
#include "ThreadedCounter.h"
#ifdef USE_PI_CAMERA
#include "ofxCvPiCam.h"
#endif
#include "GPIOClass.h"


using namespace ofxCv;
using namespace cv;

class Blob
{
public:
    ofRectangle rectangle;
    int id;
    
    bool operator > (const Blob& r) const
    {
        return (rectangle.y > r.rectangle.y);
    }
};

class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void keyReleased(int key);

    void mouseMoved(int x, int y);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void updateMouseEvents();
    void exit();

    void makeMask();
    void initCounturFinder();
    void drawMaskArea();
    void drawThresholdLines();
    void drawGui();
    void drawFinder();
    void drawFinders();
    void drawContourFinder();
    void setMask();
    void removeMask();
    void resetCounters();
    void drawCounters();
    void saveConfig();
    ofRectangle getRectangleFromMask();
    ofRectangle isIntersect(ofRectangle a, ofRectangle b);

    void setupHttp();
    void newResponse(ofxHttpResponse &response);
    
    void createThreads();

   // void eventHandlerPeopleGoingOut(int &persons);
    //void eventHandlerPeopleGoingIn(int &persons);
    void exportGrayImage(string path);
    void cropGrayImage();
    int isHuman();
    void updateCounters(vector<cv::Rect> track);
    
   // void threadedBlobCounter();

    bool isInside(int pointX, int pointY, int rectX, int rectY,
            int rectWidth, int rectHeight);

    ofEvent<int> peopleIn;
    ofEvent<int> peopleOut;

    void httpRequest(); 
    void eventHandlerPeapleIn(int &howmany);
    void eventHandlerPeapleOut(int &howmany);
    void camRotationChanged(int value);
    void camContrastChanged(int value);
    void camBrightnessChanged(int value);
    void setMachineInfo(string interface, string ipaddress, string subnet, string macaddress){
       
        
        this->interface = interface;
        this->ipaddress = ipaddress;
        this->subnet = subnet;
        this->macaddress = macaddress;
    }

private:
 void exportOutputImage(string info);
 
    ofVideoPlayer video;

#ifdef USE_PI_CAMERA
    ofxCvPiCam cam;
#else
    ofVideoGrabber cam;
#endif
    cv::BackgroundSubtractorMOG2* mog2;
    ofxCv::ContourFinder contourFinder;
    bool showLabels;
   
    GPIOClass* m_gpio4;
    bool m_movement = false;
   
    ofImage img;
    Configuration config;
    bool isOneFrameSet;
    cv::Mat frame;
    cv::Mat output;
    cv::Mat mask, maskOutput, maskmog2;
    cv::Mat resizeF;
    cv::Mat lightenMat;
    cv::Mat cannyMat;
    
    ofxCvColorImage colorImg;
    drawLine threshoLineTop;
    drawLine threshoLineCenter;
    drawLine threshoLineBottom;
    int personsIn;
    int personsOut;
    int personstotal;
    int tempGoIn;
    int tempGoOut;
    ofSoundPlayer soundIn;
    ofSoundPlayer soundOut;
    bool mousePress;
    ofPoint mouseStart;
    ofPoint mouseMove;
    bool paintLine;
    int pressedCount;
    bool activateFinder;
    bool activateCropTimer;
    int operant;
    int configIndex;
    bool isVideoMode;
    uint64_t previousMillis;
    uint64_t previousMillisCropTimer;
    ofPolyline polyline;
    vector < ofPoint > drawnPoints;
    std::vector<cv::Rect> contourFinderVector;
    ofxCvGrayscaleImage grayBg, grayDiff;
    ofxCvGrayscaleImage grayScaleImage;
    ofImage onFrameGrayImage;
    ofImage grayImage;
    ofImage grayImageMasked;
    ofImage ouputImage;
    ofImage cannyedge;
    Mat cannymatOutput;
    
    ofImage grayhapedoutput;
    Mat matshapedoutput;
    ofxCvGrayscaleImage backgroundimage;
    int mouseDelay;
    ofTrueTypeFont font;
    ofTrueTypeFont fontsmall;
    
    ofxHttpUtils httpUtils;
    vector<ThreadedCounter*> threadedCounters;
    vector<ofPoint> objcenters; //object's centers
    vector<vector<Point> > contours;
    
    vector<ofCvBlob> blobs;
    vector<Vec4i> hierarchy;
    map<int, int> object_states;
    map<int, int> object_hexes;
    int entryCount;
    int exitCount;
    ofxCv::KalmanPosition kalman;
    ofMesh predicted, line, estimated;
    ofVec2f point;
    float speed;
    
    string interface;
    string ipaddress;
    string subnet;
    string macaddress;        
    
    int currentState[BLOBMAX];
    BlobTracker blobTracker;
    
    int previousYCenter ;
     int previousPos[BLOBMAX];
      int previousDirection[BLOBMAX];
    
};
