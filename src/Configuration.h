/*
 *  created by Juan R. Gonzalez & Juan Alvarez 2016
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdio.h>
#include "ofxCv.h"
#include "ofxXmlSettings.h"
#include "constants.h"
#include "ofMain.h"

struct DefaultSettings
{
    int camviewmode = 1;
    string camvideofile = "";
    int imagelightenAmount = 15;
    int imagethreshold = 26;
    int imageblur = 6;
    int moghistory = 500;
    int mogthreshold = 20;
    int contourminarearadius = 15;
    int contourmaxarearadius = 60;
    int contourthreshold = 10;
    int minblobsize = 24;
    int midblobsize = 88;
    int maxblobsize = 140;
    int trackerpersistance = 15;
    int trackermaxDistance = 32;
    int camcontrast = 40;
    int cambrightness = 50;
    int camrotation = 0;
    int humandetection = 0;
    int preset = 1; // 0 = sensitive, 1 = accurate
    int multithread = 0;
    
    
};

struct Settings
{
    int camviewmode;
    string camvideofile;
    int imagelightenAmount;
    int imagethreshold;
    int imageblur;
    int moghistory;
    int mogthreshold;
    int contourminarearadius;
    int contourmaxarearadius;
    int contourthreshold;
    int minblobsize;
    int midblobsize;
    int maxblobsize;
    int trackerpersistance;
    int trackermaxDistance;
    int camcontrast;
    int cambrightness;
    int camrotation;
    int humandetection;
    int preset = 0; // 0 = sensitive, 1 = accurate
    int multithread = 0;
};

class Configuration
{
public:

    void load(string file);
    void save(string file);

    Settings getSettings();
    void setCamViewMode(int value);
    void setImageLightenAmount(int value);
    void setImagethreshold(int value);
    void setImageBlur(int value);
    void setContourMinAreaRadius(int value);
    void setContourMaxAreaRadius(int value);
    void setContourThreshold(int value);
    void setMinBlobSize(int value);
    void setMidBlobSize(int value);
    void setMaxBlobSize(int value);
    void setTrackerMaxDistance(int value);
    void setTrackerPersistance(int value);
    void setCamContrast(int value);
    void setCamBrightness(int value);
    void setCamRotation(int value);
    


    vector <cv::Point> maskPoints;

private:

    Settings settings;
    DefaultSettings defaultSettings;

    ofxXmlSettings XML;
};
#endif /* CONFIGURATION_H */

