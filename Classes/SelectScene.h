#ifndef __SELECT_SCENE_H__
#define __SELECT_SCENE_H__

#include "cocos2d.h"
#include "ComDef.h"

class SelectScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void gameStartCallback(cocos2d::Ref* pSender);

	CREATE_FUNC(SelectScene);

private:
	
	void onGameStart();

	virtual void onEnter();
	virtual void onExit();

	void updateTime(float f);
};

#endif // __SELECT_SCENE_H__
