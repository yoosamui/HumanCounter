/*
 *  created by Juan R. Gonzalez & Juan Alvarez 2016
 */


#include "stdio.h"
#include <stdlib.h> 
#include "ofApp.h"
#include "math.h"
#include "ofRectangle.h"
#include "ofPolyline.h"
#include "ofGraphics.h"
#include "ofAppRunner.h"


//--------------------------------------------------------------

void ofApp::setup()
{

    // glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    //glfwWindowHint(GLFW_FOCUSED, GL_TRUE);
    // glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    previousYCenter = 0;

    ofAppGLFWWindow* win;
    win = dynamic_cast<ofAppGLFWWindow *> (ofGetWindowPtr());

    ofSetVerticalSync(true);

    ofSetFrameRate(FRAMERATE);

    // center window   
    int centerx = ofGetScreenWidth() / 2 - (ofGetWindowWidth() / 2);
    int centery = ofGetScreenHeight() / 2 - (ofGetWindowHeight() / 2);
    
    #ifndef USE_PI_CAMERA
		ofSetWindowPosition(centerx, centery);
    #endif
    
    config.load(CONFIGURATION_FILENAME);
    font.load(OF_TTF_SANS, 64, true, true);
    fontsmall.load(OF_TTF_SANS, 8, true, true);
    ofEnableAlphaBlending();

    string threadstitle = config.getSettings().multithread == 0 ? " (Single Thread)" : " (Multithread)";
    string awindowtitle;
    awindowtitle.append(WINDOWTITLE);
    awindowtitle.append(threadstitle);
    
    #ifndef USE_PI_CAMERA
    ofSetWindowTitle(awindowtitle);
	#endif
/*

    isVideoMode = video.load(config.getSettings().camvideofile);

    if (isVideoMode) {
        video.setLoopState(OF_LOOP_NORMAL);
        video.setSpeed(VIDEOPLAYSPEED);
        video.play();

    } else {
        // setup camera (w,h,color = true,gray = false);
        cam.setup(CAMERAWIDTH, CAMERAHEIGHT, true);
    }

*/
	isVideoMode = false;
	cam.setup(CAMERAWIDTH, CAMERAHEIGHT, true);

    camRotationChanged(config.getSettings().camrotation);
    camContrastChanged(config.getSettings().camcontrast);
    camBrightnessChanged(config.getSettings().cambrightness);

    // thresholds lines 
    threshoLineCenter.a = ofPoint(0, CAMERAHEIGHT / 2);
    threshoLineCenter.b = ofPoint(CAMERAWIDTH, CAMERAHEIGHT / 2);

    threshoLineTop = threshoLineCenter;
    threshoLineTop.a.y -= THRESHOLINESOFFSET;
    threshoLineTop.b.y -= THRESHOLINESOFFSET;

    threshoLineBottom = threshoLineCenter;
    threshoLineBottom.a.y += THRESHOLINESOFFSET;
    threshoLineBottom.b.y += THRESHOLINESOFFSET;

    resetCounters();

    soundIn.load(SOUND_IN_FILENAME);
    soundOut.load(SOUND_OUT_FILENAME);

    // Setup mask 
    makeMask();

    // Background subtraction 
    // Gaussian Mixture-based Background/Foreground Segmentation Algorithm

    // open cv original
    //BackgroundSubtractorMOG pBSMOG = 
    /* http://stackoverflow.com/questions/21873757/opencv-c-how-to-slow-down-background-adaptation-of-backgroundsubtractormog
    BackgroundSubtractorMOG(
            int history=200,                    // history – Length of the history.
            int nmixtures=5,                    // nmixtures – Number of Gaussian mixtures.
            double backgroundRatio=0.7,         // backgroundRatio – Background ratio.
            double noiseSigma=0);               // noiseSigma – Noise strength (standard deviation of the brightness or each color channel). 0 means some automatic value.
     * 
     * Increasing the history value will slow down the adaptation rate. 
     * 
     * Ptr <BackgroundSubtractorMOG2> createBackgroundSubtractorMOG2(int
     * history=500, double varThreshold=16, bool detectShadows=true )
     * !!!!!!!This is much faster than the previous one 
     * !!!!!!!and it can eleminate detecting shadows too.
     * 
     */

    this->mog2 = new BackgroundSubtractorMOG2(
            config.getSettings().moghistory, // Increasing the history value will slow down the adaptation rate.
            config.getSettings().mogthreshold,
            false); // detecting shadows.


    // this->mog2->get
    this->initCounturFinder();

    previousMillis = 0;
    mouseDelay = MOUSE_MAXDELAY;

    setupHttp();

    activateFinder = false;
    isOneFrameSet = false;
    activateCropTimer = false;

    //if (img.load("images/background.jpg")) {
    //    backgroundimage.setFromPixels(img.getPixels());
    // }


    showLabels = true;

    kalman.init(1 / 10000., 1 / 10.); // inverse of (smoothness, rapidness)
    line.setMode(OF_PRIMITIVE_LINE_STRIP);
    predicted.setMode(OF_PRIMITIVE_LINE_STRIP);
    estimated.setMode(OF_PRIMITIVE_LINE_STRIP);
    speed = 0.f;

    entryCount = 0;
    exitCount = 0;
    ouputImage.allocate(CAMERAWIDTH, CAMERAHEIGHT, ofImageType::OF_IMAGE_GRAYSCALE);
    grayhapedoutput.allocate(CAMERAWIDTH, CAMERAHEIGHT, ofImageType::OF_IMAGE_GRAYSCALE);
    grayScaleImage.allocate(CAMERAWIDTH, CAMERAHEIGHT);


    blobTracker.setConfig(config);
    ofAddListener(blobTracker.peopleIn, this, &ofApp::eventHandlerPeapleIn);
    ofAddListener(blobTracker.peopleOut, this, &ofApp::eventHandlerPeapleOut);
    createThreads();



	m_gpio4 = new GPIOClass("4"); //create new GPIO

    cout << "Setup success!" << endl;
}
//--------------------------------------------------------------

void ofApp::exit()
{
	delete m_gpio4;
	m_gpio4 = NULL;
    //     if( isVideoMode )
    //         video.close();
    //     cam.close();

    //    ofRemoveListener(this->peopleIn, this, &ofApp::eventHandlerPeopleGoingIn);
    //    ofRemoveListener(this->peopleOut, this, &ofApp::eventHandlerPeopleGoingOut);

    // TODO:: stop and free Threads

}
//--------------------------------------------------------------

void ofApp::setupHttp()
{
    // Setup HTTP POST Unit
    ofAddListener(httpUtils.newResponseEvent, this, &ofApp::newResponse);
    httpUtils.start();
}
//--------------------------------------------------------------

void ofApp::newResponse(ofxHttpResponse &response)
{
    string responseStr = ofToString(response.status) + ":" + (string) response.responseBody;
    // cout << responseStr << endl;
}
//--------------------------------------------------------------

/**
 *  initCounturing 
 */
