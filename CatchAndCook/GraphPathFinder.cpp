#include "pch.h"
#include "GraphPathFinder.h"
#include "GraphData.h"
#include "Transform.h"
#include "ColliderManager.h"
#include "MeshRenderer.h"

COMPONENT(GraphPathFinder)


void GraphPathFinder::Init()
{
}

void GraphPathFinder::Start()
{
    auto renderer = GetOwner()->GetRenderer();

    if (renderer)
    {
        auto meshRdr = std::dynamic_pointer_cast<MeshRenderer>(renderer);
        if (meshRdr)
        {
            renderer->AddStructuredSetter(
                std::static_pointer_cast<PathFinder>(shared_from_this()),
                BufferType::SeaFIshParam
            );

            const auto& materials = meshRdr->GetMaterials();

            if (materials.empty())
                return;

            const auto& mat = materials[0];


            _info.fishSpeed = mat->GetPropertyFloat("_Speed");
            _info.fishWaveAmount = mat->GetPropertyFloat("_Power");
            const BoundingBox& box = meshRdr->GetOriginBound();
            _info.boundsSizeZ = box.Extents.z;
            _info.boundsCenterZ = box.Center.z;

        }
        else
        {
            cout << "캐스팅실패" << endl;
        }
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
    
    CalculatePath(300.0f);

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


    auto hit = ColliderManager::main->RayCastSpeed({ currentPos, toTargetDir }, 50.0f, GetOwner());


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
    cout << "addData" << endl;

    buffer->AddData(_info);
}