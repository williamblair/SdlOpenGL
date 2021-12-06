#ifndef GAME_TIMER_H_INCLUDED
#define GAME_TIMER_H_INCLUDED

#include <SDL2/SDL.h>

class GameTimer
{
public:
    GameTimer() :
        mLastTicks( 0 )
    {}

    /* returns dt in seconds since the last call to Update */
    float Update()
    {
        unsigned int curTicks = SDL_GetTicks();
        
        if ( mLastTicks == 0 ) {
            float dt = 0.0f;
            mLastTicks = curTicks;
            return dt;
        }

        unsigned int dtMilli = curTicks - mLastTicks;
        float dt = dtMilli / 1000.0f;
        mLastTicks = curTicks;
        return dt;
    }
private:
    unsigned int mLastTicks;
};

#endif // GAME_TIMER_H_INCLUDED

