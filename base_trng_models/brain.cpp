#include "brain.h"

namespace Character
{
    class BrainEmpty: public IBrain
    {
        
    };
    
    class BrainHero: public IBrain
    {
        public:
        BrainHero(std::function<void(GlCharacter & character)> world_reaction)
        {
            m_world_reaction = world_reaction;
        }
        virtual void Think(GlCharacter & character) 
        {
            m_world_reaction(character);
        }
    };

    std::shared_ptr<IBrain> CreateBrain(BrainTypes brain_type, std::function<void(GlCharacter & character)> world_reaction)
    {
        switch(brain_type)
        {
            case BrainTypes::Hero:
                return std::make_shared<BrainHero>(world_reaction);
            default:
                return std::make_shared<BrainEmpty>();
        }
    }
}