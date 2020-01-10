#include "brain.h"

#include "gl_character.h"
#include <random>
#include <tuple>

#include "glm/glm.hpp"

#include "glm/trigonometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "math3d.h"  
//#include <GLFW/glfw3.h>
namespace Character
{
    constexpr float attacker_time = 0.5f;
    class BrainEmpty: public IBrain
    {
        
    };

    bool HatesLess(const std::pair<std::weak_ptr<GlCharacter>,float> a,const std::pair<std::weak_ptr<GlCharacter>,float> b, const GlCharacter & character)
    {
        auto a_ptr = a.first.lock();
        if(!a_ptr)
        {
            return false;
        }
        auto b_ptr = b.first.lock();
        if(!b_ptr)
        {
            return false;
        }

        auto l_a = character.GetPosition() - a_ptr->GetPosition();
        auto l_b = character.GetPosition() - b_ptr->GetPosition();

        float d_a = glm::dot(l_a,l_a);
        if(d_a < 0.001f)
            return true;

        float d_b = glm::dot(l_b,l_b);
        if(d_b < 0.001f)
            return false; 

        return a.second/d_a < b.second/d_b;
    }


    bool HeroChoiseLess(std::pair<std::weak_ptr<GlCharacter>,float> &a, std::pair<std::weak_ptr<GlCharacter>,float> &b, const GlCharacter & character)
    {
        auto a_ptr = a.first.lock();
        if(!a_ptr)
        {
            return false;
        }
        auto b_ptr = b.first.lock();
        if(!b_ptr)
        {
            return false;
        }

        auto l_a = character.GetPosition() - a_ptr->GetPosition();
        auto l_b = character.GetPosition() - b_ptr->GetPosition();

        glm::vec3 hero_direction;
        glm::vec3 hero_side;
        std::tie(hero_direction, hero_side) = character.Get2DBasis();
        
        float d_a = glm::dot(l_a,l_a);
        a.second = d_a;
        float d_ac = glm::dot(l_a,hero_direction);

        float d_b = glm::dot(l_b,l_b);
        b.second = d_b;
        float d_bc = glm::dot(l_b,hero_direction);
    
        return (d_ac) > (d_bc);
    }
    
    class BrainHero: public IBrain
    {
        public:
        BrainHero(std::function<void(GlCharacter & character)> world_reaction)
        {
            m_world_reaction = world_reaction;
        }
        virtual void Think(GlCharacter * character) 
        {
            constexpr float enemy_keep_range = 5.0f;
            constexpr float enemy_keep_range_2 = enemy_keep_range * enemy_keep_range;
            m_world_reaction(*character);

            if(!character->enemies_list.empty())
            {
                auto enemy_it = std::max_element(   character->enemies_list.begin(),
                                                    character->enemies_list.end(),
                                                    [&](std::pair<std::weak_ptr<GlCharacter>,float> a,std::pair<std::weak_ptr<GlCharacter>,float> b)->bool
                                                    {
                                                        return HeroChoiseLess(a,b,*character);
                                                    });
                
                
                if((enemy_it->second < enemy_keep_range_2)&&!enemy_it->first.expired() && (enemy_it->first.lock()->GetLifeValue() > 0.0f))
                {
                    character->arch_enemy = enemy_it->first;
                }
                else
                {
                    character->enemies_list.erase(enemy_it);
                }  
            }

            if(!character->arch_enemy.expired())
            {

                auto arch_distance_vec = character->GetPosition() - character->arch_enemy.lock()->GetPosition();
                float arch_distance = glm::dot(arch_distance_vec,arch_distance_vec);
                if(arch_distance > enemy_keep_range_2)
                {
                    character->arch_enemy.reset();
                }
            }

            
            switch(character->GetDamageReaction())
            {
                case DamageReaction::Block:
                case DamageReaction::StrikeBack:
                {
                    auto p_d_info = character->GetDungeonHeroInfo();
                    if(p_d_info && !p_d_info->attackers.empty())
                    {
                        auto attacker = p_d_info->attackers.front().second;
                        if(auto p_attacker = attacker.lock())
                        {
                            auto arch_distance_vec = character->GetPosition() - p_attacker->GetPosition();
                            float arch_distance = glm::dot(arch_distance_vec,arch_distance_vec);
                            if(arch_distance < enemy_keep_range_2)
                                character->arch_enemy = attacker;
                        }
                    }
                }
                break;
                default:
                break;
            }
        }
    };

    


