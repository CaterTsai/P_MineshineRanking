#include "rankingDisplay.h"

#pragma region CLASS rankData
//-----------------------------
rankingDisplay::rankData::rankData()
	:_id(-1)
	, _teamName("")
	, _scoreStr("")
	, _score(0)
	, _fontType(eFontNormal)
{
	setFontType(eFontNormal);
}
//-----------------------------
rankingDisplay::rankData::rankData(int id, string teamName, int score, eFontType type)
	:_id(id)
	,_teamName(teamName)
	,_score(score)
{
	setScore();
	setFontType(type);
}

//-----------------------------
rankingDisplay::rankData::~rankData()
{
	_teamNameCanvas.clear();
}

//-----------------------------
void rankingDisplay::rankData::update(float delta)
{
	if (!_teamNameCanvas.isAllocated())
	{
		_teamNameCanvas.allocate(_canvasWidth, _canvasHeight, GL_RGBA);
	}

	updateAnim(delta);
	if (_isWait)
	{
		_waitTime -= delta;
		if (_waitTime <= 0)
		{
			_isWait = false;
			resetMove();
		}
	}


	auto rectTeam = fontMgr::GetInstance()->getStringBoundingBox(_fontType, _teamName);
	_teamNameCanvas.begin();
	{
		ofClear(255);
		ofSetColor(cRankColor);
		fontMgr::GetInstance()->drawString(
			_fontType,
			_teamName,
			ofVec2f(_canvasWidth + -rectTeam.x - _moveLength * _animTextMove.getCurrentValue(), (_canvasHeight * 0.5) - (rectTeam.y * 0.5))
		);
	}
	_teamNameCanvas.end();
}

//-----------------------------
void rankingDisplay::rankData::draw(int x, int y, float alpha)
{
	ofPushStyle();
	{
		ofSetColor(255, alpha);
		_teamNameCanvas.draw(x, y + _teamNameCanvas.getHeight() * -0.5);

		auto width = _teamNameCanvas.getWidth();
		auto scoreRect = fontMgr::GetInstance()->getStringBoundingBox(_fontType, _scoreStr);
		ofVec2f scorePos(
			x + (width * 1.1),
			y + scoreRect.y * -0.5
		);

		ofSetColor(cRankColor, alpha);
		fontMgr::GetInstance()->drawString(_fontType, _scoreStr, scorePos);
	}
	ofPopStyle();
}

//-----------------------------
void rankingDisplay::rankData::setData(int id, string teamName, int score)
{
	_id = id;
	_teamName = teamName;
	_score = score;


	setScore();
}

//-----------------------------
int rankingDisplay::rankData::getID()
{
	return _id;
}

//-----------------------------
void rankingDisplay::rankData::setFontType(eFontType type)
{
	_fontType = type;
	if (type == eFontNormal)
	{
		_canvasWidth = cRankNameCanvasWidth;
		_canvasHeight = cRankNameCanvasHeight;
	}
	else
	{
		_canvasWidth = cRankNameLargeCanvasWidth;
		_canvasHeight = cRankNameLargeCanvasHeight;
	}
}


//-----------------------------
void rankingDisplay::rankData::resetMove()
{
	auto rectTeam = fontMgr::GetInstance()->getStringBoundingBox(_fontType, _teamName);

	if (rectTeam.getWidth() - rectTeam.x > _canvasWidth)
	{
		_animTextMove.setRepeatType(AnimRepeat::PLAY_ONCE);
		_animTextMove.setCurve(AnimCurve::LINEAR);
		_animTextMove.setDuration(cTextAnimTime);

		_animTextMove.animateFromTo(0.0, 1.0);

		_moveLength = rectTeam.getWidth();

		_isWait = false;
	}
}

//-----------------------------
void rankingDisplay::rankData::setScore()
{
	_scoreStr = ofToString(_score, 6, ' ') + fontMgr::GetInstance()->ws2s(L"¤À");
	if (_score > 999)
	{
		_scoreStr.insert(3, ",");
	}
}

//-----------------------------
void rankingDisplay::rankData::updateAnim(float delta)
{
	if (_isWait)
	{
		return;
	}
	_animTextMove.update(delta);

	if (_animTextMove.hasFinishedAnimating() && _animTextMove.getPercentDone() == 1.0)
	{
		_isWait = true;
		_waitTime = cTextAnimWaitTime;
	}
}
#pragma endregion

