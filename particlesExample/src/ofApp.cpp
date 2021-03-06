#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);

	int num = 1500; //TODO: move hard coded values into GUI
	particles.assign(num, demoParticle());
	currentMode = PARTICLE_MODE_NOISE;//TODO: move hard coded values into GUI

	currentModeStr = "1 - PARTICLE_MODE_ATTRACT: attracts to mouse"; //TODO: move hard coded values into GUI

	resetParticles(true);

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

	/////////////////////
	// Initialize GUIS //
	/////////////////////
	lookChanged = true; // forces GUI to load settings
	//
	//------------
	// Main GUI --
	//////////////
	gui = new ofxUISuperCanvas("preSENCE part");
	ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
	gui->addSpacer();
	gui->addTextArea("text", "'h' to hide this panel", OFX_UI_FONT_SMALL);
	//    gui -> addLabel("text", "'h' to hide this panel");
	gui->addSpacer();
	//
	// radio list and key bindings to select different looks
	gui->addLabel("looks: "); // TODO: Make this more like the editors where there's a "look Editor" that handles larger settings files which can be changed between
	vector<string> looksList; 
	for (int i = 0; i < 4; i++) {
		looksList.push_back(ofToString(i + 1));
	}
	ofxUIRadio *radioLook = gui->addRadio("look", looksList, OFX_UI_ORIENTATION_HORIZONTAL);
	vector< ofxUIToggle*> toggles = radioLook->getToggles();
	for (int i = 0; i < 4; i++) {
		toggles[i]->bindToKey(ofToChar( ofToString(i + 1) ));
	}
	gui->addTextArea("text", "press '1', '2', etc. to switch between different looks", OFX_UI_FONT_SMALL);
	gui->addSpacer();
	//
	// FPS
	gui->addFPSSlider("fps");
	gui->addSpacer();
	gui->addTextArea("text", "'+' or '-' to change frame rate");
	gui->addIntSlider("set fps", 1, 60, &drawFrameRate);
	gui->addSpacer();
	//
	// Background Color
	gui->addTextArea("text", "background color");
	gui->addSlider("red", 0.0, 255.0, &bgRed);
	gui->addSlider("green", 0.0, 255.0, &bgGreen);
	gui->addSlider("blue", 0.0, 255.0, &bgBlue);
	gui->addSpacer();
	//
	// Background Gradient Color
	gui->addTextArea("text", "gradient color");
	gui->addSlider("gradRed", 0.0, 255.0, &bgGradRed);
	gui->addSlider("gradGreen", 0.0, 255.0, &bgGradGreen);
	gui->addSlider("gradBlue", 0.0, 255.0, &bgGradBlue);
	gui->addSpacer();
	//
	// fullscreen toggle
	ofxUIToggle *toggleFullScreen = gui->addToggle("fullscreen", false);
	toggleFullScreen->bindToKey('f');
	toggleFullScreen->bindToKey('F');
	//
	//
	// draw index toggle
	ofxUIToggle* toggleIndex = gui->addToggle("body index", &drawBodyIndex);
	toggleIndex->bindToKey('i');
	toggleIndex->bindToKey('I');
	//
	// draw bones toggle
	ofxUIToggle* toggleBones = gui->addToggle("bones", &drawBones);
	toggleBones->bindToKey('b');
	toggleBones->bindToKey('B');
	gui->addSpacer();
	//
	// debugging toggle
	ofxUIToggle* toggleDebugging = gui->addToggle("debugging", &debugging);
	toggleDebugging->bindToKey('d');
	toggleDebugging->bindToKey('D');
	//
	// hard reset particles
	ofxUIButton *buttonReset = gui->addButton("hardreset", false);
	buttonReset->bindToKey('r');
	buttonReset->bindToKey('R');
	//
	// save Settings
	gui->addLabelButton("save main settings", false);
	gui->autoSizeToFitWidgets();


	//
	//-------------
	// Color GUI --
	///////////////

	guiColor = new ofxUISuperCanvas("preSENSE colors");
	ofAddListener(guiColor->newGUIEvent, this, &ofApp::guiEvent);
	guiColor->addSpacer();
	//
	guiColor->addLabel("foreground color settings", OFX_UI_FONT_MEDIUM);
	vector< string > vnamesBlendIMG; vnamesBlendIMG.push_back("i0"); vnamesBlendIMG.push_back("iA"); vnamesBlendIMG.push_back("i+"); vnamesBlendIMG.push_back("i-"); vnamesBlendIMG.push_back("i*"); vnamesBlendIMG.push_back("iS");
	ofxUIRadio *radioBlendIMG = guiColor->addRadio("foreground blend mode", vnamesBlendIMG, OFX_UI_ORIENTATION_HORIZONTAL);
	guiColor->addSlider("fg red", 0.0, 255.0, &fgRed);
	guiColor->addSlider("fg green", 0.0, 255.0, &fgGreen);
	guiColor->addSlider("fg blue", 0.0, 255.0, &fgBlue);
	guiColor->addSlider("fg alpha", 0.0, 255.0, &fgAlpha);
	guiColor->addSpacer();
	//
	guiColor->addLabel("index image settings", OFX_UI_FONT_MEDIUM);
	vector< string > vnamesDepthCLR; vnamesDepthCLR.push_back("PSYCHEDELIC_SHADES"); vnamesDepthCLR.push_back("PSYCHEDELIC"); vnamesDepthCLR.push_back("RAINBOW"); vnamesDepthCLR.push_back("CYCLIC_RAINBOW"); vnamesDepthCLR.push_back("BLUES"); vnamesDepthCLR.push_back("BLUES_INV"); vnamesDepthCLR.push_back("GREY"); vnamesDepthCLR.push_back("STATUS");
	ofxUIRadio *radioMode = guiColor->addRadio("index color mode", vnamesDepthCLR, OFX_UI_ORIENTATION_VERTICAL);
	vector< string > vnamesBlendDEPTH; vnamesBlendDEPTH.push_back("d0"); vnamesBlendDEPTH.push_back("dA"); vnamesBlendDEPTH.push_back("d+"); vnamesBlendDEPTH.push_back("d-"); vnamesBlendDEPTH.push_back("d*"); vnamesBlendDEPTH.push_back("dS");
	ofxUIRadio *radioBlendDepth = guiColor->addRadio("index blend mode", vnamesBlendDEPTH, OFX_UI_ORIENTATION_HORIZONTAL);
	guiColor->addSlider("index red", 0.0, 255.0, &indexRed);
	guiColor->addSlider("index green", 0.0, 255.0, &indexGreen);
	guiColor->addSlider("index blue", 0.0, 255.0, &indexBlue);
	guiColor->addSlider("index alpha", 0.0, 255.0, &indexAlpha);
	guiColor->addSpacer();
	//
	guiColor->addLabel("skeleton drawing settings", OFX_UI_FONT_MEDIUM);
	vector< string > vnamesBlendSKEL; vnamesBlendSKEL.push_back("s0"); vnamesBlendSKEL.push_back("sA"); vnamesBlendSKEL.push_back("s+"); vnamesBlendSKEL.push_back("s-"); vnamesBlendSKEL.push_back("s*"); vnamesBlendSKEL.push_back("sS");
	ofxUIRadio *radioBlendSkel = guiColor->addRadio("skeleton blend mode", vnamesBlendSKEL, OFX_UI_ORIENTATION_HORIZONTAL);
	guiColor->addSlider("skel red", 0.0, 255.0, &skelRed);
	guiColor->addSlider("skel green", 0.0, 255.0, &skelGreen);
	guiColor->addSlider("skel blue", 0.0, 255.0, &skelBlue);
	guiColor->addSlider("skel alpha", 0.0, 255.0, &skelAlpha);
	guiColor->addSpacer();
	//
	// Save Settings
	guiColor->addLabelButton("save color settings", false);
	guiColor->autoSizeToFitWidgets();

}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e) {
	//*
	bool noCallbackForWidget = false;
	string nameStr = e.widget->getName();
	int kind = e.widget->getKind();

	if (nameStr == "fullscreen") {
		ofSetFullscreen(((ofxUIToggle *)e.widget)->getValue());
	}

	else if (nameStr == "save main settings") {
		gui->saveSettings("guiSettings_" + ofToString(currentMode) + ".xml");
	}
	else if (nameStr == "save color settings") {
		guiColor->saveSettings("guiSettings_" + ofToString(currentMode) + "_color.xml");
	}

	else if (nameStr == "look" || nameStr == "1" || nameStr == "2" || nameStr == "3" || nameStr == "4") {
		ofxUIRadio *radioLook;
		if (kind == OFX_UI_WIDGET_RADIO) radioLook = (ofxUIRadio *)e.widget;
		else radioLook = (ofxUIRadio *)e.widget->getParent();
		switch (radioLook->getValue()) {

		case 0: // 1
			currentMode = PARTICLE_MODE_ATTRACT;
			currentModeStr = "1 - PARTICLE_MODE_ATTRACT: attracts to mouse";
			break;

		case 1: // 2
			currentMode = PARTICLE_MODE_REPEL;
			currentModeStr = "2 - PARTICLE_MODE_REPEL: repels from mouse";
			break;

		case 2: // 3
			currentMode = PARTICLE_MODE_NEAREST_POINTS;
			currentModeStr = "3 - PARTICLE_MODE_NEAREST_POINTS: hold 'n' to disable force";
			break;

		case 3: // 4
			currentMode = PARTICLE_MODE_NOISE;
			currentModeStr = "4 - PARTICLE_MODE_NOISE: snow particle simulation";
			resetParticles(false);
			break;

		default:
			break;
		}

		lookChanged = true;
	}

	else if (nameStr == "hardreset") {
		resetParticles(true);
	}
	else {
		// default
		noCallbackForWidget = true;
	}

	// debug
	if (ofGetLogLevel() == OF_LOG_VERBOSE) {
		if (noCallbackForWidget) {
			cout << "[verbose] ofApp::guiEvent(ofxUIEventArgs &e) -- unset callback for gui element name = " << nameStr << endl;
		}
		else {
			cout << "[verbose] ofApp::guiEvent(ofxUIEventArgs &e) -- gui element name = " << nameStr << endl;
		}
	}
}

