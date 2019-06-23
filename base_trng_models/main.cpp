#define GLEW_STATIC
#include <iostream>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <map>

#include "glm/glm.hpp"
#include "glm/trigonometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "glresourses.h"
#include "gl_resources_manager.h"

#include "glscene.h"
#include "gl_light.h"
#include "gl_render_target.h"
#include "gl_model.h"
#include "gl_character.h"
#include "gl_game_state_dungeon.h"
#include "animation_sequence.h"
#include "engine_settings.h"
#include "game_status.h"

extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

GLuint SCR_WIDTH = 800, SCR_HEIGHT = 600;

float key_angle = 0.0f;

// float clamp(float value, float min, float max)
// {
// 	return 
// }

std::map <int, bool> inputs;

void SetRenderTargets(
					std::map<std::string,std::shared_ptr<glRenderTargetSimple>> &render_target_map,
					float width,
					float height)
{
	render_target_map.clear();
	EngineSettings::Settings * settings = EngineSettings::GetEngineSettings();
	float quality = settings->GetQualityFactor();

	std::cout << "Set RT quality "<<quality<<"\n";
	auto base = std::make_pair("base_deffered",std::make_shared<glRenderTargetDeffered>());
	render_target_map.insert( base);
	base.second->InitBuffer(width, height,quality);
	auto fin = std::make_pair("final",std::make_shared<glRenderTarget>());
	render_target_map.insert( fin);
	fin.second->InitBuffer(width, height,quality);
	auto post = std::make_pair("postprocess",std::make_shared<glRenderTarget>());
	render_target_map.insert( post);
	post.second->InitBuffer(width, height,quality);

	auto buff1 = std::make_pair("buffer_1",std::make_shared<glRenderTargetSimple>());
	render_target_map.insert( buff1);
	buff1.second->InitBuffer(width, height,quality);

	auto buff2 = std::make_pair("buffer_2",std::make_shared<glRenderTargetSimple>());
	render_target_map.insert( buff2);
	buff2.second->InitBuffer(width, height,quality);
}

