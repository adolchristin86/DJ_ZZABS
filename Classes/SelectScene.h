#ifndef __SELECT_SCENE_H__
#define __SELECT_SCENE_H__

#include "cocos2d.h"
#include "extensions\cocos-ext.h"
#include "ComDef.h"
#include "BMSNoteData.h"

USING_NS_CC;
using namespace cocos2d::extension;
class SelectScene : public cocos2d::LayerColor, 
					public cocos2d::extension::TableViewDataSource,
					public cocos2d::extension::TableViewDelegate
{
public:
	enum{
		kTagTableView = 0,
		kTagMusicInfoBase,
	};

	static cocos2d::Scene* createScene();

	virtual bool init();

	void gameStartCallback(cocos2d::Ref* pSender);

	CREATE_FUNC(SelectScene);

private:

	virtual void scrollViewDidScroll(ScrollView* view) {};
	virtual	void scrollViewDidZoom(ScrollView* view) {};
	virtual void tableCellTouched(TableView* table, TableViewCell* cell);
	virtual Size tableCellSizeForIndex(TableView* table, ssize_t idx);
	virtual TableViewCell* tableCellAtIndex(TableView* table, ssize_t idx);
	virtual ssize_t numberOfCellsInTableView(TableView* table);
	
	void onGameStart();
	void refreshMusinInfo(int nIndex);

	virtual void onEnter();
	virtual void onExit();

	void updateTime(float f);

	int m_nSelectIndex;

	inline int getSelectIndex() { return m_nSelectIndex; }
	inline void setSelectIndex(int nVal) { m_nSelectIndex = nVal; }

	std::vector<BMSHeaderData> vecHeaderData;
	std::vector<BMSHeaderData>* getVecHeaderDataPtr() { return &vecHeaderData; }
};

#endif // __SELECT_SCENE_H__
