/*
 * FSMController.cpp
 *
 *  Created on: Apr 23, 2018
 *      Author: nbarriga
 */

#include "FSMController.h"
#include <iostream>
#include "FSM.h"

FSMController::FSMController(std::shared_ptr<Character> character):
	Controller(character),
	e(rand()),
	uniform_dist(0,3),
	fsm(std::make_shared<BlinkyStateMachine>(character)) {
}

FSMController::~FSMController() {
	// TODO Auto-generated destructor stub
}

Move
FSMController::getMove(const GameState& game){
	return fsm->update(game);
}


///////////////////////////////////PillTransition///////////////////////////////
PillTransition::PillTransition(std::shared_ptr<FSMState> next):last(0),_next(next){

}

bool PillTransition::isValid(const GameState& gs){
	int quedan=gs.getMaze().getPillPositions().size();
	if(last!=quedan && quedan%20==0){
		last =quedan;
		return true;
	}
	return false;
}
std::shared_ptr<FSMState> PillTransition::getNextState(){
	return _next;
}



///////////////////////////////ChaseState///////////////////////////////////////
ChaseState::ChaseState(std::shared_ptr<Character> _character):FSMState(_character){

}
void ChaseState::onEnter(const GameState& ){
	std::dynamic_pointer_cast<Ghost>(character)->revert();
}
Move ChaseState::onUpdate(const GameState& game){
	std::vector<Move> moves;
	const auto pacmanCoord=game.getMaze().getNodePos(game.getPacmanPos());
	const auto myPos=character->getPos();
	//const auto myCoord=game.getMaze().getNodePos(myPos);

	if(character->getDirection()==PASS){
		moves=game.getMaze().getPossibleMoves(myPos);
	}else{
		moves=game.getMaze().getGhostLegalMoves(myPos,character->getDirection());
	}

	float min=euclid2(
		game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[0])),
			pacmanCoord);
	int minI=0;
	for(unsigned int i=1;i<moves.size();i++){
		auto dist=euclid2(
			game.getMaze().getNodePos(game.getMaze().getNeighbour(myPos,moves[i])),
			pacmanCoord);
		if(dist<min){
			min=dist;
			minI=i;
		}
	}
	return moves[minI];
}
ChaseState::~ChaseState(){

}


/////////////////////////////////////BlinkyStateMachine/////////////////////////////
BlinkyStateMachine::BlinkyStateMachine(std::shared_ptr<Character> _character):FiniteStateMachine(_character){
	/*initialState = std::make_shared<ChaseState>(character);
	activeState=initialState;
	states.push_back(initialState);
	activeState->addTransition(std::make_shared<PillTransition>(activeState));
	*/

	chase = std::make_shared<ChaseState>(character);
	scatter = std::make_shared<ScatterState>(character);
	frightened = std::make_shared<FrightenedState>(character);

	//Guardar estados
	states.push_back(chase);
	states.push_back(scatter);
	states.push_back(frightened);

	//Estado incial
	initialState = chase;
	activeState = initialState;

	modeStart = std::chrono::high_resolution_clock::now();
	inScatter = false; // empieza en Chase

	//Transiciones entre estados
	//Chase --> Frightened
	chase->addTransition(std::make_shared<ToFrightenedTransition>(frightened, character));

	//Scatter --> Frightened
	scatter->addTransition(std::make_shared<ToFrightenedTransition>(frightened, character));

	//Frightened --> Chase
	frightened->addTransition(std::make_shared<ToChaseTransition>(chase, character));

}



Move BlinkyStateMachine::update(const GameState& gs){

	//Revisar transiciones a frigtened
	auto t=activeState->getActiveTransition(gs);
	if(t!=nullptr){
		activeState->onExit(gs);
		t->onTransition(gs);
		activeState=t->getNextState();
		activeState->onEnter(gs);
	}

	//Manejar tiempo si no se esta en frightened
	if(activeState != frightened)
	{
		auto now = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration<double>(now - modeStart).count();

        if(inScatter && elapsed >= 7.0)
        {
            activeState->onExit(gs);
            activeState = chase;
            activeState->onEnter(gs);

            inScatter = false;
            modeStart = now;

            std::cout << "CHANGE TO CHASE\n";
        }
        else if(!inScatter && elapsed >= 20.0)
        {
            activeState->onExit(gs);
            activeState = scatter;
            activeState->onEnter(gs);

            inScatter = true;
            modeStart = now;

            std::cout << "CHANGE TO SCATTER\n";
        }
	}


	return activeState->onUpdate(gs);
}


