#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
	loadConfig();
	_rank.setup(_exServerID, _exServerUrl);

	_background.loadImage("images/bg.jpg");

	ofBackground(0);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
#ifndef _DEBUG
	ofToggleFullscreen();
#endif //_DEBUG
	_mainTimer = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofApp::update()
{
	float delta = ofGetElapsedTimef() - _mainTimer;
	_mainTimer += delta;

	_rank.update(delta);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofSetColor(255);
	_background.draw(0, 0);
	_rank.draw();
	
#ifdef _DEBUG
	ofDrawBitmapStringHighlight("FPS:" + ofToString(ofGetFrameRate()), ofVec2f(0, 15));
#endif //_DEBUG
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	switch (key)
	{
	case 'q':
	{
		_rank.rankEnter();
		break;
	}
	case 'w':
	{
		_rank.randExit();
		break;
	}
	case 'f':
	{
		ofToggleFullscreen();
		break;
	}
	}
}

//--------------------------------------------------------------
void ofApp::loadConfig()
{
	ofxXmlSettings xml;

	if (!xml.load("_config.xml"))
	{
		ofLog(OF_LOG_ERROR, "[ofApp::loadConfig]Load xml failed");
	}

	_exServerUrl = xml.getValue("URL", "http://192.168.1.156/mineshine2017/s/", 0);
	_exServerID = xml.getValue("ServerID", 0, 0);

	xml.clear();
}
