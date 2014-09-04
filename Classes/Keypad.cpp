#include "Keypad.h"
#include "GameScene.h"
#include "Gear.h"

USING_NS_CC;

//////////////////////////////////////////
// 키 스프라이트
CKeyNode::CKeyNode()
{
	m_eType = EM_RED_TYPE;
	//m_listener = nullptr;
	m_nKeyNum = -1;
	m_nTouchID = -1;
	m_bIsSelect = false;

	m_cCallInfo.pWhoCallPtr = NULL;
	m_pCallFunc = NULL;
}

CKeyNode::~CKeyNode()
{
	removeAllChildrenWithCleanup(true);
}

CKeyNode* CKeyNode::create(EnumKeyType eType, int nKeyNumber, void* pWhoCall/* = NULL*/, cocos2d::SEL_CallFuncND pFunc/* = NULL*/)
{
	CKeyNode *pRet = new CKeyNode();
	if (pRet && pRet->init(eType, nKeyNumber))
	{
		pRet->m_cCallInfo.pWhoCallPtr = pWhoCall;
		pRet->setCallFunc(pFunc);
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

bool CKeyNode::init(EnumKeyType eType, int nKeyNumber)
{
	if (nKeyNumber < 0 && nKeyNumber > 5)
	{
		return false;
	}

	setKeyNumber(nKeyNumber);
	setType(eType);

	char szPath[32] = { 0, };
	
	switch (eType)
	{
	case EM_RED_TYPE:
		strcpy(szPath, "Images/KeypadRed_");
		break;
	case EM_BLUE_TYPE:
		strcpy(szPath, "Images/KeypadBlue_");
		break;
	case EM_WHITE_TYPE:
		strcpy(szPath, "Images/KeyboardBlue_");
		break;
	case EM_BLACK_TYPE:
		strcpy(szPath, "Images/KeyboardBlack_");
		break;
	}

	char szFileName[32] = { 0, };
	sprintf(szFileName, "%s%s", szPath, "Normal.png");
	auto pNormalSprite = Sprite::create(szFileName);
	pNormalSprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	pNormalSprite->setPosition(Vec2::ZERO);
	this->addChild(pNormalSprite, UI_DEPTH_BOTTOM, kTagNormal);

	sprintf(szFileName, "%s%s", szPath, "Select.png");
	auto pSelectSprite = Sprite::create(szFileName);
	pSelectSprite->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	pSelectSprite->setPosition(Vec2::ZERO);
	this->addChild(pSelectSprite, UI_DEPTH_MIDDLE, kTagSelect);
	pSelectSprite->setVisible(false);

	setContentSize(pSelectSprite->getContentSize());
	
	return true;
}

void CKeyNode::setNormal()
{
	auto pNormalSprite = dynamic_cast<Sprite*>(getChildByTag(kTagNormal));
	auto pSelectSprite = dynamic_cast<Sprite*>(getChildByTag(kTagSelect));

	if (pNormalSprite && pSelectSprite)
	{
		pNormalSprite->setVisible(true);
		pSelectSprite->setVisible(false);
	}

	setIsSelect(false);
	setTouchID(-1);

	if (getCallFunc())  {
		m_cCallInfo.nParam1 = getKeyNumber();
		m_cCallInfo.nParam2 = 0;	//Hide
		CCCallFuncND* pND = CCCallFuncND::create(this, getCallFunc(), &m_cCallInfo);
		pND->execute();
	}
}

void CKeyNode::setSelect(int nTouchID/* = -1*/)
{
	auto pNormalSprite = dynamic_cast<Sprite*>(getChildByTag(kTagNormal));
	auto pSelectSprite = dynamic_cast<Sprite*>(getChildByTag(kTagSelect));

	if (pNormalSprite && pSelectSprite)
	{
		pNormalSprite->setVisible(false);
		pSelectSprite->setVisible(true);
	}

	setIsSelect(true);

	//멀티 터치로 인해 터치 ID가 넘어옴
	if (nTouchID > -1)
	{
		setTouchID(nTouchID);

		if (getCallFunc())  {
			m_cCallInfo.nParam1 = getKeyNumber();
			m_cCallInfo.nParam2 = 1;	//Show
			CCCallFuncND* pND = CCCallFuncND::create(this, getCallFunc(), &m_cCallInfo);
			pND->execute();
		}
	}
}

//////////////////////////////////////////
// 키패드
CKeypadLayer* CKeypadLayer::create(CKeyboardEventDelegate* pDelegate/* = NULL*/)
{
	CKeypadLayer *pRet = new CKeypadLayer();
	if (pRet && pRet->init(pDelegate))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return NULL;
	}
}

bool CKeypadLayer::init(CKeyboardEventDelegate* pDelegate/* = NULL*/)
{
	if (!Layer::init())
	{
		return false;
	}

	setDelegate(pDelegate);
	setIsOnKeyPress(false);
	//log("CKeypadLayer::init()");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//키패드 리소스 초기화
	
	//좌/우 키패드 버튼
	Vec2 aKeypadPoint[kTagKeypadButtonMax] = {
		Vec2(origin.x + 60,							origin.y + visibleSize.height / 2 - 40),
		Vec2(origin.x + 110,						origin.y + visibleSize.height / 2 + 40),
		Vec2(origin.x + 160,						origin.y + visibleSize.height / 2 - 40),
		Vec2(origin.x + visibleSize.width - 160,		origin.y + visibleSize.height / 2 - 40),
		Vec2(origin.x + visibleSize.width - 110,		origin.y + visibleSize.height / 2 + 40),
		Vec2(origin.x + visibleSize.width - 60,		origin.y + visibleSize.height / 2 - 40) };

	for (int i = 0; i < kTagKeypadButtonMax; i++)
	{
		CKeyNode::EnumKeyType eType = CKeyNode::EM_RED_TYPE;

		if (i == 1 || i == 4)
		{
			eType = CKeyNode::EM_BLUE_TYPE;
		}

		//Key Buttons
		auto pButton = CKeyNode::create(eType, i, this, callfuncND_selector(CKeypadLayer::onKeyCallBack));
		pButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		pButton->setPosition(aKeypadPoint[i]);
		this->addChild(pButton, UI_DEPTH_BOTTOM, kTagKeypadButton1 + i);
	}

	//하단 키보드 버튼
	Vec2 centerPoint = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);
	Vec2 aKeyboardPoint[kTagKeyboardButtonMax] = {
		Vec2(centerPoint.x - 42.5 - 170,		40),
		Vec2(centerPoint.x - 42.5 - 85,		40),
		Vec2(centerPoint.x - 42.5,			40),
		Vec2(centerPoint.x + 42.5,			40),
		Vec2(centerPoint.x + 42.5 + 85,		40),
		Vec2(centerPoint.x + 42.5 + 170,	40) };

	for (int i = 0; i < kTagKeypadButtonMax; i++)
	{
		CKeyNode::EnumKeyType eType = CKeyNode::EM_BLACK_TYPE;

		if (i == 1 || i == 4)
		{
			eType = CKeyNode::EM_WHITE_TYPE;
		}

		//Key Buttons
		auto pButton = CKeyNode::create(eType, i);
		pButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		pButton->setPosition(aKeyboardPoint[i]);
		this->addChild(pButton, UI_DEPTH_BOTTOM, kTagKeyboardButton1 + i);
	}
	
	return true;
}

