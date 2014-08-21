#ifndef __SCORE_MANAGER_H__
#define __SCORE_MANAGER_H__

#include "Singleton.h"
#include "ComDef.h"

//////////////////////////////////////////////
/// 점수 및 콤보를 관리함
//////////////////////////////////////////////
class CScoreManager : public CSingleton<CScoreManager>
{
	const int nPerfectScore = 123;
	const int nGoodScore = 58;
	const int nMissScore = 0;
public:

	CScoreManager();
	~CScoreManager();

	void resetScore();
	void addJudge(EnumJudgeType eJudge);

	inline int getMaxCombo() { return m_nMaxCombo; }
	inline int getCurrentCombo() { return m_nCurrentCombo; }
	inline int getTotalPerfect() { return m_nTotalPerfect; }
	inline int getTotalGood() { return m_nTotalGood; }
	inline int getTotalMiss() { return m_nTotalMiss; }
	inline int getTotalScore() { return m_nTotalScore; }
	inline int getAddScore() { return m_nAddScore; }
private:

	int m_nTotalPerfect;	//퍼펙 갯수
	int m_nTotalGood;		//굿 갯수
	int m_nTotalMiss;		//미스 갯수

	int m_nMaxCombo;		//최대콤보
	int m_nCurrentCombo;	//현재콤보

	int m_nTotalScore;		//총 점수
	int m_nAddScore;		//증가량
	
	inline void setTotalPerfect(int nVal) { m_nTotalPerfect = nVal; }
	inline void setTotalGood(int nVal) { m_nTotalGood = nVal; }
	inline void setTotalMiss(int nVal) { m_nTotalMiss = nVal; }

	inline void setMaxCombo(int nVal) { m_nMaxCombo = nVal; }
	inline void setCurrentCombo(int nVal) { m_nCurrentCombo = nVal; }

	inline void setTotalScore(int nVal) { m_nTotalScore = nVal; }
	inline void setAddScore(int nVal) { m_nAddScore = nVal; }
};

#endif // __SCORE_MANAGER_H__
