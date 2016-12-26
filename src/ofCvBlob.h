

#ifndef OFCVBLOB_H
#define OFCVBLOB_H

#include "ofMain.h"
#include <vector>

class ofCvBlob {
  public:

    float area;
    float length;
    ofRectangle box;
    ofPoint center;
   
    bool hole;
    vector<ofPoint> contour;
    
    int direction;
    int previousPos;
    
   
    ofCvBlob() {
        area = 0.0f;
        length = 0.0f;
        hole = false;    
        direction = 0;
        previousPos = 0;
        
        
    }
    
};

#endif /* OFCVBLOB_H */

