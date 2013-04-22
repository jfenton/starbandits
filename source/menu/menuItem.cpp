#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/logic/component/graphics/sprite.h"
#include "pixelboost/logic/component/graphics/text.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/scene.h"

#include "common/layers.h"
#include "core/game.h"
#include "gameplay/health.h"
#include "menu/menuItem.h"
#include "player/player.h"
#include "screens/game.h"

PB_DEFINE_ENTITY(MenuItem)

MenuItem::MenuItem(pb::Scene* scene, pb::Entity* parent, pb::DbEntity* creationEntity)
    : pb::Entity(scene, parent, creationEntity)
{
    _ShowingControls = false;
    
    auto transform = CreateComponent<pb::TransformComponent>();
    transform->SetPosition(glm::vec3(0.f, 0.f, 0.f));
    
    auto sprite = CreateComponent<pb::SpriteComponent>();
    sprite->SetSprite("/spritesheets/game", "menu");
    sprite->SetRenderPass(pb::kRenderPassUi);
    
    char highscoreText[128];
    snprintf(highscoreText, 128, "Highscore: %08.f", Game::Instance()->GetGameScreen()->GetBestScore());
    
    char lastScoreText[128];
    snprintf(lastScoreText, 128, "Last Score: %08.f", Game::Instance()->GetGameScreen()->GetScore());
    
    /*
    pb::TextComponent* highscore = new pb::TextComponent(this, "font", highscoreText);
    highscore->SetAlignment(pb::kFontAlignCenter);
    highscore->SetRenderPass(pb::kRenderPassUi);
    highscore->SetLayer(kGraphicLayerUi);
    highscore->SetSize(0.5f);
    highscore->SetLocalTransform(glm::translate(glm::mat4x4(), glm::vec3(0.f, 340.f/32.f, 0.f)));
    highscore->SetTint(glm::vec4(1,0,0,1));
    
    pb::TextComponent* lastscore = new pb::TextComponent(this, "font", lastScoreText);
    lastscore->SetAlignment(pb::kFontAlignCenter);
    lastscore->SetRenderPass(pb::kRenderPassUi);
    lastscore->SetLayer(kGraphicLayerUi);
    lastscore->SetSize(0.5f);
    lastscore->SetLocalTransform(glm::translate(glm::mat4x4(), glm::vec3(0.f, 310.f/32.f, 0.f)));
    lastscore->SetTint(glm::vec4(1,0,0,1));
    
    _Single = new pb::TextComponent(this, "font", "Press O for One Player");
    _Single->SetAlignment(pb::kFontAlignLeft);
    _Single->SetRenderPass(pb::kRenderPassUi);
    _Single->SetLayer(kGraphicLayerUi);
    _Single->SetSize(0.5f);
    _Single->SetLocalTransform(glm::translate(glm::mat4x4(), glm::vec3(-620.f/32.f, 340.f/32.f, 0.f)));
    
    _Multi = new pb::TextComponent(this, "font", "Press A for Two Player");
    _Multi->SetAlignment(pb::kFontAlignRight);
    _Multi->SetRenderPass(pb::kRenderPassUi);
    _Multi->SetLayer(kGraphicLayerUi);
    _Multi->SetSize(0.5f);
    _Multi->SetLocalTransform(glm::translate(glm::mat4x4(), glm::vec3(620.f/32.f, 340.f/32.f, 0.f)));
    */
    
    RegisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &MenuItem::OnUpdate));
    
    pb::Engine::Instance()->GetJoystickManager()->AddHandler(this);
    pb::Engine::Instance()->GetKeyboardManager()->AddHandler(this);
}

MenuItem::~MenuItem()
{
    pb::Engine::Instance()->GetJoystickManager()->RemoveHandler(this);
    pb::Engine::Instance()->GetKeyboardManager()->RemoveHandler(this);
    
    UnregisterMessageHandler<pb::UpdateMessage>(pb::MessageHandler(this, &MenuItem::OnUpdate));
}

int MenuItem::GetInputHandlerPriority()
{
    return 0;
}

void MenuItem::OnUpdate(const pb::Message& message)
{
    /*
    bool alpha = glm::mod(Game::Instance()->GetGameTime(), 1.5f) < 0.75f;
    if (_Single && _Multi)
    {
        _Single->SetTint(glm::vec4(1,1,1,alpha?1.f:0.f));
        _Multi->SetTint(glm::vec4(1,1,1,alpha?1.f:0.f));
    }
    */
}

bool MenuItem::OnButtonDown(int joystick, int button)
{
    if (_ShowingControls)
    {
        Game::Instance()->SetMode(kGameModeGame);
    } else {
        if (button == 0) {
            Game::Instance()->GetGameScreen()->SetNumPlayers(1);
            GetComponent<pb::SpriteComponent>()->SetSprite("/spritesheets/game", "control");
            _ShowingControls = true;
        } else if (button == 1) {
            Game::Instance()->GetGameScreen()->SetNumPlayers(2);
            GetComponent<pb::SpriteComponent>()->SetSprite("/spritesheets/game", "control");
            _ShowingControls = true;
        }
        
        if (_ShowingControls)
        {
            DestroyComponent(GetComponent<pb::TextComponent>());
            
            _Single = 0;
            _Multi = 0;
            
            /*
            pb::TextComponent* single = new pb::TextComponent(this, "font", "Press O to Start");
            single->SetAlignment(pb::kFontAlignCenter);
            single->SetRenderPass(pb::kRenderPassUi);
            single->SetLayer(kGraphicLayerUi);
            single->SetSize(0.5f);
            single->SetTint(glm::vec4(0,0,0,1));
            single->SetLocalTransform(glm::translate(glm::mat4x4(), glm::vec3(0.f, -340.f/32.f, 0.f)));
            */
        }
    }
    
    return false;
}

bool MenuItem::OnButtonUp(int joystick, int button)
{
    return false;
}

bool MenuItem::OnKeyboardEvent(pb::KeyboardEvent event)
{
    if (event.Type == pb::KeyboardEvent::kKeyboardEventDown)
    {
        Game::Instance()->GetGameScreen()->SetNumPlayers(1);
        Game::Instance()->SetMode(kGameModeGame);
    }
    
    return false;
}