void FillShaders(std::map<const std::string,GLuint> &shader_map, const std::string filename)
{
	std::cout<<"\nTextures max:"<<(GL_MAX_TEXTURE_UNITS - GL_TEXTURE0)<<"\n";
    //shader_map.insert ( std::pair<const std::string,GLuint>("sobel", LoadshaderProgram("shaders/dbg.vs","shaders/sobel_cross.fs")) );
    shader_map.insert ( std::pair<const std::string,GLuint>("sobel_aa", LoadshaderProgram("shaders/dbg.vs","shaders/sobel_aa.fs")) );
    shader_map.insert ( std::pair<const std::string,GLuint>("sobel_blur", LoadshaderProgram("shaders/dbg.vs","shaders/sobel_blur.fs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("shadowmap", LoadshaderProgram("shaders/vertex1.vs","shaders/frag1.fs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("sprite", LoadshaderProgram("shaders/sprite.vs","shaders/sprite.fs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("fullscreen", LoadshaderProgram("shaders/dbg.vs","shaders/sprite.fs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("fullblur_h", LoadshaderProgram("shaders/dbg.vs","shaders/sprite_blur_h.fs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("fullblur_w", LoadshaderProgram("shaders/dbg.vs","shaders/sprite_blur_w.fs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("sprite2d", LoadshaderProgram("shaders/sprite2d.vs","shaders/sprite2d.fs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("sprite2dsimple", LoadshaderProgram("shaders/sprite2dsimple.vs","shaders/sprite2dsimple.fs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("skybox", LoadshaderProgram("shaders/skybox.vs","shaders/skybox.fs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("deffered",LoadshaderProgram("shaders/dbg.vs","shaders/deffered.fs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("deffered_global",LoadshaderProgram("shaders/dbg.vs","shaders/deffered_global.fs")) );
    shader_map.insert ( std::pair<const std::string,GLuint>("deffered_simple",LoadshaderProgram("shaders/dbg.vs","shaders/deff_simple.fs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("deffered_cheap",LoadshaderProgram("shaders/dbg.vs","shaders/deffered_cheap.fs")) );
    shader_map.insert ( std::pair<const std::string,GLuint>("deffered_simple_cheap",LoadshaderProgram("shaders/dbg.vs","shaders/deff_simple_cheap.fs")) );
    shader_map.insert ( std::pair<const std::string,GLuint>("deff_1st_pass",LoadshaderProgram("shaders/vert_norm.vs","shaders/frag_norm.fs")) );
    shader_map.insert ( std::pair<const std::string,GLuint>("deff_1st_pass_terrain",LoadshaderProgram("shaders/vert_norm.vs","shaders/frag_norm_terrain.fs")) );
    shader_map.insert ( std::pair<const std::string,GLuint>("deff_1st_pass_heght",LoadshaderProgram("shaders/vert_norm_height.vs","shaders/frag_norm_height.fs")) );
    shader_map.insert ( std::pair<const std::string,GLuint>("deff_heght",LoadshaderProgram("shaders/norm_height.vs","shaders/normals.fs","shaders/norm_height.gs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("luminocity",LoadshaderProgram("shaders/dbg.vs","shaders/luminocity.fs")) );
	shader_map.insert ( std::pair<const std::string,GLuint>("simple_heightmap",LoadshaderProgram("shaders/vert_height_simple.vs","shaders/frag_height_simple.fs")) );
}

std::map<std::string,std::shared_ptr<glRenderTargetSimple>> m_render_target_map;

int main(int argc, char const *argv[])
{


    bool is_fullscreen = true;

    if(argc > 1) is_fullscreen = false;

    inputs[GLFW_KEY_LEFT] =  false;
	inputs[GLFW_KEY_RIGHT] = false;
	inputs[GLFW_KEY_UP] =  false;
	inputs[GLFW_KEY_DOWN] = false;
    inputs[GLFW_KEY_RIGHT_BRACKET] = false;
    inputs[GLFW_KEY_LEFT_BRACKET] = false;
    inputs[GLFW_KEY_F1] = false;
    inputs[GLFW_KEY_F9] = false;
    inputs[GLFW_KEY_F10] = false;
    inputs[GLFW_KEY_LEFT_ALT] = false;


	//Инициализация GLFW
	glfwInit();
	//Настройка GLFW
	//Задается минимальная требуемая версия OpenGL.
	//Мажорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//Минорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
/*
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//Минорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);*/
	//Установка профайла для которого создается контекст
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Выключение возможности изменения размера окна
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//multisampling
	//glfwWindowHint(GLFW_SAMPLES, 4);

	//GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "My Title", monitor, NULL);
    GLFWwindow* window;

    if(is_fullscreen)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        SCR_WIDTH = mode->width;
    	SCR_HEIGHT = mode->height;
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", monitor, nullptr);
    }
    else
    {
	    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", nullptr, nullptr);
    }

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	

	GLResourcesManager resources_atlas("material/textures/","material/meshes/","material/animations/","");
	SetResourceManager(&resources_atlas);

	EngineSettings::Settings main_settings;
	EngineSettings::SetEngineSettings(&main_settings);

	

	GameSettings::HeroStatus hero_status;
	GameSettings::SetHeroStatus(&hero_status);

   
	SetRenderTargets(m_render_target_map,width,height);
    auto pmanager = GetResourceManager();
	
	FillShaders(pmanager->m_shader_map,"shaders/list.shd");

    std::map<std::string,std::shared_ptr<GlCharacter>> m_glmodels_map;


    auto hero = std::make_shared<GlCharacter>(CharacterTypes::hero);
    m_glmodels_map.insert( std::pair<std::string,std::shared_ptr<GlCharacter>>("Hero",hero));
	UpdateCharacterFromFile(argc > 2 ?  argv[2]:"material/hero.chr",*hero);
	hero->SetName("Hero");
	//hero->model_matrix = glm::rotate(hero->model_matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	
    GlGameStateDungeon game_state_dungeon(pmanager->m_shader_map,m_render_target_map,m_glmodels_map,resources_atlas,width,height);
    IGlGameState * game_state = nullptr;
    game_state = &game_state_dungeon;

	while(!glfwWindowShouldClose(window))
	{
		GLuint current_shader;
		double xpos, ypos;

		
		glfwGetCursorPos(window, &xpos, &ypos);
		xpos = (xpos * 2.0f - width)/width;
		ypos = (ypos * 2.0f - height)/height;
		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        inputs[GLFW_MOUSE_BUTTON_LEFT] = (state != GLFW_RELEASE) ?  true : false;

		
        static size_t counter = 0;
        static double time_r  = 0;

		if(counter++ == 0) time_r = glfwGetTime();
		if(counter > 30)
		{
			EngineSettings::GetEngineSettings() ->SetFPS((1.0f*counter/(glfwGetTime() - time_r)));
			counter = 0;
		}
		
		game_state->Process(inputs, xpos, ypos);
		EngineSettings::GetEngineSettings()->BeginNewFrame();
        game_state->Draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}

	std::cout << "exit";

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //std::cout << key << std::endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

	/*if (action == GLFW_RELEASE)
		return;*/
	if (key == GLFW_KEY_LEFT_CONTROL)
        inputs[GLFW_KEY_LEFT_CONTROL] = (action != GLFW_KEY_LEFT_CONTROL) ?  true : false;

	if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A )
        inputs[GLFW_KEY_LEFT] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D )
		inputs[GLFW_KEY_RIGHT] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_UP || key == GLFW_KEY_W )
        inputs[GLFW_KEY_UP] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S)
		inputs[GLFW_KEY_DOWN] = (action != GLFW_RELEASE) ?  true : false;

    if (key == GLFW_KEY_SPACE )
    		inputs[GLFW_KEY_SPACE] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_LEFT_ALT )
    		inputs[GLFW_KEY_LEFT_ALT] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_LEFT_SHIFT )
		inputs[GLFW_KEY_LEFT_SHIFT] = (action != GLFW_RELEASE) ?  true : false;

    if (key == GLFW_KEY_LEFT_BRACKET )
        inputs[GLFW_KEY_LEFT_BRACKET] = (action != GLFW_RELEASE) ?  true : false;

    if (key == GLFW_KEY_RIGHT_BRACKET )
        inputs[GLFW_KEY_RIGHT_BRACKET] = (action != GLFW_RELEASE) ?  true : false;
    
	if (key == GLFW_KEY_F9 )
        inputs[GLFW_KEY_F9] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_F10 )
        inputs[GLFW_KEY_F10] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_F1 )
	{
        inputs[GLFW_KEY_F1] = (action != GLFW_RELEASE) ?  true : false;
		if(action == GLFW_RELEASE)
		{
			EngineSettings::Settings * settings = EngineSettings::GetEngineSettings();
			settings->SetPbr(!settings->IsPbrON());
		}
	}

	

	switch (key)
	{

		case GLFW_KEY_F3:	
			if(action == GLFW_RELEASE)
			{
				EngineSettings::Settings * settings = EngineSettings::GetEngineSettings();
				float qf = settings->GetQualityFactor();
				qf = glm::clamp(qf*1.25f,0.5f,1.0f);
				settings->SetQualityFactor(qf);
				SetRenderTargets(m_render_target_map,SCR_WIDTH, SCR_HEIGHT);
			}
		break;
		case GLFW_KEY_F4:	
			if(action == GLFW_RELEASE)
			{
				EngineSettings::Settings * settings = EngineSettings::GetEngineSettings();
				float qf = settings->GetQualityFactor();
				qf = glm::clamp(qf*0.8f,0.5f,1.0f);
				settings->SetQualityFactor(qf);
				SetRenderTargets(m_render_target_map,SCR_WIDTH, SCR_HEIGHT);
			}
		break;

		case GLFW_KEY_F2:	
			if(action == GLFW_RELEASE)
			{
				EngineSettings::Settings * settings = EngineSettings::GetEngineSettings();
				settings->SetQualityFactor(1.0);
				SetRenderTargets(m_render_target_map,SCR_WIDTH, SCR_HEIGHT);
			}
		break;

		default:
		break;

	}
	
}
