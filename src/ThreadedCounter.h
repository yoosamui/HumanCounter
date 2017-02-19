/*
 *  created by Juan R. Gonzalez & Juan Alvarez 2016
 */
#ifndef THREADCOUNTER_H
#define THREADCOUNTER_H

#include "ofMain.h"
#include "ofxCv.h"
#include <time.h>
#include <iostream>
#include "constants.h"
#include "ofUtils.h"
#include "ofThread.h"
#include <Poco/Mutex.h>

using namespace std;

class Duplicate
{
public:
    int blobidx;
    int value;
    int mode;
    int threadid;
    int labelid;

    Duplicate() {
        blobidx = 0;
        value = 0;
        mode = -1;
        threadid - 1;
        labelid = -1;
    }
};

class ThreadedCounter : public ofThread
{
public:

    enum Preset
    {
        Accurate, Sensitive
    };
    ThreadedCounter(ofPoint threshoLineTop, ofPoint threshoLineCenter,
            ofPoint threshoLineBottom, int blobidx, int classid, Preset preset);
    virtual ~ThreadedCounter();
    void update(ofRectangle& rectangle, int minBlobsize, int midBlobsize, int maxBlobsize,
            int width, const int &blobidx, const int &labelid, int yCenter);


    void update(ofxCv::ContourFinder contourFinder,cv::Mat outputimage, int nr, int direction);


    void displayInfo(string text);
    void reset();


    static Duplicate* widthduplicates[THREADDUPLICATESMAX];

    int blobidx;
    int labelid;

    int classid;
    uint64_t previousMillis;

    ofEvent<int> onPeapleIn;
    ofEvent<int> onPeapleOut;
    ofEvent<int> onPepaleChanged;
    ofEvent<int> onObjectWidthOutOfRange;

    int width;
    bool done;

    int currentState[BLOBMAX];


    ofxCv::ContourFinder contourFinder;
    cv::Mat outputimage;
    int nr;
    int direction;
    ofImage img;

protected:


    Preset preset;

    void notify(int width, int mode);
    bool isThreadWorking(int a, int b);
    ofVec2f currentCenterPos;
    ofVec2f previousPos;

    ofPoint threshoLineTop;
    ofPoint threshoLineCenter;
    ofPoint threshoLineBottom;
    ofRectangle rectangle;
    int minBlobsize;
    int midBlobsize;
    int maxBlobsize;
    int yCenter;
    int yCenterGobal;

    ofRectangle inRegion;
    ofRectangle bondaryRegion;
    ofRectangle outRegion;

    // map<int, int> statemapin;
    //map<int, int> statemapout;
    //  static map<int, int> stateduplicates;
    static int labelduplicat;

    int old_blobidx_in;
    int old_blobidx_out;

    static int previous_threadid;
    static int previous_blobidx;


    // int currentState;
    time_t dataEndwait;
    int dataSeconds;
    int oldy;
    int exitcounter;
    int offset;
};

class ThreadedCounterIn : public ThreadedCounter
{
public:

    ThreadedCounterIn(ofPoint threshoLineTop, ofPoint threshoLineCenter,
            ofPoint threshoLineBottom, int blobidx, int classid, Preset preset) :
    ThreadedCounter(threshoLineTop, threshoLineCenter, threshoLineBottom, blobidx, classid, preset) {
    }

    void threadedFunction();
};

class ThreadedCounterOut : public ThreadedCounter
{
public:

    ThreadedCounterOut(ofPoint threshoLineTop, ofPoint threshoLineCenter,
            ofPoint threshoLineBottom, int blobidx, int classid, Preset preset) :
    ThreadedCounter(threshoLineTop, threshoLineCenter, threshoLineBottom, blobidx, classid, preset) {
    }
    void threadedFunction();
};


#endif /* THREADCOUNTER_H */

