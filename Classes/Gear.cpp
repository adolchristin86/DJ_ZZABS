#include "Gear.h"
#include "SimpleAudioEngine.h"
#include "Keypad.h"
#include "ScoreManager.h"

USING_NS_CC;
using namespace CocosDenshion;

const float BOTTOM_HEIGHT = 80.0f;		//건반 높이
const int JUDGE_LINE_HEIGHT = 10;		//판정선 높이
const float DEFAULT_SPEED = 1.0f;		//기본 배속

//밀리세컨을 분/초로
void ConvertToMS(long lMillisec, int& nMin, int& nSec)
{
	float fCurrentSec = lMillisec / 1000.0f;
	int nCurrentSec = static_cast<int>(fCurrentSec);

	nMin = nCurrentSec / 60;
	nSec = nCurrentSec % 60;
}

//////////////////////////////////////////
// 기어
CGearUI::CGearUI()
{
	m_nCurrentBeat = 0;
	m_fSpeed = 1.0f;
	m_lStartTime = 0;
	m_fMadiHeight = 0.0f;
	m_dTotalFrame = 0.0f;
	m_nTotalSec = 0;
	m_nTotalScore = 0;
	m_bIsAutoPlay = false;
}

CGearUI::~CGearUI()
{
	if (m_pAnimate->getReferenceCount() == 1)
		m_pAnimate->release();
}

