#include "pch.h"
#include "GraphPathFinder.h"
#include "GraphData.h"
#include "Transform.h"
#include "ColliderManager.h"
COMPONENT(GraphPathFinder)


void GraphPathFinder::Init()
{
}

void GraphPathFinder::Start()
{
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
    std::unordered_map<LeftRight, std::unordered_map<int, vec3>>& leftrightData = GraphData::datas;

    auto& data = leftrightData[_leftRight];

    vec3& TargetPos = data[_currentTargetIndex];
    vec3 currentPos = GetOwner()->_transform->GetWorldPosition();

    vec3 toTargetDir = TargetPos - currentPos;
    toTargetDir.Normalize();

    currentPos = GetOwner()->_transform->SetWorldPosition(currentPos + toTargetDir * Time::main->GetDeltaTime() * 1000.0f);

    GetOwner()->_transform->LookUpSmooth(toTargetDir, vec3::Up, 3.0f);

    auto hit = ColliderManager::main->RayCastSpeed({ currentPos, toTargetDir }, 5.0f, GetOwner());

    if (hit)
    {
        GetRamdomTarget();
        return;
    }

    if ((TargetPos - currentPos).LengthSquared() < 1.0f) // 거리 1 이하
    {
        GetRamdomTarget();
        return;
    }

}

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

    _currentTargetIndex = newIndex;
    return sideData[_currentTargetIndex];
};
