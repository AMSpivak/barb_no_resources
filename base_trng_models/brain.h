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
        virtual void Think(GlCharacter * character, std::weak_ptr<GlCharacter> this_weak) {}
        virtual ~IBrain(){}
        protected:
        std::function<void(GlCharacter & character)> m_world_reaction;
    };
    

    std::shared_ptr<IBrain> CreateBrain(BrainTypes brain_type, std::function<void(GlCharacter & character)> world_reaction);
}
#endif