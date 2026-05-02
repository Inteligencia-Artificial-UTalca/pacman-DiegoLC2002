/*
 * FSMController.h
 *
 *  Created on: Apr 23, 2018
 *      Author: nbarriga
 */

#ifndef FSMCONTROLLER_H_
#define FSMCONTROLLER_H_

#include "Controller.h"
#include <random>
#include "FSM.h"

class BlinkyStateMachine;

class FSMController: public Controller {
	std::mt19937 e;
	std::uniform_int_distribution<int> uniform_dist;
	std::shared_ptr<BlinkyStateMachine> fsm;
public:
	FSMController(std::shared_ptr<Character> character);
	virtual ~FSMController();
	virtual Move getMove(const GameState& game)override;
};

class PillTransition:public FSMTransition{
	int last;
	std::shared_ptr<FSMState> _next;
public:
	PillTransition(std::shared_ptr<FSMState> next);
	bool isValid(const GameState& gs)override;
	std::shared_ptr<FSMState> getNextState()override;
};

class ChaseState:public FSMState{

public:
	ChaseState(std::shared_ptr<Character> _character);
	Move onUpdate(const GameState& gs) override;
	void onEnter(const GameState& gs) override;
	~ChaseState();

};

class BlinkyStateMachine: public FiniteStateMachine
{
    std::shared_ptr<FSMState> frightened;
    std::shared_ptr<FSMState> nonFrightened;

	public:
		BlinkyStateMachine(std::shared_ptr<Character> _character);
		Move update(const GameState& gs) override;
		~BlinkyStateMachine();
};


class FrightenedState : public FSMState
{
	public:
		FrightenedState(std::shared_ptr<Character> character);
		Move onUpdate(const GameState& gs) override;

};

class ScatterState : public FSMState
{
	public:
		ScatterState(std::shared_ptr<Character> character);
		Move onUpdate(const GameState& gs) override;
};

class NonFrightenedState : public FSMState
{
    std::shared_ptr<FSMState> chase;
    std::shared_ptr<FSMState> scatter;
    std::shared_ptr<FSMState> activeSubState;

    std::chrono::time_point<std::chrono::high_resolution_clock> modeStart;
    bool inScatter;

public:
    NonFrightenedState(std::shared_ptr<Character> character,
                       std::shared_ptr<FSMState> chase,
                       std::shared_ptr<FSMState> scatter);

    Move onUpdate(const GameState& gs) override;
    void onEnter(const GameState& gs) override;
};


class ToFrightenedTransition : public FSMTransition
{
	std::shared_ptr<FSMState> next;
	std::shared_ptr<Character> character;

	public:
		ToFrightenedTransition(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character);
		bool isValid(const GameState& gs) override;
		std::shared_ptr<FSMState> getNextState() override;
};

class ToNonFrightenedTransition : public FSMTransition
{
	std::shared_ptr<FSMState> next;
	std::shared_ptr<Character> character;

	public:
		ToNonFrightenedTransition(std::shared_ptr<FSMState> next, std::shared_ptr<Character> character);
		bool isValid(const GameState& gs) override;
		std::shared_ptr<FSMState> getNextState() override;
};

/*
class TimeTransition : public FSMTransition
{
	std::shared_ptr<FSMState> next;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastSwitch;
	double duration;

	public:
		TimeTransition(std::shared_ptr<FSMState> next, double seconds);
		bool isValid(const GameState& gs) override;
		std::shared_ptr<FSMState> getNextState() override;
};
*/

#endif /* FSMCONTROLLER_H_ */
