#include "SelectScene.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include "BMSParser.h"
#include "CustomTableViewCell.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#endif


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
	if (!LayerColor::initWithColor(Color4B(255, 255, 255, 255)))
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
	std::list<std::string> listPathStr;
	listPathStr.clear();
	log("aaaaaaa 1111");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	getBMSFileFullPathJNI(listPathStr);
#else
	//TODO: 일단 급한데로 경로만
	std::string bmsPath = CCFileUtils::sharedFileUtils()->getWritablePath() + "BMS/Private Party/bms.bms";
	listPathStr.push_back(bmsPath);

	bmsPath.clear();
	bmsPath = CCFileUtils::sharedFileUtils()->getWritablePath() + "BMS/robin/robin.bms";
	listPathStr.push_back(bmsPath);

	bmsPath.clear();
	bmsPath = CCFileUtils::sharedFileUtils()->getWritablePath() + "BMS/Sweet Rain/ad.bms";
	listPathStr.push_back(bmsPath);

	bmsPath.clear();
	bmsPath = CCFileUtils::sharedFileUtils()->getWritablePath() + "BMS/kissing the scarlet/Kissing the Scarlet(extreme).bms";
	listPathStr.push_back(bmsPath);

	bmsPath.clear();
	bmsPath = CCFileUtils::sharedFileUtils()->getWritablePath() + "BMS/x-treme grade/nm.bms";
	listPathStr.push_back(bmsPath);

#endif

	log("aaaaaaa 22222");
	auto it = listPathStr.begin();
	auto stop = listPathStr.end();

	//파싱된 데이터를 벡터에 담는다
	getVecHeaderDataPtr()->clear();
	for (; it != stop; it++)
	{
		std::string strData = *it;
		if (CBMSParser::getInstancePtr()->loadBMSFile(strData, true))
		{
			BMSHeaderData stHeaderData;
			if (CBMSParser::getInstancePtr()->getBMSHeader(stHeaderData))
			{
				getVecHeaderDataPtr()->push_back(stHeaderData);
			}
		}
		
	}

	log("aaaaaaa 3333");
	//곡 목록을 테이블 뷰로 구성
	TableView* tableView = TableView::create(this, Size(visibleSize.width / 2, visibleSize.height));
	tableView->setDirection(ScrollView::Direction::VERTICAL);
	tableView->setDelegate(this);
	tableView->setPosition(Vec2::ZERO);
	this->addChild(tableView, UI_DEPTH_BOTTOM, kTagTableView);
	tableView->reloadData();

	log("aaaaaaa 4444");
	//곡정보 갱신 (디폴트로 첫번째)
	refreshMusinInfo(0);

	log("aaaaaaa 5555");
	return true;
}

void SelectScene::tableCellTouched(TableView* table, TableViewCell* cell)
{
	//log("Tag : %d\nCell touched at index : %ld", table->getTag(), cell->getIdx());
	int nSize = getVecHeaderDataPtr()->size();
	for (int i = 0; i < nSize; i++)
	{
		CustomTableViewCell* tableCell = (CustomTableViewCell*)table->cellAtIndex(i);
		if (tableCell)
		{
			bool bIsSelect = cell->getIdx() == tableCell->getIdx();
			tableCell->setSelect(bIsSelect);
		}
	}
	
	refreshMusinInfo(cell->getIdx());
}

Size SelectScene::tableCellSizeForIndex(TableView* table, ssize_t idx)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Size cellSize = Size(visibleSize.width/2, visibleSize.height / 8 + 1);
	return cellSize;
}

TableViewCell* SelectScene::tableCellAtIndex(TableView* table, ssize_t idx)
{
	BMSHeaderData stData = getVecHeaderDataPtr()->at(idx);
	
	CustomTableViewCell* cell = (CustomTableViewCell*)table->dequeueCell();
	if (!cell)
	{
		cell = new CustomTableViewCell();
		cell->autorelease();
		cell->setText(stData.title);
		cell->setSelect(false);
	}
	else
	{
		auto lable = (LabelTTF*)cell->getChildByTag(CustomTableViewCell::kTagCellTextTag);
		if (lable)
		{
			lable->setString(stData.title);
		}
	}

	return cell;
}

ssize_t SelectScene::numberOfCellsInTableView(TableView* table)
{
	int nSize = getVecHeaderDataPtr()->size();
	return nSize;
}

