#ifndef BRAIN_H
#define BRAIN_H
#include <memory>

class GlCharacter;

namespace Character
{
    enum class BrainTypes {Empty,Hero,Npc};

    class IBrain
    {
        public:
        virtual void Think(GlCharacter & character) {}
        virtual ~IBrain(){}
        protected:
        std::function<void()> m_world_reaction;
    };
    

    std::shared_ptr<IBrain> CreateBrain(BrainTypes brain_type, std::function<void()> world_reaction);
}
#endif