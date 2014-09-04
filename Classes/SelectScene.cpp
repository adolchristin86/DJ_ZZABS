#include "SelectScene.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "BMSParser.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#endif

USING_NS_CC;
using namespace CocosDenshion;

Scene* SelectScene::createScene()
{
	auto scene = Scene::create();

	auto layer = SelectScene::create();

	scene->addChild(layer);

	return scene;
}

bool SelectScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto pMenuItem = MenuItemImage::create(
		"backtotopnormal.png",
		"backtotoppressed.png",
		CC_CALLBACK_1(SelectScene::gameStartCallback, this));

	pMenuItem->setPosition(Vec2(origin.x + visibleSize.width - pMenuItem->getContentSize().width / 2,
		origin.y + pMenuItem->getContentSize().height / 2));

	// create menu, it's an autorelease object
	auto menu = Menu::create(pMenuItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, UI_DEPTH_TOP);

	std::string strFullPath;
	std::list<std::string> listStr;
	listStr.clear();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

	getBMSFileFullPathJNI(listStr);

	auto it = listStr.begin();
	auto stop = listStr.end();

	for (; it != stop; it++)
	{
		std::string strData = *it;
		log("!====== BMS List : %s", strData.c_str());

		//TODO: 테스트 코드
		if (strData.find("bms.bms") != std::string::npos)
		{
			strFullPath = strData;
		}
	}
#else
	//TODO: 일단 급한데로 경로만
	std::string bmsPath = CCFileUtils::sharedFileUtils()->getWritablePath() + "BMS/Private Party/bms.bms";
	listStr.push_back(bmsPath);
	strFullPath = bmsPath;

	bmsPath.clear();
	bmsPath = CCFileUtils::sharedFileUtils()->getWritablePath() + "BMS/robin/robin.bms";
	listStr.push_back(bmsPath);
	//strFullPath = bmsPath;

	bmsPath.clear();
	bmsPath = CCFileUtils::sharedFileUtils()->getWritablePath() + "BMS/Sweet Rain/ad.bms";
	listStr.push_back(bmsPath);

#endif

	log("!====== loadBMSFile : %s", strFullPath.c_str());
	/// 파일가져오기
	auto pBmsParser = new CBMSParser();


	//CBMSParser::getInstancePtr()->getBMSHeader()

	log("!====== Load Fail Bms File...");

	return true;
}

void SelectScene::onEnter()
{
	Layer::onEnter();

	this->schedule(schedule_selector(SelectScene::updateTime), 1.0f);
}

void SelectScene::onExit()
{
	Layer::onExit();
}

void SelectScene::updateTime(float f)
{
	
}

void SelectScene::gameStartCallback(Ref* pSender)
{
	///게임 씬으로 전환
}