void SelectScene::refreshMusinInfo(int nIndex)
{
	CCNode* pBase = getChildByTag(kTagMusicInfoBase);
	if (pBase)
	{
		pBase->removeFromParentAndCleanup(true);
	}

	pBase = CCNode::create();
	this->addChild(pBase, UI_DEPTH_BOTTOM, kTagMusicInfoBase);

	if (nIndex >= getVecHeaderDataPtr()->size())
		return;

	BMSHeaderData stData = getVecHeaderDataPtr()->at(nIndex);
	std::string strRoot = stData.path;
	std::string strIcon = stData.stagefile;
	std::string strImagePath;
	strImagePath.clear();
	if (strIcon.length() > 0)
	{
		strImagePath = strRoot + strIcon;
	}
	else
	{
		strImagePath = "Images/No_Icon.png";
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();

	//이미지
	auto pStageSprite = Sprite::create(strImagePath);
	pStageSprite->setPosition(Vec2(visibleSize.width / 2 + visibleSize.width / 4, visibleSize.height - 220));
	pStageSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

	auto originalSize = pStageSprite->getContentSize();
	pStageSprite->setScaleX(440 / originalSize.width);
	pStageSprite->setScaleY(440 / originalSize.height);
	pBase->addChild(pStageSprite, UI_DEPTH_BOTTOM);

	//제목
	char szText[256] = { 0, };
	sprintf(szText, "TITLE: %s", stData.title.c_str());
	auto titleLabel = LabelTTF::create(szText, "Helvetica", 18.0f);
	titleLabel->setColor(Color3B::BLACK);
	titleLabel->setAnchorPoint(Vec2::ZERO);
	titleLabel->setPosition(Vec2(visibleSize.width / 2 + 10, visibleSize.height - 470));
	pBase->addChild(titleLabel, UI_DEPTH_BOTTOM_1);

	//아티스트
	sprintf(szText, "ARTIST: %s", stData.artist.c_str());
	auto artistLabel = LabelTTF::create(szText, "Helvetica", 18.0f);
	artistLabel->setColor(Color3B::BLACK);
	artistLabel->setAnchorPoint(Vec2::ZERO);
	artistLabel->setPosition(Vec2(visibleSize.width / 2 + 10, visibleSize.height - 490));
	pBase->addChild(artistLabel, UI_DEPTH_BOTTOM_1);

	//장르
	sprintf(szText, "GENRE: %s", stData.genre.c_str());
	auto genreLabel = LabelTTF::create(szText, "Helvetica", 18.0f);
	genreLabel->setColor(Color3B::BLACK);
	genreLabel->setAnchorPoint(Vec2::ZERO);
	genreLabel->setPosition(Vec2(visibleSize.width / 2 + 10, visibleSize.height - 510));
	pBase->addChild(genreLabel, UI_DEPTH_BOTTOM_1);

	//BPM
	sprintf(szText, "BPM: %.1f", stData.bpm);
	auto bpmLabel = LabelTTF::create(szText, "Helvetica", 18.0f);
	bpmLabel->setColor(Color3B::BLACK);
	bpmLabel->setAnchorPoint(Vec2::ZERO);
	bpmLabel->setPosition(Vec2(visibleSize.width / 2 + 10, visibleSize.height - 530));
	pBase->addChild(bpmLabel, UI_DEPTH_BOTTOM_1);

	//레벨
	sprintf(szText, "LEVEL: %d", stData.playlevel);
	auto levelLabel = LabelTTF::create(szText, "Helvetica", 18.0f);
	levelLabel->setColor(Color3B::BLACK);
	levelLabel->setAnchorPoint(Vec2::ZERO);
	levelLabel->setPosition(Vec2(visibleSize.width / 2 + 10, visibleSize.height - 550));
	pBase->addChild(levelLabel, UI_DEPTH_BOTTOM_1);

	//랭크
	char* szRank[4] = { "Very Hard", "Hard", "Normal", "Easy" };
	sprintf(szText, "DIFFICULTY: %s", szRank[stData.rank]);
	auto rankLabel = LabelTTF::create(szText, "Helvetica", 18.0f);
	rankLabel->setColor(Color3B::BLACK);
	rankLabel->setAnchorPoint(Vec2::ZERO);
	rankLabel->setPosition(Vec2(visibleSize.width / 2 + 10, visibleSize.height - 570));
	pBase->addChild(rankLabel, UI_DEPTH_BOTTOM_1);

	//bms 경로 저장
	std::string strBmsPath;
	strBmsPath.clear();

	setSelectIndex(nIndex);

	CBMSParser::getInstancePtr()->setBmsIndex(nIndex);
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
	log("gameStartCallback");

	if (getSelectIndex() > -1)
	{
		auto pScene = GameScene::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(1.0f, pScene));
		//Director::getInstance()->replaceScene(pScene);
	}
}