void CKeypadLayer::onKeyCallBack(Node* pNode, void* pPtr)
{
	//log("CKeypadLayer::onKeyCallBack()");

	stCallInfo* pCallInfo = static_cast<stCallInfo*>(pPtr);
	if (pCallInfo->pWhoCallPtr)
	{
		CKeypadLayer* pMe = static_cast<CKeypadLayer*>(pCallInfo->pWhoCallPtr);
		pMe->onKeyEvent(pCallInfo->nParam1, pCallInfo->nParam2);
	}
}

void CKeypadLayer::onKeyEvent(int nParam1, int nParam2)
{
	if (getDelegate())
	{
		getDelegate()->onKeyboardEvent(nParam1, nParam2 == 1);
	}
}

void CKeypadLayer::onEnter()
{
	//log("CKeypadLayer::onEnter()");

	Layer::onEnter();

	//멀티 터치 이외에도 키 노드 자체에 터치를 주는 방법이 있지만 터치 스크린으로 하는 게임이기 때문에
	//무브 상태에서도 키가 눌려야함. 그래서 키 자체보단 레이어 전체에 멀티 터치 이벤트로 주어 여러 키를 누를 수 있게 함

	//멀티 터치 모드로 터치 리스너 등록
	auto listener = EventListenerTouchAllAtOnce::create();

	listener->onTouchesBegan = CC_CALLBACK_2(CKeypadLayer::onTouchesBegan, this);
	listener->onTouchesMoved = CC_CALLBACK_2(CKeypadLayer::onTouchesMoved, this);
	listener->onTouchesEnded = CC_CALLBACK_2(CKeypadLayer::onTouchesEnded, this);

	//터치 리스너의 우선순위를 화면에 그려진 순서대로 한다
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	auto keylistener = EventListenerKeyboard::create();
	keylistener->onKeyPressed = CC_CALLBACK_2(CKeypadLayer::onKeyPressed, this);
	keylistener->onKeyReleased = CC_CALLBACK_2(CKeypadLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keylistener, this);
#endif
}

void CKeypadLayer::onExit()
{
	//log("CKeypadLayer::onExit()");
	_eventDispatcher->removeAllEventListeners();

	Layer::onExit();
}

void CKeypadLayer::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	int nIndex = 0;
	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_S:
		nIndex = 0;
		break;
	case EventKeyboard::KeyCode::KEY_D:
		nIndex = 1;
		break;
	case EventKeyboard::KeyCode::KEY_F:
		nIndex = 2;
		break;
	case EventKeyboard::KeyCode::KEY_J:
		nIndex = 3;
		break;
	case EventKeyboard::KeyCode::KEY_K:
		nIndex = 4;
		break;
	case EventKeyboard::KeyCode::KEY_L:
		nIndex = 5;
		break;
	case EventKeyboard::KeyCode::KEY_ESCAPE:
		Director::getInstance()->end();
		break;
	default:
		return;
		break;
	}

	onCheckKeyboardEvent(nIndex, true);
}

