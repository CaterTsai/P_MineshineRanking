#pragma once

#include "constParameter.h"
#include "ofxTrueTypeFontUC.h"

enum eFontType : int
{
	eFontNormal = 0
	,eFontLarge
	,eFontNum
};

class fontMgr
{
public:
	
	void setup(string fontPath);	
	
	void drawString(eFontType type, string& msg, ofVec2f pos);
	void setFontLetterSpace(eFontType type, float spaceSize);
	ofRectangle getStringBoundingBox(eFontType type, string& msg);
	vector<int> getEachWordWidth(eFontType type, string& msg);
private:
	bool _isSetup;
	array<ofxTrueTypeFontUC, eFontNum>	_fontList;

public:
	static string ws2s(const wstring& wstr);
	static wstring s2ws(const string& str);

#pragma region Singleton
//-------------------
//Singleton
//-------------------
private:
	fontMgr();
	~fontMgr(){};
	void operator=(fontMgr const&) {};

public:
	static fontMgr* GetInstance();
	static void Destroy();

private:
	static fontMgr *_pInstance;
#pragma endregion
};