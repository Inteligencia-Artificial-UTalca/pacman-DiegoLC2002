#ifndef MSPACMANUTILITYSYS_H_
#define MSPACMANUTILITYSYS_H_

#include "Controller.h"

class MsPacmanUtilitySys: public Controller 
{
    private:
	    Move getClosestMove(const GameState& game, std::pair<int,int> target) const;
	    Move getFarthestMove(const GameState& game, std::pair<int,int> target) const;
	    float getDistanceToGhost(const GameState& game, int ghost) const;

    public:
	    MsPacmanUtilitySys(std::shared_ptr<Character> character);
	    virtual ~MsPacmanUtilitySys();
	    virtual Move getMove(const GameState& game) override;
};

#endif /* MSPACMANUTILITYSYS_H_ */