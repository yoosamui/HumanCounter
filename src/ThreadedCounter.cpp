/*
 *  created by Juan R. Gonzalez & Juan Alvarez 2016
 */
#include "ThreadedCounter.h"

//--------------------------------------------------------------
//map<int, int> ThreadedCounter::stateduplicates;
Poco::FastMutex fmutex;
Poco::Mutex mutex;

int ThreadedCounter::labelduplicat;
//map<int, int> ThreadedCounter::statemapin;
//map<int, int> ThreadedCounter::statemapout;
//int ThreadedCounter::currentState[BLOBMAX];
Duplicate* ThreadedCounter::widthduplicates[THREADDUPLICATESMAX];



int ThreadedCounter::previous_threadid;
int ThreadedCounter::previous_blobidx;

ThreadedCounter::ThreadedCounter(ofPoint threshoLineTop, ofPoint threshoLineCenter,
        ofPoint threshoLineBottom, int blobidx, int classid, ThreadedCounter::Preset preset)
{
    this->threshoLineTop = threshoLineTop;
    this->threshoLineCenter = threshoLineCenter;
    this->threshoLineBottom = threshoLineBottom;


    this->classid = classid;
    this->blobidx = blobidx;
    this->labelid = 0;
    this->done = false;
    this->previousMillis = 0;
    this->inRegion = ofRectangle(0, 0, CAMERAWIDTH, CAMERAHEIGHT / 2 - THRESHOLINESOFFSET);
    this->bondaryRegion = ofRectangle(0, threshoLineTop.y, CAMERAWIDTH, THRESHOLINESOFFSET * 2);
    this->outRegion = ofRectangle(0, threshoLineBottom.y, CAMERAWIDTH, CAMERAHEIGHT / 2 - THRESHOLINESOFFSET);

    this->dataSeconds = THREADEDCOUNTER_CHKNODATA;
    this->dataEndwait = time(NULL) + dataSeconds;
    time_t endwait;
    int seconds = THREADEDCOUNTER_LIFETIME;
    endwait = time(NULL) + seconds;
    this->dataSeconds = THREADEDCOUNTER_CHKNODATA;
    this->dataEndwait = time(NULL) + dataSeconds;
    this->oldy = 0;
    this->exitcounter = THREADEDCOUNTER_NODATAREPEATER;
    this->preset = preset;

    previous_threadid = 0;
    previous_blobidx = 0;
    std::fill_n(currentState, BLOBMAX, 0);

    for (int i = 0; i < THREADDUPLICATESMAX; i++)
        widthduplicates[i] = new Duplicate();


    yCenter = 0;


    this->displayInfo("created!");
}
//--------------------------------------------------------------

ThreadedCounter::~ThreadedCounter()
{
    mutex.unlock(); // just in case...
    waitForThread(true);
    this->displayInfo("destroy it!");
}
//--------------------------------------------------------------

bool ThreadedCounter::isThreadWorking(int a, int b)
{
    return true;
    // Dont used. it breaks the system...
    if (time(NULL) > dataEndwait /*&& blobidx > 0*/) {
        if (a == b && exitcounter == 0) {
            displayInfo("no data available. terminate!");
            return false;
        }
        if (exitcounter-- < 0)
            exitcounter = THREADEDCOUNTER_NODATAREPEATER;

        this->dataEndwait = time(NULL) + dataSeconds;
    }

    return true;
}
//--------------------------------------------------------------

void ThreadedCounter::reset()
{
    //currentState = 1;
    std::fill_n(currentState, BLOBMAX, 0);

}

void ThreadedCounter::update(ofxCv::ContourFinder contourFinder, cv::Mat outputimage, int nr, int direction)
{

    this->contourFinder = contourFinder;
    this->outputimage = outputimage;
    this->nr = nr;
    this->direction = direction;

}

int oldDuplicate = 0;
bool duplicatefound = false;

void ThreadedCounter::update(ofRectangle& rectangle, int minBlobsize, int midBlobsize,
        int maxBlobsize, int width, const int &blobidx, const int &labelid, int yCenter)

{
    if (this->blobidx != blobidx) {

        return;
    }

    if (labelid == -1) {
        cout << "---------------------------------------------------------------------------Reinits" << endl;
        std::fill_n(currentState, BLOBMAX, 0);
    }


    this->yCenter = yCenter;
    yCenterGobal = yCenter;

    //cout << " in update " <<  rectangle.y<< endl;
    //if (lock())

    this->rectangle = rectangle;

    this->minBlobsize = minBlobsize;
    this->midBlobsize = midBlobsize;
    this->maxBlobsize = maxBlobsize;
    this->width = width;
    this->labelid = labelid;

    this->offset = (int) this->rectangle.height / 2;
    this->currentCenterPos = rectangle.getCenter();
    this->previousPos.interpolate(currentCenterPos, 0.3);

}

