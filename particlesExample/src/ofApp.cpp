#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);

	int num = 1500; //TODO: move hard coded values into GUI
	p.assign(num, demoParticle());
	currentMode = PARTICLE_MODE_ATTRACT;//TODO: move hard coded values into GUI

	currentModeStr = "1 - PARTICLE_MODE_ATTRACT: attracts to mouse"; //TODO: move hard coded values into GUI

	resetParticles();

	//initialize kinect object
	//TODO: only initialize necessary sources
	kinect.open();
	kinect.initDepthSource();
	kinect.initColorSource();
	kinect.initInfraredSource();
	kinect.initBodySource();
	kinect.initBodyIndexSource();

	//set debugging variables
	debugging = false;//TODO: put this into the GUI
	previewScaleH = 1.0f;//TODO: put this into the GUI
	previewScaleW = 1.0f;//TODO: put this into the GUI

	//TODO: setup gui & text instructions for keypresses, etc.
	// change color settings
	// set mode to debugging

}

//--------------------------------------------------------------
void ofApp::resetParticles() {

	//these are the attraction points used in the forth demo 
	//attractPoints.clear();
	//attractPoints = hands;
	//for(int i = 0; i < 4; i++){ //TODO: move hard coded values into GUI
		//attractPoints.push_back( ofPoint( ofMap(i, 0, 4, 100, ofGetWidth()-100) , ofRandom(100, ofGetHeight()-100) ) );
	//}

	//attractPointsWithMovement = attractPoints;

	for (unsigned int i = 0; i < p.size(); i++) {
		p[i].setMode(currentMode);
		p[i].setAttractPoints(&attractPointsWithMovement);
		if (attractPointsWithMovement.size() > 0) {
			p[i].setAttractPoint(&attractPointsWithMovement[attactionHandID]);
		} else {
			p[i].setAttractPoint(NULL);
		}
		p[i].reset();
	}	
}

