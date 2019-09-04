#include "gl_model.h"
#include <fstream>
#include <iostream>



/*void glModel::LoadModelBones(std::string FileName)
{
	IGlJubStruct * bone_ptr = jub_bones.get();
	LoadBonesArray(FileName,bone_ptr->bones,bone_ptr->bonescount);

}*/

void glModel::Draw()
{
	
	if(jal_mesh->vertexcount > 2)
	{
	    glBindVertexArray(jal_mesh->VAO);
	    glDrawArrays(GL_TRIANGLES, 0, jal_mesh->vertexcount);
	    glBindVertexArray(0);
	}
}
void glModel::SetDrawMatrix(const glm::mat4 &value)
{
	draw_matrix = value;
}
void glModel::Draw(GlScene::Scene &scene, Animation &animation, int now_frame)
{
	Draw(scene, animation,now_frame,draw_matrix);
}
void glModel::Draw(GlScene::Scene &scene, Animation &animation, int now_frame,const glm::mat4 &matrix)
{
	//glUseProgram(shader);
	// if(m_shader && (shaderProgram != m_shader) )
	// {
	// 	shaderProgram = m_shader;
	// 	glUseProgram(shaderProgram);
	// }
	if(m_shader && (scene.render_shader != m_shader) )
	{
		scene.render_shader = m_shader;
		glUseProgram(scene.render_shader);
		unsigned int cameraLoc  = glGetUniformLocation(scene.render_shader, "camera");
		glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(scene.render_camera->CameraMatrix()));
		unsigned int zero_offset = glGetUniformLocation(scene.render_shader, "zero_offset");
		glUniform3fv(zero_offset, 1, glm::value_ptr(scene.zero_offset));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_material->m_albedo_texture->m_texture);

		glUniform1i(glGetUniformLocation(scene.render_shader, "UtilityTexture"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_material->m_roughness_metalness_texture->m_texture);

		glUniform1i(glGetUniformLocation(scene.render_shader, "NormalTexture"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_material->m_normal_height_texture->m_texture);
		scene.material = m_material;
	}
	else
	{
		/* code */
		if((scene.material.expired())||(m_material != scene.material.lock()))
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_material->m_albedo_texture->m_texture);

			glUniform1i(glGetUniformLocation(scene.render_shader, "UtilityTexture"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_material->m_roughness_metalness_texture->m_texture);

			glUniform1i(glGetUniformLocation(scene.render_shader, "NormalTexture"), 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_material->m_normal_height_texture->m_texture);
			scene.material = m_material;
		}
	}
	
	unsigned int modelLoc = glGetUniformLocation(scene.render_shader, "model");
	unsigned int drawLoc = glGetUniformLocation(scene.render_shader, "draw");
	unsigned int boneLoc  = glGetUniformLocation(scene.render_shader, "u_BoneMatrices");


	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(drawLoc, 1, GL_FALSE, glm::value_ptr(matrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_material->m_albedo_texture->m_texture);

	glUniform1i(glGetUniformLocation(scene.render_shader, "UtilityTexture"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_material->m_roughness_metalness_texture->m_texture);
    
	glUniform1i(glGetUniformLocation(scene.render_shader, "NormalTexture"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_material->m_normal_height_texture->m_texture);
	const std::vector <Bone> &bones = jub_bones.get()->bones;
	glUniformMatrix4fv(boneLoc, bones.size(), GL_FALSE, animation.GetDrawValues(now_frame,bones));
	//glUniformMatrix4fv(boneLoc, jub_bones.get()->bones.size(), GL_FALSE, glm::value_ptr(animation.frames[now_frame].bones[0]));
    Draw();
}

void glModel::Draw(GlScene::Scene &scene, int now_frame)
{
	Draw(scene, *animation ,now_frame);
}
void glModel::Draw(GlScene::Scene &scene, int now_frame,const glm::mat4 &matrix)
{
	Draw(scene, *animation ,now_frame,matrix);
}
void glModel::AttachAnimation(std::vector <std::shared_ptr<Animation> > &animations, std::string Filename)
{
	animations.emplace_back(std::make_shared<Animation>());
	animations.back()->LoadAnimation(Filename);
	animation = animations.back();
}
	
const glm::mat4 &glModel::GetBoneMatrix(size_t frame, size_t bone)
{
	animation->GetBoneMatrix(frame,bone,jub_bones.get()->bones);//frames[now_frame].bones[Models[i]->parent_bone];
}

const glm::mat4 &glModel::GetRotationMatrix(size_t frame)
{
	animation->GetRotationMatrix(frame,jub_bones.get()->bones);//frames[now_frame].bones[Models[i]->parent_bone];
}

void glModel::LoadAll(std::string FileName)
{
	size_t pos = FileName.rfind("/");
	std::ifstream modelfile;
	modelfile.open(FileName);

	std::string path = pos == std::string::npos ? "" : FileName.erase(pos+1,std::string::npos);
	std::string tmp_str;
	std::string jal_name;
	std::string jub_name;
	std::string png_name;
    std::string png_utility_name;
	std::string frames_name = "";

	getline(modelfile, tmp_str);
	jal_name = /*path + */tmp_str;
	getline(modelfile, tmp_str);
	jub_name = path + tmp_str;
	getline(modelfile, tmp_str);
	png_name = /*path + */tmp_str;
    getline(modelfile, tmp_str);
    png_utility_name = /*path + */tmp_str;
	getline(modelfile, tmp_str);
    std::string png_normal_name = /*path + */tmp_str;
	//std::cout<<"\n===\n";
	modelfile >> parent_idx >> parent_bone >> frames_name;
    //std::cout<<jal_name<<"\n"<<jub_name<<"\n"<<png_name<<"\n"<<png_utility_name<<"\n"<<"!"<<parent_idx<<"!"<<parent_bone<<"\n"<<frames_name<<"\n";
	getline(modelfile, tmp_str);

	getline(modelfile, tmp_str);
	if(tmp_str=="")
	{
		tmp_str=="deff_1st_pass";
	}
	m_shader = GetResourceManager()->GetShader(tmp_str);

	modelfile.close();


	GLResourcesManager * resources = GetResourceManager();

	jal_mesh = resources->m_mesh_atlas.Assign(jal_name);


	m_material = std::make_shared<GameResource::GlMaterial>(png_name,png_normal_name,png_utility_name);

	jub_bones = resources->m_bones_atlas.Assign(jub_name);
	if(frames_name.compare("")) animation = resources->m_animation_atlas.Assign(frames_name);

	//if(frames_name.compare("")) AttachAnimation(animations,path + frames_name);


}