#pragma region rankingDisplay
//-----------------------------
void rankingDisplay::setup(int serverId, string url)
{
	_serverID = serverId;
	_newData.setFontType(eFontLarge);

	fontMgr::GetInstance()->setup(cFontPath);
	setupConnector(url);
	setupRankNumber();
	setupAnimate();
	getRankingData();
	_timer = cChangePageTime;

	_newData.resetMove();

}

//-----------------------------
void rankingDisplay::update(float delta)
{
	for (auto& iter : _totalRankingData)
	{
		iter.update(delta);
	}
	_newData.update(delta);


	if (_needUpdate)
	{
		if (_canEnter)
		{
			sortRankData();
		}
		_newData.resetMove();
		_needUpdate = false;
	}

	if (_canEnter)
	{		
		rankEnter();
		_canEnter = false;
	}

	if (_eRankState == eDisplay)
	{
		_timer -= delta;
		if (_timer < 0)
		{
			randExit();
			getRankingData();
			_rankingStartID += cRankingEachPageNum;
			if (_rankingStartID >= _totalRankingData.size())
			{
				_rankingStartID = 0;
			}
			_timer = cChangePageTime;
		}
	}
	updateAnimate(delta);
}

//-----------------------------
void rankingDisplay::draw()
{
	drawNew();
	drawRanking();
}

//-----------------------------
void rankingDisplay::drawNew()
{
	ofPushStyle();
	ofSetColor(255);
	{
		if (_newData._id != -1)
		{
			ofVec2f drawPos = cRankNewDataPos;
			drawNumber(_rankNo + 1, drawPos.x, drawPos.y);
			_newData.draw(drawPos.x + (_numberWidth * 0.8f), drawPos.y, 255);
		}
	}
	ofPopStyle();
}

//-----------------------------
void rankingDisplay::drawRanking()
{
	if (_eRankState == eExit)
	{
		return;
	}

	int rankID = _rankingStartID;
	ofVec2f drawPos = cRankStartPosLeft;
	int counter = 0;
	float animVal = 1.0;
	ofPushStyle();
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (rankID >= _totalRankingData.size())
			{
				break;
			}

			if (_eRankState == eEnter)
			{
				animVal = _animDisplay[counter].getCurrentValue();
			}
			float shiftX = cRankShiftXMax * (animVal - 1.0);

			float alpha = 255 * animVal;
			ofSetColor(255, alpha);
			drawNumber(rankID + 1, drawPos.x + shiftX, drawPos.y);
			_totalRankingData[rankID].draw(drawPos.x + (_numberWidth * 0.8f) + shiftX, drawPos.y, alpha);
			drawPos.y += cRankYInterval;
			rankID++;
			counter++;
		}
		drawPos.x += cRankXInterval;
		drawPos.y = cRankStartPosLeft.y;
	}
	ofPopStyle();
}

//-----------------------------
void rankingDisplay::addNewRankData(int id, string team, int score)
{
	rankData newRankData(id, team, score);
	
	_totalRankingData.push_back(newRankData);
	_teamSet.insert(id);
}

//-----------------------------
void rankingDisplay::updateRankData(int id, string team, int score)
{
	for (auto& iter : _totalRankingData)
	{
		if (iter._id == id)
		{
			iter.setData(id, team, score);
		}
	}
}

//-----------------------------
void rankingDisplay::sortRankData()
{
	sort(
		_totalRankingData.begin(), 
		_totalRankingData.end(), 
		[](const rankData &a, const rankData &b) -> bool
		{
			return a._score > b._score;
		}
	);

	for (int i = 0; i < _totalRankingData.size(); i++)
	{
		if (_newData._id == _totalRankingData[i]._id)
		{
			_rankNo = i;
			break;
		}
	}
}

#pragma endregion

#pragma region Animation
//-----------------------------
void rankingDisplay::setupAnimate()
{
	for (auto& iter : _animDisplay)
	{
		iter.setRepeatType(AnimRepeat::PLAY_ONCE);
		iter.setDuration(0.5);
		iter.reset(0);
	}
}

//-----------------------------
void rankingDisplay::updateAnimate(float delta)
{
	for (auto& iter : _animDisplay)
	{
		iter.update(delta);
	}
	animStateCheck();
}

//-----------------------------
void rankingDisplay::animStateCheck()
{
	auto anim = _animDisplay.back();
	switch (_eRankState)
	{
	case eEnter:
	{
		if (!anim.isWaitingForAnimationToStart() && anim.hasFinishedAnimating() && anim.getPercentDone() == 1.0)
		{
			_eRankState = eDisplay;
		}
		break;
	}
	}
}

