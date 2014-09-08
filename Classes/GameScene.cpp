#include "GameScene.h"
#include "Keypad.h"
#include "Gear.h"
#include "SimpleAudioEngine.h"
#include "BMSParser.h"
#include "ScoreManager.h"

USING_NS_CC;
using namespace CocosDenshion;

Scene* GameScene::createScene()
{
	auto scene = Scene::create();

	auto layer = GameScene::create();

	scene->addChild(layer);

	return scene;
}

bool GameScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(GameScene::menuCloseCallback, this));

	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2));

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, UI_DEPTH_TOP);

	//TODO: TEST 게임 시작 버튼
	auto pMenuItem = MenuItemFont::create("Start", CC_CALLBACK_0(GameScene::onGameStart, this));
	pMenuItem->setColor(Color3B::WHITE);
	pMenuItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width,
		origin.y + visibleSize.height - closeItem->getContentSize().height / 2));
	menu->addChild(pMenuItem, UI_DEPTH_TOP);

	int nBmsIndex = CBMSParser::getInstancePtr()->getBmsIndex();
	std::string strFullPath = CBMSParser::getInstancePtr()->getFullPathAtIndex(nBmsIndex);
	
	log("!====== loadBMSFile : %s", strFullPath.c_str());
	
	//bms로드
	if (CBMSParser::getInstancePtr()->loadBMSFile(strFullPath))
	{
		log("!====== Set My Bms File!!");
		CBMSParser::getInstancePtr()->setNoteTime();

		/// 기어
		auto pGearLayer = CGearUI::create();
		this->addChild(pGearLayer, UI_DEPTH_MIDDLE, kTagGear);

		//스코어 매니저
		auto pScoreMgr = new CScoreManager();
	}
	return true;
}

void GameScene::onEnter()
{
	Layer::onEnter();

	this->schedule(schedule_selector(GameScene::updateTime), 1.0f);
}

void GameScene::onExit()
{
	bool bIsPlaying = SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
	SimpleAudioEngine::getInstance()->stopBackgroundMusic(true);
	SimpleAudioEngine::getInstance()->end();

	//해제
	auto pScoreMgr = CScoreManager::getInstancePtr();
	CC_SAFE_DELETE(pScoreMgr);

	Layer::onExit();
}

void GameScene::onGameStart()
{
	//게임 시작
	/*SimpleAudioEngine::getInstance()->playBackgroundMusic(getFilePath().c_str());
	bool bIsPlaying = SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();

	log("File Path : %s, isPlaying : %s", getFilePath().c_str(), bIsPlaying ? "true" : "false");

	this->schedule(schedule_selector(GameScene::updateTime), 1.0f);*/
	auto pGearLayer = dynamic_cast<CGearUI*>(getChildByTag(kTagGear));
	if (pGearLayer)
	{
		pGearLayer->onGameStart();
	}
}

void GameScene::updateTime(float f)
{
	//float fTotalTime = SimpleAudioEngine::getInstance()->getBackgroundMusicLength() / 1000.0f;
	//unsigned long lCurrentTime = SimpleAudioEngine::getInstance()->getBackgroundMusicPosition();
	//float fCurrentTime = lCurrentTime / 1000.0f;

	//log("GameScene::updateTime : %f, lTime : %f, IsPlaying : %s, Total : %f", f, fCurrentTime, SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying() ? "True" : "False", fTotalTime);
	//log("GameScene::currentTime--------------- : %d", GetCurrentTime());
}

void GameScene::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
	return;
#endif

	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}