void ofApp::initCounturFinder()
{
    contourFinder.setMinAreaRadius(config.getSettings().contourminarearadius);
    contourFinder.setMaxAreaRadius(config.getSettings().contourmaxarearadius);
    contourFinder.setThreshold(config.getSettings().contourthreshold);
    contourFinder.setFindHoles(false);
    // contourFinder.setSimplify(true);
    // contourFinder.setSortBySize(true);

    // https://github.com/kylemcdonald/ofxCv/blob/master/libs/ofxCv/include/ofxCv/Tracker.h
    // persistence determines how many frames an
    // object can last without being seen until the tracker forgets about it
    // wait for half a frame before forgetting something
    contourFinder.getTracker().setPersistence(config.getSettings().trackerpersistance);
    // maximumDistance determines how far an object can move until the tracker
    // considers it a new object.
    // an object can move up to n pixels per frame
    contourFinder.getTracker().setMaximumDistance(config.getSettings().trackermaxDistance);



}
//--------------------------------------------------------------

/**
 *  Update 
 */
void ofApp::update()
{


#ifdef USE_PI_CAMERA

    if (isVideoMode) {
        video.update();
        frame = toCv(video);
    } else {
        frame = cam.grab();
    }

#else

    if (isVideoMode) {
        video.update(); // get all the new frames
        frame = toCv(video);
    } else {
        cam.update();
        frame = toCv(cam);
    }

#endif


    //if (video.isFrameNew()) {
    if (!frame.empty()) {

        //if (!video.isFrameNew())
        //    cout << "Drop frame " << endl;

        //  if (video.isFrameNew()) {
        convertColor(frame, grayImage, CV_RGB2GRAY);
        Canny(grayImage, cannyedge, 140, 140, 3);
        cannyedge.update();
        grayImage.update();

        //auto lines = HoughLine(edges, 1, math.pi/2, 2, None, 30, 1);

        resize(frame, resizeF, cv::Size(frame.size().width, frame.size().height));
        lightenMat = resizeF + cv::Scalar(config.getSettings().imagelightenAmount,
                config.getSettings().imagelightenAmount, config.getSettings().imagelightenAmount);

        lightenMat.copyTo(maskOutput, mask);
        lightenMat.copyTo(cannyMat, mask);
        cv::Canny(cannyMat, cannymatOutput, 80, 240, 3);


        // Activate the background subtraction
        // The second parameter is called learning rate. 
        // In the code you mentioned bg(frame,fmask,-1); where -1 is your 
        // learning rate. you can set it to 0.0 to 1.0 and default is -1. 
        // When you set it 0, you will get what you want for the objects which 
        // are not part of the frame in the starting of the video. You can call 
        // this kind of object "foreign objects". You will get foreign object covered with white pixels. 
        mog2->operator()(maskOutput, maskmog2, -1.f);

        // Threshold the image
        threshold(maskmog2, output, config.getSettings().imagethreshold);


        //grayScaleImage.setFromPixels(grayImage.getPixels());
        //grayScaleImage.absDiff(backgroundimage);

        // use Blur/dilate after thresholding, which in general removes noise 
        // and stretches white areas,
        blur(output, config.getSettings().imageblur);

        // Dilates the image by using a specific structuring element.
        // The function dilates the source image using the specified structuring element 
        // that determines the shape of a pixel neighborhood over which the maximum is taken:
        // An example using the morphological dilate operation can be found at opencv_source_code/samples/cpp/morphology2.cpp
        dilate(output);

        // Pass through the Contour Finder
        if (ofGetFrameNum() > MAXLERNINGFRAMES) {

            ofxCv::toOf(output, ouputImage);
            ofPixelsRef pixels = ouputImage.getPixels();
            pixels.setImageType(OF_IMAGE_GRAYSCALE);
            //            for (auto& p : pixels) {
            //                if (p < 225 && p > 0) p = 200;
            //            }

            //            // Shape detection
            //
            //            //            lightenMat.copyTo(cannyMat, mask);
            //            //            cv::Canny(cannyMat, cannymatOutput, 80, 240, 3);
            //            //            blur(cannymatOutput, 1);
            //
            //            grayhapedoutput.update();
            //            grayhapedoutput.setFromPixels(ouputImage.getPixels());
            //            ofPixelsRef pixelsshaped = grayhapedoutput.getPixels();
            //            pixels.setImageType(OF_IMAGE_GRAYSCALE);
            //            for (auto& p : pixelsshaped) {
            //                if (p < 225 && p > 0) p = 0;
            //            }
            //            matshapedoutput = toCv(grayhapedoutput.getPixels());
            //            // threshold(matshapedoutput,127,255);
            //            blur(matshapedoutput, 6);
            //            // \\Shape detection

            ouputImage.setFromPixels(pixels);

            contourFinder.findContours(ouputImage);
            // segment blob
            if (contourFinder.size() == 1 &&
                    contourFinder.getBoundingRect(0).width >= config.getSettings().midblobsize) {

                int w = ouputImage.getWidth();
                int h = ouputImage.getHeight();

                for (int y = 0; y < h; y++) {
                    for (int x = 0; x < w; x++) {
                        if (x == w / 2) {

                            (unsigned char&) pixels[x + 1 + w * y] = 0;
                            (unsigned char&) pixels[x + w * y] = 0;
                            (unsigned char&) pixels[x - 1 + w * y] = 0;

                        }
                    }
                }
                contourFinder.findContours(ouputImage);
            }

            blobs.clear();
            for (int blobidx = 0; blobidx < (int) contourFinder.size(); blobidx++) {
                ofRectangle rectangle = toOf(contourFinder.getBoundingRect(blobidx));
                bool found = false;
                for (ofCvBlob b : blobs) {
                    if ((int) rectangle.y == 0 || rectangle.y == b.box.y) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    ofCvBlob b;
                    b.box.y = rectangle.y;
                    b.box.x = rectangle.x;
                    b.box.width = rectangle.width;
                    b.box.height = rectangle.height;
                    //b.box.height = BLOBMAXHEIGHT / 2; //rectangle.height;

                    blobs.push_back(b);
                }
            }

            if (config.getSettings().multithread == 1) {

                // Multitread
                blobTracker.trackStable(blobs,
                        config.getSettings().minblobsize,
                        config.getSettings().midblobsize,
                        config.getSettings().maxblobsize);

            } else {

                // Single Thread
                blobTracker.trackSingleThread(blobs,
                        config.getSettings().minblobsize,
                        config.getSettings().midblobsize,
                        config.getSettings().maxblobsize);

            }
        }




    } else {

        if (ouputImage.isAllocated()) {
            contourFinder.findContours(ouputImage);
        }
        // REFACTOR 

        // tracker.track(blobs);
        //updateCounters(contourFinder.getBoundingRects());
        // tracker.track( )
    }



    updateMouseEvents();
}
//--------------------------------------------------------------

