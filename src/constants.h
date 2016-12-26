/*
 *  created by Juan R. Gonzalez & Juan Alvarez 2016
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

//-----------------------------------------------------------------------------
// uncomment this if you want to use the raspberry PI CAM.
//#define USE_PI_CAMERA
//-----------------------------------------------------------------------------

class drawLine
{
public:
    ofPoint a;
    ofPoint b;
};


#define CONFIGURATION_FILENAME "configuration.xml"
#define HAARCASCADE_FILENAME "haarcascade_headandshoulder.xml"

#define CAMERAWIDTH 320//640
#define CAMERAHEIGHT 240//480

#define CONFIG_CAMVIEWMODE "config:camviewmode"
#define CONFIG_CAMVIDEOFILE "config:camvideofile"
#define CONFIG_LIGHTENAMOUNT "config:lightenAmount"
#define CONFIG_MOGHISTORY "config:moghistory"
#define CONFIG_MOGTHRESHOLD "config:mogthreshold"
#define CONFIG_IMAGETHRESHOLD "config:imagethreshold"
#define CONFIG_IMAGEBLUR "config:imageblur"
#define CONFIG_CONTOURMINAREARADIUS "config:contourminarearadius"
#define CONFIG_CONTOURMAXAREARADIUS "config:contourmaxarearadius"
#define CONFIG_CONTOURTHRESHOLD "config:contourthreshold"
#define CONFIG_MASKPOINTS "config:maskpoints"
#define CONFIG_MASKPOINTS_POINT "maskpoints:point"
#define CONFIG_MINBLOBSIZE "config:minblobsize"
#define CONFIG_MIDBLOBSIZE "config:midblobsize"
#define CONFIG_MAXBLOBSIZE "config:maxblobsize"
#define CONFIG_TRACKERPERSISTANCE "config:trackerpersistance"
#define CONFIG_TRACKERMAXDISTANCE "config:trackermaxDistance"
#define CONFIG_CAMCONTRAST "config:camcontrast"
#define CONFIG_CAMBRIGHTNESS "config:cambrightness"
#define CONFIG_CAMROTATION "config:camrotation"
#define CONFIG_HUMANDETECTION "config:humandetection"
#define CONFIG_PRESET "config:preset"
#define CONFIG_MULTITHREAD "config:multithread"



#define BLOBMAX 10
#define THREADDUPLICATESMAX  100


#define ALLOWEDBLOBSINFIELD 3
//#define DEBUGLINESON 1


#define WINDOWTITLE "Peaple Counter"
#define APPVERSION "Pontevedra 1.0"

#define MOUSE_MAXDELAY 500
#define MOUSE_MINDELAY 30
#define MOUSE_INTERVAL 80

#define MAXLERNINGFRAMES 100 // frames
#define FRAMERATE 19       
#define VIDEOPLAYSPEED 1.0f
#define THRESHOLINESOFFSET 10 
#define BLOBMAXHEIGHT (THRESHOLINESOFFSET * 4 ) + THRESHOLINESOFFSET //(dont change this!!! is ok!!!)
#define SOUND_IN_FILENAME "sounds/in.wav"
#define SOUND_OUT_FILENAME "sounds/out.wav"

#define THREADEDCOUNTER_MAXTHREADS      10  // Max number of threads used for in and out.
#define THREADEDCOUNTER_LIFEFOREVER     1   // uncoment this if you want limited thread lifetime.
#define THREADEDCOUNTER_LIFETIME        6   // Thread Lifetime in seconds.
#define THREADEDCOUNTER_DELAYTIME       50  // Thread loop delay time in ms.(dont change this!!!10 is ok!!!)
#define THREADEDCOUNTER_CHKNODATA       30  // Check interval for receive data in seconds.
#define THREADEDCOUNTER_NODATAREPEATER  6   // Check seconds*n times for receive data.
#define THREADEDCOUNTER_DESTROYTIME   5000  // Time in ms. before the Thread destructor is executed.


#endif /* CONSTANTS_H */

