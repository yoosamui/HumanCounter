
#ifndef TRACKEDBLOB_H
#define TRACKEDBLOB_H

#include "ofMain.h"

class TrackedBlob : public ofCvBlob {
  public:

    int id;
    int nr;
    ofPoint deltaLoc;
    ofPoint deltaLocTotal;
    ofPoint predictedPos;
    float deltaArea;
    
    
    // Used only by BlobTracker
    //
    bool markedForDeletion;
    int framesLeft;
    vector<float> error;
    vector<int> closest;  // ids of the closest points, sorted
    
    

    TrackedBlob() {
        id = -1;
        area = 0.0f;
        length = 0.0f;
        deltaArea = 0.0f;
        markedForDeletion = false;
        framesLeft = 0;
         nr= 0;
    }
    
    TrackedBlob( const ofCvBlob& b ) {
        area = b.area;
        length = b.length;
        box = b.box;
        center = b.center;
        hole = b.hole;
        contour = b.contour;

        id = -1;        
        deltaArea = 0.0f;
        markedForDeletion = false;
        framesLeft = 0;
    }
    
    


    int getLowestError() {
        int best=-1;
        float best_v=99999.0f;
        for( unsigned int i=0; i<error.size(); i++ ) {
            if( error[i] < best_v ) {
                best = i;
                best_v = error[i];
            }
        }
        return best;
    }


};


#endif /* TRACKEDBLOB_H */

