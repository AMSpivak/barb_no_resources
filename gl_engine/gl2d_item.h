#ifndef GL_2D_ENGINE_ITEM
#define GL_2D_ENGINE_ITEM
#include <memory>
#include <tuple>

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
        std::weak_ptr<Gl2dItem> m_parent;
        void CalculateAligment();
        void UseAspectRatioKeeper();
        void RecalculateGeometry();
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
        void SetParent(std::weak_ptr<Gl2dItem> parent){m_parent = parent;}
        AspectRatioKeeper GetAspectRatioKeeper(){ return m_aspect_ratio_keeper;}
        void SetItemAligment(ItemAligment aligment){m_aligment = aligment;}
        ItemAligment GetItemAligment(){ return m_aligment;}
        std::tuple<float,float,float, float> GetPosAndSize(){return std::make_tuple(real_x, real_y, real_width, real_height);}
        virtual void Draw() = 0;
        virtual ~Gl2dItem(){}
    };

}

#endif