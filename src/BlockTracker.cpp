#include "BlobTracker.h"
#include "ofApp.h"
#include "ofBaseTypes.h"

BlobTracker::BlobTracker() {

    // thresholds lines 
    threshoLineCenter.a = ofPoint(0, CAMERAHEIGHT / 2);
    threshoLineCenter.b = ofPoint(CAMERAWIDTH, CAMERAHEIGHT / 2);

    threshoLineTop = threshoLineCenter;
    threshoLineTop.a.y -= THRESHOLINESOFFSET;
    threshoLineTop.b.y -= THRESHOLINESOFFSET;

    threshoLineBottom = threshoLineCenter;
    threshoLineBottom.a.y += THRESHOLINESOFFSET;
    threshoLineBottom.b.y += THRESHOLINESOFFSET;


    swapset = false;
    // std::fill_n(posItems, BLOBMAX, 0);
    frameCount = 0;
    createThreads();

}
// RECFACT   by Ref & not by copy like here

ofRectangle BlobTracker::getNext() {
    ofRectangle result(0, 0, 0, 0);

    if (blobsPositions.size() > 0) {
        result = blobsPositions.back();

        blobsPositions.erase(blobsPositions.begin() + blobsPositions.size() - 1);


    }

    return result;
}
//---------------------------------------------------------

void BlobTracker::setConfig(const Configuration& config) {
    this->config = config;
}

void BlobTracker::eventHandlerPeapleIn(int &howmany) {

    if (config.getSettings().multithread == 0) {
        if (howmany > 1)
            howmany = 1;
    }

    ofNotifyEvent(this->peopleIn, howmany, this);

}

void BlobTracker::eventHandlerPeapleOut(int &howmany) {

    if (config.getSettings().multithread == 0) {
        if (howmany > 1)
            howmany = 1;
    }

    ofNotifyEvent(this->peopleOut, howmany, this);
}

void BlobTracker::eventHandlerPeapleChange(int &index) {

}

int BlobTracker::countRunningThreads() {
    int count = 0;
    for (int i = 0; i < BLOBMAX; i++) {
        if (inCounters[i]->isThreadRunning()) {
            count++;
        }
        if (outCounters[i]->isThreadRunning()) {
            count++;
        }
    }

    return count;

}

int BlobTracker::getDirection(int index) {

    return direction[index];
}

void BlobTracker::createThreads() {

    for (int i = 0; i < BLOBMAX; i++) {
        outCounters[i] = nullptr;
        inCounters[i] = nullptr;
    }

    //out
    for (int i = 0; i < BLOBMAX; i++) {
        outCounters[i] = new ThreadedCounterOut(
                this->threshoLineTop.a,
                this->threshoLineCenter.a,
                this->threshoLineBottom.a,
                i, 0, config.getSettings().preset == 1 ? ThreadedCounter::Accurate : ThreadedCounter::Sensitive);
        ofAddListener(outCounters[i]->onPeapleOut, this, &BlobTracker::eventHandlerPeapleOut);
        ofAddListener(outCounters[i]->onPepaleChanged, this, &BlobTracker::eventHandlerPeapleChange);
        outCounters[i]->classid = 0;
    }


    // IN 
    for (int i = 0; i < BLOBMAX; i++) {
        inCounters[i] = new ThreadedCounterIn(
                this->threshoLineTop.a,
                this->threshoLineCenter.a,
                this->threshoLineBottom.a,
                i, 1, config.getSettings().preset == 1 ? ThreadedCounter::Accurate : ThreadedCounter::Sensitive);
        ofAddListener(inCounters[i]->onPeapleIn, this, &BlobTracker::eventHandlerPeapleIn);
        ofAddListener(inCounters[i]->onPepaleChanged, this, &BlobTracker::eventHandlerPeapleChange);

        inCounters[i]->blobidx = 0;
        inCounters[i]->labelid = 0;
        inCounters[i]->classid = 1;
    }



}
int previousPos0 = 0;
int currentdirection0 = 0;
int currentdirection1 = 0;
int counterdetectedDir = 0;

