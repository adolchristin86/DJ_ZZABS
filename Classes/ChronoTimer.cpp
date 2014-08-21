#include <ChronoTimer.h>

ChronoTimer::ChronoTimer() : _startTime(chrono::high_resolution_clock::now())
{
	_resumeTime = 0;
}

ChronoTimer::~ChronoTimer()
{

}

long ChronoTimer::getElapseTime()
{
	//밀리세컨 단위로 반환
	auto elapseTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - _startTime).count();
	elapseTime -= _resumeTime;
	return elapseTime;
}

void ChronoTimer::resetTime()
{
	//시작 시간 초기화
	_startTime = chrono::high_resolution_clock::now();
	_resumeTime = 0;
}
void ChronoTimer::pauseTime()
{
	//멈췄을 때의 시간을 기록
	_pauseTime = chrono::high_resolution_clock::now();
}
void ChronoTimer::resumeTime()
{
	auto _resumeMillisec = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - _pauseTime).count();
}