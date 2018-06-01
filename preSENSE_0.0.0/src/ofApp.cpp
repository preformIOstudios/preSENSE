#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	/////////////////////
	// particle system 
	/////////////////////
	int particlesMax = 1500; //TODO: move hard coded values into GUI
	particles.assign(particlesMax, demoParticle());
	resetParticles(true);

	/////////////////////
	// ribbons system
	/////////////////////
	usecamera = false;
	pointsMax = 100; //TODO: move hard coded values into GUI

	/////////////////////
	// kinect object
	/////////////////////
	//TODO: only initialize necessary sources
	kinect.open();
	kinect.initDepthSource();
	kinect.initColorSource();
	kinect.initInfraredSource();
	kinect.initBodySource();
	kinect.initBodyIndexSource();

	/////////////////////
	// debugging variables
	/////////////////////
	debugging = false;//TODO: put this into the GUI
	previewScaleH = 1.0f;//TODO: put this into the GUI
	previewScaleW = 1.0f;//TODO: put this into the GUI

	//TODO: setup gui & text instructions for keypresses, etc.
	// change color settings
	// set mode to debugging

	/////////////////////
	// GUIS
	/////////////////////
	currentLookBank = 0;
	currentLook = 0;
	lookChanged = true; // forces GUI to load settings // TODO: Debug this
	//
	//------------
	// Main GUI --
	//////////////
	gui = new ofxUISuperCanvas("preSENCE part");
	ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
	gui->addSpacer();
	gui->addTextArea("text", "'h' to hide this panel", OFX_UI_FONT_SMALL);
	gui->addSpacer();
	//
	// radio list and key bindings to select different banks of looks
	gui->addLabel("banks: "); 
	vector<string> lookBankList;
	lookBankList.push_back("!");
	lookBankList.push_back("@");
	lookBankList.push_back("#");
	lookBankList.push_back("$");
	lookBankList.push_back("%");
	lookBankList.push_back("^");
	lookBankList.push_back("&");
	ofxUIRadio *radioLookBanks = gui->addRadio("banks", lookBankList, OFX_UI_ORIENTATION_HORIZONTAL);
	vector< ofxUIToggle*> togglesBanks = radioLookBanks->getToggles();
	togglesBanks[0]->bindToKey('!');
	togglesBanks[1]->bindToKey('@');
	togglesBanks[2]->bindToKey('#');
	togglesBanks[3]->bindToKey('$');
	togglesBanks[4]->bindToKey('%');
	togglesBanks[5]->bindToKey('^');
	togglesBanks[6]->bindToKey('&');
	//
	// radio list and key bindings to select different looks
	gui->addLabel("looks: "); // TODO: Make this more like the editors where there's a "look Editor" that handles larger settings files which can be changed between
	vector<string> looksList; 
	for (int i = 0; i < 4; i++) {
		looksList.push_back(ofToString(i + 1));
	}
	ofxUIRadio *radioLook = gui->addRadio("look", looksList, OFX_UI_ORIENTATION_HORIZONTAL);
	vector< ofxUIToggle*> togglesLooks = radioLook->getToggles();
	for (int i = 0; i < 4; i++) {
		togglesLooks[i]->bindToKey(ofToChar( ofToString(i + 1) ));
	}
	gui->addSpacer();
	//
	// FPS
	gui->addFPSSlider("fps");
	gui->addSpacer();
	//gui->addTextArea("text", "'+' or '-' to change frame rate");
	//gui->addIntSlider("set fps", 1, 60, &drawFrameRate);
	//gui->addSpacer();
	//
	// Background Color
	gui->addTextArea("text", "background color");
	gui->addSlider("red", 0.0, 255.0, &bgRed_to);
	gui->addSlider("green", 0.0, 255.0, &bgGreen_to);
	gui->addSlider("blue", 0.0, 255.0, &bgBlue_to);
	gui->addSpacer();
	//
	// radio list for BG Gradient type
	// OF_GRADIENT_LINEAR(0), OF_GRADIENT_CIRCULAR, OF_GRADIENT_BAR
	gui->addLabel("gradient: ");
	vector<string> gradsList;
	gradsList.push_back("linear");
	gradsList.push_back("circular");
	gradsList.push_back("bar");
	gui->addRadio("gradient", gradsList, OFX_UI_ORIENTATION_HORIZONTAL);
	gui->addSpacer();
	//
	// Background Gradient Color
	gui->addTextArea("text", "gradient color");
	gui->addSlider("gradRed", 0.0, 255.0, &bgGradRed_to);
	gui->addSlider("gradGreen", 0.0, 255.0, &bgGradGreen_to);
	gui->addSlider("gradBlue", 0.0, 255.0, &bgGradBlue_to);
	gui->addSpacer();
	//
	// fullscreen toggle
	ofxUIToggle *toggleFullScreen = gui->addToggle("fullscreen", false);
	toggleFullScreen->bindToKey('f');
	toggleFullScreen->bindToKey('F');
	//
	// draw ribbons toggle
	ofxUIToggle* toggleRibbons = gui->addToggle("ribbons", &drawRibbons_to);
	toggleRibbons->bindToKey('o');
	toggleRibbons->bindToKey('O');
	//
	// draw particles toggle
	ofxUIToggle* toggleParticles = gui->addToggle("particles", &drawParticles_to);
	toggleParticles->bindToKey('p');
	toggleParticles->bindToKey('P');
	//
	// particle modes
	gui->addLabel("particle mode: ");
	vector<string> particleModes;
	particleModes.push_back("a");
	particleModes.push_back("r");
	particleModes.push_back("n");
	particleModes.push_back("x");
	ofxUIRadio *radioParticleModes = gui->addRadio("particle mode", particleModes, OFX_UI_ORIENTATION_HORIZONTAL);
	gui->addSpacer();
	//
	// draw index toggle
	ofxUIToggle* toggleIndex = gui->addToggle("body index", &drawBodyIndex_to);
	toggleIndex->bindToKey('i');
	toggleIndex->bindToKey('I');
	//
	// draw mirrored
	ofxUIToggle* mirrorImageToggle = gui->addToggle("mirror image", &drawMirrored);
	mirrorImageToggle->bindToKey('m');
	mirrorImageToggle->bindToKey('M');
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
	gui->addSpacer();
	//
	// take screenshot
	ofxUILabelButton *buttonScreenshot = gui->addLabelButton("take screenshot", false);
	buttonScreenshot->bindToKey('C');
	//
	// save Settings
	ofxUILabelButton *buttonSave = gui->addLabelButton("save main settings", false);
	buttonSave->bindToKey('S');
	gui->autoSizeToFitWidgets();

	/*

	// Kinect
	gui->addTextArea("text", "'k' to kinect, 'x' to diskinnect");
	ofxUIToggle* toggleKinected = gui->addToggle("kinected", &kinected);
	toggleKinected->bindToKey('k');
	toggleKinected->bindToKey('K');
	toggleKinected->bindToKey('x');
	toggleKinected->bindToKey('X');
	gui->addSpacer();
	gui->addToggle("draw depth image", &drawDepth);1
	gui->addToggle("draw depth behind", &drawDepthBehind);
	gui->addToggle("add joints 2 MSG", &drawJoints2MSG);
	//
	// Debug Messages
	//*/


	//
	//-------------
	// Color GUI --
	///////////////

	guiColor = new ofxUISuperCanvas("preSENSE colors");
	ofAddListener(guiColor->newGUIEvent, this, &ofApp::guiEvent);
	guiColor->addSpacer();
	//
	// foreground color settings
	guiColor->addLabel("foreground color settings", OFX_UI_FONT_MEDIUM);
	vector< string > vnamesBlendIMG; vnamesBlendIMG.push_back("i0"); vnamesBlendIMG.push_back("iA"); vnamesBlendIMG.push_back("i+"); vnamesBlendIMG.push_back("i-"); vnamesBlendIMG.push_back("i*"); vnamesBlendIMG.push_back("iS");
	ofxUIRadio *radioBlendIMG = guiColor->addRadio("foreground blend mode", vnamesBlendIMG, OFX_UI_ORIENTATION_HORIZONTAL);
	guiColor->addSlider("fg red", 0.0, 255.0, &fgRed_to);
	guiColor->addSlider("fg green", 0.0, 255.0, &fgGreen_to);
	guiColor->addSlider("fg blue", 0.0, 255.0, &fgBlue_to);
	guiColor->addSlider("fg alpha", 0.0, 255.0, &fgAlpha_to);
	guiColor->addSpacer();
	//
	// index image display settings
	guiColor->addLabel("index image settings", OFX_UI_FONT_MEDIUM);
	// vector< string > vnamesDepthCLR; vnamesDepthCLR.push_back("PSYCHEDELIC_SHADES"); vnamesDepthCLR.push_back("PSYCHEDELIC"); vnamesDepthCLR.push_back("RAINBOW"); vnamesDepthCLR.push_back("CYCLIC_RAINBOW"); vnamesDepthCLR.push_back("BLUES"); vnamesDepthCLR.push_back("BLUES_INV"); vnamesDepthCLR.push_back("GREY"); vnamesDepthCLR.push_back("STATUS");
	// ofxUIRadio *radioMode = guiColor->addRadio("index color mode", vnamesDepthCLR, OFX_UI_ORIENTATION_VERTICAL);
	vector< string > vnamesBlendDEPTH; vnamesBlendDEPTH.push_back("d0"); vnamesBlendDEPTH.push_back("dA"); vnamesBlendDEPTH.push_back("d+"); vnamesBlendDEPTH.push_back("d-"); vnamesBlendDEPTH.push_back("d*"); vnamesBlendDEPTH.push_back("dS");
	ofxUIRadio *radioBlendDepth = guiColor->addRadio("index blend mode", vnamesBlendDEPTH, OFX_UI_ORIENTATION_HORIZONTAL);
	guiColor->addSlider("index red", 0.0, 255.0, &indexRed_to);
	guiColor->addSlider("index green", 0.0, 255.0, &indexGreen_to);
	guiColor->addSlider("index blue", 0.0, 255.0, &indexBlue_to);
	guiColor->addSlider("index alpha", 0.0, 255.0, &indexAlpha_to);
	guiColor->addSpacer();
	//
	// skeleton drawing
	//guiColor->addLabel("skeleton drawing settings", OFX_UI_FONT_MEDIUM);
	//vector< string > vnamesBlendSKEL; vnamesBlendSKEL.push_back("s0"); vnamesBlendSKEL.push_back("sA"); vnamesBlendSKEL.push_back("s+"); vnamesBlendSKEL.push_back("s-"); vnamesBlendSKEL.push_back("s*"); vnamesBlendSKEL.push_back("sS");
	//ofxUIRadio *radioBlendSkel = guiColor->addRadio("skeleton blend mode", vnamesBlendSKEL, OFX_UI_ORIENTATION_HORIZONTAL);
	//guiColor->addSlider("skel red", 0.0, 255.0, &skelRed);
	//guiColor->addSlider("skel green", 0.0, 255.0, &skelGreen);
	//guiColor->addSlider("skel blue", 0.0, 255.0, &skelBlue);
	//guiColor->addSlider("skel alpha", 0.0, 255.0, &skelAlpha);
	//guiColor->addSpacer();
	//
	// particles
	guiColor->addLabel("particle settings", OFX_UI_FONT_MEDIUM);
	guiColor->addSlider("r min", 0.0, 100.0, &partRmin_to);
	guiColor->addSlider("r max", 0.0, 100.0, &partRmax_to);
	guiColor->addSlider("particle velocity range", 0.0, 100.0, &partVelRange_to);
	guiColor->addSpacer();
	//
	// transitions
	guiColor->addLabel("transition settings", OFX_UI_FONT_MEDIUM);
	guiColor->addSlider("seconds", 0.0, 30.0, &transDuration);
	vector< string > vtransEaseTypes; vtransEaseTypes.push_back("in"); vtransEaseTypes.push_back("out"); vtransEaseTypes.push_back("both");
	ofxUIRadio *radioTransEase = guiColor->addRadio("ease", vtransEaseTypes, OFX_UI_ORIENTATION_HORIZONTAL);
	guiColor->addSpacer();
	//
	// Save Settings
	ofxUILabelButton *buttonSaveColor = guiColor->addLabelButton("save color settings", false);
	buttonSaveColor->bindToKey('S');
	guiColor->autoSizeToFitWidgets();

	///////////////////////
	// rendering settings
	///////////////////////
	ofSetFrameRate(60);//TODO: put this into the GUI
	ofSetVerticalSync(true);
	gradientType = 0;
}

