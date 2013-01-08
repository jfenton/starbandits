#include "pixelboost/input/keyboardManager.h"
#include "pixelboost/logic/entity.h"

namespace pb
{
    class Message;
    class Scene;
}

class PlayerShip : public pb::Entity, pb::KeyboardHandler
{
public:
    PlayerShip(pb::Scene* scene);
    ~PlayerShip();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
    
    virtual bool OnKeyDown(pb::KeyboardKey key, char character);
    virtual bool OnKeyUp(pb::KeyboardKey key, char character);
    
private:
    float _ThrustForward;
    float _ThrustBackward;
    float _ThrustRight;
    float _ThrustLeft;
    
    bool _Firing;
    float _FiringDelay;
};
