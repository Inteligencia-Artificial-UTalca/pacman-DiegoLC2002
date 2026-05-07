#ifndef BLINKYFSM_H_
#define BLINKYFSM_H_

#include "Controller.h"
#include <random>
#include "FSM.h"
#include <chrono>

class BlinkyStateMachine;

class BlinkyFSM: public Controller 
{
	std::shared_ptr<BlinkyStateMachine> fsm;
public:
	BlinkyFSM(std::shared_ptr<Character> character);
	virtual ~BlinkyFSM();
	virtual Move getMove(const GameState& game)override;
};

/////////////////////////////// States ////////////////////////////////////
class ChaseState:public FSMState
{
    public:
        ChaseState(std::shared_ptr<Character> _character);
        Move onUpdate(const GameState& gs) override;
        void onEnter(const GameState& gs) override;
        ~ChaseState();

};


class ScatterState : public FSMState
{
	public:
		ScatterState(std::shared_ptr<Character> character);
		Move onUpdate(const GameState& gs) override;
};

class FrightenedState : public FSMState
{
	public:
		FrightenedState(std::shared_ptr<Character> character);
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


//////////////////////////// Transiciones entre States ////////////////////////////7
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

////////////////////////// State Machine ///////////////////////////////////

class BlinkyStateMachine: public FiniteStateMachine
{
    std::shared_ptr<FSMState> frightened;
    std::shared_ptr<FSMState> nonFrightened;

	public:
		BlinkyStateMachine(std::shared_ptr<Character> _character);
		Move update(const GameState& gs) override;
		~BlinkyStateMachine();
};

#endif /* BLINKYFSM_H_ */
