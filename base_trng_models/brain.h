#ifndef BRAIN_H
#define BRAIN_H
#include <memory>
#include <iostream>
#include <sstream>
class GlCharacter;

namespace Character
{
    enum class BrainTypes {Empty,Hero,Npc};

    class IBrain
    {
        public:
        virtual void Think(GlCharacter * character) {}
        virtual ~IBrain(){}
        bool LoadBrain(const std::string &filename);
        virtual bool LoadBrain(std::istream &is) = 0;
        protected:
        std::function<void(GlCharacter & character)> m_world_reaction;
    };
    

    std::shared_ptr<IBrain> CreateBrain(BrainTypes brain_type, std::function<void(GlCharacter & character)> world_reaction);
}
#endif