//-----------------------------
void rankingDisplay::rankEnter()
{
	if (_eRankState != eExit)
	{
		return;
	}

	float delay = 0;
	for (auto& iter : _animDisplay)
	{
		iter.reset(0.0);
		iter.animateToAfterDelay(1.0, delay);
		delay += 0.2;
	}
	_eRankState = eEnter;

	auto id = _rankingStartID;
	for (int i = 0; i < cRankingEachPageNum; i++)
	{
		if (_rankingStartID + i >= _totalRankingData.size())
		{
			break;
		}

		_totalRankingData[_rankingStartID + i].resetMove();
	}

}

//-----------------------------
void rankingDisplay::randExit()
{
	if (_eRankState != eDisplay)
	{
		return;
	}

	_eRankState = eExit;
}
#pragma endregion

#pragma region Rank Number
//-----------------------------
void rankingDisplay::setupRankNumber()
{
	for (int i = 0; i < 10; i++)
	{
		if (!_rankNumber[i].loadImage("images/number/" + ofToString(i) + ".png"))
		{
			ofLog(OF_LOG_ERROR, "[rankingDisplay::setupRankNumber]setup ranking number failed");
		}
	}
	_numberWidth = _rankNumber[0].getWidth() * cRankNumWidthScale * cRankNumScale * 2;
}

//-----------------------------
void rankingDisplay::drawNumber(int val, int x, int y)
{
	ofPushStyle();
	{
		string number = ofToString(val);
		
		float drawWidth = _rankNumber[0].getWidth() * cRankNumScale;
		float unitWidth = drawWidth * cRankNumWidthScale;
		float width = unitWidth * number.size();
		float height = _rankNumber[0].getHeight() * cRankNumScale;
		ofVec2f drawPos(width * -0.5, height * -0.5);
		ofPushMatrix();
		ofTranslate(x, y);
		for (auto& iter : number)
		{
			int index = iter - '0';
			if (index >= 0 && index < 10)
			{
				_rankNumber[iter - '0'].draw(drawPos, drawWidth, height);
			}
			drawPos.x += unitWidth;
		}
		ofPopMatrix();
	}
	ofPopStyle();
}
#pragma endregion

#pragma region Connector
//-----------------------------
void rankingDisplay::setupConnector(string url)
{
	ofAddListener(_conn.newResponseEvent, this, &rankingDisplay::httpRespone);
	_conn.start();
	_conn.setTimeoutSeconds(5);
	_url = url;
}

//-----------------------------
void rankingDisplay::getRankingData()
{
	ofxHttpForm postForm;
	postForm.action = _url + "backstage.php";
	postForm.method = OFX_HTTP_POST;

	postForm.addFormField("active", "getRanking");
	_conn.addForm(postForm);
}

//-----------------------------
void rankingDisplay::httpRespone(ofxHttpResponse & response)
{
	if (response.status != 200)
	{
		ofLog(OF_LOG_ERROR, "[rankingDisplay::httpRespone]Network error");
		return;
	}

	Json::Value root;
	Json::Reader reader;

	if (!reader.parse(response.responseBody.getText(), root))
	{
		ofLog(OF_LOG_ERROR, "[rankingDisplay::httpRespone]Decode json failed");
		return;
	}

	string result = root.get("result", 0).asString();
	if (result != "1")
	{
		ofLog(OF_LOG_ERROR, "[rankingDisplay::httpRespone] Http request failed");
		return;
	}

	handleRankingData(root);
}

//-----------------------------
void rankingDisplay::handleRankingData(Json::Value & root)
{
	bool needUpdate = false;
	Json::Value rank = root.get("ranking", 0);
	for (int i = 0; i < rank.size(); i++)
	{
		string team = rank[i].get("team", 0).asString();
		int score = stoi(rank[i].get("score", 0).asString());
		int id = stoi(rank[i].get("id", 0).asString());
		int sid = stoi(rank[i].get("sid", 0).asString());

		if (sid == _serverID && _newData._id <= id)
		{
			_newData.setData(id, team, score);
			
		}

		if (_teamSet.find(id) == _teamSet.end())
		{
			addNewRankData(id, team, score);
		}
		else
		{
			updateRankData(id, team, score);
		}
		needUpdate = true;
	}

	_needUpdate = needUpdate;
	_canEnter = true;

}
#pragma endregion