BlinkyStateMachine::~BlinkyStateMachine(){

}

///////////////////////////////FrightenedState///////////////////////////////////////
FrightenedState::FrightenedState(std::shared_ptr<Character> _character) : FSMState(_character){}

Move FrightenedState::onUpdate(const GameState& game)
{
	//Posicion del pacman
	auto pacmanPos = game.getMaze().getNodePos(game.getPacmanPos());

	std::vector<Move> moves;
	
	//Obtener posibles movimientos
	if(character->getDirection() == PASS)
	{
		moves = game.getMaze().getPossibleMoves(character->getPos()); 
	}
	else
	{
		moves = game.getMaze().getGhostLegalMoves(character->getPos(), character->getDirection()); 
	}

	//Evitar crasheo
	if(moves.empty()) { return PASS; }

	float maxDis = -1.0f;
	Move bestMove = moves[0];

	//Buscar el movimiento más lejano
	for(auto m : moves)
	{
		int vecino = game.getMaze().getNeighbour(character->getPos(), m);

		if(vecino < 0) { continue; }

		auto vecinoPos = game.getMaze().getNodePos(vecino);
		float dist = euclid2(vecinoPos, pacmanPos);

		if(dist > maxDis)
		{
			maxDis = dist;
			bestMove = m;
		}
	}

	return bestMove;
}

///////////////////////////////ScatterState///////////////////////////////////////
ScatterState::ScatterState(std::shared_ptr<Character> _character) : FSMState(_character){}

Move ScatterState::onUpdate(const GameState& game)
{
	std::vector<Move> moves;

	const auto myPos = character->getPos();

	//Esquina superior derecha
	std::pair<int,int> target = {1000,0};	//Afuera del mapa (aproximacion)

	if(character->getDirection() == PASS)
	{
		moves = game.getMaze().getPossibleMoves(myPos);
	}
	else
	{
		moves = game.getMaze().getGhostLegalMoves(myPos, character->getDirection());
	}

	float minDis = 1e9;
	Move bestMove = moves[0];

	//Buscar el movimiento más lejano
	for(auto m : moves)
	{
		int vecino = game.getMaze().getNeighbour(myPos, m);

		if(vecino < 0) { continue; }

		auto vecinoPos = game.getMaze().getNodePos(vecino);
		float dist = euclid2(vecinoPos, target);

		if(dist < minDis)
		{
			minDis = dist;
			bestMove = m;
		}
	}

	return bestMove;
}

/////////////////////////////////State Transitions//////////////////////////////////////////7
// De chase --> Frightened
ToFrightenedTransition::ToFrightenedTransition(std::shared_ptr<FSMState> next,
    std::shared_ptr<Character> character) : next(next), character(character) {}

bool ToFrightenedTransition::isValid(const GameState& gs)
{
	auto ghost = std::dynamic_pointer_cast<Ghost>(character);
	return ghost->isEdible();
}

std::shared_ptr<FSMState> ToFrightenedTransition::getNextState(){ return next; }


//De Frightened --> Chase
ToChaseTransition::ToChaseTransition(std::shared_ptr<FSMState> next,
    std::shared_ptr<Character> character) : next(next), character(character) {}

bool ToChaseTransition::isValid(const GameState& gs)
{
	auto ghost = std::dynamic_pointer_cast<Ghost>(character);
	return !ghost->isEdible();
}

std::shared_ptr<FSMState> ToChaseTransition::getNextState(){ return next; }



/*
//////////////////////////////TimeTransition//////////////////////
TimeTransition::TimeTransition(std::shared_ptr<FSMState> next, double seconds) : next(next), duration(seconds)
{
	lastSwitch = std::chrono::high_resolution_clock::now();
}

bool TimeTransition::isValid(const GameState&)
{
	auto now = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = now - lastSwitch;

	if(diff.count() >= duration)
	{
		lastSwitch = now;
		return true;
	}
	return false;
}

std::shared_ptr<FSMState> TimeTransition::getNextState() { return next; } */