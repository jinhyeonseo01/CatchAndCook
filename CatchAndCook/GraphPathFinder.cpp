#include "pch.h"
#include "GraphPathFinder.h"
#include "GraphData.h"
#include "Transform.h"
#include "ColliderManager.h"
#include "MeshRenderer.h"
#include "SkinnedMeshRenderer.h"
COMPONENT(GraphPathFinder)

static random_device dre;
static mt19937 gen(dre());
static uniform_real_distribution<float> randomMoveSpeed(100.0f, 150.0f);


void GraphPathFinder::Init()
{
    _speed = randomMoveSpeed(dre);
}

void GraphPathFinder::Start()
{
    auto& skinedrenderer = GetOwner()->GetComponentWithChilds<SkinnedMeshRenderer>();

    if (skinedrenderer ==nullptr)
    {
        auto& meshrenderer = GetOwner()->GetComponentWithChilds<MeshRenderer>();

        meshrenderer->AddStructuredSetter(
            std::static_pointer_cast<GraphPathFinder>(shared_from_this()),
            BufferType::SeaFIshParam
        );

        const auto& materials = meshrenderer->GetMaterials();

        if (materials.empty())
            return;

        const auto& mat = materials[0];

        _info.fishSpeed = mat->GetPropertyFloat("_Speed");
        _info.fishWaveAmount = mat->GetPropertyFloat("_Power");
        const BoundingBox& box = meshrenderer->GetOriginBound();
        _info.boundsSizeZ = box.Extents.z;
        _info.boundsCenterZ = box.Center.z;
    }
 

    ///////////////////////////////////////////////////////////

    vec3& currentPos =GetOwner()->_transform->GetWorldPosition();

    if (currentPos.x < 0)
    {
        _leftRight = LeftRight::Left;
    }
    else
    {
        _leftRight = LeftRight::Right;
    }

    GetRamdomTarget();


}

void GraphPathFinder::Update()
{
    if (_autoPliot)
        return;
    
    CalculatePath(_speed);

};

void GraphPathFinder::Update2()
{
}

void GraphPathFinder::Enable()
{
}

void GraphPathFinder::Disable()
{
}

void GraphPathFinder::RenderBegin()
{
}

void GraphPathFinder::RenderEnd()
{
}

void GraphPathFinder::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void GraphPathFinder::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void GraphPathFinder::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void GraphPathFinder::ChangeScene(const std::shared_ptr<Scene>& currentScene, const std::shared_ptr<Scene>& nextScene)
{
}

void GraphPathFinder::SetDestroy()
{
}

void GraphPathFinder::Destroy()
{
}


vec3 GraphPathFinder::GetRamdomTarget()
{
 

    LeftRight opposite = (_leftRight == LeftRight::Left) ? LeftRight::Right : LeftRight::Left;
    auto& sideData = GraphData::datas[opposite];

    int newIndex;

    do
    {
        newIndex = rand() % sideData.size();
    } while (newIndex == _currentTargetIndex);

    _leftRight = opposite;
    _currentTargetIndex = newIndex;
    return sideData[_currentTargetIndex];
};

void GraphPathFinder::CalculatePath(float speed)
{
    std::unordered_map<LeftRight, std::unordered_map<int, vec3>>& leftrightData = GraphData::datas;

    auto& data = leftrightData[_leftRight];

    vec3& TargetPos = data[_currentTargetIndex];
    vec3 currentPos = GetOwner()->_transform->GetWorldPosition();

    vec3 toTargetDir = TargetPos - currentPos;
    toTargetDir.Normalize();

    currentPos = GetOwner()->_transform->SetWorldPosition(currentPos + toTargetDir * Time::main->GetDeltaTime() * speed);
   
    GetOwner()->_transform->LookUpSmooth(toTargetDir, vec3::Up, 3.0f);

    bool hit = ColliderManager::main->RayCastSpeed({ currentPos, toTargetDir }, 100.0f, GetOwner(),GameObjectTag::Monster);

    if (hit)
    {
        GetRamdomTarget();
        return;
    }

    if ((TargetPos - currentPos).LengthSquared() < 1.0f)
    {
        GetRamdomTarget();
        return;
    }

}

void GraphPathFinder::SetData(StructuredBuffer* buffer, Material* material)
{
    buffer->AddData(_info);
}