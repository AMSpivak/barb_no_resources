#ifndef GL_I_GL_GAME_STATE_MENU
#define GL_I_GL_GAME_STATE_MENU
//#define GLEW_STATIC
//#include <GL/glew.h>
#include "i_gl_game_state.h"
#include "gl_light.h"
#include "gl_render_target.h"
#include "gl_model.h"
#include "gl_character.h"
#include "gl_dungeon.h"
#include "i_map_event.h"
#include "loader.h"
#include "gl_font16x16.h"
#include "gl2d_item.h"
#include "gl2d_image.h"
#include "map_heightmap.h"
#include <list>
#include <sound/irrKlang.h>

class GlGameStateMenu: public IGlGameState
{

public:
    GlGameStateMenu(std::map<const std::string,GLuint> &shader_map,
        std::map<std::string,std::shared_ptr<glRenderTargetSimple>> &render_target_map,
        std::map<std::string,std::shared_ptr<GlCharacter>> &models_map,
        GLResourcesManager &resources_manager,
        size_t screen_width,
        size_t screen_height,
        irrklang::ISoundEngine *sound_engine);

    ~GlGameStateMenu()
    {
        
    }
    void Draw();
    IGlGameState * Process(std::map <int, bool> &inputs, float joy_x, float joy_y);
    void SwitchIn(){}
    void SwitchOut(){}
private:

    irrklang::ISoundEngine  *m_sound_engine;
    bool m_show_intro;

    std::shared_ptr<IGlText> m_gl_text;

    std::list<std::shared_ptr<Gl2D::Gl2dItem>> Interface2D;
    std::map<std::string,std::shared_ptr<glRenderTargetSimple>> &m_render_target_map;
    std::map<std::string,std::shared_ptr<GlCharacter>> & m_models_map;
    std::list<std::string> m_messages;

    std::map<std::string,std::vector<std::string>> m_scripts;
    
    std::string m_level_file;

    
    bool m_antialiase_enabled;
    int now_frame;
    double time;
    LoaderUtility::LinesProcessor m_message_processor;

    void LoadScript(std::vector<std::string> &lines);
    void Draw2D(GLuint depth_map);

    void PostMessage(const std::string & event_string);
    void ProcessMessages();
    std::pair<AnimationCommand,const glm::mat4> ProcessInputs(std::map <int, bool> &inputs);  
    
};

#endif
