#include "Box2D/Box2D.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#include "pixelboost/framework/engine.h"
#include "pixelboost/graphics/device/device.h"
#include "pixelboost/graphics/device/indexBuffer.h"
#include "pixelboost/graphics/device/vertexBuffer.h"
#include "pixelboost/graphics/renderer/model/modelRenderer.h"
#include "pixelboost/logic/component/graphics/buffer.h"
#include "pixelboost/logic/component/graphics/model.h"
#include "pixelboost/logic/component/physics/2d/physicsBody.h"
#include "pixelboost/logic/component/transform/basic.h"
#include "pixelboost/logic/message/update.h"
#include "pixelboost/logic/system/graphics/render/render.h"
#include "pixelboost/logic/scene.h"

#include "background/stars.h"
#include "common/layers.h"

BackgroundStars::BackgroundStars(pb::Scene* scene)
    : pb::Entity(scene, 0)
{
    pb::BasicTransformComponent* transform = new pb::BasicTransformComponent(this);
    
    pb::IndexBuffer* starsIndex = pb::GraphicsDevice::Instance()->CreateIndexBuffer(pb::kBufferFormatStatic, 1000*6);
    pb::VertexBuffer* starsVertex = pb::GraphicsDevice::Instance()->CreateVertexBuffer(pb::kBufferFormatStatic, pb::kVertexFormat_P3_UV, 1000*4);
    
    pb::BufferComponent* buffer = new pb::BufferComponent(this, starsIndex, starsVertex, pb::Engine::Instance()->GetModelRenderer()->GetTexture("star"), 1000*6);
    buffer->SetLayer(kGraphicLayerStars);
    
    starsIndex->Lock();
    starsVertex->Lock();
    unsigned short* indexData = starsIndex->GetData();
    pb::Vertex_P3_UV* vertexData = static_cast<pb::Vertex_P3_UV*>(starsVertex->GetData());
    
    float size = 1.f;
    
    for (int i=0; i<1000; i++)
    {
        indexData[0] = (i*4) + 0;
        indexData[1] = (i*4) + 1;
        indexData[2] = (i*4) + 2;
        indexData[3] = (i*4) + 0;
        indexData[4] = (i*4) + 2;
        indexData[5] = (i*4) + 3;
        
        glm::vec3 position = glm::linearRand(glm::vec3(-1100,-500,-1000), glm::vec3(1100,2000,-2000));
        
        vertexData[0].position[0] = position.x - size;
        vertexData[0].position[1] = position.y - size;
        vertexData[0].position[2] = position.z;
        vertexData[1].position[0] = position.x + size;
        vertexData[1].position[1] = position.y - size;
        vertexData[1].position[2] = position.z;
        vertexData[2].position[0] = position.x + size;
        vertexData[2].position[1] = position.y + size;
        vertexData[2].position[2] = position.z;
        vertexData[3].position[0] = position.x - size;
        vertexData[3].position[1] = position.y + size;
        vertexData[3].position[2] = position.z;
        
        vertexData[0].uv[0] = 0.5;
        vertexData[0].uv[1] = 0.5;
        vertexData[1].uv[0] = 0.5;
        vertexData[1].uv[1] = 0.5;
        vertexData[2].uv[0] = 0.5;
        vertexData[2].uv[1] = 0.5;
        vertexData[3].uv[0] = 0.5;
        vertexData[3].uv[1] = 0.5;
        
        indexData += 6;
        vertexData += 4;
    }
    
    starsIndex->Unlock();
    starsVertex->Unlock();
}

BackgroundStars::~BackgroundStars()
{
    
}

pb::Uid BackgroundStars::GetType() const
{
    return GetStaticType();
}

pb::Uid BackgroundStars::GetStaticType()
{
    return pb::TypeHash("BackgroundStars");
}
