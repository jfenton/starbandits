#include "pixelboost/framework/screen.h"

namespace pb
{
    class Scene;
    class Viewport;
}

class MenuScreen : public pb::Screen
{
public:
    MenuScreen();
    ~MenuScreen();
    
public:
    virtual void Update(float time);
    
    virtual void SetActive(bool active);
    
private:
    pb::Scene* _Scene;
    pb::Viewport* _Viewport;
};