//--------------------------------------------------------------
void ofApp::update(){
	/////////////////
	// Kinect
	///////////////////
	kinect.update();

	//Getting joint positions (skeleton tracking)
	{
		auto bodies = kinect.getBodySource()->getBodies();
		hands.clear();
		for (auto body : bodies) {
			if (body.tracked) {
				//TODO: now do something with the joints

				// turn hands into attractors / repellants
				//auto bID = body.bodyId; // TODO: capture instance of a particular body to act as attractor for single attractors
				
				hands.push_back(body.joints[JointType_HandTipLeft].getPositionInDepthMap()*depthMapScale); // TODO: figure out projective position getting
				hands.push_back(body.joints[JointType_HandTipRight].getPositionInDepthMap()*depthMapScale); // TODO: figure out projective position getting

				//for (auto joint : body.joints) {
				//}
			}
		}
	}

	//Getting bones (connected joints)
	{
		// Note that for this we need a reference of which joints are connected to each other.
		// We call this the 'boneAtlas', and you can ask for a reference to this atlas whenever you like
		auto bodies = kinect.getBodySource()->getBodies();
		auto boneAtlas = ofxKinectForWindows2::Data::Body::getBonesAtlas();

		for (auto body : bodies) {
			for (auto bone : boneAtlas) {
				auto firstJointInBone = body.joints[bone.first];
				auto secondJointInBone = body.joints[bone.second];

				//TODO: now do something with the joints
			}
		}
	}

	//////////////////
	// Particles
	///////////////////
	for(unsigned int i = 0; i < p.size(); i++){
		p[i].setMode(currentMode);
		p[i].update();
	}

	attractPoints = hands;
	if (attractPoints.size() != attractPointsWithMovement.size()) {
		attractPointsWithMovement.resize(attractPoints.size());
		attactionHandID = ofRandom(hands.size());
		resetParticles();
	}

	//lets add a bit of movement to the attract points
	if (attractPoints.size() > 0) {
		for (unsigned int i = 0; i < attractPointsWithMovement.size(); i++) {
			attractPointsWithMovement[i].x = attractPoints[i].x + ofSignedNoise(i * 10, ofGetElapsedTimef() * 0.7) * 12.0;//TODO: move hard coded values into GUI
			attractPointsWithMovement[i].y = attractPoints[i].y + ofSignedNoise(i * -10, ofGetElapsedTimef() * 0.7) * 12.0;//TODO: move hard coded values into GUI
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(60,60,60), ofColor(10,10,10));//TODO: move hard coded values into GUI

	for(unsigned int i = 0; i < p.size(); i++){
		p[i].draw();
	}
	
	ofSetColor(190);
	if( currentMode == PARTICLE_MODE_NEAREST_POINTS ){
		if (attractPoints.size() > 0 && debugging) {
			for (unsigned int i = 0; i < attractPoints.size(); i++) {
				ofNoFill();
				ofDrawCircle(attractPointsWithMovement[i], 10);//TODO: move hard coded values into GUI
				ofFill();
				ofDrawCircle(attractPointsWithMovement[i], 4);//TODO: move hard coded values into GUI
			}
		}
	}

	ofSetColor(230);	//TODO: move hard coded values into GUI
	ofDrawBitmapString(currentModeStr + "\n\nSpacebar to reset. \nKeys 1-4 to change mode.", 10, 20);//TODO: move hard coded values into GUI

	ofPushStyle();
	//TODO: move these hardcoded numbers into GUI
	ofEnableBlendMode(OF_BLENDMODE_SCREEN);//TODO: move hard coded values into GUI
	ofSetColor(127);//TODO: move hard coded values into GUI
	if (debugging) {//TODO: move hard coded values into GUI

		//kinect.getDepthSource()->draw(0, 0, previewWidth, previewHeight);  // note that the depth texture is RAW so may appear dark
																		   // Color is at 1920x1080 instead of 512x424 so we should fix aspect ratio
		//float colorHeight = previewWidth * (kinect.getColorSource()->getHeight() / kinect.getColorSource()->getWidth());
		//float colorTop = (previewHeight - colorHeight) / 2.0;

		//kinect.getColorSource()->draw(previewWidth, 0 + colorTop, previewWidth, colorHeight);
		//kinect.getBodySource()->drawProjected(previewWidth, 0 + colorTop, previewWidth, colorHeight);

		//kinect.getInfraredSource()->draw(0, previewHeight, previewWidth, previewHeight);

		kinect.getBodyIndexSource()->draw(0, 0, previewWidth, previewHeight);
		kinect.getBodySource()->drawProjected(0, 0, previewWidth, previewHeight, ofxKFW2::ProjectionCoordinates::DepthCamera);

		//TODO: add debug instrcutions gui & text
		// change color settings
		// set mode to debugging
	}
	ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){ //TODO: move key presses into GUI
	switch (key) {

	case '1':
		currentMode = PARTICLE_MODE_ATTRACT;
		currentModeStr = "1 - PARTICLE_MODE_ATTRACT: attracts to mouse";
		break;

	case '2':
		currentMode = PARTICLE_MODE_REPEL;
		currentModeStr = "2 - PARTICLE_MODE_REPEL: repels from mouse";
		break;

	case '3':
		currentMode = PARTICLE_MODE_NEAREST_POINTS;
		currentModeStr = "3 - PARTICLE_MODE_NEAREST_POINTS: hold 'f' to disable force";
		break;

	case '4':
		currentMode = PARTICLE_MODE_NOISE;
		currentModeStr = "4 - PARTICLE_MODE_NOISE: snow particle simulation";
		resetParticles();
		break;

	case ' ':
		resetParticles();
		break;

	case 'd':
	case 'D':
		//hitting space key swaps the camera view
		debugging = !debugging;
		break;

	case 'f':
	case 'F':
		ofToggleFullscreen();
		break;

	default :
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	previewWidth = ofGetWindowWidth() * previewScaleW;
	previewHeight = ofGetWindowHeight() * previewScaleH;
	float depthMapScaleW = previewWidth / 512.0f;
	float depthMapScaleH = previewHeight / 424.0f;
	depthMapScale = ofVec3f(depthMapScaleH, depthMapScaleW, (depthMapScaleH + depthMapScaleW) / 2.0f);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