bool CGearUI::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto pKeypadLayer = CKeypadLayer::create(this);
	pKeypadLayer->setDelegate(this);
	this->addChild(pKeypadLayer, UI_DEPTH_TOP, kTagKeyPad);
	//pKeypadLayer->setVisible(false);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	setMadiHeight(Director::getInstance()->getVisibleSize().height - BOTTOM_HEIGHT - JUDGE_LINE_HEIGHT);

	log("+-CearUI-------------------------------------------------");
	log("|-Director::getInstance()->getVisibleSize() : %f x %f", visibleSize.width, visibleSize.height);
	log("|-Director::getInstance()->getVisibleOrigin() : %f , %f", origin.x, origin.y);
	log("+--------------------------------------------------------");

	std::string strRootPath = CBMSParser::getInstancePtr()->getDirectoryPath();
	if (strRootPath.length() <= 0) return false;

	//제목
	auto pTitleLabel = LabelTTF::create(CBMSParser::getInstancePtr()->getTitle(), "Arial", 24);
	/*label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - label->getContentSize().height));*/
	pTitleLabel->setPosition(Vec2(origin.x + pTitleLabel->getContentSize().width / 2, origin.y + visibleSize.height - pTitleLabel->getContentSize().height));
	this->addChild(pTitleLabel, UI_DEPTH_TOP, kTagInfoTitle);

	float fFontScale = 0.5f;
	//마디정보
	auto pMadiBMFont = LabelBMFont::create("00/00", "fonts/futura-48.fnt");
	pMadiBMFont->setPosition(Vec2(0, pTitleLabel->getPosition().y - pMadiBMFont->getContentSize().height*fFontScale));
	pMadiBMFont->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	pMadiBMFont->setScale(fFontScale);
	this->addChild(pMadiBMFont, UI_DEPTH_TOP, kTagInfoMadi);

	//시간정보
	auto pTimeBMFont = LabelBMFont::create("00:00/00:00", "fonts/futura-48.fnt");
	pTimeBMFont->setPosition(Vec2(0, pMadiBMFont->getPosition().y - pTimeBMFont->getContentSize().height*fFontScale));
	pTimeBMFont->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	pTimeBMFont->setScale(fFontScale);
	this->addChild(pTimeBMFont, UI_DEPTH_TOP, kTagInfoTime);

	//배속정보
	char szSpeed[16];
	sprintf(szSpeed, "%.1f X", getSpeed());
	auto pSpeedBMFont = LabelBMFont::create(szSpeed, "fonts/futura-48.fnt");
	pSpeedBMFont->setPosition(Vec2(0, pTimeBMFont->getPosition().y - pSpeedBMFont->getContentSize().height*fFontScale));
	pSpeedBMFont->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	pSpeedBMFont->setScale(fFontScale);
	this->addChild(pSpeedBMFont, UI_DEPTH_TOP, kTagInfoSpeed);

	//스코어
	char szScore[16];
	sprintf(szScore, "Score : %d", 0);
	auto pScoreBMFont = LabelBMFont::create(szScore, "fonts/futura-48.fnt");
	pScoreBMFont->setPosition(Vec2(0, pSpeedBMFont->getPosition().y - pScoreBMFont->getContentSize().height*fFontScale));
	pScoreBMFont->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	pScoreBMFont->setScale(fFontScale);
	this->addChild(pScoreBMFont, UI_DEPTH_TOP, kTagInfoScore);

	if (CBMSParser::getInstancePtr()->getStageFileName().size() > 0)
	{
		std::string strStageImage = strRootPath + CBMSParser::getInstancePtr()->getStageFileName();
		auto pStageSprite = Sprite::create(strStageImage);
		pStageSprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

		// 화면 전체 사이즈에 맞게 조절
		auto originalSize = pStageSprite->getContentSize();
		pStageSprite->setScaleX(visibleSize.width / originalSize.width);
		pStageSprite->setScaleY(visibleSize.height / originalSize.height);
		pStageSprite->setOpacity(255 * 0.4);		//40%의 투명도
		this->addChild(pStageSprite, UI_DEPTH_BOTTOM, kTagGameBackground);
	}
	

	// 기어 배경
	auto pGearBack = Sprite::create("Images/Gear_Frame.png");
	pGearBack->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	pGearBack->setScaleY(visibleSize.height / pGearBack->getContentSize().height);
	this->addChild(pGearBack, UI_DEPTH_BOTTOM_1, kTagGearBackground);

	Vec2 centerPoint = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);
	Vec2 aPoint[kTagKeyEffectMax] = {
		Vec2(centerPoint.x - 42.5 - 170,		80),		//6
		Vec2(centerPoint.x - 42.5 - 85,		80),		//1
		Vec2(centerPoint.x - 42.5,			80),		//2
		Vec2(centerPoint.x + 42.5,			80),		//3
		Vec2(centerPoint.x + 42.5 + 85,		80),		//4
		Vec2(centerPoint.x + 42.5 + 170,	80) };		//5

	//건반 눌림 효과
	for (int i = 0; i < kTagKeyEffectMax; i++)
	{
		auto pEffectSprite = Sprite::create("Images/Effect_1.png");
		pEffectSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
		pEffectSprite->setPosition(aPoint[i]);
		this->addChild(pEffectSprite, UI_DEPTH_BOTTOM_2, kTagKeyEffect1 + i);
		pEffectSprite->setVisible(false);
	}

	//이펙트 애니
	auto pBatchNode = SpriteBatchNode::create("Images/Effect_Ani.png", 10);
	auto pTexture = pBatchNode->getTexture();
	auto pAnimation = Animation::create();
	pAnimation->setDelayPerUnit(0.05f);

	for (int i = 0; i < 6; i++)
	{
		pAnimation->addSpriteFrameWithTexture(pTexture, Rect((2 + i) + (i * 100), 0, 98, 100));
	}

	for (int i = 0; i < kTagKeyEffectAniMax; i++)
	{
		auto pEffectAni = Sprite::createWithTexture(pTexture, Rect(1, 0, 100, 100));
		pEffectAni->setPosition(aPoint[i]);
		//pEffectAni->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
		this->addChild(pEffectAni, UI_DEPTH_TOP_1, kTagKeyEffectAni1 + i);
		pEffectAni->setVisible(false);
	}
	
	m_pAnimate = Animate::create(pAnimation);		//매번 생성하기 번거로우므로 따로 관리
	m_pAnimate->retain();

	//판정선
	auto pJudgeLine = Sprite::create("Images/Judge_Line.png");
	pJudgeLine->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	pJudgeLine->setPosition(Vec2(centerPoint.x, BOTTOM_HEIGHT));
	this->addChild(pJudgeLine, UI_DEPTH_TOP, kTagJudgeLine);
	//pJudgeLine->setVisible(false);

	//판정
	for (int i = 0; i < EM_JUDGE_MAX; i++)
	{
		char szFileName[32] = { 0, };
		switch (i)
		{
		case EM_JUDGE_PERFECT:
			sprintf(szFileName, "%s", "Images/Judge_Perfect.png");
			break;
		case EM_JUDGE_GOOD:
			sprintf(szFileName, "%s", "Images/Judge_Good.png");
			break;
		case EM_JUDGE_MISS:
			sprintf(szFileName, "%s", "Images/Judge_Miss.png");
			break;
		}
		
		auto pJudgeSprite = Sprite::create(szFileName);
		pJudgeSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		pJudgeSprite->setPosition(Vec2(centerPoint.x, centerPoint.y + pJudgeLine->getContentSize().height * 4));
		this->addChild(pJudgeSprite, UI_DEPTH_TOP, kTagJudgePerfect + i);
		pJudgeSprite->setOpacity(255 * 0.9f);
		pJudgeSprite->setVisible(false);
	}

	//마디선
	for (int i = 0; i < CBMSParser::getInstancePtr()->getTotalMadiCount(); i++)
	{
		auto pMadiLine = Sprite::create("Images/Madi_Line.png");
		pMadiLine->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
		pMadiLine->setPosition(Vec2(centerPoint.x, visibleSize.height));
		pMadiLine->setScaleX(255.0f);
		this->addChild(pMadiLine, UI_DEPTH_BOTTOM_2, kTagMadiLine + i);
	}
	
	//노트 위치 셋팅
	m_aNotePosX[0] = aPoint[0].x;		//6
	m_aNotePosX[1] = aPoint[1].x;		//1
	m_aNotePosX[2] = aPoint[2].x;		//2
	m_aNotePosX[3] = aPoint[3].x;		//3
	m_aNotePosX[4] = aPoint[4].x;		//4
	m_aNotePosX[5] = aPoint[5].x;		//5

	//bms 데이터 만큼 노트 미리 생성
	std::string strWhiteNotePath = "Images/White_Note.png";
	std::string strBlueNotePath = "Images/Blue_Note.png";

	auto it_bms = CBMSParser::getInstancePtr()->getBmsDataListPtr()->begin();
	auto stop_bms = CBMSParser::getInstancePtr()->getBmsDataListPtr()->end();
	for (; it_bms != stop_bms; it_bms++)
	{
		BMSNoteData* pData = *it_bms;
		
		int nSpriteIndex = pData->index;
		int nKeyIndex = pData->key % 10;

		//좌측부터 위치 인덱스는 0 ~ 5 이기 때문에
		float fPosX = m_aNotePosX[nKeyIndex == 6 ? 0 : nKeyIndex];

		bool bIsBlue = nKeyIndex == 1 || nKeyIndex == 4;

		if (nKeyIndex > 5) continue;

		auto pNoteSprite = Sprite::create(bIsBlue ? strBlueNotePath : strWhiteNotePath);
		pNoteSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
		pNoteSprite->setPosition(Vec2(fPosX, visibleSize.height + origin.y));
		this->addChild(pNoteSprite, UI_DEPTH_MIDDLE, kTagNote + nSpriteIndex);
	}

	//bgm 미리 로딩
	auto it_bgm = CBMSParser::getInstancePtr()->getBgmDataListPtr()->begin();
	auto stop_bgm = CBMSParser::getInstancePtr()->getBgmDataListPtr()->end();
	for (; it_bgm != stop_bgm; it_bgm++)
	{
		BMSNoteData* pData = *it_bgm;
		if (pData->key == 1)
		{
			if (pData->attr == 0)
			{
				if (CBMSParser::getInstancePtr()->getBgmArray(static_cast<int>(pData->value)).length() > 0)
				{
					std::string strSoundPath = CBMSParser::getInstancePtr()->getBgmArray(static_cast<int>(pData->value));
					setBgmFullPath(strRootPath + strSoundPath);
					SimpleAudioEngine::getInstance()->preloadBackgroundMusic(getBgmFullPath().c_str());
					SimpleAudioEngine::getInstance()->playBackgroundMusic(getBgmFullPath().c_str());
					SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
				}
			}
		}
	}

	//TODO: TEST 배속 테스트
	auto pMenuItem = MenuItemImage::create("backtotopnormal.png", "backtotoppressed.png",CC_CALLBACK_0(CGearUI::changeSpeed, this));
	pMenuItem->setPosition(Vec2(origin.x + visibleSize.width - pMenuItem->getContentSize().width / 2,
		origin.y + visibleSize.height - pMenuItem->getContentSize().height * 2));

	auto menu = Menu::create(pMenuItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, UI_DEPTH_TOP, kTagSpeedBtn);

	setIsAutoPlay(true);

	return true;
}