void ofApp::eventHandlerPeapleIn(int &howmany)
{
   // if (isHuman() == 0)
   //     return;
	//m_movement = true;

    if (m_movement) {

        this->personsIn += howmany;
        this->personstotal += howmany;

        httpRequest();

        string msg = " " + ofToString(howmany) + " IN ";
        //   exportOutputImage(msg);
    }
    else
    {
		cout << "NO MOVEMENT DETECTED!!!" << endl;
	}

}
//--------------------------------------------------------------

void ofApp::eventHandlerPeapleOut(int &howmany)
{
    //if (isHuman() == 0)
    //    return;
	//m_movement = true;

    if (m_movement) {
        this->personsOut += howmany;
        this->personstotal -= howmany;

        httpRequest();
        string msg = " " + ofToString(howmany) + " OUT ";
        // exportOutputImage(msg);
    }
    else
    {
		cout << "NO MOVEMENT DETECTED!!!" << endl;
	}
  
}

void ofApp::httpRequest()
{

    try {
        ofxHttpForm form;
        form.action = "http://324663.de.hosting.internet1.de/api/values";
        form.method = OFX_HTTP_POST;
        form.addFormField("Id", "10001");
        form.addFormField("customerId", "1123125gfsd0001");
        form.addFormField("submit", "1");
        httpUtils.addForm(form);
    } catch (int e) {

        //swallow
    }
}

//------------------------------------------------------------------------------
// Treaded counter 

int maxframes = 2;

void ofApp::createThreads()
{

    return;
    ThreadedCounterOut* currentCounterOut1 = nullptr;
    ThreadedCounterOut* currentCounterOut2 = nullptr;

    currentCounterOut1 = new ThreadedCounterOut(
            this->threshoLineTop.a,
            this->threshoLineCenter.a,
            this->threshoLineBottom.a,
            0, 0,
            config.getSettings().preset == 0 ?
            ThreadedCounter::Sensitive : ThreadedCounter::Accurate);

    ofAddListener(currentCounterOut1->onPeapleOut, this, &ofApp::eventHandlerPeapleOut);
    ofAddListener(currentCounterOut1->onPeapleIn, this, &ofApp::eventHandlerPeapleIn);
    threadedCounters.push_back(currentCounterOut1);
    currentCounterOut1->startThread();



    currentCounterOut2 = new ThreadedCounterOut(
            this->threshoLineTop.a,
            this->threshoLineCenter.a,
            this->threshoLineBottom.a,
            1, 0,
            config.getSettings().preset == 0 ?
            ThreadedCounter::Sensitive : ThreadedCounter::Accurate);

    ofAddListener(currentCounterOut2->onPeapleOut, this, &ofApp::eventHandlerPeapleOut);
    ofAddListener(currentCounterOut2->onPeapleIn, this, &ofApp::eventHandlerPeapleIn);
    threadedCounters.push_back(currentCounterOut2);
    currentCounterOut2->startThread();




}
//--------------------------------------------------------------

void ofApp::makeMask()
{
    if (config.maskPoints.size() == 0) {

        config.maskPoints.push_back(cv::Point(2, 2));
        config.maskPoints.push_back(cv::Point(CAMERAWIDTH - 2, 2));
        config.maskPoints.push_back(cv::Point(CAMERAWIDTH - 2, CAMERAHEIGHT - 2));
        config.maskPoints.push_back(cv::Point(2, CAMERAHEIGHT - 2));
        config.maskPoints.push_back(cv::Point(2, 2));
    }

    mask = cvCreateMat(CAMERAHEIGHT, CAMERAWIDTH, CV_8UC1);
    for (int i = 0; i < mask.cols; i++)

        for (int j = 0; j < mask.rows; j++)
            mask.at<uchar>(cv::Point(i, j)) = 0;

    vector<cv::Point> polyright;
    approxPolyDP(config.maskPoints, polyright, 1.0, true);
    fillConvexPoly(mask, &polyright[0], polyright.size(), 255, 8, 0);
}
//--------------------------------------------------------------

