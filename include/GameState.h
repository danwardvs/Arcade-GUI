#ifndef GAMESTATE_H
#define GAMESTATE_H

// GameState, holds all levels
class GameState
{
  public:
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual ~GameState(){};
};

#endif
