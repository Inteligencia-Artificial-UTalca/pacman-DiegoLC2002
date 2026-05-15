#ifndef CLYDEUTILITYSYS_H_
#define CLYDEUTILITYSYS_H_

#include "Controller.h"
#include <chrono>
#include <vector>

class ClydeUtilitySys : public Controller
{
    private:
        bool chasing;
        std::chrono::time_point<std::chrono::high_resolution_clock> chaseStart;

        std::vector<std::pair<int,int>> patrolPoints;
        std::pair<int,int> territoryCenter;
        size_t currentPatrol;

	    Move getClosestMove(const GameState& game, std::pair<int,int> target) const;
	    Move getFarthestMove(const GameState& game, std::pair<int,int> target) const;

    public:
        ClydeUtilitySys(std::shared_ptr<Character> character);
        virtual ~ClydeUtilitySys();
	    virtual Move getMove(const GameState& game) override;
};

#endif