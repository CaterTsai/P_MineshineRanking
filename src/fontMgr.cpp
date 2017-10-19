#include "fontMgr.h"

//-----------------------------------------------------------------------------
void fontMgr::setup(string fontPath)
{
	_isSetup = true;
	//------------------------------------------------
	//MainUI
	_fontList[eFontNormal].setGlobalDpi(72);
	_isSetup &= _fontList[eFontNormal].load(fontPath + "NotoSansCJKtc-Black.otf", cFontSize);
	
	_fontList[eFontLarge].setGlobalDpi(72);
	_isSetup &= _fontList[eFontLarge].load(fontPath + "NotoSansCJKtc-Black.otf", cFontLSize);
	
	if (!_isSetup)
	{
		ofLog(OF_LOG_ERROR, "[fontMgr::setup]load font failed");
	}

}

//-----------------------------------------------------------------------------
void fontMgr::drawString(eFontType type, string& msg, ofVec2f pos)
{
	if (!_isSetup)
	{
		return;
	}

	try
	{
		_fontList.at(type).drawString(msg, pos.x, pos.y);
	}
	catch (const std::exception& e)
	{
		ofLog(OF_LOG_ERROR, "[fontMgr::drawString]failed");
		ofLog(OF_LOG_ERROR, e.what());
	}	
}


//-----------------------------------------------------------------------------
void fontMgr::setFontLetterSpace(eFontType type, float size)
{
	if (!_isSetup)
	{
		return;
	}

	try
	{
		_fontList[type].setLetterSpacing(size);
	}
	catch (const std::exception& e)
	{
		ofLog(OF_LOG_ERROR, "[fontMgr::setFontLetterSpace]failed");
	}
}

//-----------------------------------------------------------------------------
ofRectangle fontMgr::getStringBoundingBox(eFontType type, string& msg)
{
	if (!_isSetup)
	{
		return ofRectangle();
	}

	ofRectangle stringBoundingBox_;
	try
	{
		stringBoundingBox_ = _fontList[type].getStringBoundingBox(msg, 0, 0);
	}
	catch (const std::exception& e)
	{
		ofLog(OF_LOG_ERROR, "[fontMgr::getStringBoundingBox]failed");
		ofLog(OF_LOG_ERROR, e.what());
	}
	return stringBoundingBox_;
}

//-----------------------------------------------------------------------------
vector<int> fontMgr::getEachWordWidth(eFontType type, string& msg)
{
	vector<int> eachWordWidth_;
	try
	{
		_fontList[type].getEachWordWidth(msg, eachWordWidth_);
	}
	catch (const std::exception& e)
	{
		ofLog(OF_LOG_ERROR, "[fontMgr::getEachWordWidth]failed");
		ofLog(OF_LOG_ERROR, e.what());
	}
	return eachWordWidth_;
}

//-----------------------------------------------------------------------------
string fontMgr::ws2s(const wstring & wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, 0);
	string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, 0);
	return strTo;
}

//--------------------------------------------------------------
wstring fontMgr::s2ws(const string & str)
{
	int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
	wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

#pragma region Singleton
//--------------------------------------------------------------
fontMgr::fontMgr()
{}

//--------------------------------------------------------------
fontMgr* fontMgr::_pInstance = nullptr;
fontMgr * fontMgr::GetInstance()
{
	if (_pInstance == nullptr)
	{
		_pInstance = new fontMgr();
	}
	return _pInstance;
}

//--------------------------------------------------------------
void fontMgr::Destroy()
{
	if (_pInstance == nullptr)
	{
		delete _pInstance;
	}
}
#pragma endregion