void CKeypadLayer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	int nIndex = 0;
	switch (keyCode)
	{
	case EventKeyboard::KeyCode::KEY_S:
		nIndex = 0;
		break;
	case EventKeyboard::KeyCode::KEY_D:
		nIndex = 1;
		break;
	case EventKeyboard::KeyCode::KEY_F:
		nIndex = 2;
		break;
	case EventKeyboard::KeyCode::KEY_J:
		nIndex = 3;
		break;
	case EventKeyboard::KeyCode::KEY_K:
		nIndex = 4;
		break;
	case EventKeyboard::KeyCode::KEY_L:
		nIndex = 5;
		break;
	default:
		return;
		break;
	}

	onCheckKeyboardEvent(nIndex, false);
}

void CKeypadLayer::onCheckKeyboardEvent(int nIndex, bool bIsPress)
{
	auto pKeypadNode = dynamic_cast<CKeyNode*>(getChildByTag(kTagKeypadButton1 + nIndex));
	auto pKeyboardNode = dynamic_cast<CKeyNode*>(getChildByTag(kTagKeyboardButton1 + nIndex));
	if (pKeypadNode && pKeyboardNode)
	{
		if (bIsPress)
		{
			pKeypadNode->setSelect(nIndex);
			pKeyboardNode->setSelect(nIndex);
		}
		else
		{
			pKeypadNode->setNormal();
			pKeyboardNode->setNormal();
		}
	}
}

void CKeypadLayer::onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	for (auto &item : touches)
	{
		auto touch = item;
		checkTouch(touch, EM_TOUCH_BEGAN);
	}
}
void CKeypadLayer::onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	for (auto &item : touches)
	{
		auto touch = item;
		checkTouch(touch, EM_TOUCH_MOVED);
	}
}
void CKeypadLayer::onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event)
{
	for (auto &item : touches)
	{
		auto touch = item;
		checkTouch(touch, EM_TOUCH_ENDED);
	}
}

void CKeypadLayer::checkTouch(cocos2d::Touch* touch, EnumTouchType eTouchType)
{
	auto location = touch->getLocation();

	for (int i = 0; i < kTagKeypadButtonMax; i++)
	{
		auto pKeypadNode = dynamic_cast<CKeyNode*>(getChildByTag(kTagKeypadButton1 + i));
		auto pKeyboardNode = dynamic_cast<CKeyNode*>(getChildByTag(kTagKeyboardButton1 + i));
		if (pKeypadNode && pKeyboardNode)
		{
			float fAnchorOffsetX = pKeypadNode->getContentSize().width * pKeypadNode->getAnchorPoint().x;
			float fAnchorOffsetY = pKeypadNode->getContentSize().height * pKeypadNode->getAnchorPoint().y;
			Rect rect = Rect(
				pKeypadNode->getPosition().x - fAnchorOffsetX,
				pKeypadNode->getPosition().y - fAnchorOffsetY,
				pKeypadNode->getContentSize().width,
				pKeypadNode->getContentSize().height);

			switch (eTouchType)
			{
			case EM_TOUCH_BEGAN:
				if (rect.containsPoint(location))
				{
					//키 중복 방지
					//if (getIsOnKeyPress() == false)
					if (pKeypadNode->getIsSelect() == false)
					{
						pKeypadNode->setSelect(touch->getID());
						pKeyboardNode->setSelect(touch->getID());
						//log("button EM_TOUCH_BEGAN!! ID : %d", touch->getID());

						//setIsOnKeyPress(true);
					}
					
				}
				break;
			case EM_TOUCH_MOVED:
				if (rect.containsPoint(location))
				{
					//if (getIsOnKeyPress() == false)
					if (pKeypadNode->getIsSelect() == false)
					{
						//log("button EM_TOUCH_MOVED!! ID : %d", touch->getID());
						pKeypadNode->setSelect(touch->getID());
						pKeyboardNode->setSelect(touch->getID());

						//setIsOnKeyPress(true);
					}
				}

				if (rect.containsPoint(location) == false && pKeypadNode->getTouchID() == touch->getID())
				{
					//log("button EM_TOUCH_MOVED end!! ID : %d", touch->getID());
					pKeypadNode->setNormal();
					pKeyboardNode->setNormal();

					//setIsOnKeyPress(false);
				}
				break;
			case EM_TOUCH_ENDED:
				if (rect.containsPoint(location) && pKeypadNode->getTouchID() == touch->getID())
				{
					//log("button EM_TOUCH_ENDED!! ID : %d", touch->getID());
					pKeypadNode->setNormal();
					pKeyboardNode->setNormal();

					//setIsOnKeyPress(false);
				}
				break;
			}
		}
	}
}