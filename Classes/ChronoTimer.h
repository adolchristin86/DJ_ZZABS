#ifndef __CHORONO_TIMER_H__
#define __CHORONO_TIMER_H__
//////////////////
///Cocos2d-x의 ProfilingTimer를 조금 변형해서 씀
//////////////////
#include <iostream>
#include <chrono>

using namespace std;

class ChronoTimer
{
private:
	chrono::high_resolution_clock::time_point _startTime;
	chrono::high_resolution_clock::time_point _pauseTime;
	long _resumeTime;		//pause에서 resume 되기까지의 시간
public:

	ChronoTimer();
	~ChronoTimer();

	long getElapseTime();

	void resetTime();		//타이머 초기화
	void pauseTime();		//타이머 멈춤
	void resumeTime();		//타이머 재시작
};



#endif // __CHORONO_TIMER_H__