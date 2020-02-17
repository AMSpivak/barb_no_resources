#ifndef GL2D_BUTTON
#define GL2D_BUTTON
#include <memory>
#include "gl2d_item.h"
#include "i_gl_texture_struct.h"
namespace Gl2D
{
    class GlButton : public Gl2dItem
    {
        using sp_texture = std::shared_ptr<IGlTextureStruct>;
        private:
        sp_texture m_texture;
        GLuint m_shader;
        float m_active_mul;
        public:
        GlButton(float x,float y,float width, float height, float aspect_ratio, sp_texture texture,GLuint shader):
                                                        Gl2dItem(x,y,width,height, aspect_ratio), m_texture(texture)
                                                        ,m_shader(shader),m_active_mul(1.0f)

                                                         {}
        ~GlButton(){}

        void SetImage(sp_texture image);
        void SetActiveSizer(float value);

        void Draw();
    };

}

#endif