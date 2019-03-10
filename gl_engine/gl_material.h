#ifndef _GL_MATERIAL_H
#define _GL_MATERIAL_H
#include <GL/glew.h>
#include <string>
#include "glresourses.h"
#include "i_gl_texture_struct.h"

namespace GameResource
{
    struct GlMaterialStruct
    {
        std::shared_ptr<IGlTextureStruct> m_albedo_texture;
        std::shared_ptr<IGlTextureStruct> m_normal_height_texture;
        std::shared_ptr<IGlTextureStruct> m_roughness_metalness_texture;

        GlMaterialStruct(const std::string & filename)
        {
            //LoadTexture(filename,m_texture);
        }

        GlMaterialStruct(   const std::string & albedo,
                            const std::string & normal_height,
                            const std::string & roughness_metalness
                        );


        ~GlMaterialStruct()
        {
            //glDeleteTextures(1,&m_texture);
        }
    };
}
#endif