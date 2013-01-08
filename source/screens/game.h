#include "pixelboost/framework/screen.h"

namespace pb
{
    class Camera;
    class Scene;
    class Viewport;
}

class PlayerShip;

class GameScreen : public pb::Screen
{
public:
    GameScreen();
    ~GameScreen();
    
public:
    virtual void Update(float time);
    
    virtual void SetActive(bool active);
    
private:
    pb::Camera* _Camera;
    pb::Scene* _Scene;
    pb::Viewport* _Viewport;
    
    PlayerShip* _Player;
};
