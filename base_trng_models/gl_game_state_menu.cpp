#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <functional>
#include <utility>
#include <algorithm>
#include <math.h>

#include "glm/glm.hpp"

#include "glm/trigonometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "gl_physics.h"
#include "gl_game_state_menu.h"


#include "map_event_general.h"
#include "collision.h"
#include "loader.h"
#include "engine_settings.h"
#include "game_status.h"
#include "game_event_fabric.h"
#include "gl2d_progressbar.h"
#include "glresourses.h"
#include "game_inputs.h"
#include "brain.h"
//#include "glfw3.h"

constexpr float sound_mul = 0.1f;





// bool GlGameStateMenu::AddObjectsFromFile(const std::string & object)
// {
//     std::ifstream objects_file;
// 	objects_file.open(object);
//     if(objects_file.is_open())
//     {
//         std::vector<std::string> obj_lines;

//         while(!objects_file.eof())
//         {
//             std::string prefix = LoaderUtility::FindPrefix(objects_file);
//             if(prefix == "object")
//             {
//                 LoaderUtility::LoadLineBlock(objects_file,prefix,obj_lines);
//                 LoadObject(obj_lines);
//             }
//         }
//         objects_file.close();
//         return true;
//     }
//     else
//         return false;
// }

// bool GlGameStateMenu::AddObjectFromFile(const std::string & object,const std::string & name,glm::vec3 position)
// {
//     std::ifstream objects_file;
// 	objects_file.open(object);
//     if(objects_file.is_open())
//     {
//         std::vector<std::string> obj_lines;

//         if(!objects_file.eof())
//         {
//             LoaderUtility::LoadLineBlock(objects_file,LoaderUtility::FindPrefix(objects_file),obj_lines);
//             auto object = LoadObject(obj_lines);
//             object->SetPosition(position);
//             object->SetName(name);
//         }
//         objects_file.close();
//         return true;
//     }
//     else
//         return false;
// }

GlGameStateMenu::GlGameStateMenu(std::map<const std::string,GLuint> &shader_map,
                                    std::map<std::string,std::shared_ptr<glRenderTargetSimple>> & render_target_map,
                                    std::map<std::string,std::shared_ptr<GlCharacter>> & models_map,
                                    GLResourcesManager &resources_manager,
                                    size_t screen_width,
                                    size_t screen_height,
                                    irrklang::ISoundEngine *sound_engine):
                                                        IGlGameState(shader_map,resources_manager,screen_width,screen_height)
                                                        ,m_render_target_map(render_target_map)
                                                        ,m_models_map(models_map)
                                                        ,m_antialiase_enabled(true)
                                                        ,now_frame(91)
                                                        ,m_sound_engine(sound_engine)
{
    //m_sound_engine->play2D("material/audio/breakout.mp3", GL_TRUE);

    glClearColor(0.0f,0.0f,0.0f,1.0f);


    float a_ratio = screen_width;
    a_ratio /= screen_height;


    auto object_ptr = std::make_shared<Gl2D::GlProgressbar>(-1.0,0.9,0.8,0.1,a_ratio,
                                GetResourceManager()->m_texture_atlas.Assign("halfbar_border.png"),
                                GetResourceManager()->m_texture_atlas.Assign("halfbar.png"),
                                m_shader_map["sprite2dsimple"],
                                []() { return GameSettings::GetHeroStatus()->GetLife(); }
                                );
                                
    object_ptr->SetItemAligment(Gl2D::ItemAligment::Left);
    object_ptr->SetAspectRatioKeeper(Gl2D::AspectRatioKeeper::Minimal);                    
    Interface2D.push_back(object_ptr);   

    m_gl_text = std::make_shared<GlText16x16>("font2.png",GetResourceManager()->m_texture_atlas,0.1f,0.1f);

   
   

    m_message_processor.Add("run_script",[this](std::stringstream &sstream)
                                    {
                                        std::string name;
                                        sstream >> name;
                                        auto script = m_scripts.at(name);
                                        for(auto message:script)
                                        {
                                            PostMessage(message);
                                            //std::cout<<message<<"\n";
                                        }
                                    });
    
                  



    m_message_processor.Add("sound",[this](std::stringstream &sstream)
    {  

        std::string sound;
        sstream >>  sound;   
        m_sound_engine->play2D(sound.c_str(),false);
        
    });



    time = glfwGetTime();
    //LoadMap("levels/test.lvl","base");

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);   
}


void GlGameStateMenu::LoadScript(std::vector<std::string> &lines)
{
    std::vector<std::string> clean_lines(lines.begin()+1,lines.end());
    m_scripts.insert(std::make_pair(lines[0],clean_lines));
}




