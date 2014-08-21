#include "ScoreManager.h"

CScoreManager::CScoreManager()
{
	resetScore();
}

CScoreManager::~CScoreManager()
{

}

void CScoreManager::resetScore()
{
	m_nTotalPerfect = 0;
	m_nTotalGood = 0;
	m_nTotalMiss = 0;

	m_nMaxCombo = 0;
	m_nCurrentCombo = 0;

	m_nTotalScore = 0;
}

void CScoreManager::addJudge(EnumJudgeType eJudge)
{
	int nJudgeSocre = 0;
	switch (eJudge)
	{
	case EM_JUDGE_PERFECT:
		//콤보 증가
		m_nCurrentCombo++;
		m_nTotalPerfect++;
		nJudgeSocre = nPerfectScore;
		break;
	case EM_JUDGE_GOOD:
		//콤보 증가
		m_nCurrentCombo++;
		m_nTotalGood++;
		nJudgeSocre = nGoodScore;
		break;
	case EM_JUDGE_MISS:
		//콤보 초기화
		m_nCurrentCombo = 0;
		m_nTotalMiss++;
		nJudgeSocre = nMissScore;
		break;
	}
	
	if (getCurrentCombo() > getMaxCombo())
	{
		//최대 콤보 갱신
		setMaxCombo(getCurrentCombo());
	}

	int nTotalScore = getTotalScore();

	int nAddScore = (nJudgeSocre * getCurrentCombo());

	setAddScore(nAddScore);
	setTotalScore(nTotalScore + nAddScore);
}
