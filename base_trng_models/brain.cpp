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

            std::weak_ptr<GlCharacter> arch_enemy;

            if(!character.enemies_list.empty())
            {
                auto enemy_it = std::max_element( character.enemies_list.begin(),
                                                    character.enemies_list.end(),
                                                    [](std::pair<std::weak_ptr<GlCharacter>,float> a,std::pair<std::weak_ptr<GlCharacter>,float> b){return a.second < b.second;});
                
                arch_enemy = enemy_it->first;
            }

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
                    if(dice_roll > (random_maximum/4))
                    {
                        character.UseSequence("stance");
                    }
                    else
                    {
                        //character.UseSequence("strike");
                    }
                }  
            }
            if(arch_enemy.expired())
            {
                if(rotator == 0)
                {
                    dice_roll = distribution(random_generator);
                    
                    if(dice_roll>(random_maximum - 50))
                    {
                        rotator = distribution(random_generator)-random_maximum/2;
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
                    }
                    character.model_matrix = glm::rotate(character.model_matrix , glm::radians(sign * 0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
                }
            }
            else
            {
                glm::vec3 y_basis = glm::vec3(0.0f,1.0f,0.0f);
                glm::vec3 z_basis = glm::vec3(0.0f,0.0f,0.0f);

                glm::vec3 enemy_vector = arch_enemy.lock()->GetPosition() - character.GetPosition();
                float enemy_distance = glm::length(enemy_vector);
                z_basis = -glm::normalize(enemy_vector);
                
                glm::vec3 x_basis = glm::cross(y_basis, z_basis);

                character.model_matrix = glm::mat4(
                    glm::vec4(x_basis[0],x_basis[1],x_basis[2],0.0f),
                    glm::vec4(y_basis[0],y_basis[1],y_basis[2],0.0f),
                    glm::vec4(z_basis[0],z_basis[1],z_basis[2],0.0f),
                    glm::vec4(0.0,0.0,0.0,1.0f)
                    );

                if(enemy_distance > 7.0f)
                {
                    character.UseSequence("run");
                }
                else
                {
                    if(enemy_distance > 2.0f)
                    {
                        character.UseSequence("walk");   /* code */
                    }
                    else
                    {
                        dice_roll = distribution(random_generator);
                        if(dice_roll>(random_maximum - 50))
                        {
                            character.UseSequence("strike");/* code */
                        }
                    }
                    
                }
                
            }

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