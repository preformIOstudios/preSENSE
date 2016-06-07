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
    ofBackground(255);//TODO: put this into the GUI

	//initialize the variable so it's off at the beginning
    usecamera = false;
	pointsMax = 1000; //TODO: put this into the GUI

	//initialize kinect object
	//TODO: only initialize necessary sources
	kinect.open();
	kinect.initDepthSource();
	//kinect.initColorSource();
	//kinect.initInfraredSource();
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
void ofApp::update(){
	//don't move the points if we are using the camera
    if(!usecamera){
        ofVec3f sumOfAllPoints(0,0,0);
        for(unsigned int i = 0; i < points.size(); i++){
            points[i].z -= 4;
            sumOfAllPoints += points[i];
        }
        center = sumOfAllPoints / points.size();
    }

	/////////////////
	// Kinect
	///////////////////
	kinect.update();

	//--
	//Getting joint positions (skeleton tracking)
	//--
	//
	{
		auto bodies = kinect.getBodySource()->getBodies();
		for (auto body : bodies) {
			for (auto joint : body.joints) {
				//TODO: now do something with the joints
			}
		}
	}
	//
	//--



	//--
	//Getting bones (connected joints)
	//--
	//
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
	//
	//--
}

//--------------------------------------------------------------
void ofApp::draw(){


	//if we're using the camera, start it.
	//everything that you draw between begin()/end() shows up from the view of the camera
    if(usecamera){
        camera.begin();
    }

	ofSetColor(0);//TODO: put this into the GUI
	//do the same thing from the first example...
    ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	for(unsigned int i = 1; i < points.size(); i++){

		//find this point and the next point
		ofVec3f thisPoint = points[i-1];
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
		ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,0,1));
		ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,0,1));

		//use the map function to determine the distance.
		//the longer the distance, the narrower the line.
		//this makes it look a bit like brush strokes
		float thickness = ofMap(distance, 0, 60, 20, 2, true);//TODO: put these constants into the GUI
															  //TODO: have tail shrink towards end so it disappears

		//calculate the points to the left and to the right
		//by extending the current point in the direction of left/right by the length
		ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
		ofVec3f rightPoint = thisPoint+toTheRight*thickness;

		//add these points to the triangle strip
		mesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
		mesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
	}

	//end the shape
	mesh.draw();


	//if we're using the camera, take it away
    if(usecamera){
    	camera.end();
    }

	ofPushStyle();
	//TODO: move these hardcoded numbers into GUI
	ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);//TODO: put this into the GUI
	ofSetColor(127);//TODO: put this into the GUI
	if (debugging) {
		kinect.getDepthSource()->draw(0, 0, previewWidth, previewHeight);  // note that the depth texture is RAW so may appear dark

		// Color is at 1920x1080 instead of 512x424 so we should fix aspect ratio
		//float colorHeight = previewWidth * (kinect.getColorSource()->getHeight() / kinect.getColorSource()->getWidth());
		//float colorTop = (previewHeight - colorHeight) / 2.0;

		//kinect.getColorSource()->draw(previewWidth, 0 + colorTop, previewWidth, colorHeight);
		//kinect.getBodySource()->drawProjected(previewWidth, 0 + colorTop, previewWidth, colorHeight);

		//kinect.getInfraredSource()->draw(0, 0, previewWidth, previewHeight);
		kinect.getBodyIndexSource()->draw(0, 0, previewWidth, previewHeight);
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

	case 'd':
	case 'D':
		//hitting space key swaps the camera view
		debugging = !debugging;
		break;

	case 'f':
	case 'F':
		//hitting 'f' key toggles full screen mode
		ofToggleFullscreen();
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
    else{
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
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