void BlobTracker::trackSingleThread(const vector<ofCvBlob>& bloblist, int minblobsize, int midblobsize, int maxblobsize) {
    this->minblobsize = minblobsize;
    this->midblobsize = midblobsize;
    this->maxblobsize = maxblobsize;

    history.push_back(this->blobs);
    if (history.size() > 4) {
        history.erase(history.begin());
    }
    this->blobs.clear();

    //---------------------------------------------
    // Load new blobs
    for (int i = 0; i < (int) bloblist.size(); i++) {
        this->blobs.push_back(TrackedBlob(bloblist[i]));
    }
    vector<TrackedBlob> *prev = &history[ history.size() - 1 ];
    int cursize = this->blobs.size();
    int prevsize = (*prev).size();


    // Start values filtering and compensation section
    //
    // Initialize every time no have blobs
    if ((int) this->blobs.size() == 0) {

        // initialize arrays 
        std::fill_n(swapPreviousPos, BLOBMAX, 0);
        std::fill_n(previousPos, BLOBMAX, 0);
        std::fill_n(direction, BLOBMAX, 0);
        std::fill_n(currentState, BLOBMAX, 0);

        swapset = false;
        swapidx = 0;

        // cout << "######" <<endl;
    }





    // resets all running threads
    for (int i = threadedCounters.size() - 1; i >= 0; i--) {
        ThreadedCounter* counter = threadedCounters.at(i);
        if (counter->done)
            continue;

        if (!counter->isThreadRunning())
            continue;

        ofRectangle rect(0, 0, 0, 0);

        counter->update(rect,
                minblobsize,
                midblobsize,
                maxblobsize,
                i, i, -1, 0);
    }



    // Navigate over the blobs and start compensation
    int offset = 0;

    int yCenter = 0;


    int substration = 0;
    int nr = 0;
    int labelid = 0;
    int blobwidth;
    
    if( this->blobs.size() < 1  )
        return ;
    

    for (int i = 0; i < 1/*(int) this->blobs.size()*/; i++) {

        labelid = i;
        TrackedBlob b = this->blobs[i];
        ofRectangle rect(b.box);
        blobwidth = (int) rect.width;
        offset = (int) rect.height / 2;
        yCenter = rect.y + offset;
        string remove0 = "";
        string remove1 = "";
        string remove2 = "";




        // Here we check the yCenter value diference beetwin current and previous.
        // Is the diference greater or equal 100 then we assume that a blob has been cut in tow blobs.
        // Because before we just have one Blob and now tow. We need to swap the yCenter value beetwin the tow blobs. 
        if (!swapset) {
            if (swapPreviousPos[i] != 0 && swapPreviousPos[i] != yCenter) {
                substration = (int) (yCenter - swapPreviousPos[i]);
                if (substration >= 100) {
                    cout << "SWAP-" << i << "----------------" << yCenter << " " << swapPreviousPos[i] << endl;
                    swapset = true;
                    swapidx = i;
                }
            }
        }

        nr = i;
        //
        //        nr = i;
        //        if (nr == 0)
        //            cout << i << " " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove0 << endl;
        //        if (nr == 1)
        //            cout << i << " ------------ " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove1 << endl;
        //        if (nr == 2)
        //            cout << i << " ------------------------------ " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove2 << endl;

        if (swapset) {
            nr = (i == swapidx) ? swapidx + 1 : swapidx; // swap!
        }

        // Determine vector direction
        if (/*direction[nr] == 0 && */previousPos[nr] != 0 &&
                previousPos[nr] != yCenter && abs(previousPos[nr] - yCenter) > 2) {


            ofVec2f v1(0, yCenter);
            ofVec2f v2(0, previousPos[nr]);
            ofVec2f d = v2 - v1;
            ofVec2f unitv = d.normalize();

            b.direction = direction[nr] = (int) unitv.y; // 1 = Down to Up -1 = Up to Down

            //            
            //            if( yCenter < 125 ) {
            //                b.direction = direction[nr] = (int) 1; // 1 = Down to Up -1 = Up to Down
            //            }
            //            
            //            if( yCenter > 130 ) {
            //                b.direction = direction[nr] = (int) -1; // 1 = Down to Up -1 = Up to Down
            //            }
            // cout << " direction set +------>" << nr << " " << direction[nr] << " Values " << yCenter << " " << previousPos[nr] << endl;
        }


        if (previousPos[nr] > 0 && direction[nr] != 0) {
            // if (yCenter == previousPos[nr])
            //     continue;
            //
            //currentValue[nr]
            if (direction[nr] == -1 && yCenter < previousPos[nr]) { // Down
                //  cout << "---->d" << " " << yCenter << " - " << previousPos[nr] << endl;
                continue;
                //remove0 = "*";

            }

            if (direction[nr] == 1 && yCenter >= previousPos[nr]) { // UP
                //cout << "------------------------>u "<< nr << " " << yCenter << " - " << previousPos[nr] << endl;
                //if (previousPos[nr] != yCenter) {
                // previousPos[nr] = yCenter;
                //}
                //cout << i << " ------------------------------ " << nr << " " << yCenter << " ppos:" << previousPos[nr] << " d:" << direction[nr] << " " << remove2 << endl;

                continue;
                //yCenter >= previousPos[
                //remove2 = "*";


            }
        }




        //        if (direction[nr] != 0) {
        //            if (nr == 0)
        //                cout << i << " " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove0 << endl;
        //            if (nr == 1)
        //                cout << i << " ------------ " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove1 << endl;
        //            if (nr == 2)
        //                cout << i << " ------------------------------ " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove2 << endl;
        //        } else {
        //            cout << i << " " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove0 << endl;
        //
        //        }


        if (rect.height > BLOBMAXHEIGHT)
            rect.height = BLOBMAXHEIGHT;



        if (swapPreviousPos[i] != yCenter) {
            swapPreviousPos[i] = yCenter;
        }

        if (previousPos[nr] != yCenter) {
            previousPos[nr] = yCenter;
        }

        trackermutex.lock();
        // in
        if (nr < BLOBMAX && direction[nr] == 1) {

            ThreadedCounter* c = inCounters[ 0 ];
            if (c->classid == 1) {

                if (c->isThreadRunning() == false) {
                    c->startThread();
                }

                c->update(rect,
                        minblobsize,
                        midblobsize,
                        maxblobsize,
                        blobwidth,
                        c->blobidx,
                        labelid, yCenter);

                // cout << "update" << endl;
                // 
            }
        }
        //out        
        if (nr < BLOBMAX && direction[nr] == -1) {

            ThreadedCounter* c = outCounters[ 0 ];
            if (c->classid == 0) {

                if (c->isThreadRunning() == false) {
                    c->startThread();
                }

                c->update(rect,
                        minblobsize,
                        midblobsize,
                        maxblobsize,
                        blobwidth,
                        c->blobidx,
                        labelid, yCenter);

                // 
            }
        }

        trackermutex.unlock();

    }
}

