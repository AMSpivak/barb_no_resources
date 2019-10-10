#include "map_event_hero_strikes.h"
#include "collision.h"

void IMapEventHeroStrike::AddEdge(const std::pair<glm::vec3, glm::vec3> edge)
{
    m_edges.push_back(edge);
}

void IMapEventHeroStrike::SetIndicator(glm::vec3 indicator)
{
    m_indicator = indicator;
}

void IMapEventHeroStrike::Show(const glm::vec3 & offset, GlScene::glCamera & camera)
{
    /*renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
        m_width,m_height,position,offset,camera);*/
/*
    glm::vec4 norm4;
    glm::vec3 norm;

    for(auto edge :m_edges)
    {
        norm4 = model_matrix * glm::vec4(edge.second[0],edge.second[1],edge.second[2],1.0f);
        norm = position + glm::vec3(norm4[0],norm4[1],norm4[2]);
        renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
            m_width,m_height,glm::vec4(1.0,1.0,1.0,1.0),norm,offset,camera);
    }*/
    //m_edges[0].first
    if(m_texture)
    {
        float m_alpha = 1.0f;

        const glm::vec2 t(0.0f,1.0f);

        auto v = glm::vec3(model_matrix * glm::vec4(m_indicator,1.0));
        // renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
        //     m_width,m_height,glm::vec4(m_alpha,m_alpha,m_alpha,m_alpha),position + v,offset,camera);
        RenderSingleTriangle(m_current_shader, m_depthmap, 
        position + m_points[0], t,
        position + m_points[1], t,
        position + m_points[2], t,
        glm::vec4(m_alpha,m_alpha,m_alpha,m_alpha),
        camera,
        m_texture);

    }
}

void IMapEventHeroStrike::AssignPoints( const glm::vec3 & p1,
                                        const glm::vec3 & p2,
                                        const glm::vec3 & p3)
{
    m_points[0] = p1;
    m_points[1] = p2;
    m_points[2] = p3;
}

int IMapEventHeroStrike::AddAxes(std::vector<glm::vec3> &axes)
{
    return Collision::AddAxes(axes,m_edges,model_matrix);
}



std::pair<float, float> IMapEventHeroStrike::ProjectOnAxe(const glm::vec3 & axe)
{
    return Collision::ProjectEdgesOnAxe(model_matrix,m_edges,position,axe);
}

EventProcessResult IMapEventHeroStrike::Process()
{
    //std::cout<<"Stop event\n";
    return EventProcessResult::Kill;
}

InteractionResult IMapEventHeroStrike::Interact(GlCharacter &model,std::string &return_value)
{
    model.Damage(m_damage);
    model.AddEnemy(m_owner);
    std::cout<<model.GetName()<<" life "<<model.GetLifeValue()<<"\n";
    if((model.GetLifeValue()< 0)&&(model.GetType() != CharacterTypes::hero))
    {
        return InteractionResult::Kill;
    }
    return InteractionResult::Damage;
}

bool IMapEventHeroStrike::IsInteractable(std::weak_ptr<GlCharacter> obj)
{
    //if(m_owner.expired)
    return m_owner.lock() != obj.lock();
}