//--------------------------------------------------------------

void ThreadedCounterIn::threadedFunction()
{
    std::fill_n(currentState, BLOBMAX, 0);

    displayInfo("Start Running IN Thread!");
    while (isThreadRunning()) {

        // BUENA

        int blobwidth = (int) rectangle.width;
        int offset = (int) rectangle.height / 2;
        int yCenter = (int) rectangle.y + offset;
        int yTop = (int) rectangle.y;

        /* DON'T WORK
       int blobwidth = (int) rectangle.getWidth();
       int offset = (int) rectangle.getHeight() / 2;
       //int yCenter = (int) rectangle.y + offset;
       int yTop = (int) yCenter - offset;
         */
        /*
        string filename = ofToString(nr) + "_" + ofToString(direction) + ".jpg";
//cout << "UPDATEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"<<endl;
        if (img.load(filename)) {

            int index = nr; //nr

            ofImage image;
            ofxCv::toOf(this->outputimage, image);

            if (yCenter > threshoLineCenter.y) {
                currentState[index] = 8;
            }

            contourFinder.findContours(image);
            if (contourFinder.size() > 0) {

                //cout << "XXXXX " << (int) contourFinder.size() << endl;

                int howmany = 1;
                if (howmany > 0) {
                    displayInfo("--->IN " + ofToString(howmany) + " w:" + ofToString(blobwidth) + " " + ofToString(yCenterGobal) + " /" + ofToString(yCenter));
                    ofNotifyEvent(this->onPeapleIn, howmany, this);
                    ofNotifyEvent(this->onPepaleChanged, labelid, this);


                }
                if (lock()) {
                    ofFile file;
                    file.removeFile(filename);
                    unlock();
                }

                break;
            }
        }





        // give CPU time...
        ofSleepMillis(THREADEDCOUNTER_DELAYTIME);
        continue;
        /////////////////////////////////////
        
        
        */
        
        
        
        
        
        
        
        
        if (preset == ThreadedCounter::Sensitive) {

            int index = blobidx; //nr
            //         // in 
            if (yTop > threshoLineBottom.y) {
                currentState[index] = 1;
                //  currentState = states[nr] = 2;
                // cout << nr << " " << currentState[nr] << " " << yCenter <<" "<< thresholdPosBottom<< endl;

            }
            if (currentState[index] == 1 && yCenter < threshoLineBottom.y) {
                currentState[index] = 2;
                //  currentState = states[nr] = 2;
                //    cout << nr << " -------> IN " << currentState[nr] << " " << yCenter << endl;

            }
            //        if (currentState[index] == 2 && yCenter < threshoLineCenter.y) {
            //            currentState[index] = 3;
            //            //  currentState = states[nr] = 2;
            //            //    cout << nr << " -------> IN " << currentState[nr] << " " << yCenter << endl;
            //
            //        }
            if (currentState[index] == 2 && yCenter < threshoLineCenter.y) {
                currentState[index] = 0;


                int howmany = 0;

                duplicatefound = false;
                for (int i = 0; i < BLOBMAX; i++) {
                    Duplicate *b = widthduplicates[ i ];
                    if (b == nullptr)
                        continue;
                    if (b->mode == -1)
                        continue;


                    if (b->value != width)
                        continue;

                    if (b->threadid == getThreadId())
                        continue;

                    //cout << "--------in --duplicatefound: t: " << b->threadid << " b " << b->blobidx << " w " << width << endl;

                    duplicatefound = true;

                    if (lock()) {
                        b->blobidx = blobidx;
                        b->mode = 0;
                        b->threadid = getThreadId();
                        b->value = width;
                        b->labelid = labelid;
                        unlock();
                    }


                }

                //if (!duplicatefound) 
                {
                    if (blobwidth >= maxBlobsize) {
                        howmany = 3;
                    } else if (blobwidth >= midBlobsize) {
                        howmany = 2;
                    } else if (blobwidth >= minBlobsize) {
                        howmany = 1;
                    } else {

                        displayInfo("Object width to small W:" + ofToString(width));
                        //cout << "Object width to small W:" << ofToString(blobwidth) << endl;
                    }

                    if (howmany > 0) {
                        displayInfo("--->IN " + ofToString(howmany) + " w:" + ofToString(blobwidth) + " " + ofToString(yCenterGobal) + " /" + ofToString(yCenter));
                        ofNotifyEvent(this->onPeapleIn, howmany, this);
                        ofNotifyEvent(this->onPepaleChanged, labelid, this);



                        Duplicate *dd = widthduplicates[ getThreadId() ];
                        if (!duplicatefound) {

                            dd->blobidx = blobidx;
                            dd->mode = 0;
                            dd->threadid = getThreadId();
                            dd->value = width;
                            dd->labelid = labelid;
                            // cout << "----------created t: " << dd->threadid << " b "<<  dd->blobidx <<" w " << width << endl;
                        } else {
                            // cout << "---------in -duplicatefound: t: " << dd->threadid << " b " << dd->blobidx << " w " << width << endl;
                        }


                    }

                }
            }
        } else if (preset == ThreadedCounter::Accurate) {
            //            if ((int) (currentCenterPos.y + half) < (int) threshoLineTop.y) {
            //                currentState = 0;
            //
            //            }
            //M1
            if (currentState[0] != 7 && (int) (currentCenterPos.y - offset) > (int) threshoLineBottom.y) {
                currentState[0] = 1;
            }
            //M2
            //            if (currentState == 1 && (int) (currentCenterPos.y - half) < (int) threshoLineBottom.y &&
            //                    (int) (currentCenterPos.y - half) > (int) threshoLineTop.y) {
            //                currentState = 2;
            //            }
            if (currentState[0] == 1 && (int) (currentCenterPos.y - offset) < (int) threshoLineCenter.y) {
                currentState[0] = 2;
            }

            //M3
            if (currentState[0] == 2 && (int) (currentCenterPos.y - offset) < (int) threshoLineTop.y &&
                    (int) (currentCenterPos.y + offset) > (int) threshoLineBottom.y) {
                currentState[0] = 3;
            }

            //M4
            if (currentState[0] == 3 && (int) (currentCenterPos.y - offset) < (int) threshoLineTop.y &&
                    (int) (currentCenterPos.y + offset) < (int) threshoLineBottom.y) {
                currentState[0] = 4;
            }

            //M5
            if (currentState[0] == 4 && (int) (currentCenterPos.y + offset) < (int) threshoLineTop.y) {
                currentState[0] = 0;
                notify(width, 1);
            }
        }
        /// BUENA IN 



        ofSleepMillis(THREADEDCOUNTER_DELAYTIME);

#ifndef THREADEDCOUNTER_LIFEFOREVER
        if (time(NULL) > endwait) {
            break;
        }
#endif        

    }
    displayInfo("done");
    this->done = true;
    previousMillis = ofGetElapsedTimeMillis();
}
//--------------------------------------------------------------
// Thread out Counter Method
int tindex = 0;

