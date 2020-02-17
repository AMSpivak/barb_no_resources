#include "gl2d_button.h"
#include <algorithm>
#include "glresourses.h"
#include "glm/glm.hpp"


namespace Gl2D
{
    void GlButton::Draw()
    {
        RecalculateGeometry();


        float w = real_width;
        float h = real_height;
        float x = real_x;
        float y = real_y;

        if(m_active)
        {
            x +=0.5f * (1.0f - m_active_mul) * real_width;
            y +=0.5f * (1.0f - m_active_mul) * real_height;// * m_aspect_ratio;
            w *= m_active_mul;
            h *= m_active_mul;
        }

        renderSprite(m_shader,
			x, y,
			x + w, y, 
			x + w, y + h,
			x, y + h,
			glm::vec4(1.0f,1.0f,1.0f,1.0f),&(m_texture->m_texture),
			1.0f, 1.0f,
			0.0f,0.0f
			);
    }
    

    void GlButton::SetImage(sp_texture image)
    {
        m_texture = image;
    }

    void GlButton::SetActiveSizer(float value)
    {
        m_active_mul = value;
    }


}