//--------------------------------------------------------------
void ofApp::resetParticles(bool posReset = false) {

	//these are the attraction points used in the forth demo 
	//attractPoints.clear();
	//attractPoints = hands;
	//for(int i = 0; i < 4; i++){ //TODO: move hard coded values into GUI
		//attractPoints.push_back( ofPoint( ofMap(i, 0, 4, 100, ofGetWidth()-100) , ofRandom(100, ofGetHeight()-100) ) );
	//}
	//attractPointsWithMovement = attractPoints;

	for (unsigned int i = 0; i < particles.size(); i++) {
		particles[i].setMode(currentMode);
		particles[i].setAttractPoints(&attractPointsWithMovement);
		if (attractPointsWithMovement.size() > 0) {
			particles[i].setAttractPoint(&attractPointsWithMovement[attractionHandID]);
		} else {
			particles[i].setAttractPoint(NULL);
		}
		particles[i].reset(posReset);
	}	
}

//--------------------------------------------------------------
void ofApp::update(){

	fgColor = ofColor(fgRed, fgGreen, fgBlue, fgAlpha);
	bgColor = ofColor(bgRed, bgGreen, bgBlue);
	bgGradient = ofColor(bgGradRed, bgGradGreen, bgGradBlue);

	indexColor = ofColor(indexRed, indexBlue, indexGreen);
	skelColor = ofColor(skelRed, skelGreen, skelBlue, skelAlpha);


	if (lookChanged) {
		gui->loadSettings("guiSettings_" + ofToString(currentMode) + ".xml");
		guiColor->loadSettings("guiSettings_" + ofToString(currentMode) + "_color.xml");
		lookChanged = false;
	}

	// GUI
	/////////
	drawFrameRate = ofGetFrameRate();

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
	for(unsigned int i = 0; i < particles.size(); i++){
		particles[i].setMode(currentMode);
		particles[i].update();
	}

	attractPoints = hands;
	if (attractPoints.size() != attractPointsWithMovement.size()) {
		attractPointsWithMovement.resize(attractPoints.size());
		attractionHandID = ofRandom(hands.size());
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
    ofBackgroundGradient(bgGradient, bgColor);
	
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
	if (debugging) {//TODO: move hard coded values into GUI

		//kinect.getDepthSource()->draw(0, 0, previewWidth, previewHeight);  // note that the depth texture is RAW so may appear dark
																		   // Color is at 1920x1080 instead of 512x424 so we should fix aspect ratio
		//float colorHeight = previewWidth * (kinect.getColorSource()->getHeight() / kinect.getColorSource()->getWidth());
		//float colorTop = (previewHeight - colorHeight) / 2.0;

		//kinect.getColorSource()->draw(previewWidth, 0 + colorTop, previewWidth, colorHeight);
		//kinect.getBodySource()->drawProjected(previewWidth, 0 + colorTop, previewWidth, colorHeight);

		//kinect.getInfraredSource()->draw(0, previewHeight, previewWidth, previewHeight);

		//TODO: add debug instrcutions gui & text
		// change color settings
		// set mode to debugging
	}
	if (debugging || drawBodyIndex) {
		ofSetColor(indexColor);
		kinect.getBodyIndexSource()->draw(0, 0, previewWidth, previewHeight);
	}

	if (debugging || drawBones) {
		ofSetColor(skelColor);
		kinect.getBodySource()->drawProjected(0, 0, previewWidth, previewHeight, ofxKFW2::ProjectionCoordinates::DepthCamera);
	}
	ofPopStyle();

	ofSetColor(fgColor);
	for (unsigned int i = 0; i < particles.size(); i++) {
		particles[i].draw();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){ //TODO: move key presses into GUI
	switch (key) {

	case 'h':
	case 'H':
		gui->toggleVisible();
		guiColor->toggleVisible();
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
	depthMapScale = ofVec3f(depthMapScaleW, depthMapScaleH, (depthMapScaleH + depthMapScaleW) / 2.0f);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
