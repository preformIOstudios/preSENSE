/**
 *
 * OFDevCon Example Code Sprint
 * Camera Ribbon example
 * This example generates ribbons along the mouse trail that descend in space
 * When you click space bar, you can
 *
 * Created by James George for openFrameworks workshop at Waves Festival Vienna sponsored by Lichterloh and Pratersauna
 * Adapted during ofDevCon on 2/23/2012
 */


#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	//just set up the openFrameworks stuff
    ofSetFrameRate(60);//TODO: put this into the GUI
    ofSetVerticalSync(true);

	//initialize the variable so it's off at the beginning
    usecamera = false;
	pointsMax = 100; //TODO: put this into the GUI

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
	currentLook = 1;
	lookChanged = true;
	//
	//------------
	// Main GUI --
	//////////////
	gui = new ofxUISuperCanvas("preSENCE ribb");
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
		toggles[i]->bindToKey(ofToChar(ofToString(i + 1)));
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
	// hard reset
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
		gui->saveSettings("guiSettings_" + ofToString(currentLook) + ".xml");
	}
	else if (nameStr == "save color settings") {
		guiColor->saveSettings("guiSettings_" + ofToString(currentLook) + "_color.xml");
	}

	else if (nameStr == "look" || nameStr == "1" || nameStr == "2" || nameStr == "3" || nameStr == "4") {
		ofxUIRadio *radioLook;
		if (kind == OFX_UI_WIDGET_RADIO) radioLook = (ofxUIRadio *)e.widget;
		else radioLook = (ofxUIRadio *)e.widget->getParent();
		switch (radioLook->getValue()) {

		case 0: // 1
			currentLook = 1;
			//currentLookStr = "1 - PARTICLE_MODE_ATTRACT: attracts to mouse";
			break;

		case 1: // 2
			currentLook = 2;
			//currentLookStr = "2 - PARTICLE_MODE_REPEL: repels from mouse";
			break;

		case 2: // 3
			currentLook = 3;
			//currentLookStr = "3 - PARTICLE_MODE_NEAREST_POINTS: hold 'n' to disable force";
			break;

		case 3: // 4
			currentLook = 4;
			//currentLookStr = "4 - PARTICLE_MODE_NOISE: snow particle simulation";
			//resetParticles(false);
			break;

		default:
			break;
		}

		lookChanged = true;
	}

	else if (nameStr == "hardreset") {
		//resetParticles(true);
	}
	else if (nameStr == "foreground blend mode" || nameStr == "i0" || nameStr == "iA" || nameStr == "i+" || nameStr == "i-" || nameStr == "i*" || nameStr == "iS") {
		ofxUIRadio *radio;
		if (nameStr == "foreground blend mode") {
			radio = (ofxUIRadio *)e.widget;
		}
		else {
			radio = (ofxUIRadio *)e.widget->getParent();
		}
		fgBlendMode = radio->getValue();
	}
	else if (nameStr == "index color mode" || nameStr == "PSYCHEDELIC_SHADES" || nameStr == "PSYCHEDELIC" || nameStr == "RAINBOW" || nameStr == "CYCLIC_RAINBOW" || nameStr == "BLUES" || nameStr == "BLUES_INV" || nameStr == "GREY" || nameStr == "STATUS") {
		ofxUIRadio *radio;
		if (nameStr == "index color mode") {
			radio = (ofxUIRadio *)e.widget;
		}
		else {
			radio = (ofxUIRadio *)e.widget->getParent();
		}
		indexColorMode = radio->getValue();
		//kinect.setDepthColoring((DepthColoring)indexColorMode); // TODO: is this even possible with ofxKFW2?
	}
	else if (nameStr == "index blend mode" || nameStr == "d0" || nameStr == "dA" || nameStr == "d+" || nameStr == "d-" || nameStr == "d*" || nameStr == "dS") {
		ofxUIRadio *radio;
		if (nameStr == "index blend mode") {
			radio = (ofxUIRadio *)e.widget;
		}
		else {
			radio = (ofxUIRadio *)e.widget->getParent();
		}
		indexBlendMode = radio->getValue();
	}
	else if (nameStr == "skeleton blend mode" || nameStr == "s0" || nameStr == "sA" || nameStr == "s+" || nameStr == "s-" || nameStr == "s*" || nameStr == "sS") {
		ofxUIRadio *radio;
		if (nameStr == "skeleton blend mode") {
			radio = (ofxUIRadio *)e.widget;
		}
		else {
			radio = (ofxUIRadio *)e.widget->getParent();
		}
		skelBlendMode = radio->getValue();
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
void ofApp::update(){

	fgColor = ofColor(fgRed, fgGreen, fgBlue, fgAlpha);
	bgColor = ofColor(bgRed, bgGreen, bgBlue);
	bgGradient = ofColor(bgGradRed, bgGradGreen, bgGradBlue);

	indexColor = ofColor(indexRed, indexBlue, indexGreen);
	skelColor = ofColor(skelRed, skelGreen, skelBlue, skelAlpha);

	// GUI load settings when state changes
	if (lookChanged) {
		gui->loadSettings("guiSettings_" + ofToString(currentLook) + ".xml");
		guiColor->loadSettings("guiSettings_" + ofToString(currentLook) + "_color.xml");
		lookChanged = false;
	}
	drawFrameRate = ofGetFrameRate();

	//don't move the points if we are using the camera
    if(!usecamera){
        ofVec3f sumOfAllPoints(0,0,0);
		int pointCount = 0;
        for(unsigned int i = 0; i < points.size(); i++){
            points[i].z -= 4;
            sumOfAllPoints += points[i];
			pointCount += 1;
        }

		for (auto body : ribbons) {
			for (auto ribbon : body) {
				for (unsigned int i = 0; i < ribbon.size(); i++) {
					ribbon[i].z += 4; // TODO: figure out why this isn't working
					sumOfAllPoints += ribbon[i];
					pointCount += 1;
				}
			}
		}

        center = sumOfAllPoints / pointCount;
    }

	/////////////////
	// Kinect
	///////////////////
	kinect.update();

	//Getting joint positions (skeleton tracking)
	/*
	{
		auto bodies = kinect.getBodySource()->getBodies();
		for (auto body : bodies) {
			for (auto joint : body.joints) {
				//TODO: now do something with the joints
			}
		}
	}
	//*/
	//
	//Getting bones (connected joints)
	{
		// Note that for this we need a reference of which joints are connected to each other.
		// We call this the 'boneAtlas', and you can ask for a reference to this atlas whenever you like
		auto bodies = kinect.getBodySource()->getBodies();
		auto boneAtlas = ofxKinectForWindows2::Data::Body::getBonesAtlas();

		ribbons.resize(bodies.size());
		int bodyIDX = 0;
		vector <float> bodyDepths;
		for (auto body : bodies) {
			if (body.tracked) {
				bodyDepths.push_back(body.joints[JointType_SpineBase].getPositionInWorld().z);
				ribbons[bodyIDX].resize(boneAtlas.size());
				int boneIDX = 0; // each bone gets a ribbon (for now)
				for (auto bone : boneAtlas) {

					auto firstJointInBone = body.joints[bone.first];
					auto secondJointInBone = body.joints[bone.second];
					ofVec3f firstJPos = firstJointInBone.getPositionInDepthMap();
					ofVec3f secondJPos = secondJointInBone.getPositionInDepthMap();

					firstJPos.z = firstJointInBone.getPosition().z;
					secondJPos.z = secondJointInBone.getPosition().z;
					firstJPos *= depthMapScale;
					secondJPos *= depthMapScale;

					//store joint positions for ribbon drawing later on
					if (ribbons[bodyIDX][boneIDX].size() <= pointsMax -2) {
						ribbons[bodyIDX][boneIDX].push_back(firstJPos);
						ribbons[bodyIDX][boneIDX].push_back(secondJPos);
					} else {
						for (int i = 0; i < pointsMax - 2; i += 2) {
							ribbons[bodyIDX][boneIDX][i] = ribbons[bodyIDX][boneIDX][i + 2];
							ribbons[bodyIDX][boneIDX][i+1] = ribbons[bodyIDX][boneIDX][i + 3];
						}
						ribbons[bodyIDX][boneIDX][pointsMax - 2] = firstJPos;
						ribbons[bodyIDX][boneIDX][pointsMax - 1] = secondJPos;
					}
					boneIDX += 1;
				}
			} else {
				ribbons[bodyIDX].resize(0);
				bodyDepths.push_back(-1);
			}
			bodyIDX += 1;
		}
		//cout << "ofApp :: update () -- bodyDepthOrder = " + ofToString(bodyDepthOrder) << endl;
		//cout << "ofApp :: update () -- bodyDepths = " + ofToString(bodyDepths) << endl;
		// TODO: get / create a depth-sorted version of this list
		bodyDepthOrder.clear();
		for (int i = 0; i < bodyDepths.size(); i++) {
			float depth = bodyDepths[i];
			if (bodyDepthOrder.size() == 0) {
				bodyDepthOrder.push_back(i);
			} else if (depth > bodyDepthOrder.back()) {
				bodyDepthOrder.push_back(i);
			} else if (depth < bodyDepthOrder.front()) {
				bodyDepthOrder.insert(bodyDepthOrder.begin(), i);
			} else {
				for (int idx = 1; idx < bodyDepthOrder.size(); idx++) {
					if (depth < bodyDepthOrder[idx]) {
						bodyDepthOrder.insert(bodyDepthOrder.begin() + idx, i);
						break;
					}
				}
			}
		}
		//cout << "ofApp :: update () -- bodyDepthOrder (sorted) = " + ofToString(bodyDepthOrder) << endl;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackgroundGradient(bgGradient, bgColor, OF_GRADIENT_LINEAR); //todo: put gradient mode into UI
	//if we're using the camera, start it.
	//everything that you draw between begin()/end() shows up from the view of the camera
    if(usecamera){
        camera.begin();
    }

	ofPushStyle();
	if (debugging) {

		ofSetColor(fgColor);
		ofEnableBlendMode((ofBlendMode)fgBlendMode);

		//do the same thing from the first example...
		ofMesh mesh;
		mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
		for (unsigned int i = 1; i < points.size(); i++) {

			//find this point and the next point
			ofVec3f thisPoint = points[i - 1];
			ofVec3f nextPoint = points[i];

			//get the direction from one to the next.
			//the ribbon should fan out from this direction
			ofVec3f direction = (nextPoint - thisPoint);

			//get the distance from one point to the next
			float distance = direction.length();

			//get the normalized direction. normalized vectors always have a length of one
			//and are really useful for representing directions as opposed to something with length
			ofVec3f unitDirection = direction.getNormalized();

			//find both directions to the left and to the right
			ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0, 0, 1));
			ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0, 0, 1));

			//use the map function to determine the distance.
			//the longer the distance, the narrower the line.
			//this makes it look a bit like brush strokes
			float thickness = ofMap(distance, 0, 60, 20, 2, true);//TODO: put these constants into the GUI
																  //TODO: have tail shrink towards end so it disappears

			//calculate the points to the left and to the right
			//by extending the current point in the direction of left/right by the length
			ofVec3f leftPoint = thisPoint + toTheLeft*thickness;
			ofVec3f rightPoint = thisPoint + toTheRight*thickness;

			//add these points to the triangle strip
			mesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
			mesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
		}

		//end the shape
		mesh.draw();
	}
	ofPopStyle();

	ofPushStyle();
	// TODO: sort so foremost bodies appear foremost
	// draw ribbons
	for (unsigned int bodyIDX = 0; bodyIDX < bodyDepthOrder.size(); bodyIDX++) {
		ofSetColor(fgColor / (bodyDepthOrder.size() -bodyIDX));
		ofEnableBlendMode((ofBlendMode)fgBlendMode);
		for (unsigned int boneIDX = 0; boneIDX < ribbons[bodyDepthOrder[bodyIDX]].size(); boneIDX++) {
			ofMesh meshRibbon;	
			meshRibbon.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
			for (unsigned int point = 0; point < ribbons[bodyDepthOrder[bodyIDX]][boneIDX].size(); point++) {
				//add each joint to the triangle strip
				meshRibbon.addVertex(ribbons[bodyDepthOrder[bodyIDX]][boneIDX][point]);
			}

			//end the shape
			meshRibbon.draw();
		}
	}
	ofPopStyle();

	//if we're using the camera, take it away
    if(usecamera){
    	camera.end();
    }

	ofPushStyle();
	//TODO: move these hardcoded numbers into GUI
	if (debugging) {
		//kinect.getDepthSource()->draw(0, 0, previewWidth, previewHeight);  // note that the depth texture is RAW so may appear dark

		// Color is at 1920x1080 instead of 512x424 so we should fix aspect ratio
		//float colorHeight = previewWidth * (kinect.getColorSource()->getHeight() / kinect.getColorSource()->getWidth());
		//float colorTop = (previewHeight - colorHeight) / 2.0;

		//kinect.getColorSource()->draw(previewWidth, 0 + colorTop, previewWidth, colorHeight);
		//kinect.getBodySource()->drawProjected(previewWidth, 0 + colorTop, previewWidth, colorHeight);

		//kinect.getInfraredSource()->draw(0, 0, previewWidth, previewHeight);
	}
	ofPopStyle();
	ofPushStyle();
	if (debugging || drawBodyIndex) {
		ofSetColor(indexColor);
		ofEnableBlendMode((ofBlendMode)indexBlendMode);
		kinect.getBodyIndexSource()->draw(0, 0, previewWidth, previewHeight);
	}
	ofPopStyle();
	ofPushStyle();
	if (debugging || drawBones) {
		ofSetColor(skelColor);
		ofEnableBlendMode((ofBlendMode)skelBlendMode);
		kinect.getBodySource()->drawProjected(0, 0, previewWidth, previewHeight, ofxKFW2::ProjectionCoordinates::DepthCamera);
	}	
	ofPopStyle();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {

	case ' ':
		//hitting space key swaps the camera view
		usecamera = !usecamera;
		break;

	case 'h':
	case 'H':
		gui->toggleVisible();
		guiColor->toggleVisible();
		break;


	default:
		break;
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	//if we are using the camera, the mouse moving should rotate it around the whole sculpture
    if(usecamera){
        float rotateAmount = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0, 360);//TODO: put this into the GUI
        ofVec3f furthestPoint;
        if (points.size() > 0) {
            furthestPoint = points[0];
        }
        else
        {
            furthestPoint = ofVec3f(x, y, 0);
        }

        ofVec3f directionToFurthestPoint = (furthestPoint - center);
        ofVec3f directionToFurthestPointRotated = directionToFurthestPoint.getRotated(rotateAmount, ofVec3f(0,1,0));
        camera.setPosition(center + directionToFurthestPointRotated);
        camera.lookAt(center);
    }
	//otherwise add points like before
    else {
        ofVec3f mousePoint(x,y,0);
		if (points.size() < pointsMax) {
			points.push_back(mousePoint);
		}
		else {
			for (int i = 0; i < pointsMax-1; i++) {
				points[i] = points[i + 1];
			}
			points[pointsMax-1] = mousePoint;
		}
    }
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
	//float depthMapScaleW = 1.5 * previewWidth / 512.0f; //TODO: put hard-coded values into GUI 
	//float depthMapScaleH = 1.5 * previewHeight / 424.0f; //TODO: put hard-coded values into GUI 
	//depthMapScale = ofVec3f(depthMapScaleW, depthMapScaleH, -100.0f * (depthMapScaleH + depthMapScaleW) / 2.0f); //TODO: put hard-coded values into GUI 
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
