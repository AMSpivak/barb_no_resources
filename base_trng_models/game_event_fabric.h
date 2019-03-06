#ifndef GL_I_GAME_EVENT_FABRIC
#define GL_I_GAME_EVENT_FABRIC
#include "i_map_event.h"
#include <memory>
namespace GameEvents
{
    enum class EventTypes {HeroStrike,HeroUse,BarrelValhalla};
    struct GeneralEventStruct
    {
        GlCharacter * object;
        GLuint current_shader;
        GLuint depthmap;
        const GLuint * texture;
        // float width;
        // float height;
    };

    std::shared_ptr<IMapEvent> CreateGameEvent(EventTypes event_type, const void * parameters);
}
#endif