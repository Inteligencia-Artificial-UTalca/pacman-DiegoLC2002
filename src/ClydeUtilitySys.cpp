#include "ClydeUtilitySys.h"
#include "Ghost.h"
#include <cmath>

ClydeUtilitySys::ClydeUtilitySys(std::shared_ptr<Character> character):
	Controller(character)
{
    chasing = false;

    territoryCenter = {70, 580};

    //Zona inferior izquierda
    patrolPoints.push_back({20, 650});
    patrolPoints.push_back({120, 650});
    patrolPoints.push_back({20, 520});
    patrolPoints.push_back({120, 520});

    currentPatrol = 0;
}

ClydeUtilitySys::~ClydeUtilitySys() 
{
	// TODO Auto-generated destructor stub
}

////////////////////////////////////////////////////////////////

Move ClydeUtilitySys::getClosestMove(const GameState& game, std::pair<int,int> target)const
{
	int minDist = 1e9;
	Move bestMove = PASS;

    //Obtener posibles movimientos
    std::vector<Move> moves;

	if(character->getDirection() == PASS)
	{
		moves = game.getMaze().getPossibleMoves(character->getPos()); 
	}
	else
	{
		moves = game.getMaze().getGhostLegalMoves(character->getPos(), character->getDirection()); 
	}

    for(auto move : moves)
    {
        int next = game.getMaze().getNeighbour(character->getPos(), move);

        if(next < 0) { continue; }

        auto pos = game.getMaze().getNodePos(next);

        float dist = euclid2(pos, target);

        if(dist < minDist)
        {
            minDist = dist;
            bestMove = move;
        }
    }

    return bestMove;
}


Move ClydeUtilitySys::getFarthestMove(const GameState& game, std::pair<int,int> target)const
{
	int maxDist = -1;

    Move bestMove = PASS;

    //Obtener posibles movimientos
    std::vector<Move> moves;

	if(character->getDirection() == PASS)
	{
		moves = game.getMaze().getPossibleMoves(character->getPos()); 
	}
	else
	{
		moves = game.getMaze().getGhostLegalMoves(character->getPos(), character->getDirection()); 
	}

    for(auto move : moves)
    {
        int next = game.getMaze().getNeighbour(character->getPos(), move);

        if(next < 0) { continue; }

        auto pos = game.getMaze().getNodePos(next);

        float dist = euclid2(pos, target);

        if(dist > maxDist)
        {
            maxDist = dist;
            bestMove = move;
        }
    }

    return bestMove;
}

Move ClydeUtilitySys::getMove(const GameState& game)
{
    //Utilidades
    float patrolUtility = 0.2f;
    float chaseUtility = 0.0f;
    float fleeUtility = 0.0f;

    //Posiciones
    auto ghost = std::dynamic_pointer_cast<Ghost>(character);
    auto pacmanPos = game.getMaze().getNodePos(game.getPacmanPos());
    auto myPos = game.getMaze().getNodePos(character->getPos());

    //Detectar si pacman entra en el territorio
    float distToTerritory = sqrt(euclid2(pacmanPos, territoryCenter));

    float maxTerritoryDist = 600.0f;

    float normalizedDist = std::min(distToTerritory / maxTerritoryDist, 1.0f);

    chaseUtility = 1.0f - normalizedDist;

    //Estado frigtened
    if(ghost->isEdible())
    {
        fleeUtility = 1.0f;
    }

    //Calcular utilidad de perseguir
    //chaseUtility =  std::max(0.0f, 600.0f - distToTerritory) / 600.0f;

    //debug
    std::cout << "Patrol: " << patrolUtility
              << " Chase: " << chaseUtility
              << " Flee: " << fleeUtility
              << std::endl;

    //Comparar utilidades
    //Escapar
    if(fleeUtility > chaseUtility && fleeUtility > patrolUtility)
    {
        return getFarthestMove(game, pacmanPos);
    }

    //Perseguir
    if(chaseUtility > patrolUtility)
    {
        return getClosestMove(game, pacmanPos);
    }

    //Patrullar
    auto target = patrolPoints[currentPatrol];

    float distToTarget = sqrt(euclid2(myPos, target));

    //Cambiar punto de patrulla
    if(distToTarget < 10)
    {
        currentPatrol++;

        if(currentPatrol >= patrolPoints.size())
        {
            currentPatrol = 0;
        }

        target = patrolPoints[currentPatrol];
    }

    return getClosestMove(game, target);
}