//--------------------------------------------------------------
void ofApp::resetGuiPositions() {
	gui->setPosition(0, 0);
	guiColor->setPosition(ofGetWindowWidth() - guiColor->getGlobalCanvasWidth(), 0);
}

void ofApp::reloadLook() {
	// save current values to "_from" variables ("_to" variables are handled by the GUI)
	bgRed_from = bgRed; bgGreen_from = bgGreen; bgBlue_from = bgBlue;
	bgGradRed_from = bgGradRed; bgGradGreen_from = bgGradGreen; bgGradBlue_from = bgGradBlue;
	fgRed_from = fgRed; fgGreen_from = fgGreen; fgBlue_from = fgBlue; fgAlpha_from = fgAlpha;
	indexRed_from = indexRed; indexGreen_from = indexGreen; indexBlue_from = indexBlue; indexAlpha_from = indexAlpha;
	fgBlendMode_from = fgBlendMode;
	indexBlendMode_from = indexBlendMode;
	drawBodyIndex_from = drawBodyIndex;
	drawParticles_from = drawParticles;
	partVelRange_from = partVelRange;
	drawRibbons_from = drawRibbons;
	partRmax_from = partRmax; partRmin_from = partRmin;

	// load new color settings
	gui->loadSettings("guiSettings_" + ofToString(currentLookBank) + ofToString(currentLook) + ".xml");
	guiColor->loadSettings("guiSettings_" + ofToString(currentLookBank) + ofToString(currentLook) + "_color.xml");

	// setup transition variables
	if (transDuration > 0) {
		transSteps = 30.0 * transDuration;
	}
	else {
		transSteps = 1.0;
	}
	transD = 1.0 / transSteps; // (amount to transtion each step)
	transStatus = 0.0;

	// reset GUI
	resetGuiPositions();
}


