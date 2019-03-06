#ifndef GL_MODEL
#define GL_MODEL
#include <GL/glew.h>

#include <string>
#include <vector>
#include <memory>
#include "glresourses.h"
#include <iostream>
#include "i_gl_jal_struct.h"
#include "i_gl_jub_struct.h"
#include "i_gl_texture_struct.h"

#include "gl_resources_manager.h"

class glModel
{

	std::shared_ptr<IGlJalStruct> jal_mesh;



	//GLResourcesManager &m_resources_manager;

public:
	int parent_idx;
	int parent_bone;

	glm::mat4 model;
	glm::mat4 draw_matrix;
	std::shared_ptr<IGlTextureStruct> diffuse_texture;
    std::shared_ptr<IGlTextureStruct> utility_texture;
    std::shared_ptr<IGlTextureStruct> normal_texture;
	std::shared_ptr<IGlJubStruct> jub_bones;
	std::shared_ptr<Animation> animation;

	//glModel(GLResourcesManager &resources_manager):model(1.0f),draw_matrix(1.0f)
	glModel(GLResourcesManager &resources_manager)//:draw_matrix(1.0f)
	{}

	glModel(std::string FileName)//:draw_matrix(1.0f)
	{
		LoadAll(FileName);
	}

	const glm::mat4 &GetBoneMatrix(size_t frame, size_t bone);
	const glm::mat4 &GetRotationMatrix(size_t frame);



	const glm::mat4 &ModelMatrix()
	{
		return model;
	}


	~glModel()
	{
		
	}





	void LoadModelBones(std::string FileName);
	void LoadAll(std::string FileName);
	void Draw();
	void Draw(GLuint shaderProgram, Animation &animation, int now_frame,const glm::mat4 &matrix);
	void Draw(GLuint shaderProgram, Animation &animation, int now_frame);
	void Draw(GLuint shaderProgram, int now_frame);
	void Draw(GLuint shaderProgram, int now_frame,const glm::mat4 &matrix);
	void AttachAnimation(std::vector <std::shared_ptr<Animation> > &animations, std::string Filename);
	void SetDrawMatrix(const glm::mat4 &value);
};

#endif