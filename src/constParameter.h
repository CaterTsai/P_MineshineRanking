#pragma once

#include "ofMain.h"

//rankingDisplay
const int cRankingEachPageNum = 8;
const int cChangePageTime = 10; //s
const string cFontPath = "font/NotoSansCJKtc-Black.otf";
const int cFontSize = 68;
const int cFontLSize = 90;

const int cRankCanvasWidth = 800;
const int cRankCanvasHeight = 120;
const ofColor cRankColor(100, 183, 185);

const ofVec2f cRankStartPosLeft(147, 430);
const ofVec2f cRankNewDataPos(580, 150);
const int cRankYInterval = 180;
const int cRankXInterval = 925;
const float cRankNumWidthScale = 0.8;
const float cRankNumScale = 0.8;
const float cRankShiftXMax = 100.0;
