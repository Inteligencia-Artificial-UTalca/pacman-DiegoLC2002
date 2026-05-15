#include "MsPacmanUtilitySys.h"
#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>


MsPacmanUtilitySys::MsPacmanUtilitySys(std::shared_ptr<Character> character):
	Controller(character)
{
}

MsPacmanUtilitySys::~MsPacmanUtilitySys() 
{
	// TODO Auto-generated destructor stub
}

Move MsPacmanUtilitySys::getClosestMove(const GameState& game, std::pair<int,int> target)const
{
	int minDist = 1e9;
	Move bestMove = PASS;

	auto moves = game.getMaze().getPossibleMoves(character->getPos());

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


Move MsPacmanUtilitySys::getFarthestMove(const GameState& game, std::pair<int,int> target)const
{
	int maxDist = -1;

    Move bestMove = PASS;

    auto moves = game.getMaze().getPossibleMoves(character->getPos());

    for(auto move : moves)
    {
        int next = game.getMaze().getNeighbour(character->getPos(), move);

        if(next < 0) continue;

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


float MsPacmanUtilitySys::getDistanceToGhost(const GameState& game, int g)const
{
	return sqrt(euclid2(
		game.getMaze().getNodePos(character->getPos()),
		game.getMaze().getNodePos(game.getGhostsPos(g))));
}


Move MsPacmanUtilitySys::getMove(const GameState& game)
{
    //para cerrar la ventana
	SDL_Event e;
	if( SDL_PollEvent( &e ) != 0 )
	{
		if( e.type == SDL_QUIT || 
			(e.type == SDL_KEYDOWN && 
				(e.key.keysym.sym==SDLK_ESCAPE || 
				e.key.keysym.sym==SDLK_q) ))
		{
			SDL_Quit();
			exit(0);
		}
	}

    //Variables de utilidad
    float fear = 0.0f;
    Move escapeMove = PASS;

    float hunger = 0.0f;
    Move huntMove = PASS;

    float pillUtility = 0.3f;
    Move pillMove = PASS;


    //Escapar de fantasmas
	for(int i = 0; i < 4; i++)
    {
        if(game.isGhostEdible(i)) { continue; } //Ignorar fantasmas comestibles

        float dist = getDistanceToGhost(game, i);

        // curava logística
        float utility = 1.0f / (1.0f + exp((dist - 30.0f) * 0.15f));

        if(utility > fear)
        {
            fear = utility;
            auto ghostPos = game.getMaze().getNodePos(game.getGhostsPos(i));
            escapeMove = getFarthestMove(game, ghostPos);
        }
		
	}


    //Perseguir fantasmas azules
    for(int i = 0; i < 4; i++)
    {
        if(!game.isGhostEdible(i)) { continue; }

        float dist = getDistanceToGhost(game, i);

        // cuadrática
        //Usa max para evitar valores negativos y establecerlos en 0 si lo hace.
        float utility = pow(std::max(0.0f, 80.0f - dist), 2) / (80.0f * 80.0f);

        if(utility > hunger)
        {
            hunger = utility;

            auto ghostPos = game.getMaze().getNodePos(game.getGhostsPos(i));

            huntMove = getClosestMove(game, ghostPos);
        }
    }

    //Comer pildoras
    auto pills = game.getMaze().getPillPositions();
    float minPillDist = 1e9;
    std::pair<int,int> bestPill;

    auto pacPos = game.getMaze().getNodePos(character->getPos());

    for(auto pill : pills)
    {
        float dist = euclid2(pacPos, pill);

        if(dist < minPillDist)
        {
            minPillDist = dist;
            bestPill = pill;
        }
    }

    if(!pills.empty())
    {
        pillMove = getClosestMove(game, bestPill);

        // utilidad para pildoras (a menor distancia mayor utilidad)
        pillUtility = 0.4f / (1.0f + minPillDist * 0.02f);
    }


    //Comparar utilidades 
    std::cout << "fear = " << fear << std::endl;
    std::cout << "hunger = " << hunger << std::endl;
    std::cout << "pill = " << pillUtility << std::endl;

    if(fear > hunger && fear > pillUtility)
    {
        std::cout << "Estamos escapando.!!!\n";
        return escapeMove;
    }

    if(hunger > pillUtility)
    {
        std::cout << "Vamos a comer fantasmas.!!!\n";
        return huntMove;
    }

    std::cout << "Pastilllas Yumi.!!!\n";


    return pillMove;
}