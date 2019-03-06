#ifndef GL_2D_ENGINE_ITEM
#define GL_2D_ENGINE_ITEM

namespace Gl2D
{
    enum class ItemAligment {None,Fill,Center,Left,Right,Top,Bottom};
    enum class AspectRatioKeeper {None,Width,Height,Maximal,Minimal};

    class Gl2dItem
    {
        protected:
        float real_width;
        float real_height;
        float real_x;
        float real_y;
        float m_x;
        float m_y;
        float m_width;
        float m_height;
        float m_aspect_ratio;
        ItemAligment m_aligment;
        AspectRatioKeeper m_aspect_ratio_keeper;
        void RecalculateGeometry()
        {
            real_x = m_x;
            real_y = m_y * m_aspect_ratio;
            real_width = m_width;
            real_height = m_height * m_aspect_ratio;

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

            if(m_aligment == ItemAligment::Center)
            {
                real_x = - real_width *0.5f;
                real_y = - real_height *0.5f;
            }
            
            if(m_aligment == ItemAligment::Left)
            {
                real_x = -1.0f;
            }
        }
        public:
        // Gl2dItem(){}
        Gl2dItem(float x,float y,float width, float height, float aspect_ratio):
                                                                                     m_x(x)
                                                                                    ,m_y(y)
                                                                                    ,m_width(width)
                                                                                    ,m_height(height)
                                                                                    ,m_aspect_ratio(aspect_ratio)

        {}
        void SetAspectRatioKeeper(AspectRatioKeeper keeper){m_aspect_ratio_keeper = keeper;}
        AspectRatioKeeper GetAspectRatioKeeper(){ return m_aspect_ratio_keeper;}
        void SetItemAligment(ItemAligment aligment){m_aligment = aligment;}
        ItemAligment GetItemAligment(){ return m_aligment;}

        virtual void Draw() = 0;
        virtual ~Gl2dItem(){}
    };

}

#endif