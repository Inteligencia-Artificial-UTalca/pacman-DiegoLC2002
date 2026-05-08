#include "PinkyBT.h"
#include "Ghost.h"
#include <iostream>

PinkyInfo* PinkyInfo::info = nullptr;

/////////////////////// Controladores ////////////////////////
PinkyBT::PinkyBT(std::shared_ptr<Character> character) : Controller(character),
                                                         root(std::make_shared<Selector>())
{
    //Frightened
    auto filter1 = std::make_shared<Filter>();
    filter1->addCondition(std::make_shared<PinkyPowerpill>());
    filter1->addAction(std::make_shared<PinkyFrightened>());
    root->addChild(filter1);

    //Scatter
    auto filter2 = std::make_shared<Filter>();
    filter2->addCondition(std::make_shared<PinkyTimeout>());
    filter2->addAction(std::make_shared<PinkyScatter>());
    root->addChild(filter2);

    //Chase
    root->addChild(std::make_shared<PinkyChase>());
};

PinkyBT::~PinkyBT()
{
}

Move PinkyBT::getMove(const GameState& gs)
{
    PinkyInfo::getInfo()->in_character = character;
    PinkyInfo::getInfo()->in_gameState = &gs;

    root->tick();

    return PinkyInfo::getInfo()->out_move;
};


//////////////////////// Pinky Chase ////////////////////////
Status PinkyChase::update()
{
    auto character = PinkyInfo::getInfo()->in_character;
    auto gs = PinkyInfo::getInfo()->in_gameState;

    std::vector<Move> moves;

    //Obtener posibles movimientos
	if(character->getDirection() == PASS)
	{
		moves = gs->getMaze().getPossibleMoves(character->getPos()); 
	}
	else
	{
		moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection()); 
	}

    auto targetNode = gs->getPacmanPos();   //Obtener posicion de pacman
    Move pacDir = (Move)gs->getPacmanDir(); //obtener direccion del pacman

    //Apuntar 4 casillas delante del pacman
    for(int i = 0; i < 4; i++)
    {
        int next = gs->getMaze().getNeighbour(targetNode, pacDir);

        if(next < 0) { break; }
        
        targetNode = next;
    }

    auto target = gs->getMaze().getNodePos(targetNode);

    float minDist = 1e9;
    Move bestMove = PASS;

    for(auto move : moves)
    {
        if(move == PASS) { continue; }

        int next = gs->getMaze().getNeighbour(character->getPos(), move);
        auto pos = gs->getMaze().getNodePos(next);
        float dist = euclid2(target, pos);

        if(dist < minDist)
        {
            minDist = dist;
            bestMove = move;
        }
    }

    PinkyInfo::getInfo()->out_move = bestMove;

    return BH_SUCCESS;
}


//////////////////////// Scatter ////////////////////////
PinkyScatter::PinkyScatter()
{
    target = std::make_pair(0,0);   //Esquina superior izquierda
}

Status PinkyScatter::update()
{
    auto character = PinkyInfo::getInfo()->in_character;
    auto gs = PinkyInfo::getInfo()->in_gameState;

    std::vector<Move> moves;

    //Obtener posibles movimientos
	if(character->getDirection() == PASS)
	{
		moves = gs->getMaze().getPossibleMoves(character->getPos()); 
	}
	else
	{
		moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection()); 
	}

    float minDist = 1e9;
    Move bestMove = PASS;

    for(auto move : moves)
    {
        if(move == PASS) { continue; }

        int next = gs->getMaze().getNeighbour(character->getPos(), move);
        auto pos = gs->getMaze().getNodePos(next);
        float dist = euclid2(target, pos);

        if(dist < minDist)
        {
            minDist = dist;
            bestMove = move;
        }
    }

    PinkyInfo::getInfo()->out_move = bestMove;

    return BH_SUCCESS;
}


//////////////////////// Frightened ////////////////////////
PinkyFrightened::PinkyFrightened() : Behavior(), e(rand()), uniform_dist(0, 3)
{
}


Status PinkyFrightened::update()
{
    auto character = PinkyInfo::getInfo()->in_character;
    auto gs = PinkyInfo::getInfo()->in_gameState;

    std::vector<Move> moves;

    //Obtener posibles movimientos
	if(character->getDirection() == PASS)
	{
		moves = gs->getMaze().getPossibleMoves(character->getPos()); 
	}
	else
	{
		moves = gs->getMaze().getGhostLegalMoves(character->getPos(), character->getDirection()); 
	}

    Move m = moves[rand() % moves.size()];

    PinkyInfo::getInfo()->out_move = m;

    return BH_SUCCESS;
}

//////////////////////// Conditions ////////////////////////

Status PinkyPowerpill::update()
{
    auto character = PinkyInfo::getInfo()->in_character;
    auto ghost = dynamic_cast<Ghost*>(character.get());

    if(ghost != nullptr && ghost->isEdible())
    {
        return BH_SUCCESS;
    }

    return BH_FAILURE;
}

//////////////////////// Timeout ////////////////////////

PinkyTimeout::PinkyTimeout()
{
    lastTime = std::chrono::high_resolution_clock::now();
}

Status PinkyTimeout::update()
{
    std::chrono::duration<float> timeStamp =
        std::chrono::high_resolution_clock::now() - lastTime;

    if((int)timeStamp.count() % 27 < 7)
    {
        return BH_SUCCESS;
    }

    return BH_FAILURE;
}