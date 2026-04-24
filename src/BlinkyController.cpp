#include "BlinkyController.h"
#include "Ghost.h"

//Blinky (Fantasma rojo) --> Persigue a Pacman como una sombra.

BlinkyController::BlinkyController(std::shared_ptr<Character> character):
	Controller(character){
}

BlinkyController::~BlinkyController() {

}

Move
BlinkyController::getMove(const GameState& game){
	
	auto ghost = std::dynamic_pointer_cast<Ghost>(character);
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

	Move bestMove = PASS;

	//Desicion Tree (DT)
	if(ghost != nullptr && ghost->isEdible())
	{
		//Huir de pacman cuando el fantasma es comestible
		float maxDis = -1.0f;

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
	}
	else
	{
		//Perseguir a Pacman
		float minDist = 1e9;

		for(auto m : moves)
		{
			int vecino = game.getMaze().getNeighbour(character->getPos(), m);

			if(vecino < 0) { continue; }

			auto vecinoPos = game.getMaze().getNodePos(vecino);
			float dist = euclid2(vecinoPos, pacmanPos);

			if(dist < minDist)
			{
				minDist = dist;
				bestMove = m;
			}
		}

	}
	return bestMove;
}
