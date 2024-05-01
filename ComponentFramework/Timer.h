#ifndef TIMER_H
#define TIMER_H
#include <SDL.h>


class Timer {
public:
	Timer();
	~Timer();

	Timer(const Timer&) = delete;
	Timer(Timer&&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer& operator=(Timer&&) = delete;

	void Start();
	void UpdateFrameTicks();
	float GetDeltaTime() const;
	unsigned int GetSleepTime(const unsigned int fps_) const;
	float GetCurrentTicks() const;
	static void SetSingleEvent(Uint32 interval,void* param);
private:	
	unsigned int prevTicks;
	unsigned int currentTicks;
	static unsigned int singleEventID;
	static Uint32 callBackFuncion(Uint32 interval, void* singleEventParam);
};


#endif
