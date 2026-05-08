#include "MsPacmanBT.h"
#include "Ghost.h"
#include <iostream>


MsPacmanInfo* MsPacmanInfo::info = nullptr;

//////////////////////////////// Controlador ////////////////////////
MsPacmanBT::MsPacmanBT(std::shared_ptr<Character> character) : Controller(character),
                                                         root(std::make_shared<Selector>())
{
    //Escapar de los fantasmas
    auto escapeFilter = std::make_shared<Filter>();
    escapeFilter->addCondition(std::make_shared<GhostDangerCondition>());
    escapeFilter->addAction(std::make_shared<EscapeGhostAction>());
    root->addChild(escapeFilter);

    //Comer Fantasmas si estan azules
    auto edibleFilter = std::make_shared<Filter>();
    edibleFilter->addCondition(std::make_shared<EdibleGhostCondition>());
    edibleFilter->addAction(std::make_shared<ChaseEdibleGhostAction>());
    root->addChild(edibleFilter);

    //buscar pildoras
    root->addChild(std::make_shared<SeekPilltAction>());
}

MsPacmanBT::~MsPacmanBT()
{
}

Move MsPacmanBT::getMove(const GameState& gs)
{
    MsPacmanInfo::getInfo()->in_character = character;
    MsPacmanInfo::getInfo()->in_gameState = &gs;

    root->tick();

    return MsPacmanInfo::getInfo()->out_move;
}


/////////////////////////////// Condiciones  //////////////////////////////
//
Status GhostDangerCondition::update()
{
    auto character = MsPacmanInfo::getInfo()->in_character;
    auto gs = MsPacmanInfo::getInfo()->in_gameState;

    auto pacmanPos = gs->getMaze().getNodePos(character->getPos());

    for(int i = 0; i < 4; i++)
    {
        if(gs->isGhostEdible(i)){ continue; }

        auto ghostPos = gs->getMaze().getNodePos(gs->getGhostsPos(i));

        float dist = euclid2(pacmanPos, ghostPos);

        if(dist < 100) { return BH_SUCCESS; }
    }

    return BH_FAILURE;
}


//Bucar fantasmas comestibles (azules)
Status EdibleGhostCondition::update()
{
    auto gs = MsPacmanInfo::getInfo()->in_gameState;

    for(int i = 0; i < 4; i++)
    {
        if(gs->isGhostEdible(i)){ return BH_SUCCESS; }

    }

    return BH_FAILURE;
}


//////////////////////////// Acciones /////////////////////////////////
//Hace que pacman escape de los fantasmas cercanos
Status EscapeGhostAction::update()
{
    auto gs = MsPacmanInfo::getInfo()->in_gameState;
    auto character = MsPacmanInfo::getInfo()->in_character;

    auto pacmanPos = gs->getMaze().getNodePos(character->getPos());

    //std::vector<Move> moves = gs->getMaze().getPossibleMoves(character->getPos());

    std::vector<Move> moves;

    if(character->getDirection() == PASS)
    {
        moves = gs->getMaze().getPossibleMoves(character->getPos());
    }
    else
    {
        moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection());
    }

    float closeGhost = 1e9;
    std::pair<int,int> targetGhost;

    //Buscar fantasma peligroso más cercano
    for(int i = 0; i < 4; i++)
    {
        if(gs->isGhostEdible(i)) { continue; }

        auto ghostPos = gs->getMaze().getNodePos(gs->getGhostsPos(i));
        float dist = euclid2(pacmanPos, ghostPos);

        if(dist < closeGhost)
        {
            closeGhost = dist;
            targetGhost = ghostPos;
        }
    }

    float maxDist = -1;
    Move bestMove = PASS;

	for(auto m : moves)
	{
		int vecino = gs->getMaze().getNeighbour(character->getPos(), m);

		if(vecino < 0) { continue; }

		auto vecinoPos = gs->getMaze().getNodePos(vecino);
		float dist = euclid2(vecinoPos, targetGhost);

		if(dist > maxDist)
		{
			maxDist = dist;
			bestMove = m;
		}
	}

    MsPacmanInfo::getInfo()->out_move = bestMove;

    return BH_SUCCESS;
}

////////////////////////////////////////
Status ChaseEdibleGhostAction::update()
{
    auto gs = MsPacmanInfo::getInfo()->in_gameState;
    auto character = MsPacmanInfo::getInfo()->in_character;

    auto pacmanPos = gs->getMaze().getNodePos(character->getPos());

    std::vector<Move> moves = gs->getMaze().getPossibleMoves(character->getPos());

    float closeGhost = 1e9;
    std::pair<int,int> targetGhost;

    //Buscar fantasma azul más cercano
    for(int i = 0; i < 4; i++)
    {
        if(!gs->isGhostEdible(i)) { continue; }

        auto ghostPos = gs->getMaze().getNodePos(gs->getGhostsPos(i));
        float dist = euclid2(pacmanPos, ghostPos);

        if(dist < closeGhost)
        {
            closeGhost = dist;
            targetGhost = ghostPos;
        }
    }

    float minDist = 1e9;
    Move bestMove = PASS;

	for(auto m : moves)
	{
		int vecino = gs->getMaze().getNeighbour(character->getPos(), m);

		if(vecino < 0) { continue; }

		auto vecinoPos = gs->getMaze().getNodePos(vecino);
		float dist = euclid2(vecinoPos, targetGhost);

		if(dist < minDist)
		{
			minDist = dist;
			bestMove = m;
		}
	}

    MsPacmanInfo::getInfo()->out_move = bestMove;

    return BH_SUCCESS;
}

//////////////////////////////////
Status SeekPilltAction::update()
{
    auto gs = MsPacmanInfo::getInfo()->in_gameState;
    auto character = MsPacmanInfo::getInfo()->in_character;

    auto pills = gs->getMaze().getPillPositions();

    if(pills.empty())
    {
        MsPacmanInfo::getInfo()->out_move = PASS;
        return BH_SUCCESS;
    }

    std::vector<Move> moves = gs->getMaze().getPossibleMoves(character->getPos());

    float minDist = 1e9;
    Move bestMove = PASS;

	for(auto m : moves)
	{
		int vecino = gs->getMaze().getNeighbour(character->getPos(), m);

		if(vecino < 0) { continue; }

		auto vecinoPos = gs->getMaze().getNodePos(vecino);

        for(auto pill : pills)
        {
            float dist = euclid2(vecinoPos, pill);

        if(dist < minDist)
		{
			minDist = dist;
			bestMove = m;
		}
        }
	}

    MsPacmanInfo::getInfo()->out_move = bestMove;

    return BH_SUCCESS;
}