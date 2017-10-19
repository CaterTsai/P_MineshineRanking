#pragma once

#include "constParameter.h"
#include "ofxHttpUtils.h"
#include "fontMgr.h"
#include "ofxAnimatableFloat.h"
#include "json.h"

static string ws2s(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, 0);
	string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, 0);
	return strTo;
}

class rankingDisplay
{
#pragma region CLASS rankData
private:
	class rankData
	{
	public:
		rankData();
		rankData(int id, string teamName, int score, eFontType type = eFontNormal);
		~rankData();
		void update(float delta);
		void draw(int x, int y, float alpha);
		void setData(int id, string teamName, int score);
		int getID();

		void setFontType(eFontType type);

		void resetMove();

	private:
		void setScore();
		void updateAnim(float delta);

	public:
		eFontType _fontType;
		int _id;
		int _canvasWidth, _canvasHeight;
		ofFbo _teamNameCanvas;
		string _teamName;
		
		string _scoreStr;
		int _score;

		bool _isWait;
		float _waitTime;
		float _moveLength;
		ofxAnimatableFloat	_animTextMove;
	};
#pragma endregion

public:
	rankingDisplay()
		:_rankingStartID(0)
		, _serverID(0)
		, _eRankState(eExit)
		, _canEnter(false)
	{};
	void setup(int serverId, string url);
	void update(float delta);
	void draw();

private:
	void drawNew();
	void drawRanking();
	void addNewRankData(int id, string team, int score);
	void updateRankData(int id, string team, int score);
	void sortRankData();

private:
	float _timer;
	int _rankingStartID;
	int _serverID;
	vector<rankData> _totalRankingData;
	int _rankNo;
	rankData _newData;
	set<int> _teamSet;

#pragma region Animation
private:
	void setupAnimate();
	void updateAnimate(float delta);
	void animStateCheck();

public:
	void rankEnter();
	void randExit();
private:
	enum eState
	{
		eEnter = 0
		, eDisplay
		, eExit
	}_eRankState;

	array<ofxAnimatableFloat, cRankingEachPageNum> _animDisplay;
#pragma endregion


#pragma region rank number
private:
	void setupRankNumber();
	void drawNumber(int val, int x, int y);
private:
	float _numberWidth;
	array<ofImage, 10> _rankNumber;
#pragma endregion

#pragma region Connector
private:
	void setupConnector(string url);
	void getRankingData();
	void httpRespone(ofxHttpResponse& Response);
	void handleRankingData(Json::Value& root);
private:
	bool _needUpdate, _canEnter;
	string _url;
	ofxHttpUtils _conn;
#pragma endregion


};