void CGearUI::changeSpeed()
{
	float fSpeed = getSpeed();
	auto pSpeedText = static_cast<LabelBMFont*>(getChildByTag(kTagInfoSpeed));
	if (pSpeedText)
	{
		if (fSpeed >= 5.0f)
		{
			fSpeed = 1.0f;
		}
		else
		{
			fSpeed += 0.5f;
		}
		
		char szSpeed[16];
		sprintf(szSpeed, "%.1f X", fSpeed);
		pSpeedText->setString(szSpeed);
		setSpeed(fSpeed);
	}
}


void CGearUI::onEnter()
{
	Layer::onEnter();
}

void CGearUI::onExit()
{
	Layer::onExit();
}

void CGearUI::onKeyboardEvent(int nKeyIndex, bool bShow)
{
	//log("onKeyboardEvent : %d, %s", nKeyIndex, bShow ? "Show" : "Hide");
	//해당 키 건반 눌림 효과 on/off
	auto pEffectSprite = dynamic_cast<Sprite*>(getChildByTag(kTagKeyEffect1 + nKeyIndex));
	if (pEffectSprite)
	{
		pEffectSprite->setVisible(bShow);
	}

	if (bShow == false)
		return;

	//게임 키패드 인덱스를 BMS 오브젝트 채널로 변환
	int nObjChannel = 0;
	switch (nKeyIndex)
	{
	case 0:
		nObjChannel = 6;
		break;
	case 1:
		nObjChannel = 1;
		break;
	case 2:
		nObjChannel = 2;
		break;
	case 3:
		nObjChannel = 3;
		break;
	case 4:
		nObjChannel = 4;
		break;
	case 5:
		nObjChannel = 5;
		break;
	default:
		break;
	}

	BMSNoteData* pNoteData =  CBMSParser::getInstancePtr()->getValidNoteData(nObjChannel);
	if (pNoteData != nullptr && SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())	//널체크
	{
		long lElapseTime = m_Timer.getElapseTime();
		long lNoteTime = pNoteData->time;
		int nAbs = abs(lNoteTime - lElapseTime);
		//log("index : %d,  lElapseTime : %d, lNoteTime : %d, judge : %d", pNoteData->index, lElapseTime, lNoteTime, lNoteTime - lElapseTime);

		

		//노트 판정 체크
		bool bCreateEffect = false;
		if (pNoteData->key > 10 && pNoteData->key < 20)
		{
			//일반 노트 처리
			if (nAbs <= PerfectTime)
			{
				//log("+======= PERFECT!!!");
				drawJudge(EM_JUDGE_PERFECT);
				pNoteData->attr = 1;
				bCreateEffect = true;
				removeNote(pNoteData);
			}
			else if (nAbs <= GoodTime)
			{
				//log("+======= GOOD!!!");
				drawJudge(EM_JUDGE_GOOD);
				pNoteData->attr = 1;
				bCreateEffect = true;
				removeNote(pNoteData);
			}
			else
			{
				//log("+======= MISS!!!");
				//pNoteData->attr = 1;
			}
		}
		else if (pNoteData->key > 50 && pNoteData->key < 60)
		{
			//롱 노트 처리
		}
		
		//이펙트 그리기
		if (pNoteData->attr == 1 && (pNoteData->key % 10) == nObjChannel && bCreateEffect)
		{
			createKeyEffect(nKeyIndex);
		}
		
	}

	
}

