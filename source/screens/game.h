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
    
    void SetNumPlayers(int numPlayers);
    int GetNumPlayers();
    
    pb::PerspectiveCamera* GetCamera();
    glm::vec4 GetArenaBounds();
    
    void AddScore(float score);
    float GetScore();
    float GetBestScore();
    
private:
    pb::PerspectiveCamera* _Camera;
    pb::Scene* _Scene;
    pb::Viewport* _Viewport;
    
    LevelSegment* _LevelSegment;
    
    float _BestScore;
    float _Score;
    float _CurrentY;
    int _NumPlayers;
    
    BackgroundTile* _Background;
};