void ofApp::guiEvent(ofxUIEventArgs &e) {
	//*
	bool noCallbackForWidget = false;
	string nameStr = e.widget->getName();
	int kind = e.widget->getKind();
	int _lookStash = currentLook;

	if (nameStr == "fullscreen") {
		ofSetFullscreen(((ofxUIToggle *)e.widget)->getValue());
	}

	else if (nameStr == "take screenshot") {
		screenshot.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
		screenshot.save(
			"screenshot_" + 
			ofToString(currentLookBank) + ofToString(currentLook) + "_" +
			ofToString(ofGetYear()) + ofToString(ofGetMonth()) + ofToString(ofGetDay()) + "_" +
			ofToString(ofGetHours()) + ofToString(ofGetMinutes()) + ofToString(ofGetSeconds()) +
			".jpg"

			,

			OF_IMAGE_QUALITY_BEST
		);
	}
	else if (nameStr == "save main settings") {
		gui->saveSettings("guiSettings_" + ofToString(currentLookBank) + ofToString(currentLook) + ".xml");
	}
	else if (nameStr == "save color settings") {
		guiColor->saveSettings("guiSettings_" + ofToString(currentLookBank) + ofToString(currentLook) + "_color.xml");
	}

	else if (nameStr == "bank" || nameStr == "!" || nameStr == "@" || nameStr == "#" || nameStr == "$"
		|| nameStr == "%" || nameStr == "^" || nameStr == "&" || nameStr == "*") {
		if (kind == OFX_UI_WIDGET_RADIO) {
			ofxUIRadio *radioLookBank;
			radioLookBank = (ofxUIRadio *)e.widget;
			currentLookBank = radioLookBank->getValue();
		}
		else {
			ofxUIToggle *toggleLookBank = (ofxUIToggle *)e.widget;
			if (toggleLookBank->getValue()) {
				switch (ofToChar(toggleLookBank->getName())) {
				case '!':
					currentLookBank = 0;
					break;
				case '@':
					currentLookBank = 1;
					break;
				case '#':
					currentLookBank = 2;
					break;
				case '$':
					currentLookBank = 3;
					break;
				case '%':
					currentLookBank = 4;
					break;
				case '^':
					currentLookBank = 5;
					break;
				case '&':
					currentLookBank = 6;
					break;
				case '*':
					currentLookBank = 7;
					break;

				default:
					break;
				}
			}
		}

		lookBankChanged = true; // don't change look until a look is selected from this bank
	}

	else if (nameStr == "look" || nameStr == "1" || nameStr == "2" || nameStr == "3" || nameStr == "4") {

		if (kind == OFX_UI_WIDGET_RADIO) {
			ofxUIRadio *radioLook;
			radioLook = (ofxUIRadio *)e.widget;
			currentLook = radioLook->getValue();
		}
		else {
			ofxUIToggle *toggleLook = (ofxUIToggle *)e.widget;
			if (toggleLook->getValue()) {
				currentLook = ofToInt(toggleLook->getName()) - 1;
			}
		}

		if (currentLook != _lookStash || lookBankChanged) {
			lookChanged = true;
		}
	}

	else if (nameStr == "particle mode" || nameStr == "a" || nameStr == "r" || nameStr == "n" || nameStr == "x") {
		if (kind == OFX_UI_WIDGET_RADIO) {
			ofxUIRadio *radioParticleMode;
			radioParticleMode = (ofxUIRadio *)e.widget;
			currentMode = (enum particleMode)radioParticleMode->getValue();
		}
		else {
			ofxUIToggle *toggleParticleMode = (ofxUIToggle *)e.widget;
			if (toggleParticleMode->getValue()) {
				switch (ofToChar(toggleParticleMode->getName())) {

				case 'a': // a
					currentMode = PARTICLE_MODE_ATTRACT;
					currentModeStr = "a - PARTICLE_MODE_ATTRACT: attracts to mouse";
					break;

				case 'r': // r
					currentMode = PARTICLE_MODE_REPEL;
					currentModeStr = "r - PARTICLE_MODE_REPEL: repels from mouse";
					break;

				case 'n': // n
					currentMode = PARTICLE_MODE_NEAREST_POINTS;
					currentModeStr = "n - PARTICLE_MODE_NEAREST_POINTS: hold 'n' to disable force";
					break;

				case 'x': // x
					currentMode = PARTICLE_MODE_NOISE;
					currentModeStr = "x - PARTICLE_MODE_NOISE: snow particle simulation";
					resetParticles(false);
					break;

				default:
					break;
				}

			}
		}
	}

	else if (nameStr == "gradient" || nameStr == "linear" || nameStr == "circular" || nameStr == "bar") {
		// OF_GRADIENT_LINEAR(0), OF_GRADIENT_CIRCULAR, OF_GRADIENT_BAR
		if (kind == OFX_UI_WIDGET_RADIO) {
			ofxUIRadio *radioGradient;
			radioGradient = (ofxUIRadio *)e.widget;
			gradientType = radioGradient->getValue();
		} else {
			ofxUIToggle *toggleGradient;
			toggleGradient = (ofxUIToggle *)e.widget;
			if (toggleGradient->getValue()) {
				switch (ofToChar(toggleGradient->getName().substr(0, 1))) {

				case 'l': // linear
					gradientType = 0;
					break;

				case 'c': // circular
					gradientType = 1;
					break;

				case 'b': // bar
					gradientType = 2;
					break;

				default:
					break;
				}

			}
		}
	}


	else if (nameStr == "hardreset") {
		resetParticles(true);
	}

	else if (nameStr == "mirror image") {
		// do nothing. drawMirrored bool gets used in the draw loop
		//kinect.setsetMirror(drawMirrored); // TODO: this
	}

	/*

	else if (nameStr == "kinected") {
		if (kinected && !kinectsInitialized) // || ofGetKeyPressed('k')?
		{
			kinectsInitialized = kinectInterface->setupKinects(drawMirrored);
			kinected = kinectsInitialized;
		}
		else if (!kinected)
		{
			kinected = !kinectInterface->stopKinects();
			//            delete kinectInterface; kinectInterface = NULL;
			//            kinectInterface = new kinectIO();
		}


	}
	//*/

	else if (nameStr == "foreground blend mode" || nameStr == "i0" || nameStr == "iA" || 
		nameStr == "i+" || nameStr == "i-" || nameStr == "i*" || nameStr == "iS") {
		if (nameStr == "foreground blend mode") {
			ofxUIRadio *radio;
			radio = (ofxUIRadio *)e.widget;
			fgBlendMode = radio->getValue();
		}
		else {
			ofxUIToggle *toggle;
			toggle = (ofxUIToggle *)e.widget;
			if (toggle->getValue()) {
				switch (ofToChar(toggle->getName().substr(1, 1))) {

				case '0': // 
					fgBlendMode_to = 0;
					break;

				case 'A': // 
					fgBlendMode_to = 1;
					break;

				case '+': // 
					fgBlendMode_to = 2;
					break;

				case '-': // 
					fgBlendMode_to = 3;
					break;

				case '*': // 
					fgBlendMode_to = 4;
					break;

				case 'S': // 
					fgBlendMode_to = 6;
					break;

				default:
					break;
				}
			}
		}
	}
	else if (nameStr == "index color mode" || 
		nameStr == "PSYCHEDELIC_SHADES" || nameStr == "PSYCHEDELIC" || 
		nameStr == "RAINBOW" || nameStr == "CYCLIC_RAINBOW" || nameStr == "BLUES" || 
		nameStr == "BLUES_INV" || nameStr == "GREY" || nameStr == "STATUS") {
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
		if (nameStr == "index blend mode") {
			ofxUIRadio *radio;
			radio = (ofxUIRadio *)e.widget;
			indexBlendMode = radio->getValue();
		}
		else {
			ofxUIToggle *toggle;
			toggle = (ofxUIToggle *)e.widget;
			if (toggle->getValue()) {
				switch (ofToChar(toggle->getName().substr(1, 1))) {

				case '0': // 
					indexBlendMode_to = 0;
					break;

				case 'A': // 
					indexBlendMode_to = 1;
					break;

				case '+': // 
					indexBlendMode_to = 2;
					break;

				case '-': // 
					indexBlendMode_to = 3;
					break;

				case '*': // 
					indexBlendMode_to = 4;
					break;

				case 'S': // 
					indexBlendMode_to = 6;
					break;

				default:
					break;
				}
			}
		}

	}
	else if (nameStr == "skeleton blend mode" || nameStr == "s0" || nameStr == "sA" || nameStr == "s+" || nameStr == "s-" || nameStr == "s*" || nameStr == "sS") {

		if (nameStr == "skeleton blend mode") {
			ofxUIRadio *radio;	
			radio = (ofxUIRadio *)e.widget;
			skelBlendMode = radio->getValue();
		}
		else {
			ofxUIToggle *toggle;
			toggle = (ofxUIToggle *)e.widget;
			if (toggle->getValue()) {
				switch (ofToChar(toggle->getName().substr(1, 1))) {

				case '0': // 
					skelBlendMode = 0;
					break;

				case 'A': // 
					skelBlendMode = 1;
					break;

				case '+': // 
					skelBlendMode = 2;
					break;

				case '-': // 
					skelBlendMode = 3;
					break;

				case '*': // 
					skelBlendMode = 4;
					break;

				case 'S': // 
					skelBlendMode = 6;
					break;

				default:
					break;
				}
			}
		}

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
		particles[i].reset(posReset, partRmin, partRmax, partVelRange);
	}	
}

