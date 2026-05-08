#ifndef PINKYBT_H_ 
#define PINKYBT_H_ 

#include "Controller.h"
#include "BehaviorTree.h"
#include <random>
#include <chrono>

class PinkyInfo
{
    private:
        static PinkyInfo* info;
        PinkyInfo() {};

    public:
        static PinkyInfo* getInfo()
        {
            if(info == nullptr)
            {
                info = new PinkyInfo();
            }

            return info;
        }

        const GameState* in_gameState;
        Move out_move;
        std::shared_ptr<Character> in_character;
};

class PinkyBT : public Controller
{
    private:
        std::shared_ptr<Composite> root;

    public:
        PinkyBT(std::shared_ptr<Character> character);
        virtual ~PinkyBT();
        Move getMove(const GameState& gs) override;
        
};


////////////////////////// Behaviors Pinky //////////////////////////
class PinkyChase : public Behavior
{
    public:
        virtual Status update() override;
};

class PinkyScatter : public Behavior
{
    private:
        std::pair<int,int> target;
    
    public:
        PinkyScatter();
        virtual Status update() override;
};

class PinkyFrightened : public Behavior
{
    private:
        std::mt19937 e; 
	    std::uniform_int_distribution<int> uniform_dist; 

    public:
        PinkyFrightened();
        virtual Status update() override;
};


////////////////////////// Conditions //////////////////////////
class PinkyPowerpill : public Behavior
{
    public:
        virtual Status update() override;
};

class PinkyTimeout : public Behavior
{
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;

    public:
        PinkyTimeout();
        virtual Status update() override;
};


#endif /* PINKYBT_H_ */
