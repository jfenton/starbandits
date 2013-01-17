#include "pixelboost/db/database.h"
#include "pixelboost/db/entity.h"
#include "pixelboost/db/record.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/scene.h"

#include "database/entities/asteroid.h"
#include "database/entities/homingMine.h"
#include "database/entities/staticMine.h"
#include "database/records/levelSegment.h"
#include "enemies/asteroid.h"
#include "enemies/homingMine.h"
#include "enemies/staticMine.h"
#include "enemies/stealthBomber.h"
#include "enemies/turret.h"
#include "level/levelSegment.h"

LevelSegment::LevelSegment(pb::Scene* scene, float offset)
    : _Offset(offset)
    , _Scene(scene)
{
    const pb::Database::RecordDescriptionList& records = pb::Database::Instance()->GetRecordDescriptions();
    
    std::vector<pb::Uid> segments;
    for (pb::Database::RecordDescriptionList::const_iterator it = records.begin(); it != records.end(); ++it)
    {
        if (it->Type == pb::TypeHash("LevelSegment"))
        {
            segments.push_back(it->Id);
        }
    }
    
    pb::Uid recordId = segments[rand()%segments.size()];
    
    _Record = pb::Database::Instance()->OpenRecord(recordId);
    
    _Record->GetData<LevelSegmentDefinition>();
}

LevelSegment::~LevelSegment()
{
    
}

void LevelSegment::Create()
{
    const pb::DbRecord::EntityMap& entities = _Record->GetEntities();
    
    for (pb::DbRecord::EntityMap::const_iterator it = entities.begin(); it != entities.end(); ++it)
    {
        pb::Entity* entity = 0;
        
        if (it->second->GetType() == pb::TypeHash("Asteroid"))
        {
            entity = new Asteroid(_Scene, glm::vec2(it->second->GetPosition().x, it->second->GetPosition().y + _Offset));
        }
        
        if (it->second->GetType() == pb::TypeHash("HomingMine"))
        {
            entity = new HomingMine(_Scene, glm::vec2(it->second->GetPosition().x, it->second->GetPosition().y + _Offset));
        }
        
        if (it->second->GetType() == pb::TypeHash("StaticMine"))
        {
            entity = new StaticMine(_Scene, glm::vec2(it->second->GetPosition().x, it->second->GetPosition().y + _Offset));
        }

        if (it->second->GetType() == pb::TypeHash("StealthBomber"))
        {
            entity = new StealthBomber(_Scene, glm::vec2(it->second->GetPosition().x, it->second->GetPosition().y + _Offset));
        }
        
        if (it->second->GetType() == pb::TypeHash("Turret"))
        {
            entity = new Turret(_Scene, glm::vec2(it->second->GetPosition().x, it->second->GetPosition().y + _Offset));
        }
        
        if (entity)
            _Ids.push_back(entity->GetUid());
    }
}

float LevelSegment::GetLength()
{
    return _Record->GetData<LevelSegmentDefinition>()->Length;
}

bool LevelSegment::Cleanup(float scroll, glm::vec4 bounds)
{
    int itemCount = 0;
    
    for (std::vector<pb::Uid>::iterator it = _Ids.begin(); it != _Ids.end(); ++it)
    {
        pb::Entity* entity = _Scene->GetEntityById(*it);
        
        if (!entity)
            continue;
        
        itemCount++;
        
        glm::vec3 position = entity->GetComponentByType<pb::TransformComponent>()->GetPosition();
        
        if (position.y < scroll || position.x < -bounds[2]-5.f || position.x > bounds[2]+5.f)
        {
            entity->Destroy();
        }
    }
    
    return itemCount > 0;
}