void GlGameStateMenu::Draw2D(GLuint depth_map)
{   
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);	
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
    
    for(auto item :Interface2D) 
    {
        item->Draw();
    }
    glEnable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);

    const float text_size_y = 0.060f;
    const float text_size_x = m_aspect_ratio * text_size_y;

    m_gl_text->SetTextSize(text_size_x,text_size_y); 
    auto shader = m_shader_map["sprite2dsimple"];
    std::stringstream ss;
    ss<< std::fixed<<std::setprecision(1)<<EngineSettings::GetEngineSettings() ->GetFPS()<<" FPS; life: "<<std::setprecision(2)<<GameSettings::GetHeroStatus()->GetLife();
    m_gl_text->DrawString(ss.str(),1.0f - m_gl_text->GetStringLength(ss.str()),1.0f - text_size_y*1.2f, shader);

    // if(m_info_message.length()!=0) 
    // {
    //     m_gl_text->DrawString(m_info_message, - 0.5f * m_gl_text->GetStringLength(m_info_message),-1.0f + text_size_y*2.2f, shader);
    // }
}


void GlGameStateMenu::Draw()
{
    glRenderTargetDeffered &render_target = *(dynamic_cast<glRenderTargetDeffered*>(m_render_target_map["base_deffered"].get()));
    glRenderTarget &final_render_target = *(dynamic_cast<glRenderTarget*>(m_render_target_map["final"].get()));
    glRenderTarget &postprocess_render_target = *(dynamic_cast<glRenderTarget*>(m_render_target_map["postprocess"].get()));
    glRenderTargetSimple &buffer1_render_target = *(m_render_target_map["buffer_1"].get());
    glRenderTargetSimple &buffer2_render_target = *(m_render_target_map["buffer_2"].get());

    size_t width = IGlGameState::m_screen_width;
    size_t height = IGlGameState::m_screen_height;
    



    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);

    Draw2D(render_target.depthMap);

    glEnable(GL_DEPTH_TEST);
}

void GlGameStateMenu::PostMessage(const std::string & event_string)
{
    m_messages.push_back(event_string);
}

void GlGameStateMenu::ProcessMessages()
{
    double l_time = glfwGetTime();
    while (!m_messages.empty())//&&!pause_interface.IsPaused(l_time))
    {
        m_message_processor.Process(m_messages.front());
        m_messages.pop_front();
    }
}

IGlGameState *  GlGameStateMenu::Process(std::map <int, bool> &inputs, float joy_x, float joy_y)
{
    glRenderTargetDeffered &render_target = *(dynamic_cast<glRenderTargetDeffered*>(m_render_target_map["base_deffered"].get()));
    //std::shared_ptr<GlCharacter> hero_ptr = m_models_map["Hero"];

    double time_now = glfwGetTime();

    if((time_now - time)>(1.0/30.0))
    {

        // m_daytime_in_hours += 0.00055f;
        // if(m_daytime_in_hours>24.0f)
        // {
        //     m_daytime_in_hours -= 24.0f;
        // }

        time = time_now;        
        ProcessMessages();
              
        ProcessInputs(inputs);

    }

    return this;
}

void  GlGameStateMenu::ProcessInputs(std::map <int, bool> &inputs)
{
    if(inputs[GLFW_KEY_F9])
    {
        //m_daytime_in_hours -= 0.1;
    }

    if(inputs[GLFW_KEY_F10])
    {
        //m_daytime_in_hours += 0.1;
    }

    auto move_inputs = GameInputs::ProcessInputsMoveControl(inputs);
    float move_square = move_inputs.first * move_inputs.first + move_inputs.second * move_inputs.second;
    bool moving = move_square > 0.03f;//(std::abs(move_inputs.first)+std::abs(move_inputs.second)>0.2f);

    bool action_use = inputs[GLFW_KEY_LEFT_ALT];
    bool attack = inputs[GLFW_MOUSE_BUTTON_LEFT]|inputs[GLFW_KEY_SPACE];  
    bool fast_move = inputs[GLFW_KEY_LEFT_SHIFT];

    GLFWgamepadstate state;
    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)&&glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
    {
        attack |=state.buttons[GLFW_GAMEPAD_BUTTON_A];
        action_use |=state.buttons[GLFW_GAMEPAD_BUTTON_X];
        fast_move |=state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
    }

    // if(attack) 
    //     return std::make_pair(AnimationCommand::kStrike,rm);
    // if(action_use) 
    //     return std::make_pair(AnimationCommand::kUse,rm);
    // if(moving)
    //     return std::make_pair(fast_move ? AnimationCommand::kFastMove:AnimationCommand::kMove,rm);

    // return std::make_pair(AnimationCommand::kNone,rm);    
}