    class BrainMob: public IBrain
    {
        private:
        float distance;
        public:
        BrainMob(std::function<void(GlCharacter & character)> world_reaction):rotator(0), distance(0.0f)
        {
            m_world_reaction = world_reaction;
        }
        virtual void Think(GlCharacter * character) 
        {
            m_world_reaction(*character);

            if(!character->enemies_list.empty())
            {
                auto enemy_it = std::max_element(   character->enemies_list.begin(),
                                                    character->enemies_list.end(),
                                                    [&](std::pair<std::weak_ptr<GlCharacter>,float> a,std::pair<std::weak_ptr<GlCharacter>,float> b)->bool
                                                    {
                                                        return HatesLess(a,b,*character);
                                                    });
                
                if((enemy_it->second > 0.0f) && !enemy_it->first.expired() && (enemy_it->first.lock()->GetLifeValue() > 0.0f))
                {
                    character->arch_enemy = enemy_it->first;
                }
                else
                {
                    character->enemies_list.erase(enemy_it);
                }
                

                
            }


            std::uniform_int_distribution<int> distribution(1,random_maximum);
            int dice_roll = distribution(random_generator);
            
            if(character->arch_enemy.expired())
            {
                if(dice_roll>random_maximum - 1)
                {
                    dice_roll = distribution(random_generator);
                    if(dice_roll > (random_maximum/2))
                    {
                        if(dice_roll > (5*random_maximum/8))
                        {
                            character->UseCommand(AnimationCommand::kMove);
                        }
                        else
                        {
                            character->UseCommand(AnimationCommand::kFastMove);
                        } 
                    }
                    else
                    {
                        if(dice_roll > (random_maximum/4))
                        {
                            character->UseCommand(AnimationCommand::kStance);
                            //character.UseSequence("stance");
                        }
                    }  
                }
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
                    character->model_matrix = glm::rotate(character->model_matrix , glm::radians(sign * 0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
                }
            }
            else
            if(auto enemy = character->arch_enemy.lock())
            {
                glm::vec3 enemy_vector = enemy->GetPosition() - character->GetPosition();
                float enemy_distance = glm::length(enemy_vector);
                constexpr float distance_smooth = 0.1f;
                distance = enemy_distance * distance_smooth + (1.0f - distance_smooth) * distance;
                enemy_distance = distance; 
                
                constexpr float fit = -45.0f;
                character->model_matrix = RotateToDirection2d(*character, enemy_vector, fit);



                if(enemy_distance > 7.0f)
                {
                    character->UseCommand(AnimationCommand::kFastMove);
                }
                else
                {
                    bool no_strike = true;
                    auto p_d_info = character->GetDungeonHeroInfo();

                    if(
                        (!p_d_info->attackers.empty())
                        &&(p_d_info->attackers.front().second.lock().get() == character)
                        )
                    {
                        if((p_d_info->now_time - p_d_info->attackers.front().first) > attacker_time)
                        {
                            if(character->UseCommand(AnimationCommand::kStrike))
                            {
                                p_d_info->attackers.pop_front();
                                no_strike = false;
                            }
                        }
                        
                    }

                    if(no_strike)
                    {
                        if(enemy_distance > 3.3f)
                        {
                            character->UseCommand(AnimationCommand::kMove);   /* code */
                        }
                        else
                        if(enemy_distance < 2.0f)
                        {
                            character->UseCommand(AnimationCommand::kStepBack);   /* code */
                        }
                        else
                        {                        
                            dice_roll = distribution(random_generator);
                            if(dice_roll>(random_maximum - 50))
                            {
                                if(p_d_info->hero.lock() != character->arch_enemy.lock()) 
                                {
                                    character->UseCommand(AnimationCommand::kStrike);
                                }
                                else
                                {               
                                    bool can_attack = true;
                                    auto s = p_d_info->attackers.size();

                                    if((s > 0)&&(s < 3))
                                    {
                                        can_attack = (p_d_info->now_time - p_d_info->attackers.back().first) > attacker_time * 0.8f;
                                    }
                                    if(can_attack)
                                    {
                                        auto element = std::make_pair(p_d_info->now_time,character->GetDungeonListReference());
                                        auto res = std::find_if(p_d_info->attackers.begin(),
                                                                p_d_info->attackers.end(),
                                                                [&element](decltype(element) el){return el.second.lock()==element.second.lock();});
                                        if(res == p_d_info->attackers.end())
                                        {
                                            p_d_info->attackers.push_back(std::make_pair(p_d_info->now_time,character->GetDungeonListReference()));
                                        }
                                    }
                                }

                            }
                            else
                            {
                                character->UseCommand(AnimationCommand::kStance);
                            } 
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