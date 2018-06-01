#include "demoParticle.h"

//------------------------------------------------------------------
demoParticle::demoParticle(){
	attractPoints = NULL;
	attractPoint = NULL;
}

//------------------------------------------------------------------
void demoParticle::setMode(particleMode newMode){
	mode = newMode;
}

//------------------------------------------------------------------
void demoParticle::setAttractPoints( vector <ofPoint> * attract ){
	attractPoints = attract;
}

void demoParticle::setAttractPoint(ofPoint * attractP) {
	attractPoint = attractP;
}
//------------------------------------------------------------------
void demoParticle::reset(bool posReset) {
	reset(posReset, 0.5, 2.0, 3.9, 10000);
}
void demoParticle::reset(bool posReset, float _Rmin, float _Rmax) {
	reset(posReset, _Rmin, _Rmax, 3.9, 10000);
}

void demoParticle::reset(bool posReset, float _Rmin, float _Rmax, float _velRange) {
	reset(posReset, _Rmin, _Rmax, _velRange, 10000);
}

void demoParticle::reset(bool posReset, float _Rmin, float _Rmax, float _velRange, float _uniqueValRange) {
	//the unique val allows us to set properties slightly differently for each particle
	uniqueVal = ofRandom(-_uniqueValRange, _uniqueValRange); //TODO: move value into GUI
	
	if (posReset) {
		pos.x = ofRandomWidth(); 
		pos.y = ofRandomHeight();
	}
	
	vel.x = ofRandom(-_velRange, _velRange); //TODO: move value into GUI
	vel.y = ofRandom(-_velRange, _velRange); //TODO: move value into GUI
	
	frc   = ofPoint(0,0,0);
	
	scale = ofRandom(_Rmin, _Rmax);//TODO: move value into GUI
	
	if( mode == PARTICLE_MODE_NOISE ){
		drag  = ofRandom(0.97, 0.99);//TODO: move hard coded values into GUI
		vel.y = fabs(vel.y) * 3.0; //make the particles all be going down //TODO: move hard coded values into GUI
	}else{
		drag  = ofRandom(0.95, 0.998);	//TODO: move hard coded values into GUI
	}
}

