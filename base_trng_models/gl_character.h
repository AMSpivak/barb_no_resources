#ifndef GL_CHARACTER
#define GL_CHARACTER
#include "i_gl_model.h"
#include "animation_sequence.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <tuple>
#include "loader.h"
#include "gl_model.h"
#include "brain.h"

enum class CharacterTypes {hero,tile,map_object,mob};
enum class AffectionCharacters {enemy,ally,map_object};

class GlCharacter: public IGlModel
{
public:
    GlCharacter(CharacterTypes type);
    ~GlCharacter();
    void Draw(GlScene::Scene &scene) const override;
    void Draw(GlScene::Scene &scene,const glm::mat4 &draw_matrix) override;
    int Process(std::list<std::string> &m_messages);
    void AddModel(const std::string & name);
    void AddSequence(const std::string & name, const AnimationSequence & in_sequence);
    void UseSequence(const std::string & name);
    bool UseCommand(AnimationCommand command);
    void Damage(float damage, const glm::vec3 & from);
    void RefreshMatrixes(float approximation);
    void RefreshMatrixes(){RefreshMatrixes(1.0f);};

    int AddAxes(std::vector<glm::vec3> &axes);
    std::pair<float, float> ProjectOnAxe(const glm::vec3 & axe) const;
    void AddEdge(const std::pair<glm::vec3, glm::vec3> edge);
    void UpdateFromLines(std::vector<std::string> &lines);
    void ToStream(std::ostream& os) const;
    CharacterTypes GetType() const;
    void SetBrain(std::shared_ptr<Character::IBrain> brain);
    void AddEnemy(std::weak_ptr<GlCharacter> enemy);
    std::list<std::pair<std::weak_ptr<GlCharacter>,float>> enemies_list;
    std::tuple<glm::vec3, glm::vec3> Get2DBasis();
    bool IsNoRotateable();

    const std::pair<glm::vec3, glm::vec3> GetWeaponPosition();
    const std::pair<glm::vec3, glm::vec3> GetWeaponPositionOld();


private:
    std::shared_ptr<Character::IBrain> m_brain;
    CharacterTypes m_type;
    //glm::mat4 m_draw_matrix;
    size_t engine_frame;
    // static LoaderUtility::LinesProcessor command_proc;
    // static bool comand_proc_ready;

    size_t now_frame;
    std::vector <std::shared_ptr<glModel> > Models;
    std::list<std::string> model_list;
    std::vector<std::pair<glm::vec3, glm::vec3>> m_edges; 
    std::map<std::string, AnimationSequence> sequence;
    //std::list<std::pair<std::weak_ptr<GlCharacter>,float>> enemies_list;
    AnimationSequence * current_animation;
    void ExecuteCommand(const std::pair<AnimationCommand,std::string> &command,std::list<std::string> &m_messages);

    bool m_is_armed;

    int m_weapon_model;
    int m_weapon_bone;


    std::pair<glm::vec3, glm::vec3>  m_weapon_now;
    std::pair<glm::vec3, glm::vec3>  m_weapon_base;
    std::pair<glm::vec3, glm::vec3>  m_weapon_old;



    //void RefreshMatrixes();

};


const glm::vec3 & MoveObjectAttempt(IGlModel &object,const glm::vec3 &desired_direction, float length);

void UpdateCharacterFromFile(const std::string &filename,GlCharacter & character);
std::ostream& operator << ( std::ostream& os, const GlCharacter & character);

template<class InputIterator>
std::shared_ptr<GlCharacter> FindSharedCollectionByName(InputIterator first, InputIterator end, const std::string & name)
{
    auto iter = std::find_if(first,end,[&name](std::shared_ptr<GlCharacter> obj){return obj->GetName()==name;});
    return (iter != end)?*iter:nullptr;
}

#endif
