#include "rankingDisplay.h"

#pragma region CLASS rankData
//-----------------------------
rankingDisplay::rankData::rankData()
	:_id(-1)
	, _teamName("")
	, _score(0)
{
}
//-----------------------------
rankingDisplay::rankData::rankData(int id, string teamName, int score)
	:_id(id)
	,_teamName(teamName)
	,_score(score)
{
}

//-----------------------------
void rankingDisplay::rankData::update(float)
{
}

//-----------------------------
void rankingDisplay::rankData::draw(int x, int y)
{
	if (!_display.isAllocated())
	{
		return;
	}
	ofPushStyle();
	{
		_display.draw(x, y + _display.getHeight() * -0.5);
	}
	ofPopStyle();
}

//-----------------------------
void rankingDisplay::rankData::setDisplay(ofImage & img)
{
	_display.clone(img);
	_display.update();
}

//-----------------------------
void rankingDisplay::rankData::setData(int id, string teamName, int score)
{
	_id = id;
	_teamName = teamName;
	_score = score;
}

//-----------------------------
int rankingDisplay::rankData::getID()
{
	return _id;
}
#pragma endregion

#pragma region rankingDisplay
//-----------------------------
void rankingDisplay::setup(int serverId, string url)
{
	_serverID = serverId;
	setupFont();
	setupConnector(url);
	setupRankNumber();
	setupAnimate();
	getRankingData();
	_timer = cChangePageTime;
}

//-----------------------------
void rankingDisplay::update(float delta)
{
	if (_needUpdate)
	{
		updateDisplay();
		if (_canEnter)
		{
			sortRankData();
		}
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
		if (_newData._display.isAllocated())
		{
			ofVec2f drawPos = cRankNewDataPos;
			drawNumber(_rankNo + 1, drawPos.x, drawPos.y);
			_newData.draw(drawPos.x + (_numberWidth * 0.8f), drawPos.y);
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

			ofSetColor(255, 255 * animVal);
			drawNumber(rankID + 1, drawPos.x + shiftX, drawPos.y);
			_totalRankingData[rankID].draw(drawPos.x + (_numberWidth * 0.8f) + shiftX, drawPos.y);
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
void rankingDisplay::updateDisplay()
{
	ofFbo canvas;
	canvas.allocate(cRankCanvasWidth, cRankCanvasHeight, GL_RGBA);
	ofPixels pixel;
	ofImage display;

	for (auto& iter : _totalRankingData)
	{

		string score = ofToString(iter._score, 6, '0') + ws2s(L"¤À");
		auto rectTeam = getTextRect(iter._teamName);
		auto rectScore = getTextRect(score);

		canvas.begin();
		{	
			ofClear(0, 0);
			ofSetColor(cRankColor);
				
			drawText(iter._teamName, -rectTeam.x, (cRankCanvasHeight * 0.5) - (rectTeam.y * 0.5 ));
			drawText(score, -rectScore.x + rectTeam.width * 1.1, (cRankCanvasHeight * 0.5) - (rectScore.y * 0.5));
		}
		canvas.end();

		canvas.readToPixels(pixel);
		iter._display.clear();
		iter._display.setFromPixels(pixel);

	}

	//Update newData
	if (_newData._id != -1)
	{
		_newData._display.clear();
		string score = ofToString(_newData._score, 6, '0') + ws2s(L"¤À");
		auto rectTeam = getTextRectL(_newData._teamName);
		auto rectScore = getTextRectL(score);

		canvas.begin();
		{
			ofClear(255);
			ofSetColor(cRankColor);
			drawTextL(_newData._teamName, -rectTeam.x, (cRankCanvasHeight * 0.5) - (rectScore.y * 0.5));
			drawTextL(score, -rectScore.x + rectTeam.width * 1.1, (cRankCanvasHeight * 0.5) - (rectScore.y * 0.5));
		}
		canvas.end();

		canvas.readToPixels(pixel);
		display.setFromPixels(pixel);
		_newData.setDisplay(display);
	}
	
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

#pragma region font
//-----------------------------
void rankingDisplay::setupFont()
{
	_font.setGlobalDpi(72);
	_fontL.setGlobalDpi(72);
	if (!_font.loadFont(cFontPath, cFontSize))
	{
		ofLog(OF_LOG_ERROR, "[rankingDisplay::setupFont]load font failed");
	}


	if (!_fontL.loadFont(cFontPath, cFontLSize))
	{
		ofLog(OF_LOG_ERROR, "[rankingDisplay::setupFont]load font failed");
	}
}

//-----------------------------
void rankingDisplay::drawText(string& text, int x, int y)
{
	_font.drawString(text, x, y);
}
//-----------------------------
ofRectangle rankingDisplay::getTextRect(string & text)
{
	return _font.getStringBoundingBox(text, 0, 0);
}

//-----------------------------
void rankingDisplay::drawTextL(string& text, int x, int y)
{
	_fontL.drawString(text, x, y);
}
//-----------------------------
ofRectangle rankingDisplay::getTextRectL(string & text)
{
	return _fontL.getStringBoundingBox(text, 0, 0);
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

