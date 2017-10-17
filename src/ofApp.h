#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "rankingDisplay.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
		
private:
	float _mainTimer;
	ofImage _background;

private:
	rankingDisplay	_rank;


//--------------------
//Config
public:
	void loadConfig();

public:
	string _exServerUrl;
	int _exServerID;
};
