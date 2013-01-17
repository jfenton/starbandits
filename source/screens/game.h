#include "pixelboost/framework/screen.h"

namespace pb
{
    class PerspectiveCamera;
    class Scene;
    class Viewport;
}

class BackgroundTile;
class LevelSegment;
class PlayerShip;

class GameScreen : public pb::Screen
{
public:
    GameScreen();
    ~GameScreen();
    
public:
    virtual void Update(float time);
    
    virtual void SetActive(bool active);
    
    pb::PerspectiveCamera* GetCamera();
    glm::vec4 GetArenaBounds();
    
private:
    pb::PerspectiveCamera* _Camera;
    pb::Scene* _Scene;
    pb::Viewport* _Viewport;
    
    float _CurrentY;
    
    BackgroundTile* _Background;
    PlayerShip* _Player;
    
    std::vector<LevelSegment*> _Segments;
};
