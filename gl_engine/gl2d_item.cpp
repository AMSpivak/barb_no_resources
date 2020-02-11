#include "gl2d_item.h"
#include <iostream>
namespace Gl2D
{
    void Gl2dItem::CalculateAligment()
    {
        switch(m_aligment) 
            {
                case ItemAligment::Center:
                        real_x = - real_width *0.5f;
                        real_y = - real_height *0.5f;
                break;
                case ItemAligment::Left:
                    real_x = -1.0f;
                break;
                default: 
                break;
            }
    }
    
    void Gl2dItem::UseAspectRatioKeeper()
    {
        switch(m_aspect_ratio_keeper) 
            {
                case AspectRatioKeeper::Minimal:
                    if(m_aspect_ratio > 1.f)
                    {
                        real_x /= m_aspect_ratio;
                        real_y /= m_aspect_ratio;
                        real_width /= m_aspect_ratio;
                        real_height /= m_aspect_ratio;
                    }
                break;
                default: 
                break;
            }
    }

    void Gl2dItem::RecalculateGeometry()
    {
        real_x = m_x;
        real_y = m_y;
        real_width = m_width;
        real_height = m_height;

        if(auto parent = m_parent.lock())
        {
            float parent_x = 0;
            float parent_y = 0;
            float parent_width = 2.0f;
            float parent_height = 2.0f;

            std::tie(parent_x,parent_y,parent_width,parent_height) = parent->GetPosAndSize();
            CalculateAligment();

            real_x = parent_x + (real_x + 1.0f) * parent_width * 0.5f;
            real_y = parent_y + (real_y + 1.0f) * parent_height * 0.5f;
            real_width *= 0.5f * parent_width;
            real_height *= 0.5f * parent_height;
            //std::cout << real_x <<" "<< real_y <<" "<< real_width <<" "<< real_height <<"\n";
        }
        else
        {
            real_y = real_y * m_aspect_ratio;
            real_height = real_height * m_aspect_ratio;
            UseAspectRatioKeeper();                
            CalculateAligment();
        }
        
    }
}