//------------------------------------------------------------------
void demoParticle::update(){

	//1 - APPLY THE FORCES BASED ON WHICH MODE WE ARE IN 
	
	//if( mode == PARTICLE_MODE_ATTRACT ){
		//ofPoint attractPt(ofGetMouseX(), ofGetMouseY());
		//frc = attractPt-pos; // we get the attraction force/vector by looking at the mouse pos relative to our pos
	if( mode == PARTICLE_MODE_ATTRACT && attractPoints->size() > 0){
		//if (attractPoint == NULL) *attractPoint = &attractPoints[0];
		frc = *attractPoint-pos; // we get the attraction force/vector by looking at the mouse pos relative to our pos
		frc.normalize(); //by normalizing we disregard how close the particle is to the attraction point 
		
		vel *= drag; //apply drag
		vel += frc * 0.6; //apply force //TODO: move hard coded values into GUI
	}
	else if( mode == PARTICLE_MODE_REPEL && attractPoints->size() > 0){ //TODO: update this to be repelled by attract points

		//1 - find closest attractPoint 
		ofPoint closestPt;
		int closest = -1;
		float closestDist = 9999999;

		for (unsigned int i = 0; i < attractPoints->size(); i++) {
			float lenSq = (attractPoints->at(i) - pos).lengthSquared();
			if (lenSq < closestDist) {
				closestDist = lenSq;
				closest = i;
			}
		}

		//2 - if we have a closest point - lets calcuate the force away from it
		if (closest != -1) {
			closestPt = attractPoints->at(closest);

			frc = closestPt - pos;

			//let get the distance and only repel points close to the mouse
			float dist = frc.length();
			frc.normalize();

			vel *= drag;
			if (dist < 150) {//TODO: move hard coded values into GUI
				vel += -frc * 0.6; //notice the frc is negative //TODO: move hard coded values into GUI
			}
			else {
				//if the particles are not close to us, lets add a little bit of random movement using noise. this is where uniqueVal comes in handy. 			
				frc.x = ofSignedNoise(uniqueVal, pos.y * 0.01, ofGetElapsedTimef()*0.2);//TODO: move hard coded values into GUI
				frc.y = ofSignedNoise(uniqueVal, pos.x * 0.01, ofGetElapsedTimef()*0.2);//TODO: move hard coded values into GUI
				vel += frc * 0.04;//TODO: move hard coded values into GUI
			}
		}

	}
	else if( mode == PARTICLE_MODE_NOISE || attractPoint == NULL || attractPoints == NULL){
		//lets simulate falling snow 
		//the fake wind is meant to add a shift to the particles based on where in x they are
		//we add pos.y as an arg so to prevent obvious vertical banding around x values - try removing the pos.y * 0.006 to see the banding
		float fakeWindX = ofSignedNoise(pos.x * 0.003, pos.y * 0.006, ofGetElapsedTimef() * 0.6);
		
		frc.x = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 0.6;
		frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, ofGetElapsedTimef()*0.2) * 0.09 + 0.18;

		vel *= drag; 
		vel += frc * 0.4;
		
		//we do this so as to skip the bounds check for the bottom and make the particles go back to the top of the screen
		if( pos.y + vel.y > ofGetHeight() ){
			pos.y -= ofGetHeight();
		}
	}
	else if( mode == PARTICLE_MODE_NEAREST_POINTS ){
		
		if( attractPoints ){

			//1 - find closest attractPoint 
			ofPoint closestPt;
			int closest = -1; 
			float closestDist = 9999999;
			
			for(unsigned int i = 0; i < attractPoints->size(); i++){
				float lenSq = ( attractPoints->at(i)-pos ).lengthSquared();
				if( lenSq < closestDist ){
					closestDist = lenSq;
					closest = i;
				}
			}
			
			//2 - if we have a closest point - lets calcuate the force towards it
			if( closest != -1 ){
				closestPt = attractPoints->at(closest);				
				float dist = sqrt(closestDist);
				
				//in this case we don't normalize as we want to have the force proportional to distance 
				frc = closestPt - pos;
		
				vel *= drag;
				 
				//lets also limit our attraction to a certain distance and don't apply if 'n' key is pressed
				if( dist < 300 && dist > 40 && !ofGetKeyPressed('n') ){//TODO: move hard coded values into GUI
					vel += frc * 0.003; //TODO: move hard coded values into GUI
				}else{
					//if the particles are not close to us, lets add a little bit of random movement using noise. this is where uniqueVal comes in handy. 			
					frc.x = ofSignedNoise(uniqueVal, pos.y * 0.01, ofGetElapsedTimef()*0.2);//TODO: move hard coded values into GUI
					frc.y = ofSignedNoise(uniqueVal, pos.x * 0.01, ofGetElapsedTimef()*0.2);//TODO: move hard coded values into GUI
					vel += frc * 0.4;//TODO: move hard coded values into GUI
				}
				
			}
		
		}
		
	}
	
	
	//2 - UPDATE OUR POSITION
	
	pos += vel; 
	
	
	//3 - (optional) LIMIT THE PARTICLES TO STAY ON SCREEN 
	//we could also pass in bounds to check - or alternatively do this at the ofApp level
	if( pos.x > ofGetWidth() ){
		pos.x = ofGetWidth();
		vel.x *= -1.0;
	}else if( pos.x < 0 ){
		pos.x = 0;
		vel.x *= -1.0;
	}
	if( pos.y > ofGetHeight() ){
		pos.y = ofGetHeight();
		vel.y *= -1.0;
	}
	else if( pos.y < 0 ){
		pos.y = 0;
		vel.y *= -1.0;
	}	
		
}

//------------------------------------------------------------------
void demoParticle::draw(){		
	ofDrawCircle(pos.x, pos.y, scale);
}

