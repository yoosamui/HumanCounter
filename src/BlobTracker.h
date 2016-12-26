#ifndef BLOBTRACKER_H
#define BLOBTRACKER_H


#include "ofMain.h"
#include "ofCvBlob.h"
#include "ofxCv.h"
#include "TrackedBlob.h"
#include "constants.h"
#include "ThreadedCounter.h"
#include <Poco/Mutex.h>
#include "Configuration.h"

//Poco::Mutex trackermutex;


using namespace ofxCv;
using namespace cv;

class DirectionState
{
public:
    int blobidx;
    int direction;
    int count;
    bool isSet;

    DirectionState() {

        blobidx = -1;
        direction = 0;
        count = 0;
        isSet = false;

    }
};

class TrackPosItem
{
public:
    bool dirty;
    int blobidx;
    int direction;
    int value;
    ofRectangle rectangle;

    TrackPosItem() {
        dirty = true;
        blobidx = 0;
        direction = 0;
        value = 0;


    }

    string getDirectionToString(int direction) {
        if (direction == 0)
            return "invalid";

        return direction == 1 ? "Down" : "Up";
    }
};

class BlobTracker
{
private:
    Configuration config;

    cv::Mat output;
    
       
  
    
    vector<vector<TrackedBlob> > history;
    vector<ThreadedCounter*> threadedCounters;

    drawLine threshoLineTop;
    drawLine threshoLineCenter;
    drawLine threshoLineBottom;

    //TODO: create a class for this
    int swapPreviousPos[BLOBMAX];
    int previousPos[BLOBMAX];
    int direction[BLOBMAX];
    int currentState[BLOBMAX];
    int revokestate[BLOBMAX];
    ThreadedCounter* outCounters[BLOBMAX];
    ThreadedCounter* inCounters[BLOBMAX];

    TrackPosItem posItems[3][BLOBMAX];
    TrackPosItem previousPosItems[3][BLOBMAX];

    DirectionState directionState[BLOBMAX];
    int imageSet[BLOBMAX];


    int previousDirection[BLOBMAX];
    int previousDirection2[BLOBMAX];




    bool swapset;
    int swapidx;


    void createThreads();

    void eventHandlerPeapleOut(int &howmany);
    void eventHandlerPeapleIn(int &howmany);
    void eventHandlerPeapleChange(int &index);

    void rememberValues(int index, int yvalue);

    void populateThreads(TrackPosItem item);
    int minblobsize;
    int midblobsize;
    int maxblobsize;


public:
      void setOutputImage( cv::Mat output,ofxCv::ContourFinder contourFinder ){
        
        this->output = output;
        this->contourFinder = contourFinder;
        
    }        
    
    void populateThreads(int nr,int direction, int blobidx, int labelid, ofRectangle rect, int value);
    int getDirection(int index);
    int countRunningThreads();

    Poco::Mutex trackermutex;

    vector<TrackedBlob> blobs;
    vector<ofRectangle> blobsPositions;

    BlobTracker();

    ofRectangle getNext();
    void trackStable(const vector<ofCvBlob>& bloblist, int minblobsize, int midblobsize, int maxblobsize);
    void trackSingleThread(const vector<ofCvBlob>& bloblist, int minblobsize, int midblobsize, int maxblobsize);

    void setConfig(const Configuration& config);


    ofEvent<int> peopleIn;
    ofEvent<int> peopleOut;

    ofxCv::ContourFinder contourFinder;
    int frameCount;

    int detectedDirection;
    
    ofImage img;

};




#endif /* BLOBTRACKER_H */