void BlobTracker::trackStable(const vector<ofCvBlob>& bloblist, int minblobsize, int midblobsize, int maxblobsize) {

    this->minblobsize = minblobsize;
    this->midblobsize = midblobsize;
    this->maxblobsize = maxblobsize;

    history.push_back(this->blobs);
    if (history.size() > 4) {
        history.erase(history.begin());
    }
    this->blobs.clear();

    //---------------------------------------------
    // Load new blobs
    for (int i = 0; i < (int) bloblist.size(); i++) {
        this->blobs.push_back(TrackedBlob(bloblist[i]));
    }
    vector<TrackedBlob> *prev = &history[ history.size() - 1 ];
    int cursize = this->blobs.size();
    int prevsize = (*prev).size();

    //  

    // now figure out the 'error' (distance) to all blobs in the previous
    // frame. We are optimizing for the least change in distance.
    // While this works really well we could also optimize for lowest
    // deviation from predicted position, change in size etc...
    for (int i = 0; i < cursize; i++) {

        this->blobs[i].error.clear();
        this->blobs[i].closest.clear();

        for (int j = 0; j < prevsize; j++) {
            //calc error - distance to blob in prev frame
            float deviationX = this->blobs[i].center.x - (*prev)[j].center.x;
            float deviationY = this->blobs[i].center.y - (*prev)[j].center.y;
            float error = (float) sqrt(deviationX * deviationX
                    + deviationY * deviationY);

            this->blobs[i].error.push_back(error);
            this->blobs[i].closest.push_back(j);


        }
    }


    // Start values filtering and compensation section
    //
    // Initialize every time no have blobs
    if ((int) this->blobs.size() == 0) {

        // initialize arrays 
        std::fill_n(swapPreviousPos, BLOBMAX, 0);
        std::fill_n(previousPos, BLOBMAX, 0);
        std::fill_n(direction, BLOBMAX, 0);
        std::fill_n(currentState, BLOBMAX, 0);

        swapset = false;
        swapidx = 0;

        // cout << "######" <<endl;
    }





    // resets all running threads
    for (int i = threadedCounters.size() - 1; i >= 0; i--) {
        ThreadedCounter* counter = threadedCounters.at(i);
        if (counter->done)
            continue;

        if (!counter->isThreadRunning())
            continue;

        ofRectangle rect(0, 0, 0, 0);

        counter->update(rect,
                minblobsize,
                midblobsize,
                maxblobsize,
                i, i, -1, 0);
    }



    // Navigate over the blobs and start compensation
    int offset = 0;

    int yCenter = 0;


    int substration = 0;
    int nr = 0;
    int labelid = 0;
    int blobwidth;



    for (int i = 0; i < (int) this->blobs.size(); i++) {

        labelid = i;
        TrackedBlob b = this->blobs[i];
        ofRectangle rect(b.box);
        blobwidth = (int) rect.width;
        offset = (int) rect.height / 2;
        yCenter = rect.y + offset;
        string remove0 = "";
        string remove1 = "";
        string remove2 = "";




        // Here we check the yCenter value diference beetwin current and previous.
        // Is the diference greater or equal 100 then we assume that a blob has been cut in tow blobs.
        // Because before we just have one Blob and now tow. We need to swap the yCenter value beetwin the tow blobs. 
        if (!swapset) {
            if (swapPreviousPos[i] != 0 && swapPreviousPos[i] != yCenter) {
                substration = (int) (yCenter - swapPreviousPos[i]);
                if (substration >= 100) {
                    cout << "SWAP-" << i << "----------------" << yCenter << " " << swapPreviousPos[i] << endl;
                    swapset = true;
                    swapidx = i;
                }
            }
        }

        nr = i;
        //
        //        nr = i;
        //        if (nr == 0)
        //            cout << i << " " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove0 << endl;
        //        if (nr == 1)
        //            cout << i << " ------------ " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove1 << endl;
        //        if (nr == 2)
        //            cout << i << " ------------------------------ " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove2 << endl;

        if (swapset) {
            nr = (i == swapidx) ? swapidx + 1 : swapidx; // swap!
        }

        // Determine vector direction
        if (/*direction[nr] == 0 && */previousPos[nr] != 0 &&
                previousPos[nr] != yCenter && abs(previousPos[nr] - yCenter) > 2) {


            ofVec2f v1(0, yCenter);
            ofVec2f v2(0, previousPos[nr]);
            ofVec2f d = v2 - v1;
            ofVec2f unitv = d.normalize();

            b.direction = direction[nr] = (int) unitv.y; // 1 = Down to Up -1 = Up to Down

            //            
            //            if( yCenter < 125 ) {
            //                b.direction = direction[nr] = (int) 1; // 1 = Down to Up -1 = Up to Down
            //            }
            //            
            //            if( yCenter > 130 ) {
            //                b.direction = direction[nr] = (int) -1; // 1 = Down to Up -1 = Up to Down
            //            }
            // cout << " direction set +------>" << nr << " " << direction[nr] << " Values " << yCenter << " " << previousPos[nr] << endl;
        }


        if (previousPos[nr] > 0 && direction[nr] != 0) {
            // if (yCenter == previousPos[nr])
            //     continue;
            //
            //currentValue[nr]
            if (direction[nr] == -1 && yCenter < previousPos[nr]) { // Down
                //  cout << "---->d" << " " << yCenter << " - " << previousPos[nr] << endl;
                continue;
                //remove0 = "*";

            }

            if (direction[nr] == 1 && yCenter >= previousPos[nr]) { // UP
                //cout << "------------------------>u "<< nr << " " << yCenter << " - " << previousPos[nr] << endl;
                //if (previousPos[nr] != yCenter) {
                // previousPos[nr] = yCenter;
                //}
                //cout << i << " ------------------------------ " << nr << " " << yCenter << " ppos:" << previousPos[nr] << " d:" << direction[nr] << " " << remove2 << endl;

                continue;
                //yCenter >= previousPos[
                //remove2 = "*";


            }
        }




        //        if (direction[nr] != 0) {
        //            if (nr == 0)
        //                cout << i << " " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove0 << endl;
        //            if (nr == 1)
        //                cout << i << " ------------ " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove1 << endl;
        //            if (nr == 2)
        //                cout << i << " ------------------------------ " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove2 << endl;
        //        } else {
        //            cout << i << " " << nr << " " << yCenter << " d:" << direction[nr] << " " << remove0 << endl;
        //
        //        }


        if (rect.height > BLOBMAXHEIGHT)
            rect.height = BLOBMAXHEIGHT;



        if (swapPreviousPos[i] != yCenter) {
            swapPreviousPos[i] = yCenter;
        }

        if (previousPos[nr] != yCenter) {
            previousPos[nr] = yCenter;
        }

        trackermutex.lock();
        // in
        if (nr < BLOBMAX && direction[nr] == 1) {

            ThreadedCounter* c = inCounters[ nr ];
            if (c->classid == 1) {

                if (c->isThreadRunning() == false) {
                    c->startThread();
                }

                c->update(rect,
                        minblobsize,
                        midblobsize,
                        maxblobsize,
                        blobwidth,
                        c->blobidx,
                        labelid, yCenter);

                // cout << "update" << endl;
                // 
            }
        }
        //out        
        if (nr < BLOBMAX && direction[nr] == -1) {

            ThreadedCounter* c = outCounters[ nr ];
            if (c->classid == 0) {

                if (c->isThreadRunning() == false) {
                    c->startThread();
                }

                c->update(rect,
                        minblobsize,
                        midblobsize,
                        maxblobsize,
                        blobwidth,
                        c->blobidx,
                        labelid, yCenter);

                // 
            }
        }

        trackermutex.unlock();

    }
}

