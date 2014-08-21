#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "ComDef.h"

enum {
	kTagKeyPad = 0,
	kTagGear,
};

class GameScene : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void menuCloseCallback(cocos2d::Ref* pSender);

	CREATE_FUNC(GameScene);

private:
	std::string m_strFilePath;

	inline void setFilePath(std::string var) { m_strFilePath = var; }
	inline std::string getFilePath() { return m_strFilePath; }

	void onGameStart();

	virtual void onEnter();
	virtual void onExit();

	void updateTime(float f);	
};

#endif // __GAME_SCENE_H__