void ThreadedCounterOut::threadedFunction()
{
    std::fill_n(currentState, BLOBMAX, 0);
    displayInfo("Start Running OUT Thread!");

    while (isThreadRunning()) {

        int blobwidth = (int) rectangle.width;
        int offset = (int) rectangle.height / 2;
        int yCenter = (int) rectangle.y + offset;
        int yTop = (int) rectangle.y;
        int yBottom = rectangle.y + (int) rectangle.height;

        //  cout << yCenter << endl;
        /*
              int blobwidth = (int) rectangle.getWidth();
              int offset = (int) rectangle.getHeight() / 2;
              //int yCenter = (int) rectangle.y + offset;
         *  int yTop = (int) yCenter - offset;
         */

        ///////////////////////////////////////

/*
        string filename = ofToString(nr) + "_" + ofToString(direction) + ".jpg";

        if (img.load(filename)) {

            int index = nr; //nr

            ofImage image;
            ofxCv::toOf(this->outputimage, image);

            if (yCenter > threshoLineCenter.y) {
                currentState[index] = 8;
            }

            contourFinder.findContours(image);
            if (contourFinder.size() > 0) {

                //cout << "XXXXX " << (int) contourFinder.size() << endl;

                int howmany = 1;
                if (howmany > 0) {
                    displayInfo("--->OUT " + ofToString(howmany) + " w:" + ofToString(blobwidth) + " " + ofToString(yCenterGobal) + " /" + ofToString(yCenter));
                    ofNotifyEvent(this->onPeapleOut, howmany, this);
                    ofNotifyEvent(this->onPepaleChanged, labelid, this);


                }
                if (lock()) {
                    ofFile file;
                    file.removeFile(filename);
                    unlock();
                }

                break;
            }
        }





        // give CPU time...
        ofSleepMillis(THREADEDCOUNTER_DELAYTIME);
        continue;
        /////////////////////////////////////
*/
        if (preset == ThreadedCounter::Sensitive) {

            // out
            int index = blobidx; //nr


            //if (lock())
            {

                if (yCenter < threshoLineTop.y && yCenter > 0) {
                    currentState[index] = 7;
                    //  currentState = states[nr] = 2;
                    // cout << blobidx << " " << currentState[blobidx] << " " << yCenter << " " << threshoLineTop.y << endl;

                }
                if (currentState[index] == 7 && yCenter > threshoLineTop.y) {
                    currentState[index] = 8;
                    //  currentState = states[nr] = 2;
                    //  cout << index << " " << currentState[blobidx] << " " << yCenter << " " << threshoLineTop.y << endl;

                }
                if (currentState[index] == 8 && yCenter > threshoLineCenter.y) {
                    currentState[index] = 9;
                }

                if (currentState[index] == 9 && yCenter > threshoLineBottom.y) {
                    currentState[index] = 10;
                    // cout << "serach" << endl;
                    int howmany = 0;




                    //if (!duplicatefound)
                    {
                        if (blobwidth >= maxBlobsize) {
                            howmany = 3;
                        } else if (blobwidth >= midBlobsize) {
                            howmany = 2;
                        } else if (blobwidth >= minBlobsize) {
                            howmany = 1;
                        } else {

                            displayInfo("Object width to small W:" + ofToString(blobwidth) + " /" + ofToString(minBlobsize));
                            //cout << "Object width to small W:" << ofToString(blobwidth) << endl;
                        }

                        if (howmany > 0) {
                            displayInfo("--->OUT " + ofToString(howmany) + " w:" + ofToString(blobwidth) + " " + ofToString(yCenterGobal) + " /" + ofToString(yCenter));
                            ofNotifyEvent(this->onPeapleOut, howmany, this);
                            ofNotifyEvent(this->onPepaleChanged, labelid, this);



                        }


                    }

                }
                //unlock();
            }
        } else if (preset == ThreadedCounter::Accurate) {


            //M1
            if (currentState[0] != 1 && (int) (currentCenterPos.y + offset) < (int) threshoLineTop.y) {
                currentState[0] = 1;

            }

            //M5
            if (currentState[0] == 1 && (int) (currentCenterPos.y + offset) > (int) threshoLineTop.y &&
                    (int) (currentCenterPos.y - offset) < (int) threshoLineTop.y) {
                currentState[0] = 5;
                // cout << currentState << endl;

            }

            //M6
            if (currentState[0] == 5 && (int) (currentCenterPos.y - offset) < (int) threshoLineTop.y &&
                    (int) (currentCenterPos.y + offset) > (int) threshoLineBottom.y) {
                currentState[0] = 6;
                // cout << currentState << endl;

            }

            //M7
            if (currentState[0] == 6 && (int) (currentCenterPos.y - offset) < (int) threshoLineBottom.y &&
                    (int) (currentCenterPos.y + offset) > (int) threshoLineBottom.y) {
                currentState[0] = 7;
                // cout << currentState << endl;


            }

            //M1
            if (currentState[0] == 7 && (int) (currentCenterPos.y - offset) > (int) threshoLineBottom.y) {
                currentState[0] = 0;
                notify(width, 0);
            }
        }

        // give CPU time...
        ofSleepMillis(THREADEDCOUNTER_DELAYTIME);

    }

    displayInfo("done");
    this->done = true;
    previousMillis = ofGetElapsedTimeMillis();

}
//--------------------------------------------------------------

void ThreadedCounter::displayInfo(string text)
{
    printf("Thread:%2d, BlobIdx:%d, Labelid:%5d Mode:%d %s\n",
            getThreadId(),
            blobidx,
            labelid,
            classid,
            text.c_str());

    //cout << "Thread:" + ofToString(getThreadId()) + " blobidx:" +
    //        ofToString(label) + "/"+ofToString(labelid)  +" classid:" + ofToString(index) + " " + text << endl;
}
//--------------------------------------------------------------

void ThreadedCounter::notify(int width, int mode)
{
    int howmany = 0;

    if (width >= maxBlobsize) {
        howmany = 3;
    } else if (width >= midBlobsize) {
        howmany = 2;
    } else if (width >= minBlobsize) {
        howmany = 1;
    } else {

        displayInfo("Object width to small W:" + ofToString(width));
    }


    if (howmany > 0) {
        string msg = mode == 1 ? " -> IN " : " -> OUT ";
        displayInfo(msg + ofToString(howmany) + " W:" + ofToString(width));
        ofNotifyEvent(mode == 1 ? onPeapleIn : onPeapleOut, howmany, this);
    }


}