//--------------------------------------------------------------
void ofApp::update(){

	// calculate colors based on blend between _to and _from values
	// TODO: incorporate different ease types
	// increment transition status
	transStatus += transD;
	// clamp transition status to 1 max
	transStatus = MIN(1.0, transStatus);
	// calc new bgColors
	bgRed	=	bgRed_to	* transStatus + bgRed_from		* (1.0 - transStatus);
	bgGreen =	bgGreen_to	* transStatus + bgGreen_from	* (1.0 - transStatus);
	bgBlue	=	bgBlue_to	* transStatus + bgBlue_from		* (1.0 - transStatus);
	// calc new bgGradColors
	bgGradRed	= bgGradRed_to		* transStatus + bgGradRed_from		* (1.0 - transStatus);
	bgGradGreen	= bgGradGreen_to	* transStatus + bgGradGreen_from	* (1.0 - transStatus);
	bgGradBlue	= bgGradBlue_to		* transStatus + bgGradBlue_from		* (1.0 - transStatus);
	// alpha factor for visibility and Blend Mode changes
	float alphaFactor = MIN(0.0 + transStatus * 2, 1.0) + MIN(1.0 - transStatus * 2, 0.0);
	// calc new fgColors
	fgRed	= fgRed_to		* transStatus + fgRed_from		* (1.0 - transStatus);
	fgGreen = fgGreen_to	* transStatus + fgGreen_from	* (1.0 - transStatus);
	fgBlue	= fgBlue_to		* transStatus + fgBlue_from		* (1.0 - transStatus);
	fgAlpha = fgAlpha_to	* transStatus + fgAlpha_from	* (1.0 - transStatus);
	fgAlpha *= 1.0 - alphaFactor * (float)((fgBlendMode_from != fgBlendMode_to) || (drawRibbons_to!=drawRibbons_from) || (drawParticles_to!=drawParticles_from)); // fade alpha to zero and back if there is a blend mode change
	// calc new indexColors
	indexRed	= indexRed_to		* transStatus + indexRed_from	* (1.0 - transStatus);
	indexGreen	= indexGreen_to		* transStatus + indexGreen_from * (1.0 - transStatus);
	indexBlue	= indexBlue_to		* transStatus + indexBlue_from	* (1.0 - transStatus);
	indexAlpha	= indexAlpha_to		* transStatus + indexAlpha_from * (1.0 - transStatus);
	indexAlpha *= 1.0 - alphaFactor * (float)((indexBlendMode_to != indexBlendMode_from) || (drawBodyIndex_to!=drawBodyIndex_from)); // fade alpha to zero and back if there is a blend mode change
	// calc particle values
	partRmin		= partRmin_to		* transStatus + partRmin_from		* (1.0 - transStatus);
	partRmax		= partRmax_to		* transStatus + partRmax_from		* (1.0 - transStatus);
	partVelRange	= partVelRange_to	* transStatus + partVelRange_from	* (1.0 - transStatus);
	// calc blend modes
	fgBlendMode		= fgBlendMode_from		* (1 - round(transStatus)) + fgBlendMode_to		* round(transStatus);
	indexBlendMode	= indexBlendMode_from	* (1 - round(transStatus)) + indexBlendMode_to	* round(transStatus);
	// calc draw flags
	drawRibbons		= drawRibbons_from		* (1 - round(transStatus)) + drawRibbons_to		* round(transStatus);
	drawParticles	= drawParticles_from	* (1 - round(transStatus)) + drawParticles_to	* round(transStatus);
	drawRibbons		= drawRibbons_from		* (1 - round(transStatus)) + drawRibbons_to		* round(transStatus);

	// set draw colors
	bgColor = ofColor(bgRed, bgGreen, bgBlue);
	bgGradient = ofColor(bgGradRed, bgGradGreen, bgGradBlue);
	fgColor = ofColor(fgRed, fgGreen, fgBlue, fgAlpha);
	indexColor = ofColor(indexRed, indexBlue, indexGreen, indexAlpha);
	skelColor = ofColor(skelRed, skelGreen, skelBlue, skelAlpha);


	if (lookChanged) {
		reloadLook();
		lookChanged = false;
	}


	/////////////////
	// Kinect
	///////////////////
	kinect.update();
	//
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
					if (ribbons[bodyIDX][boneIDX].size() <= pointsMax - 2) {
						ribbons[bodyIDX][boneIDX].push_back(firstJPos);
						ribbons[bodyIDX][boneIDX].push_back(secondJPos);
					}
					else {
						for (int i = 0; i < pointsMax - 2; i += 2) {
							ribbons[bodyIDX][boneIDX][i] = ribbons[bodyIDX][boneIDX][i + 2];
							ribbons[bodyIDX][boneIDX][i + 1] = ribbons[bodyIDX][boneIDX][i + 3];
						}
						ribbons[bodyIDX][boneIDX][pointsMax - 2] = firstJPos;
						ribbons[bodyIDX][boneIDX][pointsMax - 1] = secondJPos;
					}
					boneIDX += 1;
				}
			}
			else {
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
			}
			else if (depth > bodyDepthOrder.back()) {
				bodyDepthOrder.push_back(i);
			}
			else if (depth < bodyDepthOrder.front()) {
				bodyDepthOrder.insert(bodyDepthOrder.begin(), i);
			}
			else {
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

	//////////////////
	// Particles
	///////////////////
	for (unsigned int i = 0; i < particles.size(); i++) {
		particles[i].setMode(currentMode);
		particles[i].update();
	}
	//
	attractPoints = hands;
	if (attractPoints.size() != attractPointsWithMovement.size()) {
		attractPointsWithMovement.resize(attractPoints.size());
		attractionHandID = ofRandom(hands.size());
		resetParticles();
	}
	//
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
	ofBackgroundGradient(bgGradient, bgColor, (ofGradientMode)gradientType);
	
	////////////////
	// ribbons
	////////////////
	//
	//*
	if (drawRibbons) {

		//if we're using the camera, start it.
		//everything that you draw between begin()/end() shows up from the view of the camera
		if (usecamera) {
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
		//
		// TODO: sort so foremost bodies appear foremost
		// draw ribbons
		for (unsigned int bodyIDX = 0; bodyIDX < bodyDepthOrder.size(); bodyIDX++) {
			ofSetColor(fgColor / (bodyDepthOrder.size() - bodyIDX));
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
		//
		//if we're using the camera, take it away
		if (usecamera) {
			camera.end();
		}
	}
	//*/

	ofEnableBlendMode((ofBlendMode)skelBlendMode);
	if (debugging) {//TODO: move hard coded values into GUI
		ofPushStyle();

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
		ofSetColor(230);	//TODO: move hard coded values into GUI
		ofDrawBitmapString(currentModeStr + "\n\nSpacebar to reset. \nKeys 1-4 to change mode.", 10, 20);//TODO: move hard coded values into GUI

		ofPopStyle();
	}

	//*

	if (debugging || drawBodyIndex) {
		ofPushStyle();

		ofSetColor(indexColor);
		ofEnableBlendMode((ofBlendMode)indexBlendMode);
		kinect.getBodyIndexSource()->draw(0, 0, previewWidth, previewHeight);

		ofPopStyle();
	}

	if (debugging || drawBones) {
		ofPushStyle();

		ofSetColor(skelColor);
		ofEnableBlendMode((ofBlendMode)skelBlendMode);
		kinect.getBodySource()->drawProjected(0, 0, previewWidth, previewHeight, ofxKFW2::ProjectionCoordinates::DepthCamera);

		ofPopStyle();
	}

	if (drawParticles) {
		if (debugging && currentMode == PARTICLE_MODE_NEAREST_POINTS) {
			ofPushStyle();
			ofSetColor(skelColor);
			ofEnableBlendMode((ofBlendMode)skelBlendMode);

			if (attractPoints.size() > 0) {
				for (unsigned int i = 0; i < attractPoints.size(); i++) {
					ofNoFill();
					ofDrawCircle(attractPointsWithMovement[i], 10);//TODO: move hard coded values into GUI
					ofFill();
					ofDrawCircle(attractPointsWithMovement[i], 4);//TODO: move hard coded values into GUI
				}
			}
			ofPopStyle();
		}

		ofPushStyle();
		ofSetColor(fgColor);
		ofEnableBlendMode((ofBlendMode)fgBlendMode);
		for (unsigned int i = 0; i < particles.size(); i++) {
			particles[i].draw();
		}
		ofPopStyle();
	}
	//*/
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){ //TODO: move key presses into GUI
	switch (key) {

	case ' ':
		//hitting space key swaps the camera view
		usecamera = !usecamera;
		break;

	case 'm':
	case 'M':
		gui->toggleMinified();
		guiColor->toggleMinified();
		break;

	case 'h':
	case 'H':
		gui->toggleVisible();
		gui->enableKeyEventCallbacks();
		guiColor->toggleVisible();
		guiColor->enableKeyEventCallbacks();
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
	if (drawRibbons) {
		//*
		//if we are using the camera, the mouse moving should rotate it around the whole sculpture
		if (usecamera) {
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
			ofVec3f directionToFurthestPointRotated = directionToFurthestPoint.getRotated(rotateAmount, ofVec3f(0, 1, 0));
			camera.setPosition(center + directionToFurthestPointRotated);
			camera.lookAt(center);
		}
		//otherwise add points like before
		else {
			ofVec3f mousePoint(x, y, 0);
			if (points.size() < pointsMax) {
				points.push_back(mousePoint);
			}
			else {
				for (int i = 0; i < pointsMax - 1; i++) {
					points[i] = points[i + 1];
				}
				points[pointsMax - 1] = mousePoint;
			}
		}
		//*/

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
	float depthMapScaleW = previewWidth / 512.0f;
	float depthMapScaleH = previewHeight / 424.0f;
	depthMapScale = ofVec3f(depthMapScaleW, depthMapScaleH, (depthMapScaleH + depthMapScaleW) / 2.0f);

	resetGuiPositions();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
