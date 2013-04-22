#include "pixelboost/input/joystickManager.h"
#include "pixelboost/input/keyboardManager.h"
#include "pixelboost/logic/entity.h"

namespace pb
{
    class TextComponent;
}

class MenuItem : public pb::Entity, public pb::JoystickHandler, public pb::KeyboardHandler
{
    PB_DECLARE_ENTITY
    
public:
    MenuItem(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity);
    ~MenuItem();
    
    virtual int GetInputHandlerPriority();
    
public:
    void OnUpdate(const pb::Message& message);
    
    virtual bool OnButtonDown(int joystick, int button);
    virtual bool OnButtonUp(int joystick, int button);
    
    virtual bool OnKeyboardEvent(pb::KeyboardEvent event);
    
private:
    bool _ShowingControls;
    
    pb::TextComponent* _Single;
    pb::TextComponent* _Multi;
};
