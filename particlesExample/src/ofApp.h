#pragma once

#include "ofMain.h"
#include "demoParticle.h"
#include "ofxKinectForWindows2.h"
#include "ofxUI.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void resetParticles(bool posReset);

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void guiEvent(ofxUIEventArgs &e);
		ofxUISuperCanvas *gui;
		ofxUISuperCanvas *guiColor;
		bool lookChanged;
		
		particleMode currentMode;
		string currentModeStr; 

		vector <demoParticle> particles;
		vector <ofPoint> hands;
		int attactionHandID;
		vector <ofPoint> attractPoints;
		vector <ofPoint> attractPointsWithMovement;

		//kinect device reference
		ofxKFW2::Device kinect;

private:

	//debugging variables
	bool debugging;
	int previewWidth;
	int previewHeight;
	float previewScaleW;
	float previewScaleH;
	ofVec3f depthMapScale;
	int drawFrameRate;
	bool drawBodyIndex, drawBones;
	ofColor bgColor;
	float bgRed, bgGreen, bgBlue;
	ofColor bgGradient;
	float bgGradRed, bgGradGreen, bgGradBlue;
	ofColor fgColor;
	float fgRed, fgGreen, fgBlue, fgAlpha;
	ofColor indexColor;
	float indexRed, indexGreen, indexBlue, indexAlpha;
	ofColor skelColor;
	float skelRed, skelGreen, skelBlue, skelAlpha;

};
