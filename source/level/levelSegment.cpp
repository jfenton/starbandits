#include "pixelboost/db/database.h"
#include "pixelboost/db/entity.h"
#include "pixelboost/db/record.h"
#include "pixelboost/logic/component/transform.h"
#include "pixelboost/logic/scene.h"

#include "core/game.h"
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
#include "screens/game.h"

LevelSegment::LevelSegment(pb::Scene* scene)
    : _Scene(scene)
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
    
    _EntityIt = _Record->GetEntities().begin();
}

LevelSegment::~LevelSegment()
{
    
}

pb::Uid LevelSegment::Create()
{
    //const pb::DbRecord::EntityMap& entities = _Record->GetEntities();
    
    if (_EntityIt == _Record->GetEntities().end())
        return 0;
    
    pb::Entity* entity = 0;
    
    glm::vec2 position(_EntityIt->second->GetPosition().x, _EntityIt->second->GetPosition().y);
    
    if (_EntityIt->second->GetType() == pb::TypeHash("Asteroid"))
    {
        entity = _Scene->CreateEntity<Asteroid>(0, _EntityIt->second);
        static_cast<Asteroid*>(entity)->Initialise(position, _EntityIt->second->GetScale().x);
    } else if (_EntityIt->second->GetType() == pb::TypeHash("HomingMine"))
    {
        entity = _Scene->CreateEntity<HomingMine>(0, _EntityIt->second)->Initialise(position);
    } else if (_EntityIt->second->GetType() == pb::TypeHash("StaticMine"))
    {
        entity = _Scene->CreateEntity<StaticMine>(0, _EntityIt->second)->Initialise(position);
    } else if (_EntityIt->second->GetType() == pb::TypeHash("StealthBomber"))
    {
        entity = _Scene->CreateEntity<StealthBomber>(0, _EntityIt->second)->Initialise(position);
    } else if (_EntityIt->second->GetType() == pb::TypeHash("TurretHoming"))
    {
        entity = _Scene->CreateEntity<Turret>(0, _EntityIt->second)->Initialise(position, kProjectileTypeHoming);
    } else if (_EntityIt->second->GetType() == pb::TypeHash("TurretLaser"))
    {
        entity = _Scene->CreateEntity<Turret>(0, _EntityIt->second)->Initialise(position, kProjectileTypeLaser);
    }
    
    _EntityIt++;
        
    if (entity)
        return entity->GetUid();
    
    return 0;
}

float LevelSegment::GetLength()
{
    return _Record->GetData<LevelSegmentDefinition>()->Length;
}
