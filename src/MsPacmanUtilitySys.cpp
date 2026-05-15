




























#include "UtilityPacmanController.h"

#include <cmath>
#include <iostream>

UtilityPacmanController::UtilityPacmanController(
    std::shared_ptr<Character> character)
    : Controller(character)
{
}

UtilityPacmanController::~UtilityPacmanController()
{
}

////////////////////////////////////////////////////////

Move UtilityPacmanController::getClosestMove(
    const GameState& game,
    std::pair<int,int> target) const
{
    int minDist = 1e9;

    Move bestMove = PASS;

    auto moves =
        game.getMaze().getPossibleMoves(character->getPos());

    for(auto move : moves)
    {
        int next =
            game.getMaze().getNeighbour(character->getPos(),
                                        move);

        if(next < 0) continue;

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

////////////////////////////////////////////////////////

Move UtilityPacmanController::getFarthestMove(
    const GameState& game,
    std::pair<int,int> target) const
{
    int maxDist = -1;

    Move bestMove = PASS;

    auto moves =
        game.getMaze().getPossibleMoves(character->getPos());

    for(auto move : moves)
    {
        int next =
            game.getMaze().getNeighbour(character->getPos(),
                                        move);

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

////////////////////////////////////////////////////////

float UtilityPacmanController::getDistanceToGhost(
    const GameState& game,
    int ghost) const
{
    return sqrt(
        euclid2(
            game.getMaze().getNodePos(character->getPos()),
            game.getMaze().getNodePos(game.getGhostsPos(ghost))
        )
    );
}

////////////////////////////////////////////////////////

Move UtilityPacmanController::getMove(const GameState& game)
{
    float fear = 0.0f;
    Move escapeMove = PASS;

    float hunger = 0.0f;
    Move huntMove = PASS;

    float pillUtility = 0.3f;
    Move pillMove = PASS;

    ////////////////////////////////////////////////////
    // ESCAPE
    ////////////////////////////////////////////////////

    for(int i = 0; i < 4; i++)
    {
        if(game.isGhostEdible(i)) continue;

        float dist = getDistanceToGhost(game, i);

        // logística
        float utility =
            1.0f /
            (1.0f + exp((dist - 30.0f) * 0.15f));

        if(utility > fear)
        {
            fear = utility;

            auto ghostPos =
                game.getMaze().getNodePos(
                    game.getGhostsPos(i));

            escapeMove =
                getFarthestMove(game, ghostPos);
        }
    }

    ////////////////////////////////////////////////////
    // HUNT BLUE GHOSTS
    ////////////////////////////////////////////////////

    for(int i = 0; i < 4; i++)
    {
        if(!game.isGhostEdible(i)) continue;

        float dist = getDistanceToGhost(game, i);

        // cuadrática
        float utility =
            pow(std::max(0.0f, 80.0f - dist), 2)
            / (80.0f * 80.0f);

        if(utility > hunger)
        {
            hunger = utility;

            auto ghostPos =
                game.getMaze().getNodePos(
                    game.getGhostsPos(i));

            huntMove =
                getClosestMove(game, ghostPos);
        }
    }

    ////////////////////////////////////////////////////
    // PILLS
    ////////////////////////////////////////////////////

    auto pills = game.getMaze().getPillPositions();

    float minPillDist = 1e9;
    std::pair<int,int> bestPill;

    auto pacPos =
        game.getMaze().getNodePos(character->getPos());

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

        // utilidad inversa
        pillUtility =
            1.0f / (1.0f + minPillDist * 0.01f);
    }

    ////////////////////////////////////////////////////
    // DECISION
    ////////////////////////////////////////////////////

    if(fear > hunger && fear > pillUtility)
    {
        std::cout << "ESCAPE\n";
        return escapeMove;
    }

    if(hunger > pillUtility)
    {
        std::cout << "HUNT\n";
        return huntMove;
    }

    std::cout << "PILLS\n";

    return pillMove;
}