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


void ResetModels(std::vector <std::shared_ptr<glModel> > &Models)
{
    for(auto tmpModel : Models)
    {
        tmpModel->model = glm::rotate(tmpModel->model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        tmpModel->model = glm::rotate(tmpModel->model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
}


bool GlGameStateMenu::AddObjectsFromFile(const std::string & object)
{
    std::ifstream objects_file;
	objects_file.open(object);
    if(objects_file.is_open())
    {
        std::vector<std::string> obj_lines;

        while(!objects_file.eof())
        {
            std::string prefix = LoaderUtility::FindPrefix(objects_file);
            if(prefix == "object")
            {
                LoaderUtility::LoadLineBlock(objects_file,prefix,obj_lines);
                LoadObject(obj_lines);
            }
        }
        objects_file.close();
        return true;
    }
    else
        return false;
}

bool GlGameStateMenu::AddObjectFromFile(const std::string & object,const std::string & name,glm::vec3 position)
{
    std::ifstream objects_file;
	objects_file.open(object);
    if(objects_file.is_open())
    {
        std::vector<std::string> obj_lines;

        if(!objects_file.eof())
        {
            LoaderUtility::LoadLineBlock(objects_file,LoaderUtility::FindPrefix(objects_file),obj_lines);
            auto object = LoadObject(obj_lines);
            object->SetPosition(position);
            object->SetName(name);
        }
        objects_file.close();
        return true;
    }
    else
        return false;
}

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
                                                        ,hero(models_map["Hero"])
                                                        ,m_antialiase_enabled(true)
                                                        ,now_frame(91)
                                                        ,m_info_message("")
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

   
    m_message_processor.Add("show_message",[this](std::stringstream &sstream)
                                    {
                                        std::getline(sstream,m_info_message);
                                        //m_info_message = sstream.str();
                                        //std::cout<<m_info_message<<"\n";
                                    });
    

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
    
                  



    m_message_processor.Add("sound3d",[this](std::stringstream &sstream)
    {  
        std::string name;
        std::string sound;
        sstream >> name >> sound;   
        //m_sound_engine->play2D(sound.c_str(), GL_FALSE);
        auto obj = FindSharedCollectionByName(dungeon_objects.begin(), dungeon_objects.end(),name);
        if(obj)
        {    
            auto vec = sound_mul *obj->GetPosition();
            m_sound_engine->play3D(sound.c_str(),irrklang::vec3df(vec[0],vec[1],vec[2]), false, false, false);
        }
    });

    m_message_processor.Add("strike",[this](std::stringstream &sstream)
    { 
        //m_sound_engine->play2D("material/audio/punch.wav", GL_FALSE);
        std::string name;
        float force = 0.0f;
        sstream >> name >> force;
        auto obj = FindSharedCollectionByName(dungeon_objects.begin(), dungeon_objects.end(),name);//MobPointer(name);
        
        if(obj)
        {
            glRenderTargetDeffered &render_target = *(dynamic_cast<glRenderTargetDeffered*>(m_render_target_map["base_deffered"].get()));
            GameEvents::GeneralEventStrike strike;
            strike.source = obj;
            strike.strike_force = force;
            strike.texture = &(fx_texture_2->m_texture);
            //strike.current_shader = m_shader_map["sprite2d"];
            strike.current_shader = m_shader_map["simple"];
            strike.depthmap = render_target.depthMap;
            map_events.push_back(GameEvents::CreateGameEvent(GameEvents::EventTypes::HeroStrike,&strike));
        }
    });

    m_message_processor.Add("show_intro",[this](std::stringstream &sstream)
    {  
        std::string image;
        sstream >> image;
        m_intro->SetImage(GetResourceManager()->m_texture_atlas.Assign(image)); 
        m_mode = GameStateMode::Intro;
        
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


void GlGameStateMenu::LoadMap(const std::string &filename,const std::string &start_place)
{  
    m_messages.clear();
    m_daytime_in_hours =12.0f;                                        
    
    std::ifstream level_file;
	level_file.open(filename); 
    // std::cout<<"Level:"<<filename<<" "<<(level_file.is_open()?"-opened":"-failed")<<"\n";  
    if(!level_file.is_open()) return;


    SaveObjects(m_level_file);
    // std::cout<<"Level: old saves to"<< m_level_file<<"\n";
    m_level_file = filename;

    std::string tmp_filename(filename);
    size_t ext_pos = tmp_filename.find_last_of(".") +1;
    std::string extention = tmp_filename.replace(ext_pos,tmp_filename.length() - ext_pos,"sav");
    
    hero_position = glm::vec3(10.0f,0.0f,10.0f); 
    hero->SetPosition(hero_position); 
    m_start_place = start_place;
    GLResourcesManager * resources_manager = GetResourceManager();
    hero_events.clear();
    mob_events.clear();
    map_events.clear();
    m_scripts.clear();
    
    dungeon_objects.clear();
    //dungeon_objects.push_back(m_models_map["Hero"]);


    
    std::map<std::string,const std::function<void(std::vector<std::string> &lines)>> execute_funcs;
    execute_funcs.insert(std::make_pair("sky",[this](std::vector<std::string> &lines){SetMapLight(lines);}));
    execute_funcs.insert(std::make_pair("heightmap",[this](std::vector<std::string> &lines){SetHeightmap(lines);}));
    execute_funcs.insert(std::make_pair("models",[this](std::vector<std::string> &lines)
                                        {
                                            for(auto line : lines)
                                            {
                                                Models.emplace_back(std::make_shared<glModel>(line));
                                            }
                                            ResetModels(Models);
                                        }));
    execute_funcs.insert(std::make_pair("starts",[this](std::vector<std::string> &lines){SelectStart(lines);}));
    execute_funcs.insert(std::make_pair("dungeon_params",[this](std::vector<std::string> &lines){SetDungeonSize(lines);}));
    execute_funcs.insert(std::make_pair("dungeon_tiles",[this](std::vector<std::string> &lines){LoadTiles(lines);}));
    execute_funcs.insert(std::make_pair("dungeon_objects",[this](std::vector<std::string> &lines){LoadDungeonObjects(lines);}));
    execute_funcs.insert(std::make_pair("hero_event",[this](std::vector<std::string> &lines){LoadMapEvent(lines);}));
    execute_funcs.insert(std::make_pair("script",[this](std::vector<std::string> &lines){LoadScript(lines);}));
    

    if(!AddObjectsFromFile(extention))
    {
        execute_funcs.insert(std::make_pair("object",[this](std::vector<std::string> &lines){LoadObject(lines);}));
    }


    

    Models.clear();
    
    
    std::vector<std::string> lines;
    
    std::string sufix ="";
    while(!level_file.eof())
    {
        sufix = LoaderUtility::FindPrefix(level_file);
        LoaderUtility::LoadLineBlock(level_file,sufix,lines);
        try
        {
            execute_funcs.at(sufix)(lines);
        }
        catch(const std::out_of_range& exp)
        {
        }
    }

    

    level_file.close(); 

    fx_texture = resources_manager->m_texture_atlas.Assign("valh.png");  
    fx_texture_2 = resources_manager->m_texture_atlas.Assign("fireball.png");  
    GetResourceManager()->Clean();  

}


void GlGameStateMenu::Draw2D(GLuint depth_map)
{   
    glClear( GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

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

    if(m_info_message.length()!=0) 
    {
        m_gl_text->DrawString(m_info_message, - 0.5f * m_gl_text->GetStringLength(m_info_message),-1.0f + text_size_y*2.2f, shader);
    }
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
    while (!m_messages.empty()&&!pause_interface.IsPaused(l_time))
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
              
        unit_control_action = ProcessInputs(inputs);

    }

    return this;
}

std::pair<AnimationCommand,const glm::mat4>  GlGameStateMenu::ProcessInputs(std::map <int, bool> &inputs)
{
    if(inputs[GLFW_KEY_F9])
    {
        m_daytime_in_hours -= 0.1;
    }

    if(inputs[GLFW_KEY_F10])
    {
        m_daytime_in_hours += 0.1;
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

    if(attack) 
        return std::make_pair(AnimationCommand::kStrike,rm);
    if(action_use) 
        return std::make_pair(AnimationCommand::kUse,rm);
    if(moving)
        return std::make_pair(fast_move ? AnimationCommand::kFastMove:AnimationCommand::kMove,rm);

    return std::make_pair(AnimationCommand::kNone,rm);    
}



