#include "pixelboost/input/joystickManager.h"
#include "pixelboost/logic/entity.h"

class MenuItem : public pb::Entity, public pb::JoystickHandler
{
public:
    MenuItem(pb::Scene* scene);
    ~MenuItem();
    
public:
    virtual pb::Uid GetType() const;
    static pb::Uid GetStaticType();
    
    void OnUpdate(const pb::Message& message);
    
    virtual bool OnButtonDown(int joystick, int button);
    virtual bool OnButtonUp(int joystick, int button);
    
private:
    bool _ShowingControls;
};
