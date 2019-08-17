#include "brain.h"
#include "gl_character.h"
#include <random>

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

    
    class BrainMob: public IBrain
    {
        public:
        BrainMob(std::function<void(GlCharacter & character)> world_reaction):rotator(0)
        {
            m_world_reaction = world_reaction;
        }
        virtual void Think(GlCharacter & character) 
        {
            m_world_reaction(character);

            std::uniform_int_distribution<int> distribution(1,random_maximum);
            int dice_roll = distribution(random_generator);
            if(dice_roll>random_maximum - 1)
            {
                dice_roll = distribution(random_generator);
                if(dice_roll > (random_maximum/2))
                {
                    if(dice_roll > (5*random_maximum/8))
                    {
                        character.UseSequence("walk");
                    }
                    else
                    {
                        character.UseSequence("run");
                    }
                    
                }
                else
                {
                    character.UseSequence("stance");
                }
                
                // switch(unit_control_action.first)
                // {
                //     case AnimationCommand::kStrike:
                //         character.UseSequence("strike");
                //         m_messages.push_back("hero_strike");
                //     break;
                //     case AnimationCommand::kUse:
                //         character.UseSequence("use");
                //         m_messages.push_back("hero_use");
                //     break;
                //     case AnimationCommand::kMove:
                //         character.UseSequence("walk");
                //     break;
                //     case AnimationCommand::kFastMove:
                //         character.UseSequence("run");
                //     break;
                //     default:
                //         character.UseSequence("stance");
                //     break;
                // }
            }

            if(rotator == 0)
            {
                dice_roll = distribution(random_generator);
                
                if(dice_roll>(random_maximum - 50))
                {
                    rotator = distribution(random_generator)-random_maximum/2;
                    //character.model_matrix = glm::rotate(character.model_matrix , glm::radians((dice_roll-random_maximum) * 0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
                }
            }
            else
            {
                int sign = 1;
                if(rotator > 0)
                {
                    --rotator;
                }
                else
                {
                   ++rotator;
                   sign = -1;
                    /* code */
                }
                
                character.model_matrix = glm::rotate(character.model_matrix , glm::radians(sign * 0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            
            //character.model_matrix = unit_control_action.second;
        }
        private:
        const int random_maximum = 150;
        int rotator;
        static std::default_random_engine random_generator;
    };

    std::shared_ptr<IBrain> CreateBrain(BrainTypes brain_type, std::function<void(GlCharacter & character)> world_reaction)
    {
        switch(brain_type)
        {
            case BrainTypes::Hero:
                return std::make_shared<BrainHero>(world_reaction);
            case BrainTypes::Npc:
                return std::make_shared<BrainMob>(world_reaction);
            default:
                return std::make_shared<BrainEmpty>();
        }
    }

    std::default_random_engine BrainMob::random_generator;
}