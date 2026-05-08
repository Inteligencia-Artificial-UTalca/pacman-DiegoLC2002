#ifndef MSPACMANBT_H_
#define MSPACMANBT_H_

#include "Controller.h"
#include "BehaviorTree.h"
#include <memory>

class MsPacmanInfo
{
    private:
        static MsPacmanInfo* info;
        MsPacmanInfo() {};

    public:
        static MsPacmanInfo* getInfo()
        {
            if(info == nullptr)
            {
                info = new MsPacmanInfo();
            }

            return info;
        }

        const GameState* in_gameState;
        Move out_move;
        std::shared_ptr<Character> in_character;
};


///////////////////////////////////////////////////////////
class MsPacmanBT : public Controller
{
    private:
        std::shared_ptr<Composite> root;

    public:
        MsPacmanBT(std::shared_ptr<Character> character);
        virtual ~MsPacmanBT();
        Move getMove(const GameState& gs) override;
        
};


/////////////////////////// Pacman Acciones ///////////////////////////
class EscapeGhostAction : public Behavior
{
    public:
        virtual Status update() override;
};

class ChaseEdibleGhostAction : public Behavior
{
    public:
        virtual Status update() override;
};

class SeekPilltAction : public Behavior
{
    public:
        virtual Status update() override;
};

//////////////////////////////// Condiciones //////////////////////////7
class GhostDangerCondition : public Behavior
{
    public:
        virtual Status update() override;
};

class EdibleGhostCondition : public Behavior
{
    public:
        virtual Status update() override;
};


#endif