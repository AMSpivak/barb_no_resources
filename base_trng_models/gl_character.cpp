#include "gl_character.h"
#include <sstream>
#include "engine_settings.h"
#include "collision.h"
std::ostream& operator << ( std::ostream& os, const GlCharacter & character)
{
    os<<"<object>\n";
    character.ToStream(os);
    os<<"<!object>\n";
	return os;
}
// bool GlCharacter::comand_proc_ready = false;

GlCharacter::GlCharacter(CharacterTypes type):
                            engine_frame(0)
                            ,now_frame(0)
                            ,current_animation(nullptr)
                            ,m_type(type)
{
    // if(!comand_proc_ready)
    // {
    //     comand_proc.Add()
    //     comand_proc_ready = true;

    // }

}

GlCharacter::~GlCharacter()
{

}

CharacterTypes GlCharacter::GetType() const
{
    return m_type;
}


void GlCharacter::ToStream(std::ostream& os) const
{
    os<<"name " << GetName()<<"\n";
    
    for(auto model_name : model_list)
    {
        os<<"model "<<model_name<<"\n";
    }

    for(auto seq : sequence)
    {
        os<<"sequence "<<seq<<"\n";
    }

    //auto it = std::find_if(sequence.begin(), sequence.end(), [this](std::pair<std::string, AnimationSequence> anim){return *current_animation == anim.second;});
    
    os<<"run_sequence "<<current_animation->m_name<<"\n";

    glm::mat4 tmp_matrix = glm::rotate(model_matrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    os<<"matrix "<<tmp_matrix<<"\n"
    <<"mass_inv "<<mass_inv<<"\n"
    <<"armor "<<GetArmorValue()<<"\n"
    <<"life " << GetLifeValue()<<"\n"
    <<"radius "<<radius<<"\n"
    <<"position "<<GetPosition()<<"\n";

    if(m_is_light)
    {
        os<< "light "<<" "<< m_light_color <<" "<< m_light_position <<" "<< m_light_radius<<"\n";
    }

    if(ghost)
    {
        os<< "ghost\n";
    }

    for(auto edge : m_edges)
    {
        os<<"edge "<<edge<<"\n";
    }
    
}


void UpdateCharacterFromFile(const std::string &filename,GlCharacter & character)
{
    std::ifstream char_file;
	char_file.open(filename);
    std::vector<std::string> lines;
    LoaderUtility::LoadLineBlock(char_file,"never_exists",lines);
    char_file.close(); 
    character.UpdateFromLines(lines); 
}



void GlCharacter::UpdateFromLines(std::vector<std::string> &lines)
{
    if(lines.size()<=1) 
    return;
    
    LoaderUtility::LinesProcessor proc;

    proc.Add("model",[this](std::stringstream &sstream){AddModel(LoaderUtility::GetFromStream<std::string>(sstream));});
    proc.Add("run_sequence",[this](std::stringstream &sstream){UseSequence(LoaderUtility::GetFromStream<std::string>(sstream));});
    proc.Add("ghost",[this](std::stringstream &sstream){ ghost = true;});
    proc.Add("matrix",[this](std::stringstream &sstream){ sstream >> model_matrix;});
    proc.Add("mass_inv",[this](std::stringstream &sstream){ sstream >> mass_inv;});
    proc.Add("armor",[this](std::stringstream &sstream){SetArmorValue(LoaderUtility::GetFromStream<float>(sstream));});
    proc.Add("life",[this](std::stringstream &sstream){SetLifeValue(LoaderUtility::GetFromStream<float>(sstream));});
    proc.Add("name",[this](std::stringstream &sstream){SetName(LoaderUtility::GetFromStream<std::string>(sstream));});
    proc.Add("radius",[this](std::stringstream &sstream){ sstream >> radius;});                                
    proc.Add("edge",[this](std::stringstream &sstream){AddEdge(LoaderUtility::GetFromStream<std::pair<glm::vec3,glm::vec3>>(sstream));});
    proc.Add("position",[this](std::stringstream &sstream){SetPosition(LoaderUtility::GetFromStream<glm::vec3>(sstream));}); 

    proc.Add("light",[this](std::stringstream &sstream)
                                        {
                                            float light_radius = 0.0f;
                                            glm::vec3 color;
                                            glm::vec3 l_position;
                                            sstream >> color >> l_position >> light_radius; 
                                            SetLight(true,color,l_position,light_radius);
                                        });
    proc.Add("orientation",[this](std::stringstream &sstream)
                                    {
                                        float a_x = 0.0f;
                                        float a_y = 0.0f;
                                        float a_z = 0.0f;

                                        sstream >> a_x >> a_y >> a_z; 
                                        
                                        model_matrix = glm::rotate(model_matrix, glm::radians(a_x), glm::vec3(1.0f, 0.0f, 0.0f));
                                        model_matrix = glm::rotate(model_matrix, glm::radians(a_y), glm::vec3(0.0f, 1.0f, 0.0f));
                                        model_matrix = glm::rotate(model_matrix, glm::radians(a_z), glm::vec3(0.0f, 0.0f, 1.0f));
                                    });
    proc.Add("sequence",[this](std::stringstream &sstream)
                                        {
                                            //size_t start = 0;
                                            //size_t end =0;
                                            std::string name;
                                            sstream >> name ;//>> start >> end;
                                             
                                            AnimationSequence seq(name);
                                            sstream >> seq;
                                            AddSequence(name,seq);
                                            UseSequence(name);
                                        });

    proc.Process(lines);
}

void GlCharacter::AddEdge(const std::pair<glm::vec3, glm::vec3> edge)
{
    m_edges.push_back(edge);
}

void GlCharacter::UseSequence(const std::string & name)
{
    current_animation = &sequence[name];
}

void GlCharacter::AddSequence(const std::string & name, const AnimationSequence & in_sequence)
{
    sequence.insert( std::pair<std::string, AnimationSequence>(name,in_sequence));
}

void GlCharacter::Draw(GLuint shader) const
{
    for(auto model : Models) model->Draw(shader,now_frame);
}
void GlCharacter::UseCommand(AnimationCommand command)
{
    try
    {
        std::string jumper = current_animation->jumps.at(command);
        current_animation = &sequence.at(jumper);                          
    }
    catch(const std::out_of_range& exp)
    {
        // std::cout<<"Unknown command\n";
    }   
}


void GlCharacter::Draw(GLuint shader,const glm::mat4 &draw_matrix)
{
    unsigned short frame = EngineSettings::GetEngineSettings()->GetFrame();
    if(engine_frame != frame)
    {
      
        for(auto model : Models) model->SetDrawMatrix(draw_matrix);
        Draw(shader);
        engine_frame = frame;
    }
    else
    {
        Draw(shader);
    }
    
}



void GlCharacter::RefreshMatrixes(float approximation)
{
    bool first_base = true;
    int models_count = Models.size();
    glm::mat4 target_matrix;
    for(int i = 0; i < models_count; i++)
    {

        if(Models[i]->parent_idx != -1)
        {
            IGlJubStruct * bone_ptr = Models[Models[i]->parent_idx]->jub_bones.get();
            target_matrix = Models[Models[i]->parent_idx]->model *
                Models[Models[i]->parent_idx]->GetBoneMatrix(now_frame,Models[i]->parent_bone) *
                bone_ptr->bones[Models[i]->parent_bone].matrix * glm::inverse(Models[i]-> jub_bones.get()->bones[0].matrix);
        }
        else
        {
            if(first_base&&(now_frame!=0))
            {
                glm::mat4 move_matrix = Models[i]->GetRotationMatrix(now_frame);
                glm::vec4 move_position = model_matrix * move_matrix[3];
                m_position += glm::vec3(move_position[0],move_position[1],move_position[2]);
                move_matrix[0].w = move_matrix[1].w = move_matrix[2].w = 0.0f;
                move_matrix[3] = glm::vec4(0.0f,0.0f,0.0f,1.0f);
                model_matrix = move_matrix * model_matrix;
                first_base = false;
            }        
            target_matrix = model_matrix;
        }

        //approximation = 1.0f;

        // if(approximation > 0.9f)
        // {
            Models[i]-> model = target_matrix;
        // }
        // else
        // {
        //     std::cout<<"approx: "<<approximation<<"\n";
        //     Models[i]-> model = SlerpMatrix(Models[i]-> model,target_matrix,approximation);
        // }
    }
}
void GlCharacter::ExecuteCommand(const std::pair<AnimationCommand,std::string> &command,std::list<std::string> &m_messages)
{
    switch(command.first)
    {
        case AnimationCommand::kMessage:
            m_messages.push_back(command.second);
        break;
        case AnimationCommand::kStrike:

        break;
        default:
        break;
    }
    //m_messages.push_back(current_animation->m_end_message);
}

int GlCharacter::Process(std::list<std::string> &m_messages)
{
    if(GetLifeValue() <=0.0f) return 1;
    auto control = now_frame;
    if(current_animation == nullptr)
    {
        now_frame = 0;
    }
    else
    {
        ++now_frame;

        if(now_frame < current_animation->start_frame) now_frame = current_animation->start_frame;
        if(now_frame > current_animation->end_frame) 
        {
            now_frame = current_animation->m_loop ? current_animation->start_frame:current_animation->end_frame;
            ExecuteCommand(current_animation->m_end_message,m_messages);
            if(current_animation->m_jump)
            {
                current_animation = &sequence[current_animation->m_target_sequence];
                now_frame = current_animation->start_frame;
            }
        }
    }

    size_t diff_frame = current_animation->end_frame - current_animation->start_frame > 2 ?now_frame - current_animation->start_frame:4;

    float approx = diff_frame > 3 ? 1.0f : 0.1f + 0.2f * diff_frame;

    if(control != now_frame)
        RefreshMatrixes(approx);
    return 0;
}

void GlCharacter::Damage(float damage)
{
    UseCommand(AnimationCommand::kDamaged);
    IGlModel::Damage(damage);
}

void GlCharacter::AddModel(const std::string & name)
{
    model_list.emplace_back(name);
    Models.emplace_back(std::make_shared<glModel>(name));
}

int GlCharacter::AddAxes(std::vector<glm::vec3> &axes)
{
    if(m_edges.empty()) 
        return 0;
    return Collision::AddAxes(axes,m_edges,model_matrix);

    
}

std::pair<float, float> GlCharacter::ProjectOnAxe(const glm::vec3 &axe) const
{
    if(m_edges.empty()) 
    {
        float position_on_axe = glm::dot(axe, m_position);
        std::pair<float, float> ret_value(position_on_axe - radius, position_on_axe + radius);
        return ret_value;
    }
    else
    {
        return Collision::ProjectEdgesOnAxe(model_matrix,m_edges,m_position,axe);
    }
}


