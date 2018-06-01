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

		// guis
		void reloadLook();
		void resetGuiPositions();
		void guiEvent(ofxUIEventArgs &e);
		ofxUISuperCanvas *gui;
		ofxUISuperCanvas *guiColor;
		int currentLookBank, currentLook, gradientType;
		float transDuration, transEase, transD, transSteps, transStatus;

		bool lookChanged;
		bool lookBankChanged;
		ofImage screenshot;
		
		// particles
		particleMode currentMode;
		string currentModeStr; 
		vector <demoParticle> particles;
		vector <ofPoint> hands;
		int attractionHandID;
		vector <ofPoint> attractPoints;
		vector <ofPoint> attractPointsWithMovement;
		float partRmin, partRmax;

		// ribbons
		///////////////
		vector<ofVec3f> points; //this holds all of our points
		vector < vector < vector <ofVec3f> > > ribbons;
		int pointsMax;		
		ofVec3f center; //this keeps track of the center of all the points
		ofCamera camera; //our camera objects for looking at the scene from multiple perspectives
		bool usecamera; //if usecamera is true, we'll turn on the camera view
		vector <int> bodyDepthOrder;


		// kinect device reference
		ofxKFW2::Device kinect;

private: //debugging variables

	bool debugging;
	int previewWidth;
	int previewHeight;
	float previewScaleW;
	float previewScaleH;
	ofVec3f depthMapScale;
	int drawFrameRate;

	// draw flags
	bool drawBodyIndex, drawParticles, drawRibbons, drawMirrored, drawBones;
	bool drawBodyIndex_to, drawParticles_to, drawRibbons_to, drawMirrored_to, drawBones_to;
	bool drawBodyIndex_from, drawParticles_from, drawRibbons_from, drawMirrored_from, drawBones_from;

	// bgColors
	ofColor bgColor;
	float bgRed, bgGreen, bgBlue;
	ofColor bgGradient;
	float bgGradRed, bgGradGreen, bgGradBlue;
	float bgRed_to, bgGreen_to, bgBlue_to;
	float bgGradRed_to, bgGradGreen_to, bgGradBlue_to;
	float bgRed_from, bgGreen_from, bgBlue_from;
	float bgGradRed_from, bgGradGreen_from, bgGradBlue_from;

	// fgColors
	ofColor fgColor;
	float fgRed, fgGreen, fgBlue, fgAlpha;
	ofColor indexColor;
	float indexRed, indexGreen, indexBlue, indexAlpha;
	ofColor skelColor;
	float skelRed, skelGreen, skelBlue, skelAlpha;
	float fgRed_to, fgGreen_to, fgBlue_to, fgAlpha_to;
	float indexRed_to, indexGreen_to, indexBlue_to, indexAlpha_to;
	float skelRed_to, skelGreen_to, skelBlue_to, skelAlpha_to;
	float fgRed_from, fgGreen_from, fgBlue_from, fgAlpha_from;
	float indexRed_from, indexGreen_from, indexBlue_from, indexAlpha_from;
	float skelRed_from, skelGreen_from, skelBlue_from, skelAlpha_from;

	// blend & color modes
	int fgBlendMode, indexColorMode, indexBlendMode, skelBlendMode;
	int fgBlendMode_to, indexColorMode_to, indexBlendMode_to, skelBlendMode_to;
	int fgBlendMode_from, indexColorMode_from, indexBlendMode_from, skelBlendMode_from;

};
