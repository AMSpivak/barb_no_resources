#include "game_event_fabric.h"
#include "map_event_hero_strikes.h"
#include "map_event_hero_action.h"
#include "map_event_valhalla.h"
#include "gl_character.h"
namespace GameEvents
{
    

    std::shared_ptr<IMapEvent> CreateGameEvent(EventTypes event_type, const void * parameters)
    {
        std::shared_ptr<IMapEvent> retval;
        switch(event_type)
        {
            case EventTypes::HeroStrike:
            {
                
                auto ptr = static_cast<const GlCharacter *>(parameters);
                auto e_ptr = std::make_shared<IMapEventHeroStrike>(1.0f,1.4f);
                e_ptr->model_matrix = ptr->model_matrix;
                e_ptr->AddEdge(std::pair<glm::vec3,glm::vec3>(glm::vec3(0.3f,0.0f,-0.5f),glm::vec3(0.5f,0.0f,-2.5f)));
                e_ptr->AddEdge(std::pair<glm::vec3,glm::vec3>(glm::vec3(0.5f,0.0f,-2.5f),glm::vec3(-0.5f,0.0f,-2.5f)));
                e_ptr->AddEdge(std::pair<glm::vec3,glm::vec3>(glm::vec3(-0.5f,0.0f,-2.5f),glm::vec3(-0.3f,0.0f,-0.5f)));
                e_ptr->AddEdge(std::pair<glm::vec3,glm::vec3>(glm::vec3(-0.3f,0.0f,-0.5f),glm::vec3(0.3f,0.0f,-0.5f)));
                e_ptr->position = ptr->GetPosition();
                e_ptr->position[1] = 0;
                return e_ptr;
            }
            break;
            case EventTypes::HeroUse:
            {
                auto ptr = static_cast<const GlCharacter *>(parameters);
                auto e_ptr = std::make_shared<IMapEventHeroAction>(1.0f,1.4f,AnimationCommand::kUse);
                e_ptr->model_matrix = ptr->model_matrix;
                e_ptr->AddEdge(std::pair<glm::vec3,glm::vec3>(glm::vec3(0.3f,0.0f,-0.5f),glm::vec3(0.5f,0.0f,-2.5f)));
                e_ptr->AddEdge(std::pair<glm::vec3,glm::vec3>(glm::vec3(0.5f,0.0f,-2.5f),glm::vec3(-0.5f,0.0f,-2.5f)));
                e_ptr->AddEdge(std::pair<glm::vec3,glm::vec3>(glm::vec3(-0.5f,0.0f,-2.5f),glm::vec3(-0.3f,0.0f,-0.5f)));
                e_ptr->AddEdge(std::pair<glm::vec3,glm::vec3>(glm::vec3(-0.3f,0.0f,-0.5f),glm::vec3(0.3f,0.0f,-0.5f)));
                e_ptr->position = ptr->GetPosition();
                e_ptr->position[1] = 0;
                
                return e_ptr;
            }
            break;

            case EventTypes::BarrelValhalla:
            {
                auto ptr = static_cast<const GeneralEventStruct *>(parameters);                
                auto e_ptr = std::make_shared<MapEventValhalla>(ptr->current_shader,ptr->depthmap,ptr->texture,1.0f,1.4f);
                e_ptr->position = ptr->object->GetPosition();
                e_ptr->position.y = 1.5f;
                return e_ptr;
            }
            break;    

            default:
            break;
        }
        return retval;
    }
}