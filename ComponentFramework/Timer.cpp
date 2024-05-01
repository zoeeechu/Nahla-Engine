#include <typeinfo>
#include <iostream>
#include "Timer.h"

Timer::Timer() : prevTicks{0}, currentTicks{0} {}

Timer::~Timer() {}

void Timer::Start() {
	prevTicks = SDL_GetTicks();
	currentTicks = SDL_GetTicks();
}

void Timer::UpdateFrameTicks() {
	prevTicks = currentTicks;
	currentTicks = SDL_GetTicks();
}

float Timer::GetDeltaTime() const {
	return static_cast<float>(currentTicks - prevTicks) / 1000.0f;
}

unsigned int Timer::GetSleepTime(const unsigned int fps_) const {
	unsigned int milliSecsPerFrame = 1000 / fps_;
	if (milliSecsPerFrame == 0) {
		return 0;
	}
	unsigned int sleepTime = milliSecsPerFrame - SDL_GetTicks();
	if (sleepTime > milliSecsPerFrame) {
		return milliSecsPerFrame;
	}
	return sleepTime;
}

float Timer::GetCurrentTicks() const {
	return static_cast<float>(currentTicks) / 1000.0f;
}


/// Single event stuff
Uint32 Timer::singleEventID = 0; /// initialize the static member

void Timer::SetSingleEvent(Uint32 interval, void* param){ 
	SDL_TimerID id = SDL_AddTimer(interval, callBackFuncion, (void*)param);
	
}


Uint32 Timer::callBackFuncion(Uint32 interval, void* param) {
		SDL_Event event;
		SDL_UserEvent userevent;
		  
		userevent.type = SDL_USEREVENT;
		userevent.code = 0;
		userevent.data1 = (void*)singleEventID;
		userevent.data2 = param;
		
		++singleEventID; /// Inc the ID of each event registered 

		event.type = SDL_USEREVENT;
		event.user = userevent;

		SDL_PushEvent(&event);
		return(0); /// Stop the timer
}