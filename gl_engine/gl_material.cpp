#include "gl_material.h"
#include "gl_resources_manager.h"
namespace GameResource
{
    GlMaterial::GlMaterial( const std::string & albedo,
                            const std::string & normal_height,
                            const std::string & roughness_metalness
                        )
    {
        auto pmanager = GetResourceManager();
        if(albedo!="")
        {
            m_albedo_texture = pmanager->m_texture_atlas.Assign(albedo);
        }
        if(normal_height!="")
        {
            m_normal_height_texture = pmanager->m_texture_atlas.Assign(normal_height);
        }
        if(roughness_metalness!="")
        {
            m_roughness_metalness_texture = pmanager->m_texture_atlas.Assign(roughness_metalness);
        }
    }
}