void ofApp::draw()
{
    ofSetColor(255);
    //ofBackground(255);

    //	line.draw();
    //	
    //	predicted.draw();
    //	ofPushStyle();
    //	ofSetColor(ofColor::red, 128);
    //	ofFill();
    //	ofDrawCircle(point, speed * 2);
    //        
    //	ofPopStyle();
    //	
    //	estimated.draw();
#ifdef USE_PI_CAMERA


	 
   




    if (!frame.empty()) {
        switch (config.getSettings().camviewmode) {
            case 1:
                //drawMat(output, 0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                if (ouputImage.isAllocated())
                    ouputImage.draw(0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                break;
            case 2:
                drawMat(lightenMat, 0, 0);
                break;
            case 3:
                grayImage.draw(0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                break;
            case 4:
                //cannyedge.draw(0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                drawMat(cannymatOutput, 0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                break;
            case 5:
                drawMat(maskOutput, 0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                break;
            case 6:
                grayScaleImage.draw(0, 0);
                break;
            case 7:
                drawMat(matshapedoutput, 0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                break;

        }
    }
#else
    if (!frame.empty()) {
        switch (config.getSettings().camviewmode) {
            case 1:
                //drawMat(output, 0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                if (ouputImage.isAllocated())
                    ouputImage.draw(0, 0, CAMERAWIDTH, CAMERAHEIGHT);

                break;
            case 2:
                drawMat(lightenMat, 0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                break;
            case 3:
                grayImage.draw(0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                break;
            case 4:
                //cannyedge.draw(0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                drawMat(cannymatOutput, 0, 0, CAMERAWIDTH, CAMERAHEIGHT);

                break;
            case 5:
                drawMat(maskOutput, 0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                break;
            case 6:
                grayScaleImage.draw(0, 0);
                break;
            case 7:
                drawMat(matshapedoutput, 0, 0, CAMERAWIDTH, CAMERAHEIGHT);
                break;


        }
    }





#endif


    //drawFinder();
    if (activateFinder) {
        drawFinder();
    }


    if (paintLine) {
        ofPushStyle();
        ofSetColor(ofColor::white);
        ofDrawLine(this->mouseStart.x, this->mouseStart.y,
                this->mouseMove.x, this->mouseMove.y);

        ofPopStyle();
    }

    ofPushStyle();
    polyline.draw();
    ofPopStyle();



    this->drawContourFinder();
    this->drawMaskArea();
    this->drawThresholdLines();
    this->drawGui();
    this->drawCounters();

    if (tempGoIn != personsIn) {
        soundIn.play();
        tempGoIn = personsIn;
    }

    if (tempGoOut != personsOut) {

        soundOut.play();
        tempGoOut = personsOut;
    }


}

//--------------------------------------------------------------

void ofApp::drawContourFinder()
{
    RectTracker& tracker = contourFinder.getTracker();
    if (showLabels) {
        ofNoFill();
        ofRectangle oldrect(0, 0, 0, 0);
        //for (int i = 0; i < (int) contourFinder.size(); i++)
        int idx = 0;
        for (ofCvBlob b : blobs) {
            // ofPoint center = toOf(contourFinder.getCenter(b.id));
            // ofRectangle rect = toOf(contourFinder.getBoundingRect(b.id));
            ofPoint center = b.box.getCenter();
            ofRectangle rect(b.box);

#ifdef BLOBMAXHEIGHT

            if (rect.height > BLOBMAXHEIGHT)
                rect.height = BLOBMAXHEIGHT;


#endif 


            ofPushMatrix();

            ofDrawRectangle(rect.x, rect.y, rect.width, rect.height);
            ofTranslate(rect.x, rect.y);
            unsigned int label = contourFinder.getLabel(idx);

            //string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
            //string msg = ofToString((int) rect.width) + ":" +
            //        ofToString(i) + ":" + ofToString(label) + ":" + ofToString(tracker.getAge(label));

            //string msg = ofToString(i) + ":" + ofToString(tracker.getLabelFromIndex(i))
            //        + " w:" + ofToString((int) rect.width);

            int direction = blobTracker.getDirection(idx);
            string directioninfo = "";
            if (direction != 0)
                directioninfo = blobTracker.getDirection(idx) == -1 ? "Down" : "Up";

            string msg = ofToString(idx) + "/" + ofToString(rect.width) + " " + directioninfo;
            ofDrawBitmapStringHighlight(msg, 0, 0);



            int nr = idx;
            nr = idx % 2;


            //fontsmall.drawString(msg,0,0);

            ofPopMatrix();
            string dir = "None";
            direction = 0;
            if (center.y < previousPos[nr]) {
                dir = " Up";
                direction = -1;
            }

            if (center.y > previousPos[nr]) {
                dir = " Down";
                direction = 1;
            }



            if (direction == 0)
                continue;

            idx++;
            previousYCenter = center.y;
            previousPos[nr] = center.y;

            previousDirection[nr] = direction;


            //            ofPushMatrix();
            //            ofTranslate(center.x, center.y);
            //            ofVec2f velocity = toOf(contourFinder.getVelocity(i));
            //            ofScale(5, 5);
            //            ofDrawLine(0, 0, velocity.x, velocity.y);
            //            ofPopMatrix();


        }
    } else {
        for (int i = 0; i < contourFinder.size(); i++) {
            unsigned int label = contourFinder.getLabel(i);
            // only draw a line if this is not a new label
            if (tracker.existsPrevious(label)) {
                // cout << "PRE" << endl;
                // use the label to pick a random color
                ofSeedRandom(label << 24);
                ofSetColor(ofColor::fromHsb(ofRandom(255), 255, 255));
                // get the tracked object (cv::Rect) at current and previous position
                const cv::Rect& previous = tracker.getPrevious(label);
                const cv::Rect& current = tracker.getCurrent(label);
                // get the centers of the rectangles
                ofVec2f previousPosition(previous.x + previous.width / 2, previous.y + previous.height / 2);
                ofVec2f currentPosition(current.x + current.width / 2, current.y + current.height / 2);
                ofDrawLine(previousPosition, currentPosition);
                //ofDrawRectangle( toOf( previous ) );
                // ofDrawRectangle( toOf( current ) );

            }
        }
    }
}


//--------------------------------------------------------------
// don't work! 

int ofApp::isHuman()
{
    if (config.getSettings().humandetection == 1) {
        int maxvalue = 0;
        int humand = 0;
        contourFinder.findContours(output);
        for (int i = 0; i < (int) contourFinder.size(); i++) {
            ofPolyline poly = contourFinder.getPolyline(i);
            poly.simplify(2.0);
            if ((int) poly.size() > maxvalue) {
                maxvalue = poly.size();
            }
        }
        // cout << maxvalue << endl;
        humand = (int) (maxvalue > 8);
        if (humand == 0)
            cout << " Probably not a Human (estimated)" << endl;

        return humand;
    }

    return 1;
}
//--------------------------------------------------------------

void ofApp::drawFinders()
{
}
//--------------------------------------------------------------

void ofApp::drawFinder()
{

}
//--------------------------------------------------------------

void ofApp::drawCounters()
{

    ofPushStyle();
    ofSetColor(ofColor::white);
    char buffer[512];
    sprintf(buffer, "%05d", personsIn);
    font.drawString(buffer, 2, CAMERAHEIGHT + 70);

    sprintf(buffer, "In");
    fontsmall.drawString(buffer, CAMERAWIDTH - 45, CAMERAHEIGHT + 30);



    sprintf(buffer, "%05d", personsOut);
    font.drawString(buffer, 2, CAMERAHEIGHT + 70 * 2);

    sprintf(buffer, "Out");
    fontsmall.drawString(buffer, CAMERAWIDTH - 45, CAMERAHEIGHT + 100);


    ofSetColor(ofColor::yellow);
    sprintf(buffer, "%05d", personstotal);
    font.drawString(buffer, 2, CAMERAHEIGHT + 70 * 3);


    sprintf(buffer, "Persons");
    fontsmall.drawString(buffer, CAMERAWIDTH - 45, CAMERAHEIGHT + 170);




    ofSetColor(ofColor::white);
    sprintf(buffer, "%d/%d", contourFinder.size(), blobTracker.countRunningThreads());
    ofDrawBitmapStringHighlight(buffer, CAMERAWIDTH - 28, CAMERAHEIGHT - 6);






    ofPopStyle();

}
//--------------------------------------------------------------

void ofApp::drawMaskArea()
{
    ofPushStyle();
    ofPolyline polyline1;
    ofSetColor(ofColor::yellow);
    for (int i = 0; i < config.maskPoints.size(); i++) {

        cv::Point p = cv::Point(config.maskPoints[i].x, config.maskPoints[i].y);
        polyline1.lineTo(p.x, p.y);
    }
    polyline1.draw();
    ofPopStyle();
}
//--------------------------------------------------------------

void ofApp::drawThresholdLines()
{

    ofPushStyle();
    ofSetColor(ofColor::yellowGreen);
    ofDrawLine(threshoLineCenter.a, threshoLineCenter.b);
    ofSetColor(ofColor::whiteSmoke);
    ofDrawLine(threshoLineTop.a, threshoLineTop.b);
    ofDrawLine(threshoLineBottom.a, threshoLineBottom.b);


    // Vertical Center
    ofSetColor(0, 255, 255);
    ofDrawLine(CAMERAWIDTH / 2, CAMERAHEIGHT / 2 - 5, CAMERAWIDTH / 2, CAMERAHEIGHT / 2 + 5);

    ofSetColor(ofColor::white);
    ofDrawLine(CAMERAWIDTH / 2 - config.getSettings().minblobsize / 2, CAMERAHEIGHT / 2 - THRESHOLINESOFFSET,
            CAMERAWIDTH / 2 - config.getSettings().minblobsize / 2, CAMERAHEIGHT / 2 + THRESHOLINESOFFSET);
    ofDrawLine(CAMERAWIDTH / 2 + config.getSettings().minblobsize / 2, CAMERAHEIGHT / 2 - THRESHOLINESOFFSET,
            CAMERAWIDTH / 2 + config.getSettings().minblobsize / 2, CAMERAHEIGHT / 2 + THRESHOLINESOFFSET);

    ofSetColor(ofColor::white);
    ofDrawLine(CAMERAWIDTH / 2 - config.getSettings().midblobsize / 2, CAMERAHEIGHT / 2 - THRESHOLINESOFFSET - 10,
            CAMERAWIDTH / 2 - config.getSettings().midblobsize / 2, CAMERAHEIGHT / 2 + THRESHOLINESOFFSET + 10);
    ofDrawLine(CAMERAWIDTH / 2 + config.getSettings().midblobsize / 2, CAMERAHEIGHT / 2 - THRESHOLINESOFFSET - 10,
            CAMERAWIDTH / 2 + config.getSettings().midblobsize / 2, CAMERAHEIGHT / 2 + THRESHOLINESOFFSET + 10);


    ofSetColor(ofColor::white);
    ofDrawLine(CAMERAWIDTH / 2 - config.getSettings().maxblobsize / 2, CAMERAHEIGHT / 2 - THRESHOLINESOFFSET - 20,
            CAMERAWIDTH / 2 - config.getSettings().maxblobsize / 2, CAMERAHEIGHT / 2 + THRESHOLINESOFFSET + 20);
    ofDrawLine(CAMERAWIDTH / 2 + config.getSettings().maxblobsize / 2, CAMERAHEIGHT / 2 - THRESHOLINESOFFSET - 20,
            CAMERAWIDTH / 2 + config.getSettings().maxblobsize / 2, CAMERAHEIGHT / 2 + THRESHOLINESOFFSET + 20);


    ofSetColor(ofColor::aqua);
    ofNoFill();
    ofPoint p(CAMERAWIDTH / 2, CAMERAHEIGHT / 2);
    ofDrawCircle(p, config.getSettings().contourminarearadius);
    ofSetColor(ofColor::yellow);
    ofDrawCircle(p, config.getSettings().contourmaxarearadius);
    ofPopStyle();
}
//--------------------------------------------------------------

void ofApp::drawGui()
{

    ofPushStyle();

    ofFill();
    ofSetColor(ofColor::black);
    ofDrawRectangle(0, CAMERAHEIGHT, CAMERAWIDTH, ofGetHeight() - CAMERAHEIGHT - 40);
    ofSetColor(ofColor::gray);
    ofDrawRectangle(CAMERAWIDTH, 0, ofGetWidth() - CAMERAWIDTH, ofGetHeight() - 40);
    ofSetColor(ofColor::dimGray);
    ofDrawRectangle(0, ofGetHeight() - 40, ofGetWidth(), ofGetHeight());

    string in = " IN ";
    string out = " OUT";
    ofDrawBitmapStringHighlight(in, 0, 12);
    ofDrawBitmapStringHighlight(out, 0, CAMERAHEIGHT - 6);



    ofNoFill();
    ofSetColor(ofColor::white);
    string version = "Version:" + (string) APPVERSION;
    ofDrawBitmapStringHighlight(version, 10, ofGetHeight() - 20);


    string machineinfo = interface + ", " + ipaddress + ", " + subnet + ", " + macaddress;
    ofDrawBitmapStringHighlight(machineinfo, 180, ofGetHeight() - 20);

    string preset = config.getSettings().preset == 0 ? "Sensitive" : "Accurate";
    ofDrawBitmapStringHighlight(preset, ofGetWidth() - 170, ofGetHeight() - 20);


    string mousecoord = ofToString(this->mouseMove.x) + " / " + ofToString(this->mouseMove.y);
    ofDrawBitmapStringHighlight(mousecoord, ofGetWidth() - 80, ofGetHeight() - 20);

    string key_distance = "[+][-] View Mode = " + ofToString(config.getSettings().camviewmode);
    ofDrawBitmapString(key_distance, (CAMERAWIDTH) + 20, 20);

    string key_imagelightenAmount = "[+][-] Image lighten Amount = " + ofToString(config.getSettings().imagelightenAmount);
    ofDrawBitmapString(key_imagelightenAmount, (CAMERAWIDTH) + 20, 20 * 2);

    string key_imagethreshold = "[+][-] Image threshold = " + ofToString(config.getSettings().imagethreshold);
    ofDrawBitmapString(key_imagethreshold, (CAMERAWIDTH) + 20, 20 * 3);

    string key_imageblur = "[+][-] Image Blur = " + ofToString(config.getSettings().imageblur);
    ofDrawBitmapString(key_imageblur, (CAMERAWIDTH) + 20, 20 * 4);

    string key_contourminarearadius = "[+][-] Contour min area radius = " + ofToString(config.getSettings().contourminarearadius);
    ofDrawBitmapString(key_contourminarearadius, (CAMERAWIDTH) + 20, 20 * 5);

    string key_contourmaxarearadius = "[+][-] Contour max area radius = " + ofToString(config.getSettings().contourmaxarearadius);
    ofDrawBitmapString(key_contourmaxarearadius, (CAMERAWIDTH) + 20, 20 * 6);

    string key_contourthreshold = "[+][-] Contour threshold = " + ofToString(config.getSettings().contourthreshold);
    ofDrawBitmapString(key_contourthreshold, (CAMERAWIDTH) + 20, 20 * 7);

    string key_minblobsize = "[+][-] Min (1) Blob width size = " + ofToString(config.getSettings().minblobsize);
    ofDrawBitmapString(key_minblobsize, (CAMERAWIDTH) + 20, 20 * 8);

    string key_midblobsize = "[+][-] Mid (2) Blob width size = " + ofToString(config.getSettings().midblobsize);
    ofDrawBitmapString(key_midblobsize, (CAMERAWIDTH) + 20, 20 * 9);

    string key_maxblobsize = "[+][-] Max (3) Blob width size = " + ofToString(config.getSettings().maxblobsize);
    ofDrawBitmapString(key_maxblobsize, (CAMERAWIDTH) + 20, 20 * 10);

    string key_trackermaxDistance = "[+][-] Tracker max Distance = " + ofToString(config.getSettings().trackermaxDistance);
    ofDrawBitmapString(key_trackermaxDistance, (CAMERAWIDTH) + 20, 20 * 11);

    string key_trackerpersistance = "[+][-] Tracker persistance = " + ofToString(config.getSettings().trackerpersistance);
    ofDrawBitmapString(key_trackerpersistance, (CAMERAWIDTH) + 20, 20 * 12);

    string key_camcontrast = "[+][-] PICAM contrast = " + ofToString(config.getSettings().camcontrast);
    ofDrawBitmapString(key_camcontrast, (CAMERAWIDTH) + 20, 20 * 13);

    string key_cambrightness = "[+][-] PICAM brightness = " + ofToString(config.getSettings().cambrightness);
    ofDrawBitmapString(key_cambrightness, (CAMERAWIDTH) + 20, 20 * 14);

    string key_camrotation = "[+][-] PICAM rotation = " + ofToString(config.getSettings().camrotation);
    ofDrawBitmapString(key_camrotation, (CAMERAWIDTH) + 20, 20 * 15);


    string key_saveconfig = "[ s ] Save Configuration";
    ofDrawBitmapString(key_saveconfig, (CAMERAWIDTH) + 20, 20 * 17);

    string key_setmask = "[ m ] Set mask";
    ofDrawBitmapString(key_setmask, (CAMERAWIDTH) + 20, 20 * 18);

    string key_clearmask = "[ r ] Remove mask";
    ofDrawBitmapString(key_clearmask, (CAMERAWIDTH) + 20, 20 * 19);

    string key_clearcountersk = "[ c ] Reset counters";
    ofDrawBitmapString(key_clearcountersk, (CAMERAWIDTH) + 20, 20 * 20);

    string key_exit = "[ q ] Exit Application";
    ofDrawBitmapString(key_exit, (CAMERAWIDTH) + 20, 20 * 21);

    ofPopStyle();

	
   
    string value;
    m_gpio4->getval_gpio(value);
    //ofDrawBitmapStringHighlight(value, ofGetWidth() - 20,  ofGetHeight() - 50);
    font.drawString(value.c_str(), ofGetWidth() - 60,  ofGetHeight() - 50);
    
    m_movement = value == "1";   


}
//--------------------------------------------------------------

ofRectangle ofApp::getRectangleFromMask()
{

    ofRectangle rectangle(0, 0, CAMERAWIDTH, CAMERAWIDTH);

    if (config.maskPoints.size() < 1) {

        return rectangle;
    }

    // If we have a mask. We need to convert the points to rectangle in order to 
    // get a valid area for the image crop
    cv::Point smallest(0, 0);
    cv::Point bigest(0, 0);

    int smallestx = INT_MAX;
    int smallesty = INT_MAX;
    int bigestx = 0;
    int bigesty = 0;

    for (int i = 0; i < config.maskPoints.size(); i++) {
        cv::Point p = config.maskPoints[i];

        if (p.x < smallestx) {
            smallestx = p.x;
        }

        if (p.y < smallesty) {
            smallesty = p.y;
        }

        if (p.x > bigestx) {
            bigestx = p.x;
        }

        if (p.y > bigesty) {
            bigesty = p.y;
        }
    }

    smallest.x = smallestx;
    smallest.y = smallesty;
    bigest.x = bigestx;
    bigest.y = bigesty;


    ofRectangle result(smallest.x, smallest.y,
            bigest.x - smallest.x, bigest.y - smallest.y);

    return result;

}
//--------------------------------------------------------------
// obsolete use ofRectangle instead...

ofRectangle ofApp::isIntersect(ofRectangle a, ofRectangle b)
{
    ofRectangle r;
    r.x = (a.x > b.x) ? a.x : b.x;
    r.y = (a.y > b.y) ? a.y : b.y;
    r.width = (a.x + a.width < b.x + b.width) ?
            a.x + a.width - r.x : b.x + b.width - r.x;
    r.height = (a.y + a.height < b.y + b.height) ?
            a.y + a.height - r.y : b.y + b.height - r.y;
    if (r.width <= 0 || r.height <= 0)
        r = ofRectangle(0, 0, 0, 0);

    return r;
}
//--------------------------------------------------------------
// obsolete use ofRectangle instead...

bool ofApp::isInside(int pointX, int pointY, int rectX, int rectY,
        int rectWidth, int rectHeight)
{

    return (rectX <= pointX && pointX <= rectX + rectWidth) &&
            (rectY <= pointY && pointY <= rectY + rectHeight);
}
//--------------------------------------------------------------

void ofApp::updateMouseEvents()
{
    uint64_t currentMillis = ofGetElapsedTimeMillis();
    int value = 0;
    if (mousePress) {
        if (currentMillis - previousMillis >= mouseDelay) {

            mouseDelay -= MOUSE_INTERVAL;
            if (mouseDelay < MOUSE_MINDELAY)
                mouseDelay = MOUSE_MINDELAY;


            switch (configIndex) {
                case 0:
                    value = config.getSettings().camviewmode;

                    if (operant == 1 && value == 7) break;
                    if (operant == 0 && value == 1) break;

                    operant == 1 ? value++ : value--;
                    config.setCamViewMode(value);
                    break;
                case 1:
                    value = config.getSettings().imagelightenAmount;

                    if (operant == 1 && value == 200) break;
                    if (operant == 0 && value == 0) break;

                    operant == 1 ? value++ : value--;
                    config.setImageLightenAmount(value);
                    break;
                case 2:
                    value = config.getSettings().imagethreshold;

                    if (operant == 1 && value == 200) break;
                    if (operant == 0 && value == 0) break;

                    operant == 1 ? value++ : value--;
                    config.setImagethreshold(value);
                    break;
                case 3:
                    value = config.getSettings().imageblur;

                    if (operant == 1 && value == 100) break;
                    if (operant == 0 && value == 0) break;

                    operant == 1 ? value++ : value--;
                    config.setImageBlur(value);
                    break;

                case 4:
                    value = config.getSettings().contourminarearadius;

                    if (operant == 1 && value == config.getSettings().contourmaxarearadius - 10) break;
                    if (operant == 0 && value == 1) break;

                    operant == 1 ? value++ : value--;
                    config.setContourMinAreaRadius(value);
                    initCounturFinder();
                    break;
                case 5:
                    value = config.getSettings().contourmaxarearadius;

                    if (operant == 1 && value == 115) break;
                    if (operant == 0 && value == config.getSettings().contourminarearadius + 10) break;

                    operant == 1 ? value++ : value--;
                    config.setContourMaxAreaRadius(value);
                    initCounturFinder();
                    break;
                case 6:
                    value = config.getSettings().contourthreshold;

                    if (operant == 1 && value == 100) break;
                    if (operant == 0 && value == 0) break;

                    operant == 1 ? value++ : value--;
                    config.setContourThreshold(value);
                    initCounturFinder();
                    break;

                case 7:
                    value = config.getSettings().minblobsize;

                    if (operant == 1 && value == CAMERAWIDTH - 20) break;
                    if (operant == 0 && value == 1) break;

                    operant == 1 ? value++ : value--;
                    config.setMinBlobSize(value);
                    break;

                case 8:
                    value = config.getSettings().midblobsize;

                    if (operant == 1 && value + 10 >= config.getSettings().maxblobsize) break;
                    if (operant == 0 && value - 10 <= config.getSettings().minblobsize) break;

                    operant == 1 ? value++ : value--;
                    config.setMidBlobSize(value);
                    break;


                case 9:
                    value = config.getSettings().maxblobsize;

                    if (operant == 1 && value == CAMERAWIDTH - 20) break;
                    if (operant == 0 && value - 10 <= config.getSettings().midblobsize) break;

                    operant == 1 ? value++ : value--;
                    config.setMaxBlobSize(value);
                    break;



                case 10:
                    value = config.getSettings().trackermaxDistance;

                    if (operant == 1 && value == 200) break;
                    if (operant == 0 && value == 10) break;

                    operant == 1 ? value++ : value--;
                    config.setTrackerMaxDistance(value);
                    initCounturFinder();
                    break;
                case 11:
                    value = config.getSettings().trackerpersistance;

                    if (operant == 1 && value == 200) break;
                    if (operant == 0 && value == 10) break;

                    operant == 1 ? value++ : value--;
                    config.setTrackerPersistance(value);
                    initCounturFinder();
                    break;
                case 12:
                    value = config.getSettings().camcontrast;

                    if (operant == 1 && value == 100) break;
                    if (operant == 0 && value == 0) break;

                    operant == 1 ? value++ : value--;
                    config.setCamContrast(value);
                    camContrastChanged(value);
                    break;
                case 13:
                    value = config.getSettings().cambrightness;

                    if (operant == 1 && value == 100) break;
                    if (operant == 0 && value == 0) break;

                    operant == 1 ? value++ : value--;
                    config.setCamBrightness(value);
                    camBrightnessChanged(value);
                    break;
                case 14:
                    value = config.getSettings().camrotation;

                    if (operant == 1 && value == 360) break;
                    if (operant == 0 && value == 0) break;

                    operant == 1 ? value++ : value--;
                    config.setCamRotation(value);
                    camRotationChanged(value);

                    break;

            }
            // save the last time we was here
            previousMillis = currentMillis;
        }
    }
}
//--------------------------------------------------------------

void ofApp::keyPressed(int key)
{

    switch (key) {

        case '1': config.setCamViewMode(1);
            break;
        case '2': config.setCamViewMode(2);
            break;
        case '3': config.setCamViewMode(3);
            break;
        case '4': config.setCamViewMode(4);
            break;

        case '5': config.setCamViewMode(5);
            break;

        case '6': config.setCamViewMode(6);
            break;
        case '7': config.setCamViewMode(7);
            break;
        case 'i': exportGrayImage("");
            break;

        case 'c': resetCounters();
            break;



        case 't':
            activateCropTimer = !activateCropTimer;
            break;

        default:break;
    }



    if (isVideoMode) {
        if (key == 'p') {
            video.setPaused(!video.isPaused());
            if (video.isPaused()) {
                cout << "Pause " + ofToString(contourFinderVector.size()) << endl;
            }
        }
    }


    if (key == 'f') {

        activateFinder = true;
        cout << "Activate Finder " << endl;
    }

}
//--------------------------------------------------------------

void ofApp::keyReleased(int key)
{
    if (key == 'f') {

        activateFinder = false;
        isOneFrameSet = false;
        cout << "Deactivate Finder " << endl;
    }
}
//--------------------------------------------------------------

void ofApp::mouseMoved(int x, int y)
{

    mouseMove.x = x;
    mouseMove.y = y;

}
//--------------------------------------------------------------

void ofApp::mouseReleased(int x, int y, int button)
{

    mousePress = false;
    configIndex = -1;
    mouseDelay = MOUSE_MAXDELAY;
}
//--------------------------------------------------------------

void ofApp::mousePressed(int x, int y, int button)
{

    mousePress = true;

    configIndex = -1;
    if (button == 0) {


        int plussa = CAMERAWIDTH + 21;
        int plussb = plussa + 21;
        int minusa = plussb + 4;
        int minusb = plussb + 21;

        //printf("%d %d\n", plussa, plussb);
        // camview  +
        if (x >= plussa && x <= plussb && y >= 9 && y < 18) {
            operant = 1;
            configIndex = 0;

        }
        // camview  -
        if (x >= minusa && x <= minusb && y >= 9 && y < 18) {
            operant = 0;
            configIndex = 0;

        }

        // imagelightenAmount
        if (x >= plussa && x <= plussb && y >= 30 && y < 40) {
            operant = 1;
            configIndex = 1;

        }
        // imagelightenAmount  -
        if (x >= minusa && x <= minusb && y >= 30 && y < 40) {
            operant = 0;
            configIndex = 1;

        }

        // imagethreshold
        if (x >= plussa && x <= plussb && y >= 50 && y < 60) {
            operant = 1;
            configIndex = 2;

        }
        // imagethreshold  -
        if (x >= minusa && x <= minusb && y >= 50 && y < 60) {
            operant = 0;
            configIndex = 2;

        }

        // imageblur
        if (x >= plussa && x <= plussb && y >= 70 && y < 80) {
            operant = 1;
            configIndex = 3;

        }
        // imageblur  -
        if (x >= minusa && x <= minusb && y >= 70 && y < 80) {
            operant = 0;
            configIndex = 3;

        }

        // contourminarearadius
        if (x >= plussa && x <= plussb && y >= 90 && y < 100) {
            operant = 1;
            configIndex = 4;

        }
        // contourminarearadius  -
        if (x >= minusa && x <= minusb && y >= 90 && y < 100) {
            operant = 0;
            configIndex = 4;

        }

        // contourmaxarearadius
        if (x >= plussa && x <= plussb && y >= 110 && y < 120) {
            operant = 1;
            configIndex = 5;

        }
        // contourmaxarearadius  -
        if (x >= minusa && x <= minusb && y >= 110 && y < 120) {
            operant = 0;
            configIndex = 5;

        }

        // contourtreshold
        if (x >= plussa && x <= plussb && y >= 130 && y < 140) {
            operant = 1;
            configIndex = 6;

        }
        // contourtreshold  -
        if (x >= minusa && x <= minusb && y >= 130 && y < 140) {
            operant = 0;
            configIndex = 6;

        }
        //minblobsize
        if (x >= plussa && x <= plussb && y >= 150 && y < 160) {
            operant = 1;
            configIndex = 7;

        }
        // minblobsize  -
        if (x >= minusa && x <= minusb && y >= 150 && y < 160) {
            operant = 0;
            configIndex = 7;

        }

        //midblobsize
        if (x >= plussa && x <= plussb && y >= 170 && y < 180) {
            operant = 1;
            configIndex = 8;

        }
        // midblobsize  -
        if (x >= minusa && x <= minusb && y >= 170 && y < 180) {
            operant = 0;
            configIndex = 8;

        }


        //maxblobsize
        if (x >= plussa && x <= plussb && y >= 190 && y < 200) {
            operant = 1;
            configIndex = 9;

        }
        // maxblobsize  -
        if (x >= minusa && x <= minusb && y >= 190 && y < 200) {
            operant = 0;
            configIndex = 9;

        }


        //trackermaxDistance
        if (x >= plussa && x <= plussb && y >= 210 && y < 220) {
            operant = 1;
            configIndex = 10;

        }
        // trackermaxDistance  -
        if (x >= minusa && x <= minusb && y >= 210 && y < 220) {
            operant = 0;
            configIndex = 10;

        }


        //trackerpersistance
        if (x >= plussa && x <= plussb && y >= 230 && y < 240) {
            operant = 1;
            configIndex = 11;

        }
        // trackerpersistance  -
        if (x >= minusa && x <= minusb && y >= 230 && y < 240) {
            operant = 0;
            configIndex = 11;

        }

#ifdef USE_PI_CAMERA
        //camcontrast
        if (x >= plussa && x <= plussb && y >= 250 && y < 260) {
            operant = 1;
            configIndex = 12;

        }
        // camcontrast  -
        if (x >= minusa && x <= minusb && y >= 250 && y < 260) {
            operant = 0;
            configIndex = 12;

        }

        //cambrightness
        if (x >= plussa && x <= plussb && y >= 270 && y < 280) {
            operant = 1;
            configIndex = 13;

        }

        // cambrightness  -
        if (x >= minusa && x <= minusb && y >= 270 && y < 280) {
            operant = 0;
            configIndex = 13;

        }
        //camrotation
        if (x >= plussa && x <= plussb && y >= 290 && y < 300) {
            operant = 1;
            configIndex = 14;

        }
        // camrotation  -
        if (x >= minusa && x <= minusb && y >= 290 && y < 300) {
            operant = 0;
            configIndex = 14;

        }
#endif

        // save config
        if (x >= 343 <= 531 && y >= 330 && y < 340) {

            saveConfig();
            return;
        }

        // set maskg
        if (x >= 343 <= 456 && y >= 350 && y < 360) {

            setMask();
            return;
        }
        // remove mask
        if (x >= 343 <= 475 && y >= 370 && y < 380) {

            removeMask();
            return;
        }

        // reset counters
        if (x >= 343 <= 501 && y >= 390 && y < 400) {

            resetCounters();
            return;
        }

        // reset counters
        if (x >= 343 <= 520 && y >= 410 && y < 420) {

            terminate();
            return;
        }
    }

    if (x > CAMERAWIDTH || x < 0 || y > CAMERAHEIGHT || y < 0) {
        return;
    }

    if (button == 2) {
        paintLine = false;
        return;
    }

    if (button == 0 && pressedCount == 0) {
        this->mouseStart.x = x;
        this->mouseStart.y = y;

        polyline.lineTo(x, y);

        drawnPoints.push_back(ofPoint(x, y));
        paintLine = true;
        pressedCount++;
        return;
    }


    if (button == 0 && pressedCount > 0) {

        polyline.lineTo(x, y);
        drawnPoints.push_back(ofPoint(x, y));
        pressedCount = 0;
        this->mouseStart.x = x;
        this->mouseStart.y = y;
    }

}
//--------------------------------------------------------------

void ofApp::saveConfig()
{

    this->config.save(CONFIGURATION_FILENAME);
    cout << "Configuration saved!\n";
}
//--------------------------------------------------------------

void ofApp::resetCounters()
{

    personsIn = 0;
    personsOut = 0;
    personstotal = 0;
    tempGoIn = 0;
    tempGoOut = 0;
}
//--------------------------------------------------------------

void ofApp::setMask()
{
    if (drawnPoints.size() == 0)
        return;

    config.maskPoints.clear();

    for (ofPoint p : drawnPoints) {

        config.maskPoints.push_back(cv::Point(p.x, p.y));
    }

    polyline.clear();
    drawnPoints.clear();
    paintLine = false;
    makeMask();

    cout << "mask set!\n";
}
//--------------------------------------------------------------

void ofApp::removeMask()
{

    config.maskPoints.clear();
    drawnPoints.clear();
    polyline.clear();

    makeMask();

    initCounturFinder();
    cout << "mask removed!\n";
}
//--------------------------------------------------------------

void ofApp::cropGrayImage()
{
    if (contourFinderVector.size() < 1) {
        return;
    }

    ofImage targetimage = grayImage;
    ofRectangle bigest(0, 0, 0, 0);

    for (int i = 0; i < contourFinderVector.size(); i++) {
        cv::Rect rect = contourFinderVector[i];

        if (rect.width > bigest.width) {

            bigest = toOf(rect);
        }
    }

    targetimage.crop(bigest.x, bigest.y, bigest.width, bigest.height);
    targetimage.saveImage("/home/yoo/GameDev/OpCVWorkSapce/opencv-haar-classifier-training-master/cascaden_images/humantopview/positive/" + ofGetTimestampString() + ".jpg");
}
//--------------------------------------------------------------

void ofApp::exportOutputImage(string info)
{

    ofxCv::toOf(output, img);
    img.saveImage(ofGetTimestampString() + info + ".jpg");


}

void ofApp::exportGrayImage(string path)
{

    switch (config.getSettings().camviewmode) {
        case 1:

            ofxCv::toOf(output, img);
            img.saveImage(ofGetTimestampString() + ".jpg");
            break;
        case 2:
            ofxCv::toOf(lightenMat, img);
            img.saveImage(ofGetTimestampString() + ".jpg");
            break;
        case 3:
            grayImage.saveImage(ofGetTimestampString() + ".jpg");
            break;
        case 4:
            cannyedge.saveImage(ofGetTimestampString() + ".jpg");
            break;
        case 5:
            ofxCv::toOf(maskOutput, img);
            img.save(ofGetTimestampString() + ".jpg");

            break;
        case 6:
            img.setFromPixels(grayScaleImage.getPixels());
            img.save(ofGetTimestampString() + ".jpg");

            break;

    }


}

//--------------------------------------------------------------

//void ofApp::eventHandlerPeopleGoingIn(int &persons)
//{
//
//    int human = isHuman();
//    cout << human << " in / " << persons << endl;
//
//    try {
//        ofxHttpForm form;
//        form.action = "http://324663.de.hosting.internet1.de/api/values";
//        form.method = OFX_HTTP_POST;
//        form.addFormField("Id", "10001");
//        form.addFormField("customerId", "1123125gfsd0001");
//        form.addFormField("submit", "1");
//        httpUtils.addForm(form);
//
//    } catch (int e) {
//
//        //swallow
//    }
//
//
//    cout << "People Going In: " << persons << endl;
//    // httpHandler.post(count);
//}
////--------------------------------------------------------------
//
//void ofApp::eventHandlerPeopleGoingOut(int &persons)
//{
//    int human = isHuman();
//    cout << human << " out / " << persons << endl;
//
//
//    try {
//        ofxHttpForm form;
//        form.action = "http://324663.de.hosting.internet1.de/api/values";
//        form.method = OFX_HTTP_POST;
//        form.addFormField("Id", "10001");
//        form.addFormField("customerId", "1123125gfsd0001");
//        form.addFormField("submit", "1");
//        httpUtils.addForm(form);
//    } catch (int e) {
//
//        //swallow
//    }
//
//    cout << "People Going Out: " << persons << endl;
//    //httpHandler.post(count);
//}
//--------------------------------------------------------------

void ofApp::camRotationChanged(int value)
{

#ifdef USE_PI_CAMERA

    cam.setRotation(value);
#endif    

}
//--------------------------------------------------------------

void ofApp::camContrastChanged(int value)
{

#ifdef USE_PI_CAMERA

    cam.setContrast(value);
#endif    
}
//--------------------------------------------------------------

void ofApp::camBrightnessChanged(int value)
{

#ifdef USE_PI_CAMERA
    cam.setBrightness(value);
#endif    
}

//--EOF--