void CGearUI::createKeyEffect(int nKeyIndex)
{
	auto pEffectAni = dynamic_cast<Sprite*>(getChildByTag(kTagKeyEffectAni1 + nKeyIndex));
	if (pEffectAni)
	{
		pEffectAni->stopAllActions();
		pEffectAni->setVisible(true);

		auto pCallBack = CCCallFuncN::create(this, callfuncN_selector(CGearUI::actionEffectAni));
		auto pRepeat = Repeat::create(m_pAnimate->clone(), 1);
		auto pTotalAction = Sequence::create(pRepeat, pCallBack, NULL);
		pEffectAni->runAction(pTotalAction);
	}
}

void CGearUI::actionEffectAni(Node* pNode)
{
	auto pEffectAni = dynamic_cast<Sprite*>(pNode);
	if (pEffectAni)
	{
		pEffectAni->setVisible(false);
	}
}

void CGearUI::onGameStart()
{
	// 3/ 2/ 1/ Start! 후에 불릴 함수
	this->schedule(schedule_selector(CGearUI::updateGame));

	m_Timer.resetTime();
	int nSize = CBMSParser::getInstancePtr()->getBmsDataListPtr()->size();
	log("BmsDataSize : %d", nSize);

	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void CGearUI::drawObject(long lElapseTime)
{
	double dBeat = CBMSParser::getInstancePtr()->getBeatFromTime(lElapseTime);
	double dBpm = CBMSParser::getInstancePtr()->getBPMFromBeat(dBeat);

	//마디 정보 업데이트
	if (std::fmod(dBeat, 1))
	{
		int nCurrentBeat = static_cast<int>(dBeat);
		auto pMadiText = static_cast<LabelBMFont*>(getChildByTag(kTagInfoMadi));
		if (pMadiText && nCurrentBeat > getCurrentBeat())
		{
			int nMaxMadi = CBMSParser::getInstancePtr()->getTotalMadiCount();
			if (nCurrentBeat <= nMaxMadi)
			{
				char szText[64] = { 0, };
				sprintf(szText, "%02d/%02d", nCurrentBeat, nMaxMadi);
				pMadiText->setString(szText);
				setCurrentBeat(nCurrentBeat);

				//비트가 한마디 이상 건너 뛰었을때 마디선이 사라지지 않기때문에 마디 갱신 시점에 이전 마디까지 검사후 제거
				for (int i = 0; i < nCurrentBeat; i++)
				{
					removeMadi(i);
				}
			}
		}
	}

	//시간 정보 업데이트
	int nCurrentSec = 0;
	int nMin = 0;
	int nSec = 0;
	ConvertToMS(lElapseTime, nMin, nSec);

	nCurrentSec = (nMin * 60) + nSec;
	int nCurrentBeat = static_cast<int>(dBeat);
	auto pTimeText = static_cast<LabelBMFont*>(getChildByTag(kTagInfoTime));
	if (pTimeText && nCurrentSec > getTotalSec())
	{
		int nTotalSec = static_cast<int>(CBMSParser::getInstancePtr()->getTotalTime()) + 3;		//3초의 여유시간
		if (nCurrentSec <= nTotalSec)
		{
			setTotalSec(nCurrentSec);
			char szText[64] = { 0, };
			sprintf(szText, "%02d:%02d/%02d:%02d", nMin, nSec, nTotalSec / 60, nTotalSec % 60);
			pTimeText->setString(szText);
			setCurrentBeat(nCurrentBeat);
		}	
	}

	float fBottomHeight = BOTTOM_HEIGHT;		//0부터 키패드의 높이 (판정선 제외)
	float fPosY = fBottomHeight;
	float fSpeed = DEFAULT_SPEED + getSpeed() + 1.0f;

	auto it = CBMSParser::getInstancePtr()->getBmsDataListPtr()->begin();
	auto stop = CBMSParser::getInstancePtr()->getBmsDataListPtr()->end();

	//노트 그리기
	for (; it != stop; it++)
	{
		BMSNoteData* pData = *it;
		if (pData->beat >= dBeat)	//해당 노트 데이터의 비트가 현재 비트 이상일때 그려줌
		{
			// 마디 
			float fAddPosY = 0.0f;
			while (pData->beat >= static_cast<int>(dBeat)+1)
			{
				fAddPosY = (static_cast<int>(dBeat + 1) - dBeat) * fSpeed
					* dBpm * CBMSParser::getInstancePtr()->getLengthBeat(static_cast<int>(dBeat))
					* (getMadiHeight() / (getMadiHeight()/2));
				fPosY += fAddPosY;

				dBeat = static_cast<int>(dBeat)+1;

				drawMadi(dBeat - 1, fPosY);
			}

			//노트의 PosY
			fAddPosY = (pData->beat - dBeat) * fSpeed
				* dBpm * CBMSParser::getInstancePtr()->getLengthBeat(static_cast<int>(dBeat))
				* (getMadiHeight() / (getMadiHeight() / 2));
			fPosY += fAddPosY;

			dBeat = pData->beat;

			//노트 데이터의 비트 값이 정수일 때 마디 그림
			double dIntegerBeat = std::fmod(pData->beat, 1); //C++11에서 지원하는 실수의 나머지 연산
			if (dIntegerBeat == 0)
			{
				//drawMadi(static_cast<int>(pData->beat) - 1, fPosY, 1);
			}

			//노트 타입 체크
			if (pData->key == 9)	 //STOP
			{
				continue;
			}
			if (pData->key == 3 || pData->key == 8)	//변속BPM
			{				
				dBpm = pData->value;
			}
			else if (pData->key > 10 && pData->key < 20)
			{	
				//일반 노트 처리
				drawNote(pData, fPosY);
			}
			else if (pData->key > 50 && pData->key < 60)
			{		
				// 롱노트 처리
				
			}
		}
		else
		{
			//오토 플레이 처리
			if (getIsAutoPlay())
			{
				if (pData->attr == 0 && pData->key > 10 && pData->key < 20)
				{
					int nKey = pData->key % 10;
					if (nKey == 6)
					{
						nKey = 0;
					}

					auto pKeypadLayer = dynamic_cast<CKeypadLayer*>(getChildByTag(kTagKeyPad));
					if (pKeypadLayer)
					{
						pKeypadLayer->onCheckKeyboardEvent(nKey, true);
						pKeypadLayer->onCheckKeyboardEvent(nKey, false);
					}
				}
				else if (pData->key > 50 && pData->key<60)
				{

				}
			}
			

			//현재 비트 보다 낮은 비트 처리
			if (pData->attr == 0)
			{
				//미리 구해뒀던 노트에 매긴 시간이 현재 시간보다 
				if (lElapseTime - pData->time)
				{
					if (pData->key > 10 && pData->key < 20) 
					{
						//미스 처리
						pData->attr = 1;
						
						drawJudge(EM_JUDGE_MISS);

						//미스 난 노트는 바로바로 지우자... 안그랬다간 프레임 떨어짐
						removeNote(pData);
					}
					if (pData->key > 50 && pData->key < 60) 
					{
						pData->attr = 2;
						//미스 처리
					}
				}
				else
				{
					if (pData->key > 10 && pData->key < 20)
					{
						drawNote(pData, fPosY);
					}	
				}
			}

			//딜레이된 마디선이 남아있다면 지워줌
			//for (int i = 0; i < dBeat; i++)
			//{
			//	auto pMadiLine = getChildByTag(kTagMadiLine + i);
			//	if (pMadiLine)
			//	{
			//		//log("Remove Madi Num : %d", nMadiNum);
			//		pMadiLine->removeFromParentAndCleanup(true);
			//	}
			//}
		}

		//if (fPosY < 0)	// 스크린 나감
		//{
		//	break;
		//}
	}
}

void CGearUI::drawNote(BMSNoteData* pData, float fPosY)
{
	// 일반 노트 처리
	if (fPosY <= Director::getInstance()->getVisibleSize().height)
	{
		auto pNoteSprite = dynamic_cast<Sprite*>(getChildByTag(kTagNote + pData->index));
		if (pNoteSprite)
		{
			int nKeyIndex = pData->key % 10;
			float fPosX = m_aNotePosX[nKeyIndex == 6 ? 0 : nKeyIndex];
			if (pData->attr == 0)		//그릴수 있는 오브젝트임
			{
				pNoteSprite->setPosition(Vec2(fPosX, fPosY));
			}
		}
	}

	//판정선까지 지나간 노트는 지운다
	if (fPosY < BOTTOM_HEIGHT + JUDGE_LINE_HEIGHT)
	{
		auto pNoteSprite = getChildByTag(kTagNote + pData->index);
		if (pNoteSprite)
		{
			//log(" ====== pData Beat : %lf, index : %d remove!!", pData->beat, pData->index);
			pNoteSprite->removeFromParentAndCleanup(true);
		}
	}
}

void CGearUI::removeNote(BMSNoteData* pData)
{
	auto pNoteSprite = getChildByTag(kTagNote + pData->index);
	if (pNoteSprite)
	{
		pNoteSprite->removeFromParentAndCleanup(true);
	}
}

void CGearUI::drawMadi(int nMadiNum, float fPosY)
{
	//fPosY가 화면 안에 있을 때 만 그림
	//TODO: 변속 BPM시 마디가 안사라지는경우가 있음.. 해당 if문을 제거하면 나중에 다시 고쳐지지만 변속 부드럽게 해야하는것도 봐야하므로 그때 보는게 나을듯
	if (fPosY <= Director::getInstance()->getVisibleSize().height)
	{
		auto pTestMadi = getChildByTag(kTagMadiLine + nMadiNum);
		if (pTestMadi)
		{
			//log("Madi Num : %d, fPosY : %f, nDebug : %d", nMadiNum, fPosY, nDebug);
			auto oldMadiVec2 = pTestMadi->getPosition();
			pTestMadi->setPosition(Vec2(oldMadiVec2.x, fPosY));
		}
	}
	
	//판정선까지 지나간 마디는 지운다
	if (fPosY < BOTTOM_HEIGHT + JUDGE_LINE_HEIGHT)
	{
		removeMadi(nMadiNum);
	}
}

void CGearUI::removeMadi(int nMadiNum)
{
	auto pTestMadi = getChildByTag(kTagMadiLine + nMadiNum);
	if (pTestMadi)
	{
		//log("Remove Madi Num : %d", nMadiNum);
		pTestMadi->removeFromParentAndCleanup(true);
	}
}

void CGearUI::drawJudge(EnumJudgeType eJudge)
{
	for (int i = 0; i < EM_JUDGE_MAX; i++)
	{
		auto pJudgeSprite = getChildByTag(kTagJudgePerfect + i);
		if (pJudgeSprite)
		{
			pJudgeSprite->stopAllActions();
			pJudgeSprite->setVisible(false);
		}		
	}

	auto pJudgeSprite = getChildByTag(kTagJudgePerfect + static_cast<int>(eJudge));
	if (pJudgeSprite)
	{
		pJudgeSprite->setVisible(true);
		pJudgeSprite->setScale(1.2f);
		auto pScale = ScaleTo::create(0.1f, 1.5f);
		auto pDelay = DelayTime::create(1.0f);
		auto pCallBack = CCCallFuncN::create(this, callfuncN_selector(CGearUI::actionJudgeAni));
		auto pTotalAction = Sequence::create(pScale, pDelay, pCallBack, NULL);
		pJudgeSprite->runAction(pTotalAction);
	}
	
	CScoreManager::getInstancePtr()->addJudge(eJudge);

	//콤보 그리기
	if (eJudge < EM_JUDGE_MISS)
	{
		drawCombo();
	}
}

void CGearUI::actionJudgeAni(Node* pNode)
{
	auto pJudgeSprite = dynamic_cast<Sprite*>(pNode);
	if (pJudgeSprite)
	{
		pJudgeSprite->stopAllActions();
		pJudgeSprite->setVisible(false);
	}
}

void CGearUI::drawCombo()
{
	auto pOldNode = getChildByTag(kTagInfoCombo);
	if (pOldNode)
	{
		pOldNode->removeFromParentAndCleanup(true);
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Vec2 centerPoint = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

	int nCombo = CScoreManager::getInstancePtr()->getCurrentCombo();
	char szCombo[16];
	sprintf(szCombo, "%d Combo", nCombo);
	auto pComboBMFont = LabelBMFont::create(szCombo, "fonts/futura-48.fnt");
	pComboBMFont->setPosition(Vec2(centerPoint.x, centerPoint.y + pComboBMFont->getContentSize().height*2));
	pComboBMFont->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	this->addChild(pComboBMFont, UI_DEPTH_TOP_1, kTagInfoCombo);

	pComboBMFont->setScale(1.3f);
	auto pScale = ScaleTo::create(0.1f, 1.0f);
	auto pDelay = DelayTime::create(0.5f);
	auto pFade = FadeOut::create(0.3f);
	auto pCallBack = CCCallFuncN::create(this, callfuncN_selector(CGearUI::actionComboAni));
	auto pTotalAction = Sequence::create(pScale, pDelay, pFade, pCallBack, NULL);
	pComboBMFont->runAction(pTotalAction);
}

void CGearUI::actionComboAni(Node* pNode)
{
	auto pComboBMFont = dynamic_cast<Sprite*>(pNode);
	if (pComboBMFont)
	{
		pComboBMFont->setVisible(false);
	}
}

void CGearUI::updateGame(float f)
{
	//log("updateGame : %f", f);
	long lElapseTime = m_Timer.getElapseTime();
	float fCurrentTime = lElapseTime / 1000.0f;
	drawObject(lElapseTime);

	double dBeat = CBMSParser::getInstancePtr()->getBeatFromTime(lElapseTime);
	double dBpm = CBMSParser::getInstancePtr()->getBPMFromBeat(dBeat);

	auto it_bgm = CBMSParser::getInstancePtr()->getBgmDataListPtr()->begin();
	auto stop_bgm = CBMSParser::getInstancePtr()->getBgmDataListPtr()->end();

	//bgm재생
	for (; it_bgm != stop_bgm; it_bgm++)
	{
		BMSNoteData* pData = *it_bgm;
		if (pData->beat > dBeat)
			break;

		if (pData->key == 1) 
		{
			if (pData->attr == 0) 
			{
				if (CBMSParser::getInstancePtr()->getBgmArray(static_cast<int>(pData->value)).length() > 0)
				{
					SimpleAudioEngine::getInstance()->playBackgroundMusic(getBgmFullPath().c_str());
					//SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
					pData->attr = 1;
				}
			}
		}
	}

	int nTotalScore = CScoreManager::getInstancePtr()->getTotalScore();
	auto pScoreText = static_cast<LabelBMFont*>(getChildByTag(kTagInfoScore));
	if (pScoreText && getTotalScore() < nTotalScore)	//점수 증가 연출
	{
		char szScore[256];
		sprintf(szScore, "Score : %d", nTotalScore);
		pScoreText->setString(szScore);

		setTotalScore(nTotalScore);
	}

	//	float fDefaultBPM = 120.0f;
	//	int nBpm = 114;
	//	float fBeat = fCurrentSec * nBpm / fDefaultBPM;

	//	//BPM 타이머 테스트
	//	double dBeatTime = fCurrentSec * nBpm / fDefaultBPM;
	//	double dRealTime = dBeatTime * fDefaultBPM / nBpm;
	//	double dOneBeat = fDefaultBPM / nBpm;
	

	//log("lElapseTime : %d, fCurrentTime: %f, dBeat : %lf, dBpm : %lf", lElapseTime, fCurrentTime, dBeat, dBpm);

	///밑 부분은 신경 쓰지 말자 음악 재생 시간으로 계산했던거라 잘못된 내용임
	//if (SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
	//{
	//	unsigned long lBgmCurrentTime = SimpleAudioEngine::getInstance()->getBackgroundMusicPosition();
	//	float fCurrentSec = lBgmCurrentTime / 1000.0f;

	//	float fDefaultBPM = 120.0f;
	//	int nBpm = 114;
	//	float fBeat = fCurrentSec * nBpm / fDefaultBPM;

	//	//BPM 타이머 테스트
	//	double dBeatTime = fCurrentSec * nBpm / fDefaultBPM;
	//	double dRealTime = dBeatTime * fDefaultBPM / nBpm;
	//	double dOneBeat = fDefaultBPM / nBpm;

	//	log("CGearUI::fCurrentSec : %f, fBeat : %lf", fCurrentSec, dBeatTime);

	//	int nBeat = static_cast<int>(dBeatTime);


	//	//TODO: TEST
	//	//결과 = (120 / BPM) * (화면 최상단 - 판정선)/(마디내 박자 수)
	//	float fSpeed = 1.0f;
	//	//float fAddPosY = (fDefaultBPM / nBpm) * (560 / 32);	//초당 움직임
	//	float fAddPosY = (560/60) / (fDefaultBPM / nBpm);	//초당 움직임
	//	for (int i = 0; i < 10; i++)
	//	{
	//		auto pTestNode = getChildByTag(555 + i);
	//		if (pTestNode && i > nBeat)
	//		{
	//			auto oldVec2 = pTestNode->getPosition();
	//			pTestNode->setPosition(Vec2(oldVec2.x, oldVec2.y - fAddPosY));
	//		}
	//	}
	//	
	//	//아 짱남.. 파싱부터 해야하나

	//	

	//	if (nBeat > getOldBeat())
	//	{
	//		setOldBeat(nBeat);

	//		auto pNode = getChildByTag(5195);
	//		pNode->setOpacity(255);
	//		auto pFadeOut = FadeOut::create(0.5f);
	//		pNode->runAction(pFadeOut);
	//	}
	//	//log("fCurrentSec : %f, dRealTime : %lf", fCurrentSec, dRealTime);
	//	int nTime = ((int)dBeatTime + 1 - dBeatTime) * (1.0f / nBpm * fDefaultBPM * 4) * 32;
	//	if (dBeatTime == 2)
	//	{
	//	}
	//}
}
