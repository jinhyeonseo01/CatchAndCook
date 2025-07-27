#include "pch.h"
#include "FireWork.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "FireWorkManager.h"


COMPONENT(FireWork)

FireWork::~FireWork()
{
}

void FireWork::Init()
{
	FireWorkManager::main->PushFireWork(GetOwner());
}

void FireWork::Start()
{
	

	
}

void FireWork::Update()
{

	if (_on)
	{
		_currentTime += Time::main->GetDeltaTime();

		if (_currentTime > _fireBallResponTime)
		{
			_currentTime = 0;

			_fireBallCount++;
			SpawnFireBall();

			if (_fireBallCount >= _maxFireBall)
			{
				_on = false;
				_fireBallCount = 0;
				return;
			}

		}
	}
}

void FireWork::Update2()
{
}

void FireWork::Enable()
{
}

void FireWork::Disable()
{
}

void FireWork::RenderBegin()
{
}

void FireWork::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void FireWork::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void FireWork::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void FireWork::SetDestroy()
{
}

void FireWork::Destroy()
{

}



void FireWork::SpawnFireBall()
{
	fireballColor color = static_cast<fireballColor>(rand() % _maxColor);

	shared_ptr<GameObject> origin;

	switch (color)
	{
	case yellow:
		origin = FireWorkManager::main->GetFireBall(fireballColor::yellow);
		break;
	case red:
		origin = FireWorkManager::main->GetFireBall(fireballColor::red);
		break;
	case green:
		origin = FireWorkManager::main->GetFireBall(fireballColor::green);
		break;
	default:
		break;
	}


	if (!origin)
	{
		wcout << L"FireWork::SpawnFireBall() : origin is nullptr" << endl;
		return;
	}

	auto exmeshRenderer = origin->GetComponent<MeshRenderer>();
	std::vector<std::shared_ptr<Material>>& materials = exmeshRenderer->GetMaterials();
	std::vector<std::shared_ptr<Mesh>>& meshes = exmeshRenderer->GetMeshes(); 

	shared_ptr<GameObject> fireBall = SceneManager::main->GetCurrentScene()->CreateGameObject(L"FireBall");
	auto fireBallComponent =fireBall->AddComponent<FireBall>();
	fireBallComponent->SetColor(color);
	fireBall->_transform->SetWorldPosition(GetOwner()->_transform->GetWorldPosition());

	auto meshRenderer = fireBall->AddComponent<MeshRenderer>();
	meshRenderer->SetMaterials(materials);
	meshRenderer->SetMesh(meshes);


}


