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
    GetRamdomTarget();
}

void GraphPathFinder::Update()
{
	std::unordered_map<int, vec3>& data = GraphData::datas;

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
    }



    if ((TargetPos - currentPos).Length() < 0.1f)
    {
        GetRamdomTarget();
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

void GraphPathFinder::GetRamdomTarget()
{
    auto& data = GraphData::datas;

    if (data.empty())
        return;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, static_cast<int>(data.size()) - 1);

    int randomIndex = dist(gen);
    _currentTargetIndex = randomIndex;
}
