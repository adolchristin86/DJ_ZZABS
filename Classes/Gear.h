#ifndef __GEAR_H__
#define __GEAR_H__

#include "cocos2d.h"
#include "ComDef.h"
#include "Singleton.h"
#include "ChronoTimer.h"
#include "BMSParser.h"

//기어 레이어
class CGearUI : public cocos2d::Layer, public CKeyboardEventDelegate
{
	//PERFECT : 0.1초 차이
	//GOOD : 0.18초 차이
	//MISS : 0.18초 이상 차이
	const int PerfectTime = 100;
	const int GoodTime = 180;

	enum{
		kTagKeyEffect1 = 0,
		kTagKeyEffect2,
		kTagKeyEffect3,
		kTagKeyEffect4,
		kTagKeyEffect5,
		kTagKeyEffect6,
		kTagKeyEffectMax,

		kTagKeyEffectAni1, 
		kTagKeyEffectAni2,
		kTagKeyEffectAni3,
		kTagKeyEffectAni4,
		kTagKeyEffectAni5,
		kTagKeyEffectAni6,
		kTagKeyEffectAniMax,

		kTagJudgePerfect,
		kTagJudgeGodd,
		kTagJudgeMiss,

		kTagInfoTitle,
		kTagInfoMadi,
		kTagInfoBeat,
		kTagInfoTime,
		kTagInfoSpeed,
		kTagInfoScore,
		kTagInfoCombo,

		kTagKeyPad,

		kTagSpeedBtn,
		kTagGearBackground,
		kTagGameBackground,
		kTagJudgeLine,
		kTagMadiLine,
		kTagMadiLineMax = kTagMadiLine + 1000,	//BMS포맷에 따라서 최대 1000개의 마디가 존재할수도 있음
		kTagNote,
		kTagNoteMax = kTagNote + 1296,
	};
public:

	CGearUI();
	~CGearUI();

	CREATE_FUNC(CGearUI);

	//건반 눌림
	virtual void onKeyboardEvent(int nKeyIndex, bool bShow);
	void onGameStart();
private:

	ChronoTimer m_Timer;	//초시계

	bool m_bIsAutoPlay;

	int m_nCurrentBeat;
	int m_nTotalSec;
	int m_nTotalScore;

	long m_lStartTime;		//시작 시간 (밀리세컨)

	float m_fSpeed;			//배속
	float m_fMadiHeight;	//마디의 높이
	float m_aNotePosX[kTagKeyEffectMax];	//노트 X 좌표

	double m_dTotalFrame;

	std::string m_strBgmFullPath;

	cocos2d::Animate* m_pAnimate;

	inline int getIsAutoPlay() { return m_bIsAutoPlay; }
	inline void setIsAutoPlay(int bVal) { m_bIsAutoPlay = bVal; }

	inline int getCurrentBeat() { return m_nCurrentBeat; }
	inline void setCurrentBeat(int nVal) { m_nCurrentBeat = nVal; }

	inline int getStartTime() { return m_lStartTime; }
	inline void setStartTime(long lVal) { m_lStartTime = lVal; }

	inline int getTotalSec() { return m_nTotalSec; }
	inline void setTotalSec(int nVal) { m_nTotalSec = nVal; }

	inline int getTotalScore() { return m_nTotalScore; }
	inline void setTotalScore(int nVal) { m_nTotalScore = nVal; }

	inline float getSpeed() { return m_fSpeed; }
	inline void setSpeed(float fVal) { m_fSpeed = fVal; }
	inline float getMadiHeight() { return m_fMadiHeight; }
	inline void setMadiHeight(float fVal) { m_fMadiHeight = fVal; }
	
	inline std::string getBgmFullPath() { return m_strBgmFullPath; }
	inline void setBgmFullPath(std::string strFullPath) { m_strBgmFullPath = strFullPath; }

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

	void updateGame(float f);
	void drawObject(long lElapseTime);			//노트, 오브젝트등의 그리기
	void drawNote(BMSNoteData* pData, float fPosY);	//노트 그리기
	void drawMadi(int nMadiNum, float fPosY);	//마디 그리기
	void drawJudge(EnumJudgeType eJudge);
	void drawCombo();

	void removeNote(BMSNoteData* pData);
	void removeMadi(int nMadiNum);
	
	void changeSpeed();

	void actionEffectAni(Node* pNode);
	void actionJudgeAni(Node* pNode);
	void actionComboAni(Node* pNode);

	void createKeyEffect(int nKeyIndex);
};

#endif // __GEAR_H__