void BlobTracker::populateThreads(TrackPosItem item) {


    if (item.blobidx < BLOBMAX && item.direction == 1) {

        ThreadedCounter* c = outCounters[ item.blobidx ];
        if (c->classid == 0) {

            if (c->isThreadRunning() == false) {
                c->startThread();
            }

            if (item.blobidx == 0) {
                string dir = item.getDirectionToString(item.direction);
                printf("Row:%d %d %4d %2d %s\n", 0, item.blobidx, item.value, item.direction, dir.c_str());
            }

            if (item.blobidx == 1) {
                string dir = item.getDirectionToString(item.direction);
                printf("Row:%d -------------------%d %4d %2d %s\n", 0, item.blobidx, item.value, item.direction, dir.c_str());
            }



            c->update(item.rectangle,
                    this->minblobsize,
                    this->midblobsize,
                    this->maxblobsize,
                    item.rectangle.width,
                    item.blobidx,
                    item.blobidx, item.value);

            // 
        }
    }

}

void BlobTracker::populateThreads(int nr, int direction, int blobidx, int labelid, ofRectangle rect, int value) {

    // in
    if (nr < BLOBMAX && direction == -1) {

        ThreadedCounter* c = inCounters[ nr ];
        if (c->classid == 1) {

            if (c->isThreadRunning() == false) {
                c->startThread();
            }
            c->update(rect,
                    32, //this->minblobsize,
                    80, //this->midblobsize,
                    190, //this->maxblobsize,
                    rect.width,
                    blobidx,
                    labelid, value);
        }
    } else

        //OUT 
        if (nr < BLOBMAX && direction == 1) {

        ThreadedCounter* c = outCounters[ nr ];
        if (c->classid == 0) {

            if (c->isThreadRunning() == false) {
                c->startThread();
            }



            c->update(rect,
                    32, //this->minblobsize,
                    88, //this->midblobsize,
                    180, //this->maxblobsize,
                    rect.width,
                    blobidx,
                    labelid, value);

            // 
        }
    }
}

void BlobTracker::rememberValues(int nr, int yCenter) {
    if (previousPos[nr] != yCenter) {
        previousPos[nr] = yCenter;
    }
    if (direction[nr] != previousDirection[nr]) {
        previousDirection[nr] = direction[nr];
    }

    if (previousDirection[nr] != previousDirection2[nr]) {
        previousDirection2[nr] = previousDirection[nr];
    }

}